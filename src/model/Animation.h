/**
*  @file Animation.h
*  @date December 27, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __ANIMATION_14828771764087888723_H__
#define __ANIMATION_14828771764087888723_H__

#include "../model/ModelHeader.h"

namespace BR2 {

//PRS animation
class PRSAnimation : public VirtualMemory {
public:
  mat4& getMat() {
    return _mFinal;
  }
  PRSAnimation() {}
  virtual ~PRSAnimation() override {}
  virtual bool update(float delta) = 0;
protected:
  mat4 _mFinal;
};

class RotateTo : public PRSAnimation {
public:
  RotateTo(float amt, vec3& axis, float duration) :
    _vAxis(axis)
    , _fAmount(amt)
    , _fDuration(duration) {
  }
  virtual ~RotateTo() override {}
  virtual bool update(float delta) override;
private:
  float _fAmount = 3.14f; //amount to rotate
  vec3 _vAxis;
  float _fDuration = 1.0f;
  float _fCur = 0.0f;
};


}//ns Game



#endif
