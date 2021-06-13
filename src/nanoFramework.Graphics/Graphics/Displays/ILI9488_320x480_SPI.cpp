//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"
#include "InternalFont.h"

//   The ILI9488 is a 16.7M single-chip SoC driver for a-Si TFT liquid crystal display panels with a resolution of
//   320(RGB) x 480 dots. The ILI9488 is comprised of a 960-channel source driver, a 480-channel gate driver,
//   345,600 bytes GRAM for graphic data of 320 (RGB) x 480 dots, and power supply circuit.

//  Using the default endian order for transferring bytes Normal (MSB first, default)
//
//
// Assume the following defaults from the data sheet for the ILI9488
// __________________________________________________________________
//         REGISTER                   AFTER POWER ON
// ------------------------------------------------------------------
//      Frame Memory                    Random
//      Sleep                           In
//      Display Mode                    Normal
//      Display Status                  Display Off
//      Idle Mode                       Off
//      All Pixels Off(22h)             Display OFF
//      All Pixels On (23h)             Display OFF
//      Display Inversion(20h)          OFF
//      Column Start Address(2Ah)       0000  |
//      Column End Address(2Ah)         013F  | - 0 ... 319
//      Page Start Address(2Bh)         0000  |
//      Page End Address(2Bh)           01DF  | - 0 ... 479
//      Gamma Setting                   GC0
//      Partial Area Start(30h)         0000
//      Partial Area End(30h)           01DF
//      MADCTL(36h)                     00
//      RDNUMED(05h)                    00
//      RDDPM(0Ah)                      08
//      RDDMADCTL(0Bh)                  00
//      RDDCOLMOD(0Ch)                  06
//      RDDIM(0Dh)                      00
//      RDDSM(0Eh)                      00
//      RDDSDR(0Fh)                     00
//      Color Pixel Format(3Ah)         18 Bit/Pixel
//      TE Output Line(0X35)            Off
//      TE Line Mode(0X35)              Mode 1(V - Blanking Info only)
//      RDDISBV(52h)                    00
//      RDCTRLD(54h)                    00
//      RDCABC(56h)                     00
//      RDCABCMB(5Fh)                   00
// __________________________________________________________________

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum ILI9488_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    Display_ID = 0x04,
    Power_Mode = 0x0A,
    Sleep_In = 0x10,
    Sleep_Out = 0x11,
    Normal_Display_Mode_ON = 0x13,
    Display_Inversion_Control_OFF = 0x20,
    Gamma_Set = 0x26,
    Display_OFF = 0x28,
    Display_ON = 0x29,
    Column_Address_Set = 0x2A,
    Page_Address_Set = 0x2B,
    Memory_Write = 0x2C,
    Colour_Set = 0x2D,
    Memory_Read = 0x2E,
    Partial_Area = 0x30,
    Vertical_Scrolling_Definition = 0x33,
    Tearing_Effect_Line_OFF = 0x34,
    Tearing_Effect_Line_ON = 0x35,
    Memory_Access_Control = 0x36,
    Vertical_Scrolling_Start_Address = 0x37,
    Pixel_Format_Set = 0x3A,
    Memory_Write_Continue = 0x3C,
    Write_Display_Brightness = 0x51,

    Interface_Mode_Control = 0xB0,
    Frame_Rate_Control_Normal = 0xB1, // Frame Rate Control (In Normal Mode/Full Colors)
    Display_Inversion_Control = 0xB4,

    Display_Function_Control = 0xB6,
    Entry_Mode_Set = 0xB7,
    Power_Control_1 = 0xC0, // ILI9488_PWCTR1
    Power_Control_2 = 0xC1, // ILI9488_PWCTR2

    ILI9488_PWCTR3 = 0xC2, // ILI9488_PWCTR3 (Power Control 3 (For Normal Mode))
    ILI9488_PWCTR4 = 0xC3, // ILI9488_PWCTR4 (Power Control 4 (For Idle Mode))
    ILI9488_PWCTR5 = 0xC4, // ILI9488_PWCTR5 (Power Control 5 (For Partial Mode)

    VCOM_Control_1 = 0xC5,  // ILI9488_VMCTR1
    VCOM_Control_2 = 0xC7,  // ILI9488_VMCTR2
    Power_Control_A = 0xCB, //?
    Power_Control_B = 0xCF, //?

    READ_ID1 = 0xDA,
    READ_ID2 = 0xDB,
    READ_ID3 = 0xDC,

    Positive_Gamma_Correction = 0xE0,
    Negative_Gamma_Correction = 0XE1,
    Driver_Timing_Control_A = 0xE8,
    Set_Image_Function = 0xE9,
    Driver_Timing_Control_B = 0xEA,
    Power_On_Sequence = 0xED,
    Enable_3G = 0xF2,
    Adjust_Control_3 = 0xF7,
    Adjust_Control_6 = 0xFC
};

enum ILI9488_Orientation : CLR_UINT8
{
    MADCTL_MH = BIT(2),  // bitmask for Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_BGR = BIT(3), // bitmask for RGB/BGR order
    MADCTL_ML = BIT(4),  // bitmask for Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = BIT(5),  // bitmask for Row/Column Exchange
    MADCTL_MX = BIT(6),  // bitmask for column address order
    MADCTL_MY = BIT(7)   // bitmask for Row Address Order
};

bool DisplayDriver::Initialize()
{
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // + IMPORTANT NOTE:                                                                                    +
    // + SPI interface does not support 16 bits (2 bytes), only 18-bits or 24-bits, 3 bytes                 +
    // +                 16-bit only supported in parallel 8/16 and maybe DSI interface                     +
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(3, Power_Control_1, 0x17, 0x15);      // VReg1Out
    g_DisplayInterface.SendCommand(2, Power_Control_2, 0x41);            // VGH,VGL
    g_DisplayInterface.SendCommand(4, VCOM_Control_1, 0x00, 0x12, 0x80); // Vcom
    g_DisplayInterface.SendCommand(2, Entry_Mode_Set, 0x06);
    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x66);       // 0x66 == 18-bit pixel
    g_DisplayInterface.SendCommand(2, Interface_Mode_Control, 0x80); // 4-wire SPI interface doesn't use the SDO pin
    //   g_DisplayInterface.SendCommand(3, Frame_Rate_Control_Normal, 0x0D, 0x01); // 90Hz
    g_DisplayInterface.SendCommand(2, Frame_Rate_Control_Normal, 0xA0);            // 60Hz
    g_DisplayInterface.SendCommand(2, Display_Inversion_Control, 0x02);            // 2-dot
    g_DisplayInterface.SendCommand(4, Display_Function_Control, 0x02, 0x02, 0x3B); // MCU - Source, Gate
    g_DisplayInterface.SendCommand(1, Display_Inversion_Control_OFF);

    g_DisplayInterface.SendCommand(2, Set_Image_Function, 0x00);                 // Disable 24 bit data
    g_DisplayInterface.SendCommand(5, Adjust_Control_3, 0xA9, 0x51, 0x2C, 0x82); // 82 or 02 // D7 stream, loose
    g_DisplayInterface.SendCommand(
        16,
        Positive_Gamma_Correction,
        0x00,
        0x03,
        0x09,
        0x08,
        0x16,
        0x0A,
        0x3F,
        0x78,
        0x4C,
        0x09,
        0x0A,
        0x08,
        0x16,
        0x1A,
        0x0F);
    g_DisplayInterface.SendCommand(
        16,
        Negative_Gamma_Correction,
        0x00,
        0x16,
        0x19,
        0x03,
        0x0F,
        0x05,
        0x32,
        0x45,
        0x46,
        0x04,
        0x0E,
        0x0D,
        0x35,
        0x37,
        0x0F);

    SetDefaultOrientation();

    g_DisplayInterface.SendCommand(1, Sleep_Out);
    OS_DELAY(120); // Exit Sleep
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(12); // Display On

    //   g_DisplayInterface.SendCommand(1, Normal_Display_Mode_ON);

    Clear();

    return true;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 480;
    Attributes.ShorterSide = 320;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16; // But sent out as 18 bits due
                                  // to the ILI9488 only supporting
                                  // 18-bit or 8-bit with SPI interface
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
            g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MY | MADCTL_ML | MADCTL_BGR | MADCTL_MH));
            break;
        case PORTRAIT180:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MX | MADCTL_BGR));
            break;
        case LANDSCAPE:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(
                2,
                Memory_Access_Control,
                (MADCTL_MY | MADCTL_MX | MADCTL_MV | MADCTL_ML | MADCTL_BGR | MADCTL_MH));
            break;
        case LANDSCAPE180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MV | MADCTL_ML | MADCTL_BGR | MADCTL_MH));
            break;
    }

    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(LANDSCAPE);
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
            // Illegal fall through to Power on
        case PowerSaveState::NORMAL:
            //        g_DisplayInterface.SendCommand(3, POWER_STATE, 0x00, 0x00); // leave sleep mode
            break;
        case PowerSaveState::SLEEP:
            //       g_DisplayInterface.SendCommand(3, POWER_STATE, 0x00, 0x01); // enter sleep mode
            break;
    }
    return;
}

void DisplayDriver::Clear()
{
    // Clear the ILI9488 controller frame
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    // Clear buffer
    memset(Attributes.TransferBuffer, 0, Attributes.TransferBufferSize);

    int totalBytesToClear = Attributes.Width * Attributes.Height * 3;
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

    CLR_UINT8 Page_Address_Set_Data[4];
    Page_Address_Set_Data[0] = (y1 >> 8) & 0xFF;
    Page_Address_Set_Data[1] = y1 & 0xFF;
    Page_Address_Set_Data[2] = (y2 >> 8) & 0xFF;
    Page_Address_Set_Data[3] = y2 & 0xFF;
    g_DisplayInterface.SendCommand(
        5,
        Page_Address_Set,
        Page_Address_Set_Data[0],
        Page_Address_Set_Data[1],
        Page_Address_Set_Data[2],
        Page_Address_Set_Data[3]);
    return true;
}

void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{

    ASSERT((x >= 0) && ((x + width) <= Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= Attributes.Height));

    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 *StartOfLine_src = (CLR_UINT16 *)&data[0];

    // Position to offset in data[] for start of window
    CLR_UINT16 offset = (y * Attributes.Width) + x;
    StartOfLine_src += offset;

    CLR_UINT8 *transferBufferIndex = Attributes.TransferBuffer;
    CLR_UINT32 transferBufferCount = Attributes.TransferBufferSize;
    CLR_UINT8 command = Memory_Write;

    while (height--)
    {
        CLR_UINT16 *src;
        int xCount;

        src = StartOfLine_src;
        xCount = width;

        while (xCount--)
        {
            // Convert internal 2 bytes RGB565 to supported SPI format for ILI9488 RGB666
            // RRRRRGGG,GGGBBBBB to  RRRRRR--,GGGGGG--,BBBBBB--
            CLR_UINT16 data = *src++;
            *transferBufferIndex++ = (data >> 8) & 0XF8;
            *transferBufferIndex++ = (data >> 3) & 0XFC;
            *transferBufferIndex++ = (data << 3) & 0XFC;

            transferBufferCount -= 3;

            // Send over SPI if no room for another 3 bytes
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
    CLR_UINT16 numberOfBytes = width * height * 3; // 3 bytes per pixel
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

    CLR_UINT8 fontdata[8 * 8 * 3]; // Supported SPI format for ILI9488 RGB666 (3 bytes)
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
            // the font data is mirrored
            // if ((font[y] & (1 << (width - x - 1))) != 0)
            CLR_UINT8 ff = font[y];
            int x1 = (1 << (width - x - 1));

            if ((ff & x1) != 0)
            {
                // Text Colour = > all bits one's - should be white
                fontdata[i] = 0xFF;
                i++;
                fontdata[i] = 0xFF;
                i++;
                fontdata[i] = 0xFF;
            }
            else
            {
                // background  => all bits zero - should be black
                fontdata[i] = 0x00;
                i++;
                fontdata[i] = 0x00;
                i++;
                fontdata[i] = 0x00;
            }
            i++;
        }
    }

    SendDataDirect(row, col, width, height, (CLR_UINT16 *)fontdata);

} // done

