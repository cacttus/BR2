#include "../gfx/FontTextBuffer.h"


namespace BR2 {

FontTextBuffer::FontTextBuffer(string_t& txt , int32_t x, int32_t y, vec4& cFont, string_t fontName, FontSize iFontSize){
    _strText = txt;
    _pos.x = (float)x;
    _pos.y = (float)y;
    _fontColor = cFont;
    _fontName = fontName;
    _iFontSize = iFontSize;
}
FontTextBuffer::~FontTextBuffer() 
{
}



}//ns BR2
