/**
*  @file Fingers.h
*  @date March 9, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __FINGERS_1489072950597086515_H__
#define __FINGERS_1489072950597086515_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
namespace Game {
/**
*    @class Fingers
*    @brief
*
*/
class InputManager : public VirtualMemory {
    ButtonState::e _eRmb, _eLmb, _eMmb;
    ButtonState::e _kbKeys[SDL_NUM_SCANCODES];
    vec2 _vLastMousePos;
    vec2 _vMousePos;

   // bool _bMouseWarped = false;

    bool mbState(MouseButton::e mb, ButtonState::e bs);
    bool keyState(SDL_Scancode key, ButtonState::e bs);
public:
    InputManager();
    virtual ~InputManager() override;

    void init(); // Call this get initial input & prevent garbage data.

    const vec2& getMousePos() { return _vMousePos; }
    const vec2& getLastMousePos() { return _vLastMousePos; }

    bool mousePress(MouseButton::e);
    bool mouseDown(MouseButton::e);
    bool keyPress(SDL_Scancode kc);
    bool keyPress(SDL_Scancode kc, int modFlags);
    bool keyPressOrDown(SDL_Scancode kc);
    bool keyPressOrDown(SDL_Scancode kc, int modFlags);
    bool shiftHeld();
    bool shiftPressed();
    bool altHeld();
    bool controlHeld();

    bool modsHeld(int modFlags);

    void setKeyDown(SDL_Scancode kc);
    void setKeyUp(SDL_Scancode kc);
    void setLmbState(ButtonState::e bs);
    void setRmbState(ButtonState::e bs);

    void preUpdate(); //before frame
    void postUpdate(); //after frame
    void updateButtState(Game::ButtonState::e &eState);


    ButtonState::e getLmbState(){ return _eLmb; }
    ButtonState::e getRmbState(){ return _eRmb; }
    ButtonState::e getMmbState() { return _eMmb; }

    void warpMouse(int x, int y);

};

}//ns Game



#endif
