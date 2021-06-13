//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _I2C_TO_TOUCHPANEL_H_
#define _I2C_TO_TOUCHPANEL_H_ 1

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include <esp32_os.h>
#include "TouchInterface.h"
#include "i2c.h"
#include "sys_dev_i2c_native_target.h"
#include "Debug_To_Display.h"

#define ACK_CHECK_EN  0x1 // I2C master will check ack from slave
#define ACK_CHECK_DIS 0x0 // I2C master will not check ack from slave
#define ACK_VAL       0x0 // I2C ack value
#define NACK_VAL      0x1 // I2C nack value

int I2cAddress;
i2c_port_t bus;

TouchInterface g_TouchInterface;

static esp_err_t __attribute__((unused))
i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size, int slaveAddress)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t __attribute__((unused))
i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t size, int slaveAddress)
{
    if (size == 0)
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaveAddress << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1)
    {
        i2c_master_read(cmd, data_rd, size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

bool TouchInterface::Initialize(TouchInterfaceConfig config)
//i2c_port_t busToUse, int touchI2cAddress)
{

    I2cAddress = config.I2c.address;
      //  touchI2cAddress;
    bus = (i2c_port_t)config.I2c.i2cBus;
        //busToUse;

    lcd_printf("TOUCH: Touch Interface initialized\n");

    return true;
}

bool Write(CLR_UINT8 *dataToSend, CLR_UINT16 numberOfValuesToSend)
{
    int ret = i2c_master_write_slave(bus, dataToSend, numberOfValuesToSend, I2cAddress);
    return (ret == ESP_OK);
}

bool TouchInterface::Write_Read(
    CLR_UINT8 *dataToSend,
    CLR_UINT16 numberOfValuesToSend,
    CLR_UINT8 *dataReturned,
    CLR_UINT16 numberValuesExpected)
{

    //int ret;
    //i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    int ret = i2c_master_write_slave(bus, dataToSend, numberOfValuesToSend, I2cAddress);

    if (ret != ESP_OK)
    {
        return false;
    }

    // Read response if expected
    if (numberValuesExpected > 0)
    {
        ret = i2c_master_read_slave(bus, dataReturned, numberValuesExpected, I2cAddress);
    }
    return (ret == ESP_OK);
}

#endif //_I2C_TO_TOUCHPANEL_H_
