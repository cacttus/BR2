/**
*  @file GamePad.h
*  @date 20200315
*  @author MetalMario971
*/
#pragma once
#ifndef __GAMEPAD_15843051524261011566_H__
#define __GAMEPAD_15843051524261011566_H__

#include "../base/BaseHeader.h"

namespace BR2 {

class GamePadButton : public VirtualMemory {
  friend class GamePad;
public:
  GamePadButton() {}
  virtual ~GamePadButton() override {}

  bool pressed() { return _eState == ButtonState::e::Press; }
  bool held() { return _eState == ButtonState::e::Down; }
  bool pressedOrHeld() { return pressed() || held(); }
  bool released() { return _eState == ButtonState::e::Release; }
  bool up() { return _eState == ButtonState::e::Up; }
  bool upOrReleased() { return released() || up(); }
private:
  ButtonState::e _eState = ButtonState::e::Up;
};

class GamePad : public VirtualMemoryShared<GamePad> {
public:
  GamePad(std::shared_ptr<InputManager> man);
  virtual ~GamePad() override;

  virtual bool saveScreenshot() { return false; }

  std::shared_ptr<InputManager> getInput() { return _pInput; }
private:
  std::shared_ptr<InputManager> _pInput = nullptr;

};


class KeyboardGamePad : public GamePad {
public:
  KeyboardGamePad(std::shared_ptr<InputManager> pm) : GamePad(pm) {}
  virtual ~KeyboardGamePad() override {}
  virtual bool saveScreenshot() override;

};

}//ns Game



#endif
