/**
*  @file mgPlane2.h
*  @author MetalMario971
*  @date July 20, 2008
*/
#pragma once
#ifndef __MG_MATH_PLANEEX_3_H__
#define __MG_MATH_PLANEEX_3_H__

#include "../math/Plane3.h"
#include "../math/Matrix3x3.h"

namespace BR2 {
/**
*  @class PlaneEx3
*  @brief Extension of the plane class to include tangent and binormal space.  DO NOT ADD VIRTUAL METHODS
*/
class PlaneEx3 : public Plane3f {
public:

  FORCE_INLINE PlaneEx3();
  FORCE_INLINE NOT_VIRTUAL ~PlaneEx3() override;

  float        dist2d(Vec2f& e);    // - Return the X/Z distnace from a vector2 (where y is z)

  FORCE_INLINE void        calcTBN();
  FORCE_INLINE void        construct(Vector3& Normal, Vector3& Point);
  FORCE_INLINE void        construct(Vector3& tri_p1, Vector3& tri_p2, Vector3& tri_p3);

  FORCE_INLINE Matrix3x3 getTBNMatrix();

  //virtual PlaneEx3&    operator=( const PlaneEx3& rhs ){
  //    *this = rhs;
  //    return *this;
  //}
public:
  Vector3 t, b;    // - TBN
  float u;        // - The value for texture space, this is initialized to 1.0
  Vector3 origin;        // - Point on the plane.

};

/**
*  @fn construct()
*  @brief Construct the components of the plane out of a triangle.
*  Also calculates the TBN
*  @remarks
*  The origin will be p1.
*  the tangent vector will be p2 and the binormal will be p3
*  The normal will point outward in the clockwise rotation of the tangent vector to the binormal vector.
*
*
*  assume CCW vertexes
*/
FORCE_INLINE void PlaneEx3::construct(Vector3& tri_p1, Vector3& tri_p2, Vector3& tri_p3) {
  u = 1.0;
  origin = tri_p1;

  t = (tri_p2 - tri_p1);
  b = (tri_p3 - tri_p1);

  t /= u;
  t.normalize();
  n = b.cross(t);//20161129 - NOTE: CHANGED THIS FOR THE RHS COORDINATES

  n.normalize();
  b = n.cross(t);

  d = -n.dot(origin);

}
FORCE_INLINE void PlaneEx3::construct(Vector3& normal, Vector3& point) {
  Plane3f::construct(normal, point);
  origin = point;
}

/*
*  @fn getTBN()
*  @brief Return the TBN of the matrix.
*/
FORCE_INLINE Matrix3x3 PlaneEx3::getTBNMatrix() {
  Matrix3x3 m;
  m._m11 = t.x;
  m._m12 = t.y;
  m._m13 = t.z;
  m._m21 = b.x;
  m._m22 = b.y;
  m._m23 = b.z;
  m._m31 = n.x;
  m._m32 = n.y;
  m._m33 = n.z;
  return m;
}

/*
*    @fn calcTBN()
*    @brief Without calling this the matrix will have no TB!!!!
*/
FORCE_INLINE void PlaneEx3::calcTBN() {
}


FORCE_INLINE PlaneEx3::PlaneEx3()/* : d(0.0f), n(0.0f,0.0f,0.0f), b(0.0f,0.0f,0.0f), t(0.0f,0.0f,0.0f), u(1.0)*/
{
  ;
}
FORCE_INLINE PlaneEx3::~PlaneEx3() {
  ;
}


}// ns game


#endif