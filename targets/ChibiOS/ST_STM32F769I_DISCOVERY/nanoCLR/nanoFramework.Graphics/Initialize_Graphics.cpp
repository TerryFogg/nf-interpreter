//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _INITIALIZE_GRAPHICS_H_
#define _INITIALIZE_GRAPHICS_H_ 1

#include <nanoCLR_Headers.h>
#include <target_board.h>
#include <nanoHAL_Graphics.h>
#include "Debug_To_Display.h"

extern "C"
{
    void InitializeGraphics()
    {
        g_GraphicsMemoryHeap.Initialize();
        DisplayInterfaceConfig config; // not used for DSI display
        g_DisplayInterface.Initialize(config);
        g_DisplayDriver.Initialize();

        // Touch
        //TouchInterfaceConfig config;
        //config.I2c.address = 0x38;
        //config.I2c.i2cBus = I2C_NUM_0;
        //config.I2c.fastMode = I2cBusSpeed_StandardMode;
        //config.I2c.sda = GPIO_NUM_26;
        //config.I2c.scl = GPIO_NUM_27;
        //GPIO_PIN TouchInterruptPin = GPIO_NUM_0;
        //nanoI2C_Init(config);
        //g_TouchInterface.Initialize(config);
        //g_TouchDevice.Initialize(TouchInterruptPin);
        //g_GestureDriver.Initialize();
        //g_InkDriver.Initialize();
        //g_TouchPanelDriver.Initialize();

        lcd_printf("Graphics Initialization: Finished\n");
    }

}

#endif //_INITIALIZE_GRAPHICS_H_
