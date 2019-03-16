///*
//
//    glibScreenText.h
//
//    Interface class that allows the drawing
//    of text to the screen.
//
//    Derek Page Dec 3, 2007
//
//    edit: 8 / 9 / 08
//    edit: 8 / 14 / 08 - added the integer and float draws
//    moved to bro engine and modified standards 20140817
//    edit: 20151124 - remove old windows display list fonts (bye) added Stb raster fonts.
//    2018/2/18 shared_ptr replacement
//
//    *** TODO: multiple screen texts, types and definitions!!!!
//*/
//
//#pragma once
//#ifndef __GLIB__SCREEN__TEXT__H__
//#define __GLIB__SCREEN__TEXT__H__
//
//#include "../display/DisplayHeader.h"
//#include "../math/MathAll.h"
//
//namespace Game {;
///**
//*    @class GLScreenText
//*    @brief A text drawing class.
//*    @details A versatile text drawing class from windows system fonts. Source at nehe.gamedev.net .
//*/
//class TextBoss : VirtualMemory {
//public:
//    typedef std::set<std::shared_ptr<FontSpec>> FontSet;
//protected:
//    FontSet _fontSpecs;
//    vec4 _currentRenderColor;
//    t_string _currentFontName;
//    FontSize _iCurrentFontSize;
//    float _fCachedBakedFontSize;
//
//    std::shared_ptr<GLContext> _pContext = nullptr;
//public:
//    TextBoss(std::shared_ptr<GLContext> pc);
//    virtual ~TextBoss() override;
//
//    void drawTextBuffer(RenderParams& rp);    // - Draw all lines.
//    std::shared_ptr<FontSpec> getCurrentFont();
//    std::shared_ptr<FontSpec> getDefaultFont();
//    std::shared_ptr<FontSpec> getFontSpec(t_string& name);
//    vec4 getColor();
//    void setColor( float r, float g, float b, float a ) ;
//    void setColor(vec4&) ;
//    void setFont(t_string& fontName);
//    void setFontSize(FontSize sz) ;    // The size will "snap" to the requested size.
//    FontSize getFontSize()  { return _iCurrentFontSize; } ;
//    float fontSizeToFontScale(FontSize fontSize) ;
//    void createFont(t_string& userName, t_string& fileName);
//
//    //- Buffered Lines
//    void clearTextBuffer() ;    // - Clear the buffered line list.
//    void pstrb(t_string& format, int32_t X, int32_t Y, ...) ;// - Add a text line to the draw buffer
//    void pstrb(va_list args, t_string& format, int32_t X, int32_t Y) ;// - Add a text line to the draw buffer
//    void addVec3(int32_t X, int32_t Y, t_string& format, vec3& v) ;
//    void addVec3(int32_t X, int32_t Y, t_string& format, ivec3& v) ;
//    float getCenteredTextX(t_string text);
//    float getCenteredTextY(t_string text);
//};
//
//
//}
//
//#endif