/**
*  @file TouchInfo.h
*  @date March 7, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __TOUCHINFO_14889145022426931010_H__
#define __TOUCHINFO_14889145022426931010_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
/**
*    @class TouchInfo
*    @brief
*   extedn this class to hold player touch info for the world
*/
class TouchInfo : public VirtualMemory {
public:
    ButtonState::e _eState;
    vec2 _vMousePosPress;
    vec3 _vWorldPosPress; //position pressed
    t_timeval _iPress = -1; //prss time us
    vec2 _vMousePosRelease;
    vec3 _vWorldPosRelease; //position pressed
    t_timeval _iRelease = -1; // release time us

    t_timeval getHoldTime();
    ButtonState::e getState() { return _eState; }
    vec2& getMousePosPress() { return _vMousePosPress; }
    vec2& getMousePosRelease() { return _vMousePosRelease; }

    void copyFrom(TouchInfo* rhs);

    virtual void reset();
    void touchBegin(vec2& mouse, ButtonState::e b);
    void touchEnd(vec2& mouse, ButtonState::e b);
};
}//ns Game



#endif
