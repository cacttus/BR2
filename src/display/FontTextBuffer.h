/**
*
*    @file FontTextBuffer.h
*    @date September 28, 2014
*    @author Derek Page
*
*    © 2014
*    06 30 2016 added scale to fix font systme
*
*/
#pragma once
#ifndef __FONTTEXTBUFFER_1778616551277491261427268_H__
#define __FONTTEXTBUFFER_1778616551277491261427268_H__

#include "../display/DisplayHeader.h"
#include "../math/MathAll.h"
namespace Game {;
/**
*    @class FontTextBuffer
*    @brief
*/
class FontTextBuffer : public VirtualMemory {
private:
    t_string _strText;
    FontSize _iFontSize = 16;
    vec2 _pos;
    t_string _fontName;    // Pointer to the name of the font.
    vec4 _fontColor;

public:
    FontTextBuffer(t_string& txt, int32_t x, int32_t y, Color4f& cFont, t_string fontName, FontSize fontsize);
    virtual ~FontTextBuffer() override;
    vec4 getFontColor() { return _fontColor; }
    t_string getFontName() { return _fontName; }
    t_string getText() { return _strText; }
    vec2 getPos() { return _pos; }
    FontSize getFontSize() { return _iFontSize; }
};

}//ns game



#endif
