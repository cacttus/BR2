#include "../base/BaseHeader.h"
#include "../base/GamePad.h"
#include "../base/InputManager.h"


namespace BR2 {


GamePad::GamePad(std::shared_ptr<InputManager> man) {
  _pInput = man;
}
GamePad::~GamePad() {
}

bool KeyboardGamePad::saveScreenshot() {
  return getInput()->keyPress(SDL_SCANCODE_F9, KeyMod::e::Shift);
}



}//ns Game
