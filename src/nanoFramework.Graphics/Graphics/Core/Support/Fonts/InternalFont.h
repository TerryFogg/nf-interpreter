
#ifndef _INTERNALFONT_H_
#define _INTERNALFONT_H_ 1

 #include "nanoCLR_Types.h"

struct InternalFont
{
    static CLR_UINT8* Font_GetGlyph(unsigned char c);
    static CLR_INT16  Font_Width(void);
    static CLR_INT16  Font_Height(void);
    static CLR_INT16  Font_TabWidth(void);
};


#endif
