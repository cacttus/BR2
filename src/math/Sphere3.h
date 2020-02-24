/**
*  @file Sphere3.h
*  @date August 10, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __SPHERE3F_181043110271772801121301_H__
#define __SPHERE3F_181043110271772801121301_H__

#include "../math/Vec3x.h"

namespace BR2 {
/**
*  @class Sphere3f
*  @brief
*/
class Sphere3f : public VirtualMemory {
public:
  FORCE_INLINE Sphere3f();
  FORCE_INLINE virtual ~Sphere3f() override;
  FORCE_INLINE Vec3f project(const Vec3f& x, Vec3f* outNormal = NULL); // project a point onto the sphere.  May return the normal to the point.

  Vec3f o;    //origin
  float r;    //radius
};
/**
*    @fn
*    @brief Project point onto sphere.  possibly a faster way to do this would be to get the squared length of both vectors and multiply the one by the fraction of the other
*/
FORCE_INLINE Vec3f Sphere3f::project(const Vec3f& x, Vec3f* outNormal) {
  Vec3f diff = x - o;
  float len = diff.length();
  Vec3f n;

  if (len == 0) {
    diff = o;
    if (outNormal)
      *outNormal = Vec3f(0, 0, 0);
  }
  else {
    // origin + normalized * radius
    n = diff / len;

    if (outNormal)
      *outNormal = n;

    diff = o + (n * r);

  }

  return diff;

  //Slow
  //  len = 3.5
  //  diff/len = .571, .571, .571
  //  diff/len * r = 1.142, 1.142, 1.142
  //Other
  // o = 0,0,0
  // diff = 2,2,2
  // dt = 12
  // r = 2
  // r2 = 4
  // amt = r2/dt = 4/12 = .33
  // x = .66,.66,.66
  // x = 

  ////other method that hsna't been tested
  // this doesn't w2ork there is sa missing factor in amt
  //float dt = diff.squaredLength();
  //float r2 = sqrf(r);
  //float amt = r2/dt;
  //x = o + diff * amt;

}

//+-- CTOR/DTOR --+
FORCE_INLINE Sphere3f::Sphere3f() {
}

FORCE_INLINE Sphere3f::~Sphere3f() {

}





}//ns game




#endif
