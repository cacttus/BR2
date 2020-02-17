#include "../base/Delta.h"

namespace BR2 {
Delta::Delta() {
}
Delta::~Delta() {
}
void Delta::update() {
  _iDeltaNow = SDL_GetPerformanceCounter();
  double delta = (double)((_iDeltaNow - _iDeltaLast));
  delta = delta / SDL_GetPerformanceFrequency();
  _iDeltaLast = _iDeltaNow;
  _fDelta = delta;
}
}//ns BR2

