/**
*    @file UiBase.h
*    @date February 17, 2018
*    @author MetalMario971
*/
#pragma once
#ifndef __UIBASE_15188874021299537968_H__
#define __UIBASE_15188874021299537968_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../math/MathAll.h"
#include "../base/PoundFile.h"


namespace Game {
class Orientation {
public:
    typedef enum { Horizontal, Vertical } e; 
    static Orientation::e parse(std::string str); 
    static std::string toString(Orientation::e eOr); 
};

namespace GridBorder { typedef enum { Top, Right, Bot, Left } e; }

class UiDragInfo {
public:
    typedef std::function<void(vec2& dxy)> DragFunc;
    bool _bDragStart = false;
    bool _bDrag = false;
    vec2 _vDragStart;
    //vec2 _vPosStartPx;
    // Box2f _b2StartBox;//Computed box (design space) of starting position
    DragFunc _func;
    void update(std::shared_ptr<InputManager> pInput);
};
class UiParser {
public:
    static bool parseSize(std::string cssSize, float& __out_ size, UiDimUnit::e& __out_ unit);
    static UiDimUnit::e parseLayoutUnit(std::string str, bool& bSuccess);
};
class UiDesignSize : public VirtualMemory {
    float _iWidthPixels = 1920;
    float _iHeightPixels = 1080;
public:
    UiDesignSize() {}
    UiDesignSize(float w, float h) {
        _iWidthPixels = w;
        _iHeightPixels = h;
    }
    virtual ~UiDesignSize() { }
    float getHeight()const { return _iHeightPixels; }
    float getWidth() const { return _iWidthPixels; }
};
class uDim : public VirtualMemory {
    //we could store the percentage as _val, however we would still need to know if it's a width or heigth percentage.
    //Further percentages only apply to widths, heights.
private:
    UiDimUnit::e _eDimUnit = UiDimUnit::e::Pixel; //Auto - size is automatically set (for w/h) and handled by parent.  Invalid for _top/_left/_bot/_right.
    float _units = 0.0f;//Note: for %, this is in the range [0, 100]
    virtual void setSize(std::string str);
public:
    uDim() { }
    uDim(std::string cssSize);
    uDim(char* cssSize);
    uDim(float val, UiDimUnit::e eUnit);
    virtual ~uDim() { }
    uDim& operator=(std::string str);
    uDim& operator=(char* str);
    uDim& operator=(float f);
    uDim& operator=(int f);
    uDim& operator=(const uDim& rhs);
    float pctOf(float f);
    float px() const { return _units; }
    float px(float p) {
        _eDimUnit = UiDimUnit::e::Pixel;
        _units = p;
        return _units;
    }
    //float invalid() { return FLT_MAX; }
    UiDimUnit::e getDimUnit() { return _eDimUnit; }
    void setDimUnit(UiDimUnit::e e) { _eDimUnit = e; }
    uDim autoHeight(std::shared_ptr<UiTex> tex);
    uDim autoWidth(std::shared_ptr<UiTex> tex);
    void debugThunk();
};
class UiBorderDim {
public:
    static std::shared_ptr<UiBorderDim> create(uDim d0, uDim d1, uDim d2, uDim d3);
    uDim _border[4];
    void set(uDim d) {
        _border[0] = d;
        _border[1] = d;
        _border[2] = d;
        _border[3] = d;
    }
    uDim& get(GridBorder::e eBorder) { return _border[eBorder]; }

    //void set(uDim d0, uDim d1, uDim d2, uDim d3) {
    //    _border[0] = _border[1] = _border[2] = _border[3] = d;
    //}
};

}//ns Game



#endif
