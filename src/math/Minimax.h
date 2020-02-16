/**
*  @file Minimax.h
*  @date November 22, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __MINIMAX_31366194432631567467958_H__
#define __MINIMAX_31366194432631567467958_H__

#include "../base/MachineTypes.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"

namespace BR2 {
/**
*  @class MinimaxParam
*  @brief Deals with mathematical boundaries, and random values.
*/
template < class Tx >
class MinimaxParam : public VirtualMemory {
public:
  MinimaxParam() {}
  MinimaxParam(Tx min, Tx max) : _min(min), _max(max) {}

  const Tx& getMin() { return _min; }
  const Tx& getMax() { return _max; }
  void setMin(Tx&& rhs) { _min = rhs; _bMinSet = true; }
  void setMax(Tx&& rhs) { _max = rhs; _bMaxSet = true; }
  void setMinMax(Tx&& i0, Tx&& a0) { _min = i0; _max = a0; _bMaxSet = true; _bMinSet = true; }
  MinimaxParam& operator=(Tx& rhs) {
    _min = rhs;
    _max = rhs;

    return *this;
  }
protected:
  Tx _min;
  Tx _max;
  void checkWasSet() {
    if (!_bMinSet || !_bMaxSet) {
      throw 0;//we can't do a debug break here.
    }
  }
private:
  bool _bMinSet = false, _bMaxSet = false;
};
class MpFloat : public MinimaxParam<float> {
public:
  MpFloat() { _min = 0.0f; _max = 1.0f; }
  MpFloat(float i, float a) { setMin(std::move(i)); setMax(std::move(a)); }
  virtual ~MpFloat() override {}
  float next();
};
class  MpVec3 : public MinimaxParam<vec3> {
public:
  MpVec3() { _min = vec3(0,0,0); _max = vec3(1, 1, 1); }
  virtual ~MpVec3() override {}
  vec3 next();
};
class MpVec3i : public MinimaxParam<ivec3> {
public:
  MpVec3i() { _min = ivec3(0, 0, 0); _max = ivec3(1, 1, 1); }
  virtual ~MpVec3i() override {}
  ivec3 next();
};
class MpVec4 : public MinimaxParam<vec4> {
public:
  MpVec4() { _min = vec4(0,0,0,0); _max = vec4(1, 1, 1, 1); }
  virtual ~MpVec4() override {}
  vec4 next();
};
class MpUint : public MinimaxParam<uint32_t> {
public:
  MpUint() { _min = 0; _max = 1; }
  virtual ~MpUint() override {}
  uint32_t next();
};
class MpInt : public MinimaxParam<int32_t> {
public:
  MpInt() { _min = 0; _max = 1; }
  MpInt(int32_t i, int32_t a) { setMin(std::move(i)); setMax(std::move(a)); }
  virtual ~MpInt() override {}
  int32_t next();
};
FORCE_INLINE void initMp(MpFloat x) { x.setMin(0.0); x.setMax(1.0); }
FORCE_INLINE void initMp(MpVec3 x) { x.setMin(vec3(0, 0, 0)); x.setMax(vec3(1, 1, 1)); }
FORCE_INLINE void initMp(MpVec4 x) { x.setMin(vec4(0, 0, 0, 0)); x.setMax(vec4(1, 1, 1, 1)); }
FORCE_INLINE void initMp(MpUint x) { x.setMin(0), x.setMax(INT_MAX); }
FORCE_INLINE void initMp(MpInt x) { x.setMin(-INT_MAX), x.setMax(INT_MAX); }


}//ns game



#endif
