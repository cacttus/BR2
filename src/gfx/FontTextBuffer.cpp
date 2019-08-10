#include "../gfx/FontTextBuffer.h"


namespace Game {

FontTextBuffer::FontTextBuffer(t_string& txt , int32_t x, int32_t y, vec4& cFont, t_string fontName, FontSize iFontSize){
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



}//ns game
