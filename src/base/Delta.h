/**
*  @file Delta.h
*  @date November 5, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __DELTA_14783644163757421765_H__
#define __DELTA_14783644163757421765_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class Delta
*  @brief The delta is the timekeeper of the engine.  It represents the seconds since the previous frame code has finished, up to the point that you call it.  
*  Delta is used for all the animation in the engine.
*/
class Delta : public GameMemory {
public:
  Delta();
  virtual ~Delta();
  void update();
  float get() { return (float)_fDelta; }

private:
  uint64_t _iDeltaNow = 0;
  uint64_t _iDeltaLast = 0;
  double _fDelta = 0;
};

}//ns BR2



#endif
