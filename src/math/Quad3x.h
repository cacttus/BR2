/**
*  @file Quad3x.h
*  @date March 31, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __QUAD3X_123052400970933043510229_H__
#define __QUAD3X_123052400970933043510229_H__

#include "../math/Vec3x.h"

namespace Game {
/**
*  @class Quad3x
*  @brief 3D plane quad.  In 3 space with a normal.
   vert
   3*     *2

   1*     *0 ^
   <--X      |Y
*/
template < class Tx >
class Quad3x : public VirtualMemory {
public:
  typedef Vec3x<Tx> VecType;
  VecType _p[4];    // Quad Corners

  FORCE_INLINE VecType& br() { return _p[0]; }
  FORCE_INLINE VecType& bl() { return _p[1]; }
  FORCE_INLINE VecType& tr() { return _p[2]; }
  FORCE_INLINE VecType& tl() { return _p[3]; }

  FORCE_INLINE VecType& p0() { return _p[0]; }
  FORCE_INLINE VecType& p1() { return _p[1]; }
  FORCE_INLINE VecType& p2() { return _p[2]; }
  FORCE_INLINE VecType& p3() { return _p[3]; }
  FORCE_INLINE t_string toString();

  FORCE_INLINE Quad3x();
  FORCE_INLINE Quad3x(vec2& vTopLeft, vec2& vBottomRight);
};
template < class Tx >
FORCE_INLINE Quad3x<Tx>::Quad3x() {
}
template < class Tx >
FORCE_INLINE Quad3x<Tx>::Quad3x(vec2& vTopLeft, vec2& vBottomRight) {
  _p[3].x = vTopLeft.x;
  _p[3].y = vTopLeft.y;
  _p[3].z = 0;

  _p[2].x = vBottomRight.x;
  _p[2].y = vTopLeft.y;
  _p[2].z = 0;

  _p[1].x = vTopLeft.x;
  _p[1].y = vBottomRight.y;
  _p[1].z = 0;

  _p[0].x = vBottomRight.x;
  _p[0].y = vBottomRight.y;
  _p[0].z = 0;
}
template < class Tx >
FORCE_INLINE t_string Quad3x<Tx>::toString() {
  return Stz "{" + p0().toString() + "},{" + p1().toString() + "},{" + p2().toString() + "},{" + p3().toString() + "}";
}


}//ns game



#endif
