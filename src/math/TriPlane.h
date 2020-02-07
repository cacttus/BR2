/**
*    @file TriPlane.h
*    @author MetalMario971
*
*    © 2011
*    @adate 2 / 1 / 2010
*
*/
#pragma once
#ifndef __TRI_PLANE_39024890234890802382057_H__
#define __TRI_PLANE_39024890234890802382057_H__
#include "../math/MathHeader.h"
#include "../math/Vec4x.h"
#include "../math/PlaneEx3.h"
#include "../math/Algorithm.h"
namespace Game {


/**
*    @class TriPlane
*    @brief Derived plane for extra info and stuff.
*DO NOT ADD VIRTUAL METHODS
*/
class TriPlane : public PlaneEx3 {
public:
  Vector3 p1, p2, p3;

  FORCE_INLINE TriPlane();
  FORCE_INLINE TriPlane(const vec3&, const  vec3&, const vec3&);
  FORCE_INLINE TriPlane(const vec3&, const  vec3&, const vec3&, const vec3& n, float d);
  FORCE_INLINE NOT_VIRTUAL ~TriPlane() override;

  FORCE_INLINE bool containsPoint(const vec3&) const; // - True if the point falls within the region of the quad plane.
  FORCE_INLINE PlaneHit hitTest(const vec3& p1, const vec3& p2) const;
  FORCE_INLINE bool hitTest(Ray_t* pr) const;
  FORCE_INLINE void construct(const vec3& dp1, const vec3& dp2, const vec3& dp3);
  FORCE_INLINE void construct();

  FORCE_INLINE STATIC bool containsPointSlowButWorks(const vec3& cp1, const vec3& cp2, const vec3& cp3, const vec3& point);
  FORCE_INLINE STATIC bool containsPoint(const vec3& cp1, const vec3& cp2, const vec3& cp3, const vec3& point);

};

FORCE_INLINE TriPlane::TriPlane(const vec3& dp1, const  vec3& dp2, const  vec3& dp3) {
  construct(dp1, dp2, dp3);
}
FORCE_INLINE TriPlane::TriPlane(const vec3& _dp1, const  vec3& _dp2, const  vec3& _dp3, const  vec3& _n, float _d) {
  p1 = _dp1;
  p2 = _dp2;
  p3 = _dp3;
  n = _n;
  d = _d;
}
FORCE_INLINE TriPlane::TriPlane() {
}
FORCE_INLINE TriPlane::~TriPlane() {
}
FORCE_INLINE PlaneHit TriPlane::hitTest(const vec3& p1, const vec3& p2) const {
  PlaneHit ret;
  ret._bContained = false;

  // - Basic Plane Intersection
  float t = intersectLine(p1, p2).t;// - todo

  // - Test to see if point is within triangle boundaries.
  if (t >= 0.0 && t <= 1.0) {
    Vector3 v = p1 + ((p2 - p1) * t);
    ret._fHitTime = t;
    ret._bContained = containsPoint(v);
  }
  return ret;
}

FORCE_INLINE bool TriPlane::hitTest(Ray_t* pr) const {
  PlaneHit ph = hitTest(pr->getBegin(), std::move(pr->getEnd()));
  pr->setTime(ph._fHitTime);

  return pr->isHit();
}

/*
*    @fn containsPoint()
*    @brief Test to see if the point is contained within the triangle
*
*    **TODO:
*        We need to test points and edges.
*/
FORCE_INLINE bool TriPlane::containsPoint(const vec3& cp1, const vec3& cp2, const vec3& cp3, const vec3& point) {
  vec3 dp1 = cp2 - cp1;
  vec3 dp2 = cp3 - cp1;
  vec3 v = point - cp1;

  float a = dp1.dot(dp1);
  float b = dp1.dot(dp2);
  float c = dp2.dot(dp2);
  float d = v.dot(dp1);
  float e = v.dot(dp2);

  float bx = (d * c) - (e * b);
  float by = (e * a) - (d * b);
  float bz = bx + by - (a * c) + (b * b);

  if ((((unsigned int&)(bz) & ~((unsigned int&)(bx) | (unsigned int&)(by))) & 0x80000000))
    return true;

  return false;
}
FORCE_INLINE bool TriPlane::containsPoint(const vec3& point) const {
  // - Base method copied from somewhere.  Faster than my method.
//    return containsPoint(p1,p2,p3,point);

    // My Method. This method works as well.  It isn't that slow, but it is slower than the optimal method.
    //Fully tested
  return containsPointSlowButWorks(p1, p2, p3, point);
}
/**
*    @fn construct()
*    @brief Override of the plane construct.  Will calculate the data inside of the TriPlane
*/
FORCE_INLINE void TriPlane::construct(const vec3& dp1, const vec3& dp2, const vec3& dp3) {
  p1 = dp1;
  p2 = dp2;
  p3 = dp3;

  construct();
}
/**
*    @fn construct()
*    @brief Override of the plane construct.  Will calculate the data inside of the TriPlane
*/
FORCE_INLINE void TriPlane::construct() {
  PlaneEx3::construct(p1, p2, p3);
}

/*
*    @fn containsPointSlowButWorks()
*    @brief This isn't THAT slow, but it is slower than the one above.
*    This method works.  Was used in place of the optimized method above.
*    ***Extensively tested assuming pointOnLine_t does not change*
*    Implemented: MetalMario971 1.19.14
*/
FORCE_INLINE bool TriPlane::containsPointSlowButWorks(const vec3& cp1, const vec3& cp2, const vec3& cp3, const vec3& point) {

  // get point along edge that makes edge a plane
  vec3 p1e = (cp1 - cp2); // cp3
  vec3 p1n = (cp3 - cp2);

  vec3 p2e = (cp2 - cp3); // cp1 
  vec3 p2n = (cp1 - cp3);

  vec3 p3e = (cp3 - cp1); // cp2
  vec3 p3n = (cp2 - cp1);

  // construct some normals from the edges to turn the edges into planes.
  float e1t = Alg::pointOnLine_t(cp2, cp1, cp3);
  float e2t = Alg::pointOnLine_t(cp3, cp2, cp1);
  float e3t = Alg::pointOnLine_t(cp1, cp3, cp2);
  vec3 p1pt = cp2 + (p1e * e1t);
  vec3 p2pt = cp3 + (p2e * e2t);
  vec3 p3pt = cp1 + (p3e * e3t);
  vec3 p1n1 = cp3 - p1pt;
  vec3 p2n1 = cp1 - p2pt;
  vec3 p3n1 = cp2 - p3pt;

  // get test point relative to plane coordinates.
  vec3 p1p = point - p1pt;
  vec3 p2p = point - p2pt;
  vec3 p3p = point - p3pt;

  // use dot product to test sign
  float s1 = p1n1.dot(p1p);
  float s2 = p2n1.dot(p2p);
  float s3 = p3n1.dot(p3p);

  return ((s1 >= 0) && (s2 >= 0) && (s3 >= 0));
}
//
//ctor
//






}

#endif
