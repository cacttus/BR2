#include "../base/TouchInfo.h"
#include "../base/Gu.h"

namespace Game {
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void TouchInfo::copyFrom(TouchInfo* rhs) {
    this->_eState           = rhs->_eState            ;
    this->_vMousePosPress   = rhs->_vMousePosPress    ;
    this->_vWorldPosPress   = rhs->_vWorldPosPress    ;
    this->_iPress           = rhs->_iPress            ;
    this->_vMousePosRelease = rhs->_vMousePosRelease  ;
    this->_vWorldPosRelease = rhs->_vWorldPosRelease  ;
    this->_iRelease         = rhs->_iRelease          ;
}
t_timeval TouchInfo::getHoldTime() {
    //Hold Duration us
    if (_iRelease == -1) {
        if (_iPress == -1) {
            return 0;
        }
        else {
            return Gu::getMicroSeconds() - _iPress;
        }
    }
    else {
        return _iRelease - _iPress;
    }
}
void TouchInfo::reset() {
    _vMousePosPress = vec2(0, 0);
    _vMousePosRelease = vec2(0, 0);
    _vWorldPosPress = vec3(0, 0, 0);
    _vWorldPosRelease = vec3(0, 0, 0);
    _iRelease = _iPress = -1;
    // _pReleaseSide = _pPressSide = nullptr;
}
void TouchInfo::touchBegin(vec2& mouse, ButtonState::e b) {
    reset();
    _vMousePosPress = mouse;
    _iPress = Gu::getMicroSeconds();
    _eState = b;
}
//void touchUpdate();
void TouchInfo::touchEnd(vec2& mouse, ButtonState::e b) {
    _iRelease = Gu::getMicroSeconds();
    _vMousePosRelease = mouse;
    _eState = b;
}








}//ns Game
