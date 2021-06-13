//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _TOUCHINTERFACE_H_
#define _TOUCHINTERFACE_H_ 1

#include "nanoCLR_Types.h"
#include "sys_dev_i2c_native.h"

#define NOT_IMPLEMENTED 0

struct TouchControlPin
{
    GPIO_PIN pin;
    union {
        bool activeLow;
        bool commandLow;
    } type;
};

// Touch interfaces usually use SPI or I2C
union TouchInterfaceConfig {
    struct
    {
        CLR_INT8 spiBus;
        TouchControlPin dataCommand;
        TouchControlPin chipSelect;
        TouchControlPin reset;
    } Spi;
    struct
    {
        CLR_INT8 i2cBus;
        GPIO_PIN sda;
        GPIO_PIN scl;
        CLR_INT8 address;
        I2cBusSpeed fastMode;
    } I2c;
};



struct TouchInterface
{
  public:
    bool Initialize(TouchInterfaceConfig config );
    bool Write(CLR_UINT8 *dataToSend, CLR_UINT16 numberOfValuesToSend);
    bool Read(CLR_UINT8 *dataReturned, CLR_UINT16 numberValuesExpected);
    bool Write_Read(
        CLR_UINT8 *dataToSend,
        CLR_UINT16 numberOfValuesToSend,
        CLR_UINT8 *dataReturned,
        CLR_UINT16 numberValuesExpected);
};

#endif // _TOUCHINTERFACE_H_
