#include "../base/FpsMeter.h"
#include "../base/Gu.h"


namespace BR2 {

bool FpsMeter::deltaMs(t_timeval& __inout_ last, t_timeval ms) {
  t_timeval cur = Gu::getMicroSeconds();
  if (cur - last >= ms) {
    last = cur;
    return true;
  }
  return false;
}
void FpsMeter::update() {
  //Only call this once per frame.
  t_timeval cur = Gu::getMicroSeconds();
  if (cur - _tmr > 500000) {//Update every seec
    t_timeval delta = cur - _last;
    double divisor = 1000000;
    if (delta == 0) {
      delta = (t_timeval)divisor;
    }
    _fpsLast = (float)(divisor / (double)delta);
    _tmr = cur;
  }
  _last = cur;

  _iFrame++;
}




}//ns BR2
