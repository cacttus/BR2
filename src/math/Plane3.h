//2/1/2010
//8/1/2014 Extended from "plane3f"
#pragma once
#ifndef __PLANE_3_2938572938692980329_H__
#define __PLANE_3_2938572938692980329_H__
#include "../math/Vec3x.h"
#include "../math/Vec2x.h"
namespace BR2 {
class Ray_t;
/**
*  @class Plane3f
*  @brief Simplest form of a plane with a normal and d value
*/
class Plane3f : public VirtualMemory {
public:
  vec3 n;            // - TBN
  float d = 0;            // - The D value of the plane in the plane equation.

  FORCE_INLINE float dist(const vec3&) const;    // - Returns the distance from a vector to the plane.
  FORCE_INLINE vec3 point(const vec3&) const;    // - Projects a point on to the surface of the plane. 
  FORCE_INLINE vec3 projectPoint(const vec3&) const;    // - Alias for point()
  FORCE_INLINE void construct(const vec3& normal, const vec3& point);
  FORCE_INLINE void construct(const vec3& tri_p1, const vec3& tri_p2, const vec3& tri_p3);
  bool intersectRay(Ray_t* ray);
  FORCE_INLINE LineHit intersectLine(const vec3& p1, const vec3& p2) const;
  FORCE_INLINE vec3 intersectLinePoint(const vec3& p1, const vec3& p2, bool* __out_ hit) const;
  FORCE_INLINE bool isBehind(const vec3&);    // - True if the vector is behind the plane, or opposite the direction of the normal
  FORCE_INLINE bool isInFront(const vec3&);    // - True if the vector is behind the plane, or opposite the direction of the normal
  FORCE_INLINE STATIC void getNormalCcw(const vec3& __in_ p1, const vec3& __in_ p2, const  vec3& __in_ p3, vec3& __out_ n_out);
  FORCE_INLINE void calc(const vec3& p); // calculate D value

  FORCE_INLINE Plane3f(const vec3& tri_p1, const vec3& tri_p2, const vec3& tri_p3);
  FORCE_INLINE Plane3f(const vec3&& n, const float&& d);
  FORCE_INLINE Plane3f(const vec3&& n, const vec3&& pointOnPlane);
  FORCE_INLINE Plane3f();
};
Plane3f::Plane3f(const vec3&& dn, const vec3&& pointOnPlane) {
  n = dn;
  d = dn.dot(pointOnPlane);
}
Plane3f::Plane3f(const vec3&& dn, const float&& dd) {
  n = dn; d = dd;
}
FORCE_INLINE Plane3f::Plane3f(const vec3& tri_p1, const vec3& tri_p2, const vec3& tri_p3) {
  construct(tri_p1, tri_p2, tri_p3);
}
FORCE_INLINE Plane3f::Plane3f() {
}

/**
*    @fn
*    @brief Utility function to get normal to triagnle

        CCW
               p2<-------------p1
                   \           ^
                         \     |
                             \>p3
*/
FORCE_INLINE void Plane3f::getNormalCcw(const vec3& __in_ p1, const  vec3& __in_ p2, const  vec3& __in_ p3, vec3& __out_ n_out) {
  vec3 d1 = p2 - p1;
  vec3 d2 = p3 - p1;
  n_out = d1.cross(d2);
  n_out.normalize();
}
//moved intersectRay after ray_t definition
/**
*    @fn intersectLine()
*    @brief Returns the position along the line (t) t which the input line intercepts the plane.
*    @note if the line segment (p1,p2) does not hit the plane then t will be <0 or >1
*/
FORCE_INLINE LineHit Plane3f::intersectLine(const vec3& p1, const vec3& p2) const {
  LineHit ret;

  /*
      How to solve the equation.

      Equation for a vector
      p = pa + ( pb-pa )t

      Equation for a Plane
      n . p  + d = 0

      Plug in vector equation into plane equation

      n . (pa+(pb-pa)t) + d = 0

      n . (pa+(pb-pa)t) = -d
      n . pa + n . (pb-pa)t = -d
      n . (pb-pa)t = -d -(n . pa)

      // n.n is 1

      t = -(d+n.pa) / ( n . (pb-pa))

  */
  // a . b = a.x*b.x  + a.y*b.y + a.z*b.z
  //
  //
  ret.t = -(n.dot(p1) + d) / ((p2 - p1).dot(n));

  return ret;
}
FORCE_INLINE vec3 Plane3f::intersectLinePoint(const vec3& p1, const vec3& p2, bool* __out_ hit) const {
  AssertOrThrow2(hit != nullptr);
  LineHit lh = intersectLine(p1, p2);
  vec3 vRet(0, 0, 0);

  if (lh.t >= 0.0f && lh.t <= 1.0f) {
    *hit = true;
    vRet = p1 + (p2 - p1) * lh.t;
  }
  else {
    *hit = false;
  }

  return vRet;
}
/**
*    @fn isBehind()
*    @brief Returns true if it is behind.
*/
FORCE_INLINE bool Plane3f::isBehind(const vec3& v) {
  return ((n.dot(v)) < 0.0);
}
/**
*    @fn isInFront()
*    @brief Returns true if it is in front.
*/
FORCE_INLINE bool Plane3f::isInFront(const vec3& v) {
  return ((n.dot(v)) > 0.0);
}
/**
*    @fn dist()
*    @details The distance from a point to the plane.  It is assumed the plane has been constructed.
*    @param p The point to find the distance.
*    @return The distance from the input point to the plane.
*/
FORCE_INLINE float Plane3f::dist(const vec3& p) const {
  /* Ax + By + Cz +D = 0 */
  return (float)(n.dot(p) + d);
}
/**
*    @fn point()
*    @details Projects a point onto the plane and returns the projected point.
*    @param p The point to project onto the plane (line).
*    @return The projected point.
*/
FORCE_INLINE vec3 Plane3f::point(const vec3& p) const {
  return (p - (n * dist(p)));
}
/**/
FORCE_INLINE vec3 Plane3f::projectPoint(const vec3& p) const {
  return (p - (n * dist(p)));
}
/*
*    @fn construct
*    @brief Construct the values for the plane assuming that the normal is already normalized.
*/
FORCE_INLINE void Plane3f::construct(const vec3& normal, const vec3& point) {
  n = normal;
  n.normalize();
  calc(point);
}

FORCE_INLINE void Plane3f::construct(const vec3& tri_p1, const vec3& tri_p2, const vec3& tri_p3) {
  //CCW! 
  vec3 dp1 = tri_p2 - tri_p1;
  vec3 dp2 = tri_p3 - tri_p1;
  n = dp1.cross(dp2);
  n.normalize();
  calc(tri_p1);
}

FORCE_INLINE void Plane3f::calc(const vec3& p) {
  d = -n.dot(p);
}

}
#endif
