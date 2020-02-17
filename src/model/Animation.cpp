#include "Animation.h"


namespace BR2 {

bool RotateTo::update(float delta) {
  bool bDone = false;

  _fCur += _fAmount * (delta / _fDuration);

  if (_fCur >= _fAmount) {
    bDone = true;
    _fCur = _fAmount;
  }

  _mFinal = mat4::getRotationRad(_fCur, _vAxis);
  return bDone;
}




}//ns BR2
