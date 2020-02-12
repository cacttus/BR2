/**
*  @file FontTextBuffer.h
*  @date September 28, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __FONTTEXTBUFFER_1778616551277491261427268_H__
#define __FONTTEXTBUFFER_1778616551277491261427268_H__

#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"
namespace BR2 {
/**
*    @class FontTextBuffer
*    @brief
*/
class FontTextBuffer : public VirtualMemory {
private:
    string_t _strText;
    FontSize _iFontSize = 16;
    vec2 _pos;
    string_t _fontName;    // Pointer to the name of the font.
    vec4 _fontColor;

public:
    FontTextBuffer(string_t& txt, int32_t x, int32_t y, Color4f& cFont, string_t fontName, FontSize fontsize);
    virtual ~FontTextBuffer() override;
    vec4 getFontColor() { return _fontColor; }
    string_t getFontName() { return _fontName; }
    string_t getText() { return _strText; }
    vec2 getPos() { return _pos; }
    FontSize getFontSize() { return _iFontSize; }
};

}//ns game



#endif
