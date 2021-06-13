//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _INITIALIZE_GRAPHICS_H_
#define _INITIALIZE_GRAPHICS_H_ 1

#include <target_board.h>
#include <nanoHAL_Graphics.h>
#include "Debug_To_Display.h"

#define I2C_MASTER_TX_BUF_DISABLE 0 // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 // I2C master doesn't need buffer

esp_err_t nanoI2C_Init(TouchInterfaceConfig config)
// i2c_port_t bus, int busSpeed)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)config.I2c.sda;
    conf.scl_io_num = (gpio_num_t)config.I2c.scl;
    conf.master.clk_speed = (config.I2c.fastMode == I2cBusSpeed_StandardMode) ? 100000 : 400000;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    esp_err_t err = i2c_param_config((i2c_port_t)config.I2c.i2cBus, &conf);
    if (err != ESP_OK)
    {
        return err;
    }
    lcd_printf("Graphics Initialized: Finish\n");

    return i2c_driver_install(
        (i2c_port_t)config.I2c.i2cBus,
        conf.mode,
        I2C_MASTER_RX_BUF_DISABLE,
        I2C_MASTER_TX_BUF_DISABLE,
        0);
}

extern "C"
{
    void InitializeGraphics()
    {
        g_GraphicsMemoryHeap.Initialize();

#if defined(ESP32_WROVER_KIT_V41)
        // Display
        DisplayInterfaceConfig displayConfig;
        displayConfig.Spi.spiBus = 1; // Index into array of pin values ( spiBus - 1) == 0
        displayConfig.Spi.chipSelect.pin = GPIO_NUM_22;
        displayConfig.Spi.chipSelect.type.activeLow = true;
        displayConfig.Spi.dataCommand.pin = GPIO_NUM_21;
        displayConfig.Spi.dataCommand.type.commandLow = true;
        displayConfig.Spi.reset.pin = GPIO_NUM_18;
        displayConfig.Spi.reset.type.activeLow = true;
        displayConfig.Spi.backLight.pin = GPIO_NUM_5;
        displayConfig.Spi.backLight.type.activeLow = true;
        g_DisplayInterface.Initialize(displayConfig);
        g_DisplayDriver.Initialize();

#elif defined(MAKERFAB_GRAPHICS_35)
        //Display
        DisplayInterfaceConfig displayConfig;
        displayConfig.Spi.spiBus = 2; // Index into array of pin values ( spiBus - 1) == 1
        displayConfig.Spi.chipSelect.pin = GPIO_NUM_15;
        displayConfig.Spi.chipSelect.type.activeLow = true;
        displayConfig.Spi.dataCommand.pin = GPIO_NUM_33;
        displayConfig.Spi.dataCommand.type.commandLow = true;
        displayConfig.Spi.reset.pin = IMPLEMENTED_IN_HARDWARE;
        displayConfig.Spi.reset.type.activeLow = true;
        displayConfig.Spi.backLight.pin = IMPLEMENTED_IN_HARDWARE;
        displayConfig.Spi.backLight.type.activeLow = true;
        g_DisplayInterface.Initialize(displayConfig);
        g_DisplayDriver.Initialize();

        // Touch
        TouchInterfaceConfig config;
        config.I2c.address = 0x38;
        config.I2c.i2cBus = I2C_NUM_0;
        config.I2c.fastMode = I2cBusSpeed_StandardMode;
        config.I2c.sda = GPIO_NUM_26;
        config.I2c.scl = GPIO_NUM_27;
        GPIO_PIN TouchInterruptPin = GPIO_NUM_2;
        nanoI2C_Init(config);
        g_TouchInterface.Initialize(config);
        g_TouchDevice.Initialize(TouchInterruptPin);
        g_GestureDriver.Initialize();
        g_InkDriver.Initialize();
        g_TouchPanelDriver.Initialize();

#elif defined(MAKERFAB_MAKEPYTHON)
        // Display
        DisplayInterfaceConfig displayConfig;
        displayConfig.Spi.spiBus = 2; // Index into array of pin values  ( spiBus - 1) == 1
        displayConfig.Spi.chipSelect.pin = GPIO_NUM_15;
        displayConfig.Spi.chipSelect.type.activeLow = true;
        displayConfig.Spi.dataCommand.pin = GPIO_NUM_22;
        displayConfig.Spi.dataCommand.type.commandLow = true;
        displayConfig.Spi.reset.pin = GPIO_NUM_21;
        displayConfig.Spi.reset.type.activeLow = true;
        displayConfig.Spi.backLight.pin = GPIO_NUM_5;
        displayConfig.Spi.backLight.type.activeLow = false;
        g_DisplayInterface.Initialize(displayConfig);
        g_DisplayDriver.Initialize();
#endif

        lcd_printf("Graphics Initialization: Finished\n");
    }
}

#endif //_INITIALIZE_GRAPHICS_H_
