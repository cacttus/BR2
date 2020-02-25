#include "../base/InputManager.h"
#include "../base/Logger.h"

namespace BR2 {
InputManager::InputManager() {
  //Keyboard keys init
  for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
    _kbKeys[i] = BR2::ButtonState::Up;
  }
  _eRmb = _eLmb = BR2::ButtonState::Up;

}
InputManager::~InputManager() {

}
void InputManager::init() {
  _vMousePos = 0.0f;
  _vLastMousePos = 0.0f;
  _eRmb = _eLmb = _eMmb = ButtonState::e::Up;
  //Pre + Post update to prevent garbage
  preUpdate();
  postUpdate();
}
bool InputManager::modsHeld(int modFlags) {
  bool b = true;
  b = b && ((modFlags & KeyMod::AltDontCare) || ((modFlags & KeyMod::Alt) && altHeld()) || ((~modFlags & KeyMod::Alt) && !altHeld()));
  b = b && ((modFlags & KeyMod::ShiftDontCare) || ((modFlags & KeyMod::Shift) && shiftHeld()) || ((~modFlags & KeyMod::Shift) && !shiftHeld()));
  b = b && ((modFlags & KeyMod::CtrlDontCare) || ((modFlags & KeyMod::Ctrl) && controlHeld()) || ((~modFlags & KeyMod::Ctrl) && !controlHeld()));
  return b;
}
bool InputManager::shiftHeld() {
  return keyPressOrDown(SDL_SCANCODE_LSHIFT) || keyPressOrDown(SDL_SCANCODE_RSHIFT);
}
bool InputManager::controlHeld() {
  return keyPressOrDown(SDL_SCANCODE_LCTRL) || keyPressOrDown(SDL_SCANCODE_RCTRL);
}
bool InputManager::shiftPressed() {
  return keyPress(SDL_SCANCODE_LSHIFT) || keyPress(SDL_SCANCODE_RSHIFT);
}
bool InputManager::altHeld() {
  return keyPressOrDown(SDL_SCANCODE_LALT) || keyPressOrDown(SDL_SCANCODE_RALT);
}
void InputManager::setKeyDown(SDL_Scancode keyCode) {

  if (keyCode >= SDL_NUM_SCANCODES) {
    //Error
    BRLogError("scancode outside range:" + keyCode);
  }
  else {
    _kbKeys[keyCode] = BR2::ButtonState::e::Press;
  }

  // SDL_bool withControl = !!(event->key.keysym.mod & KMOD_CTRL);
  //  SDL_bool withShift = !!(event->key.keysym.mod & KMOD_SHIFT);
  // SDL_bool withAlt = !!(event->key.keysym.mod & KMOD_ALT);

}
void InputManager::setKeyUp(SDL_Scancode keyCode) {
  if (keyCode >= SDL_NUM_SCANCODES) {
    //Error
    BRLogError("scancode outside range: " + keyCode);
  }
  else {
    _kbKeys[keyCode] = BR2::ButtonState::Release;
  }
}
void InputManager::setLmbState(ButtonState::e bs) {
  _eLmb = bs;
}
void InputManager::setRmbState(ButtonState::e bs) {
  _eRmb = bs;
}
void InputManager::preUpdate() {
  int ix, iy;
  SDL_GetMouseState(&ix, &iy);
  _vMousePos.x = (float)ix;
  _vMousePos.y = (float)iy;
}
void InputManager::postUpdate() {
  //Updates the button state per frame
  updateButtState(_eLmb);
  updateButtState(_eRmb);

  for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
    updateButtState(_kbKeys[i]);
  }

  _vLastMousePos = _vMousePos;
}
void InputManager::updateButtState(BR2::ButtonState::e& eState) {
  if (eState == BR2::ButtonState::e::Press) {
    eState = BR2::ButtonState::e::Down;
  }
  else if (eState == BR2::ButtonState::Release) {
    eState = BR2::ButtonState::e::Up;
  }
}
bool InputManager::keyState(SDL_Scancode key, ButtonState::e bs) {
  AssertOrThrow2(key <= SDL_NUM_SCANCODES);
  return _kbKeys[key] == bs;
}
bool InputManager::mbState(MouseButton::e eMb, ButtonState::e eBs) {
  if (eMb == MouseButton::e::Left) { return _eLmb == eBs; }
  else if (eMb == MouseButton::e::Right) { return _eRmb == eBs; }
  else if (eMb == MouseButton::e::Middle) { return _eMmb == eBs; }
  return false;
}
bool InputManager::keyPressOrDown(SDL_Scancode kc) {
  return keyState(kc, ButtonState::e::Press) || keyState(kc, ButtonState::e::Down);
}
bool InputManager::keyPressOrDown(SDL_Scancode kc, int modFlags) {
  return modsHeld(modFlags) && (keyState(kc, ButtonState::e::Press) || keyState(kc, ButtonState::e::Down));
}
bool InputManager::keyPress(SDL_Scancode kc) {
  return keyState(kc, ButtonState::e::Press);
}
bool InputManager::keyPress(SDL_Scancode kc, int modFlags) {
  return modsHeld(modFlags) && keyState(kc, ButtonState::e::Press);
}
bool InputManager::mousePress(MouseButton::e eMb) {
  return mbState(eMb, ButtonState::e::Press);
}
bool InputManager::mouseDown(MouseButton::e eMb) {
  return mbState(eMb, ButtonState::e::Down);
}

void InputManager::warpMouse(int x, int y) {
  SDL_WarpMouseInWindow(nullptr, x, y);
  //  _bMouseWarped = true;
}



}//ns Game

