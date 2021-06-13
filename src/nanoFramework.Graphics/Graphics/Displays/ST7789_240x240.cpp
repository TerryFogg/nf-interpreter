//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"
#include "InternalFont.h"

/*
ST7789 is a 262,144-color single-chip SOC driver for a-TFT liquid crystal display with resolution of 240RGBx320
dots, comprising a 720-channel source driver, a 320-channel gate driver, 172,800 bytes GRAM for graphic
display data of 240RGBx320 dots, and power supply circuit.


NOTE: This code is a display for 240x240 which does not match the controller 240x320
      For PORTRAIT and LANDSCAPE180 there are no problems because the remaining (320-240), 80 pixels are not displayed
      For Portrait180 and LANDSCAPE the (320-240), 80 Pixels are at the start and are visible.
      This causes an 80 pixel offset that must be programmed for.
*/

/*
 Using the default endian order for transferring bytes
 Normal (MSB first, default)
  */

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum ST7789_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    Sleep_IN = 0x10,
    Sleep_OUT = 0x11,
    ST7789_PTLON = 0x12,
    Normal_Display_On = 0x13,
    Display_Inversion_Off = 0x20,
    Display_Inversion_On = 0x21,
    Display_OFF = 0x28,
    Display_ON = 0x29,
    Column_Address_Set = 0x2A,
    Row_Address_Set = 0x2B,
    Memory_Write = 0x2C,
    Memory_Read = 0x2E,
    Partial_Area = 0x30,
    Memory_Access_Control = 0x36,
    Pixel_Format_Set = 0x3A,
    Memory_Write_Continue = 0x3C,
    Write_Display_Brightness = 0x51,
    Porch_Setting = 0xB2,
    Gate_Control = 0xB7,
    VCOMS_Setting = 0xBB,
    LCM_Control = 0xC0,
    VDV_VRH_Command_Enable = 0xC2,
    VRH_Set = 0xC3,
    VDV_Set = 0xC4,
    Frame_Rate_Control = 0xC6,
    Power_Control_1 = 0xD0,
    Positive_Voltage_Gamma = 0xE0,
    Negative_Voltage_Gamma = 0xE1,
};
enum ST7789_Orientation : CLR_UINT8
{
    MADCTL_MH = BIT(2),     // bitmask for Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_BGR = BIT(3),    // bitmask for RGB/BGR order
    MADCTL_ML = BIT(4),     // bitmask for Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = BIT(5),     // bitmask for page/column order
    MADCTL_MX = BIT(6),     // bitmask for column address order
    MADCTL_MY = BIT(7)      // bitmask for page address order
};

bool DisplayDriver::Initialize()
{
    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x55);
    g_DisplayInterface.SendCommand(6, Porch_Setting, 0x0c, 0x0c, 0x00, 0x33, 0x33);

    /* Internal LCD Voltage generator settings */
    g_DisplayInterface.SendCommand(2, Gate_Control, 0x35);
    g_DisplayInterface.SendCommand(2, VCOMS_Setting, 0x19);
    g_DisplayInterface.SendCommand(2, LCM_Control, 0x2C);
    g_DisplayInterface.SendCommand(2, VDV_VRH_Command_Enable, 0x01);
    g_DisplayInterface.SendCommand(2, VRH_Set, 0x12);
    g_DisplayInterface.SendCommand(2, VDV_Set, 0x20);

    g_DisplayInterface.SendCommand(2, Frame_Rate_Control, 0x0F);
    g_DisplayInterface.SendCommand(3, Power_Control_1, 0xA4, 0xA1);
    g_DisplayInterface.SendCommand(
        15,
        Positive_Voltage_Gamma,
        0xD0,
        0x04,
        0x0D,
        0x11,
        0x13,
        0x2B,
        0x3F,
        0x54,
        0x4C,
        0x18,
        0x0D,
        0x0B,
        0x1F,
        0x23);
    g_DisplayInterface.SendCommand(
        15,
        Negative_Voltage_Gamma,
        0xD0,
        0x04,
        0x0C,
        0x11,
        0x13,
        0x2C,
        0x3F,
        0x44,
        0x51,
        0x2F,
        0x1F,
        0x1F,
        0x20,
        0x23);

    SetDefaultOrientation();

    g_DisplayInterface.SendCommand(1, Display_Inversion_On);
    g_DisplayInterface.SendCommand(1, Sleep_OUT);
    g_DisplayInterface.SendCommand(1, Normal_Display_On);
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(50);
    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
    OS_DELAY(20);

    g_DisplayInterface.DisplayBacklight(true);

    Clear();

    return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 240;
    Attributes.ShorterSide = 240;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}
bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
    case PORTRAIT:
        Attributes.Height = Attributes.LongerSide;
        Attributes.Width = Attributes.ShorterSide;
        g_DisplayInterface.SendCommand(2, Memory_Access_Control,0);
        break;
    case PORTRAIT180:
        // Set NOTE, in comments above for explanation of adjustments for PORTRAIT180 and LANDSCAPE
        // Currently not fixed
        Attributes.Height = Attributes.LongerSide;
        Attributes.Width = Attributes.ShorterSide;
        g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MX | MADCTL_MY ));
        break;
    case LANDSCAPE:
        // Set NOTE, in comments above for explanation of adjustments for PORTRAIT180 and LANDSCAPE
        // Currently not fixed
        Attributes.Height = Attributes.ShorterSide;
        Attributes.Width = Attributes.LongerSide;
        g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MV | MADCTL_MY ));
        break;
    case LANDSCAPE180:
        Attributes.Height = Attributes.ShorterSide;
        Attributes.Width = Attributes.LongerSide;
        g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MV | MADCTL_MX ));
        break;
    }
    // Note: colstart/rowstart not working 
   // SetWindow(colstart, rowstart, Attributes.Width - 1, Attributes.Height - 1);
    return true;
}
void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(PORTRAIT);
}
bool DisplayDriver::Uninitialize()
{
    Clear();
    // Anything else to Uninitialize?
    return TRUE;
}
void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    switch (powerState)
    {
    default:
        // illegal fall through to Power on
    case PowerSaveState::NORMAL:
        g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x00); // leave sleep mode
        break;
    case PowerSaveState::SLEEP:
        g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x01); // enter sleep mode
        break;
    }
    return;
}
void DisplayDriver::Clear()
{
    // Clear the ILI9341 controller frame
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    // Clear buffer
    memset(Attributes.TransferBuffer, 0, Attributes.TransferBufferSize);

    int totalBytesToClear = Attributes.Width * Attributes.Height * 2;
    int fullTransferBuffersCount = totalBytesToClear / Attributes.TransferBufferSize;
    int remainderTransferBuffer = totalBytesToClear % Attributes.TransferBufferSize;

    CLR_UINT8 command = Memory_Write;
    for (int i = 0; i < fullTransferBuffersCount; i++)
    {
        g_DisplayInterface.WriteToFrameBuffer(command, Attributes.TransferBuffer, Attributes.TransferBufferSize);
        command = Memory_Write_Continue;
    }

    if (remainderTransferBuffer > 0)
    {
        g_DisplayInterface.WriteToFrameBuffer(command, Attributes.TransferBuffer, remainderTransferBuffer);
    }
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    g_DisplayInterface.SendCommand(2, Write_Display_Brightness, (CLR_UINT8)brightness);
    return;
}
bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    CLR_UINT8 Column_Address_Set_Data[4];
    Column_Address_Set_Data[0] = (x1 >> 8) & 0xFF;
    Column_Address_Set_Data[1] = x1 & 0xFF;
    Column_Address_Set_Data[2] = (x2 >> 8) & 0xFF;
    Column_Address_Set_Data[3] = x2 & 0xFF;
    g_DisplayInterface.SendCommand(
        5,
        Column_Address_Set,
        Column_Address_Set_Data[0],
        Column_Address_Set_Data[1],
        Column_Address_Set_Data[2],
        Column_Address_Set_Data[3]);

    CLR_UINT8 Row_Address_Set_Data[4];
    Row_Address_Set_Data[0] = (y1 >> 8) & 0xFF;
    Row_Address_Set_Data[1] = y1 & 0xFF;
    Row_Address_Set_Data[2] = (y2 >> 8) & 0xFF;
    Row_Address_Set_Data[3] = y2 & 0xFF;
    g_DisplayInterface.SendCommand(
        5,
        Row_Address_Set,
        Row_Address_Set_Data[0],
        Row_Address_Set_Data[1],
        Row_Address_Set_Data[2],
        Row_Address_Set_Data[3]);
    return true;
}
void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{
    // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((x >= 0) && ((x + width) <= Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= Attributes.Height));

    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16* StartOfLine_src = (CLR_UINT16*)&data[0];

    // Position to offset in data[] for start of window
    CLR_UINT16 offset = (y * Attributes.Width) + x;
    StartOfLine_src += offset;

    CLR_UINT8* transferBufferIndex = Attributes.TransferBuffer;
    CLR_UINT32 transferBufferCount = Attributes.TransferBufferSize;
    CLR_UINT8 command = Memory_Write;

    while (height--)
    {
        CLR_UINT16* src;
        int xCount;

        src = StartOfLine_src;
        xCount = width;

        while (xCount--)
        {
            CLR_UINT16 data = *src++;
            // Swap bytes
            *transferBufferIndex++ = (data >> 8);
            *transferBufferIndex++ = data & 0xff;
            transferBufferCount -= 2;

            // Send over SPI if no room for another 2 bytes
            if (transferBufferCount < 1)
            {
                // Transfer buffer full, send it
                g_DisplayInterface.WriteToFrameBuffer(
                    command,
                    Attributes.TransferBuffer,
                    (Attributes.TransferBufferSize - transferBufferCount));

                // Reset transfer ptrs/count
                transferBufferIndex = Attributes.TransferBuffer;
                transferBufferCount = Attributes.TransferBufferSize;
                command = Memory_Write_Continue;
            }
        }

        // Next row in data[]
        StartOfLine_src += Attributes.Width;
    }

    // Send remaining data in transfer buffer to SPI
    if (transferBufferCount < Attributes.TransferBufferSize)
    {
        // Transfer buffer full, send it
        g_DisplayInterface.WriteToFrameBuffer(
            command,
            Attributes.TransferBuffer,
            (Attributes.TransferBufferSize - transferBufferCount));
    }

    return;
}

void DisplayDriver::SendDataDirect(CLR_INT16 x, CLR_INT16 y, CLR_INT16 width, CLR_INT16 height, CLR_UINT16 data[])
{
    CLR_UINT16 numberOfBytes = width * height * 2; // 2 bytes per pixel
    SetWindow(x, y, (x + width - 1), (y + height - 1));
    g_DisplayInterface.WriteToFrameBuffer(Memory_Write, (CLR_UINT8 *)&data[0], numberOfBytes);
    return;
}

CLR_INT16 DisplayDriver::PixelsPerWord()
{
    return (32 / Attributes.BitsPerPixel);
}
CLR_INT16 DisplayDriver::WidthInWords()
{
    return ((Attributes.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
}
CLR_INT16 DisplayDriver::SizeInWords()
{
    return (WidthInWords() * Attributes.Height);
}
CLR_INT16 DisplayDriver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
}

void DisplayDriver::WriteChar(unsigned char c, int row, int col)
{
    CLR_UINT16 fontdata[8 * 8];
    int width = 8;
    int height = 8;

    // convert to LCD pixel coordinates
    row *= width;
    col *= height;

    if (row > (Attributes.Width - width))
        return;
    if (col > (Attributes.Height - height))
        return;

    const CLR_UINT8 *font = InternalFont::Font_GetGlyph(c);
    int i = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            CLR_UINT16 val;
            // the font data is mirrored
            // if ((font[y] & (1 << (width - x - 1))) != 0)
            CLR_UINT8 ff = font[y];
            int x1 = (1 << (width - x - 1));

            if ((ff & x1) != 0)
            {
                val = 0xFFFF; // Text Colour => all bits one's - should be white
            }
            else
            {
                val = 0x0000; // background  => all bits zero - should be black
            }
            fontdata[i] = val;
            i++;
        }
    }

    SendDataDirect(row, col, width, height, fontdata);

} // done
