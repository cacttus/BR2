#include "Fingers.h"
#include "../base/TStr.h"
#include "../base/Logger.h"

namespace Game {
///////////////////////////////////////////////////////////////////
Fingers::Fingers()
{
    //Keyboard keys init
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        _kbKeys[i] = Game::ButtonState::Up;
    }
    _eRmb = _eLmb = Game::ButtonState::Up;

}
Fingers::~Fingers()
{

}
///////////////////////////////////////////////////////////////////
void Fingers::init(){
    _vMousePos = 0.0f;
    _vLastMousePos = 0.0f;
    _eRmb = _eLmb = _eMmb = ButtonState::e::Up;
    //Pre + Post update to prevent garbage
    preUpdate();
    postUpdate();
}
bool Fingers::modsHeld(int modFlags) {
    bool b = true;
    b = b && ((modFlags & KeyMod::AltDontCare) || ((modFlags & KeyMod::Alt) && altHeld()) || ((~modFlags & KeyMod::Alt) && !altHeld()));
    b = b && ((modFlags & KeyMod::ShiftDontCare) || ((modFlags & KeyMod::Shift) && shiftHeld()) || ((~modFlags & KeyMod::Shift) && !shiftHeld()));
    b = b && ((modFlags & KeyMod::CtrlDontCare) || ((modFlags & KeyMod::Ctrl) && controlHeld()) || ((~modFlags & KeyMod::Ctrl) && !controlHeld()));
    return b;
}
bool Fingers::shiftHeld() {
    return keyPressOrDown(SDL_SCANCODE_LSHIFT) || keyPressOrDown(SDL_SCANCODE_RSHIFT);
}
bool Fingers::controlHeld() {
    return keyPressOrDown(SDL_SCANCODE_LCTRL) || keyPressOrDown(SDL_SCANCODE_RCTRL);
}
bool Fingers::shiftPressed() {
    return keyPress(SDL_SCANCODE_LSHIFT) || keyPress(SDL_SCANCODE_RSHIFT);
}
bool Fingers::altHeld() {
    return keyPressOrDown(SDL_SCANCODE_LALT) || keyPressOrDown(SDL_SCANCODE_RALT);
}
void Fingers::setKeyDown(SDL_Scancode keyCode) {

    if (keyCode >= SDL_NUM_SCANCODES) {
        //Error
        BroLogError("scancode outside range:" + keyCode);
    }
    else {
        _kbKeys[keyCode] = Game::ButtonState::e::Press;
    }

    // SDL_bool withControl = !!(event->key.keysym.mod & KMOD_CTRL);
    //  SDL_bool withShift = !!(event->key.keysym.mod & KMOD_SHIFT);
    // SDL_bool withAlt = !!(event->key.keysym.mod & KMOD_ALT);

}
void Fingers::setKeyUp(SDL_Scancode keyCode)
{
    if (keyCode >= SDL_NUM_SCANCODES) {
        //Error
        BroLogError("scancode outside range: " + keyCode);
    }
    else {
        _kbKeys[keyCode] = Game::ButtonState::Release;
    }
}
void Fingers::setLmbState(ButtonState::e bs) {
    _eLmb = bs;
}
void Fingers::setRmbState(ButtonState::e bs) {
    _eRmb = bs;
}
void Fingers::preUpdate() {
    int ix, iy;
    SDL_GetMouseState(&ix, &iy);
    _vMousePos.x = (float)ix;
    _vMousePos.y = (float)iy;
}
void Fingers::postUpdate() {
    //Updates the button state per frame
    updateButtState(_eLmb);
    updateButtState(_eRmb);

    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        updateButtState(_kbKeys[i]);
    }

    _vLastMousePos = _vMousePos;
}
void Fingers::updateButtState(Game::ButtonState::e &eState) {
    if (eState == Game::ButtonState::e::Press) {
        eState = Game::ButtonState::e::Down;
    }
    else if (eState == Game::ButtonState::Release) {
        eState = Game::ButtonState::e::Up;
    }
}
bool Fingers::keyState(SDL_Scancode key, ButtonState::e bs) {
    AssertOrThrow2(key <= SDL_NUM_SCANCODES);
    return _kbKeys[key] == bs;
}
bool Fingers::mbState(MouseButton::e eMb, ButtonState::e eBs) {
    if (eMb == MouseButton::e::Left) { return _eLmb == eBs; }
    else if (eMb == MouseButton::e::Right) { return _eRmb == eBs; }
    else if (eMb == MouseButton::e::Middle) { return _eMmb == eBs; }
    return false;
}
bool Fingers::keyPressOrDown(SDL_Scancode kc) {
    return keyState(kc, ButtonState::e::Press) || keyState(kc, ButtonState::e::Down);
}
bool Fingers::keyPressOrDown(SDL_Scancode kc, int modFlags) {
    return modsHeld(modFlags) && (keyState(kc, ButtonState::e::Press) || keyState(kc, ButtonState::e::Down));
}
bool Fingers::keyPress(SDL_Scancode kc) {
    return keyState(kc, ButtonState::e::Press);
}
bool Fingers::keyPress(SDL_Scancode kc, int modFlags) {
    return modsHeld(modFlags) && keyState(kc, ButtonState::e::Press);
}
bool Fingers::mousePress(MouseButton::e eMb) {
    return mbState(eMb, ButtonState::e::Press);
}
bool Fingers::mouseDown(MouseButton::e eMb) {
    return mbState(eMb, ButtonState::e::Down);
}

void Fingers::warpMouse(int x, int y) {
    SDL_WarpMouseInWindow(nullptr, x, y);
    //  _bMouseWarped = true;
}



}//ns Game

