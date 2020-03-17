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
#include "../base/SDLIncludes.h"

namespace BR2 {
/**
*  @class Fingers
*  @brief  Manages user input.
*/
class InputManager : public VirtualMemoryShared<InputManager> {
public:
  InputManager();
  virtual ~InputManager() override;

  void init(); // Call this get initial input & prevent garbage data.

  const vec2& getMousePos_Relative() { return _vMousePos_relative; }
  const vec2& getLastMousePos_Relative() { return _vLastMousePos_relative; }
  const vec2& getMousePos_Global() { return _vMousePos_global; }
  const vec2& getLastMousePos_Global() { return _vLastMousePos_global; }

  std::shared_ptr<GamePad> gamePad() { return _pGamePad; }

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
  void setMouseWheel(int x) { _iMouseWheel = x; }
  ButtonState::e getLmbState() { return _eLmb; }
  ButtonState::e getRmbState() { return _eRmb; }
  ButtonState::e getMmbState() { return _eMmb; }
  int getMouseWheel() { return _iMouseWheel; }

  void warpMouse(int x, int y);


  void preUpdate(); //before frame
  void postUpdate(); //after frame
  void updateButtState(BR2::ButtonState::e& eState);


private:
  ButtonState::e _eRmb = BR2::ButtonState::Up;
  ButtonState::e _eLmb = BR2::ButtonState::Up;
  ButtonState::e _eMmb = BR2::ButtonState::Up;
  ButtonState::e _kbKeys[SDL_NUM_SCANCODES];
  vec2 _vLastMousePos_relative;
  vec2 _vMousePos_relative;
  vec2 _vLastMousePos_global;
  vec2 _vMousePos_global;

  std::shared_ptr<GamePad> _pGamePad = nullptr;

  int _iMouseWheel;

  // bool _bMouseWarped = false;

  bool mbState(MouseButton::e mb, ButtonState::e bs);
  bool keyState(SDL_Scancode key, ButtonState::e bs);
};

}//ns Game



#endif
