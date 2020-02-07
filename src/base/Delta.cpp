#include "../base/Delta.h"

namespace Game {
Delta::Delta() {
}
Delta::~Delta() {
}

//void Engine::updateTouch(std::shared_ptr<Fingers> pInput) {
//
//
//    Gu::getApp()->updateTouch(pInput, getFrameDelta());
//}
//void Engine::userZoom(int iAmount) {
//    Gu::getApp()->userZoom(iAmount);
//}
void Delta::update() {
    _iDeltaNow = SDL_GetPerformanceCounter();
    double delta = (double)((_iDeltaNow - _iDeltaLast));
    delta = delta / SDL_GetPerformanceFrequency();
    _iDeltaLast = _iDeltaNow;
    _fDelta = delta;
}


}//ns Game

