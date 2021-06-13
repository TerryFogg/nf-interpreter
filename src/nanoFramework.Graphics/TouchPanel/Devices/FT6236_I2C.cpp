//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// For a  FT6236 by Focal Tech.

#ifndef _ft6236_H_
#define _ft6236_H_ 1

#include "TouchDevice.h"
#include "TouchInterface.h"
#include "Debug_To_Display.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

extern TouchPanelDriver g_TouchPanelDriver;

enum FT6236_CMD : CLR_UINT8
{
    DEV_MODE = 0x00,
    GEST_ID = 0x01,
    TD_STATUS = 0x02,
    P1_XH = 0x03,
    P1_XL = 0x04,
    P1_YH = 0x05,
    P1_YL = 0x06,
    P1_WEIGHT = 0x07,
    P1_MISC = 0x08,
    P2_XH = 0x09,
    P2_XL = 0x0A,
    P2_YH = 0x0B,
    P2_YL = 0x0C,
    P2_WEIGHT = 0x0D,
    P2_MISC = 0x0E,
    TH_GROUP = 0x80,
    TH_DIFF = 0x85,
    CTRL = 0x86,
    TIMEENTERMONITOR = 0x87,
    PERIODACTIVE = 0x88,
    PERIODMONITOR = 0x89,
    RADIAN_VALUE = 0x91,
    OFFSET_LEFT_RIGHT = 0x92,
    OFFSET_UP_DOWN = 0x93,
    DISTANCE_LEFT_RIGHT = 0x94,
    DISTANCE_UP_DOWN = 0x95,
    DISTANCE_ZOOM = 0x96,
    LIB_VER_H = 0xA1,
    LIB_VER_L = 0xA2,
    CIPHER = 0xA3,
    G_MODE = 0xA4,
    PWR_MODE = 0xA5,
    FIRMID = 0xA6,
    FOCALTECH_ID = 0xA8,
    RELEASE_CODE_ID = 0xAF,
    STATE = 0xBC,
};

enum FT6236_VALUES : CLR_UINT8
{
    ID_VALUE = 0x11,
    G_MODE_INTERRUPT_TRIGGER = 0x01,
    G_MODE_INTERRUPT_POLLING = 0x00,
    G_MODE_INTERRUPT_MASK = 0x03,
    G_MODE_INTERRUPT_SHIFT = 0x00,
    MSB_MASK = 0x0F,
    LSB_MASK = 0xFF
};

static GPIO_PIN InterruptPin;

// For T6202
// It is important to note that the SDA and SCL must connect with a pull-high resistor respectively before you
// read/write I2C data
//
bool TouchDevice::Initialize(GPIO_PIN TouchInterruptPin)
{
    InterruptPin = TouchInterruptPin;
    CLR_UINT8 chipIdRegister = FT6236_CMD::FOCALTECH_ID;
    CLR_UINT8 id;
    g_TouchInterface.Write_Read(&chipIdRegister, 1, &id, 1);
    bool result = (id == ID_VALUE);
    lcd_printf("TOUCH: Touch Device detected ID is %d\n", id);

    return result;
}

bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
    // Connect the Interrupt Service routine to the FT6236 interrupt pin

    if (CPU_GPIO_ReservePin(InterruptPin, true))
    {
        if (CPU_GPIO_EnableInputPin(InterruptPin, 0, touchIsrProc, NULL, GPIO_INT_EDGE_LOW, GpioPinDriveMode_Input))
        {
            // Configure the FT6236 device to generate interrupt trigger on given interrupt pin connected to MCU
            uint8_t regValue = 0;
            regValue = FT6236_VALUES::G_MODE_INTERRUPT_TRIGGER;
            CLR_UINT8 values[2] = {FT6236_CMD::G_MODE, regValue};
            g_TouchInterface.Write_Read(values, 2, NULL, 0);

            lcd_printf("TOUCH DEVICE: Interrupt service routine enabled for GPIO%d\n", InterruptPin);
            OS_DELAY(1000);
        }
    }
    return TRUE;
}

bool TouchDevice::Disable()
{
    // Configure the FT6236 device to stop generating IT on the given INT pin connected to MCU as EXTI.
    CLR_UINT8 regValue = FT6236_VALUES::G_MODE_INTERRUPT_POLLING;
    CLR_UINT8 values[2] = {FT6236_CMD::G_MODE, regValue};
    g_TouchInterface.Write(values, 2);

    lcd_printf("TOUCH DEVICE: Touch device disabled");

    return true;
}

TouchPointDevice TouchDevice::GetPoint()
{
    TouchPointDevice t;
    CLR_UINT8 regAddress = P1_XH;
    CLR_UINT8 dataxy[4];
    CLR_UINT16 numberValuesExpected = 4;

    g_TouchInterface.Write_Read(&regAddress, 1, dataxy, numberValuesExpected);

    t.x = ((dataxy[0] & 0x0F) << 8) | dataxy[1];
    t.y = ((dataxy[2] & 0x0F) << 8) | (dataxy[3]);

    lcd_printf("[%4d,%4d]                       \n", t.x, t.y);

    return t;
}

#endif // _ft6236_H_
