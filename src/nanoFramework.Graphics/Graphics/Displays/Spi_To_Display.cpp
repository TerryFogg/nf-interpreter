//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_DISPLAY_
#define _SPI_TO_DISPLAY_ 1

#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>

#define NUMBER_OF_LINES 11
#define SPI_MAX_TRANSFER_SIZE (480 * 3 * NUMBER_OF_LINES)       // 320 pixels wide, allow for 3 bytes per pixel for 18-bit SPI ILI9488

#if SPI_MAX_TRANSFER_SIZE > 16384
#pragma error "Buffer size greater than underlying SPI buffer support"
#endif

struct DisplayInterface g_DisplayInterface;

// Saved gpio pins
ControlPin lcdReset;
ControlPin lcdDC;
ControlPin lcdBacklight;
ControlPin ChipSelect;

GpioPinValue BacklightOn;
GpioPinValue BacklightOff;
GpioPinValue ResetActive;
GpioPinValue ResetIdle;
GpioPinValue DataMode;
GpioPinValue CommandMode;

CLR_UINT32 spiDeviceHandle = 0;
CLR_INT16 outputBufferSize;
CLR_UINT8 spiBuffer[SPI_MAX_TRANSFER_SIZE];
CLR_UINT8 spiCommandMode = 0; // 0 Command first byte, 1 = Command all bytes

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{

    SPI_DEVICE_CONFIGURATION spiConfig;
    spiConfig.BusMode = SpiBusMode::SpiBusMode_master;
    spiConfig.Spi_Bus = config.Spi.spiBus;
    spiConfig.DeviceChipSelect = config.Spi.chipSelect.pin;
    spiConfig.ChipSelectActive = config.Spi.reset.type.activeLow ? false : true; // false ==> low, true ==> high;
    spiConfig.Spi_Mode = SpiMode::SpiMode_Mode0;
    spiConfig.DataOrder16 = DataBitOrder::DataBitOrder_MSB;
    spiConfig.Clock_RateHz = 50 * 1000 * 1000; // Spi clock speed.

    HRESULT hr = nanoSPI_OpenDevice(spiConfig, spiDeviceHandle);
    ASSERT(hr == ESP_OK);
    if (hr == S_OK)
    {
        // Data/Command Pin
        CommandMode = config.Spi.dataCommand.type.commandLow ? GpioPinValue_Low : GpioPinValue_High;
        DataMode = (CommandMode == GpioPinValue_Low) ? GpioPinValue_High : GpioPinValue_Low;
        lcdDC.pin = config.Spi.dataCommand.pin;
        CPU_GPIO_ReservePin(lcdDC.pin, true);
        CPU_GPIO_SetDriveMode(lcdDC.pin, GpioPinDriveMode::GpioPinDriveMode_Output);

        // Save pin numbers
        lcdReset.pin = config.Spi.reset.pin;
        if ((int32_t)lcdReset.pin != IMPLEMENTED_IN_HARDWARE)
        {
            ResetActive = config.Spi.reset.type.activeLow ? GpioPinValue_Low : GpioPinValue_High;
            ResetIdle = (ResetActive == GpioPinValue_Low) ? GpioPinValue_High : GpioPinValue_Low;
            CPU_GPIO_ReservePin(lcdReset.pin, true);
            CPU_GPIO_SetDriveMode(lcdReset.pin, GpioPinDriveMode::GpioPinDriveMode_Output);
            CPU_GPIO_SetPinState(lcdReset.pin, ResetActive);
            OS_DELAY(100);
            CPU_GPIO_SetPinState(lcdReset.pin, ResetIdle);
            OS_DELAY(100);
        }

        // Initialize non-SPI GPIOs
        lcdBacklight.pin = config.Spi.backLight.pin;
        if ((int32_t)lcdBacklight.pin != IMPLEMENTED_IN_HARDWARE)
        {
            BacklightOn = config.Spi.backLight.type.activeLow ? GpioPinValue_Low : GpioPinValue_High;
            BacklightOff = (BacklightOn == GpioPinValue_Low) ? GpioPinValue_High : GpioPinValue_Low;
            CPU_GPIO_ReservePin(lcdBacklight.pin, true);
            CPU_GPIO_SetDriveMode(lcdBacklight.pin, GpioPinDriveMode::GpioPinDriveMode_Output);
            g_DisplayInterface.DisplayBacklight(true);
        }
    }

    return;
}

void DisplayInterface::SetCommandMode(int mode)
{
    spiCommandMode = mode;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer, CLR_UINT32 &TransferBufferSize)
{
    TransferBuffer = spiBuffer;
    TransferBufferSize = sizeof(spiBuffer);
}

void DisplayInterface::ClearFrameBuffer()
{
    // Not Used
}

void DisplayInterface::WriteToFrameBuffer(
    CLR_UINT8 command,
    CLR_UINT8 data[],
    CLR_UINT32 dataCount,
    CLR_UINT32 frameOffset)
{
    (void)frameOffset;

    CPU_GPIO_SetPinState(lcdDC.pin, CommandMode); // Command mode

    SendBytes(&command, 1);

    CPU_GPIO_SetPinState(lcdDC.pin, DataMode); // Data mode

    SendBytes(data, dataCount);
    return;
}

void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // Parse arguments into parameters buffer
    CLR_UINT8 parameters[arg_count];
    for (int i = 0; i < arg_count; i++)
    {
        parameters[i] = va_arg(ap, int);
    }

    CPU_GPIO_SetPinState(lcdDC.pin, CommandMode); // Command mode
    if (spiCommandMode == 0)
    {
        // Send only first byte (command) with D/C signal low
        SendBytes(&parameters[0], 1);

        CPU_GPIO_SetPinState(lcdDC.pin, DataMode); // Data mode

        // Send remaining parameters ( if any )
        if (arg_count > 1)
        {
            SendBytes(&parameters[1], arg_count - 1);
        }
    }
    else
    {
        // Send all Command bytes with D/C signal low
        SendBytes(parameters, arg_count);
        CPU_GPIO_SetPinState(lcdDC.pin, DataMode); // Data mode
    }
}

void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    if ((int32_t)lcdBacklight.pin != IMPLEMENTED_IN_HARDWARE) // If backlight power not under software control
    {
    if (on)
    {
            CPU_GPIO_SetPinState(lcdBacklight.pin, BacklightOn);
    }
    else
    {
            CPU_GPIO_SetPinState(lcdBacklight.pin, BacklightOff);
        }
    }
    return;
}

void DisplayInterface::SendBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    if (length == 0)
        return; // no need to send anything

    SPI_WRITE_READ_SETTINGS wrc;

    wrc.Bits16ReadWrite = false;
    wrc.callback = 0; // No callback, synchronous
    wrc.fullDuplex = false;
    wrc.readOffset = 0;

    // Theoretically, we could do work this asynchronously and continue to work, but this would require
    // double buffering.

    // Start a synchronous read / write spi job
    nanoSPI_Write_Read(spiDeviceHandle, wrc, data, length, NULL, 0);
    return;
}

#endif // _SPI_TO_DISPLAY_
