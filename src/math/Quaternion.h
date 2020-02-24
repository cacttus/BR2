/**
*  @file Quaternion.h
*  @date 6 / 12 / 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"

namespace Game {
/**
*  @class Quaternion
*  not that quaternions only support rotations and matrix translations would be lost in the conversion.
*  Q = [ (x,y,z), w ]
*  Q = [ v, w ]
*  Q = [ v sin(a) , cos(a) ]
*/
class Quaternion : public PureMemory {
public:
  float x, y, z, w;

  Matrix4x4 toMat4();
  Matrix3x3 toMat3();

  FORCE_INLINE float mag();
  FORCE_INLINE Vector3 vectorPart();
  FORCE_INLINE float Dot(const Quaternion& rhs) const;
  FORCE_INLINE Vector3 rotatePoint(Vector3& in);
  FORCE_INLINE Quaternion inverse();
  FORCE_INLINE Quaternion operator*(const Quaternion& rhs) const;
  FORCE_INLINE Quaternion operator*(float f) const;
  FORCE_INLINE Quaternion operator+(const Quaternion& rhs) const;
  FORCE_INLINE Quaternion slerpTo(Quaternion& rhs, float t) const;
  FORCE_INLINE Quaternion operator*(Vector3 v);
  FORCE_INLINE void getAxisAngle(vec4& v);
  FORCE_INLINE void construct(float dx, float dy, float dz, float dw);
  FORCE_INLINE Quaternion();
  FORCE_INLINE Quaternion(float, float, float, float);
};
FORCE_INLINE Quaternion::Quaternion() {

}
FORCE_INLINE Quaternion::Quaternion(float dx, float dy, float dz, float dw) {
  x = dx;
  y = dy;
  z = dz;
  w = dw;
}
FORCE_INLINE void Quaternion::construct(float dx, float dy, float dz, float dw) {
  x = dx;
  y = dy;
  z = dz;
  w = dw;
}
FORCE_INLINE float Quaternion::Dot(const Quaternion& rhs) const {
  return (x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}
FORCE_INLINE float Quaternion::mag() {
  return w * w + (x * x + y * y + z * z);
}


FORCE_INLINE Quaternion Quaternion::inverse() {
  float L = 1 / mag();
  Quaternion out;
  out.w = L * w;
  out.x = -L * x;
  out.y = -L * y;
  out.z = -L * z;
  return out;
}
//Quatern
FORCE_INLINE Quaternion Quaternion::operator*(const Quaternion& rhs) const {
  Quaternion ret;

  //Normal quaternion multiplication
  //Q(w,v) = w=w1w2 - v1 dot v2, v= w1v2 + w2v1 + v2 cross v1.

  // - This method here is that of backwards multiplication 
  // used in the 3D Math Primer For Graphics and Game Development:
  //    w=w1w2 - v1 dot v2, v= w1v2 + w2v1 + v1 cross v2.
  // this allows the quaternion rotations to be concatenated in a left to right order.
  ret.x = w * rhs.x + rhs.w * x + (y * rhs.z) - (rhs.y * z);
  ret.y = w * rhs.y + rhs.w * y + (z * rhs.x) - (rhs.z * x);
  ret.z = w * rhs.z + rhs.w * z + (x * rhs.y) - (rhs.x * y);
  ret.w = w * rhs.w - (x * rhs.x + y * rhs.y + z * rhs.z);
  return ret;
}
FORCE_INLINE Quaternion Quaternion::operator*(float f) const {
  Quaternion out;
  out.x = x * f;
  out.y = y * f;
  out.z = z * f;
  out.w = w * f;
  return out;
}
FORCE_INLINE Quaternion Quaternion::operator+(const Quaternion& rhs) const {
  Quaternion out;
  out.x = x + rhs.x;
  out.y = y + rhs.y;
  out.z = z + rhs.z;
  out.w = w + rhs.w;
  return out;
}
/*
wikipedia implemenation may be beter
Quaternion slerp(Quaternion v0, Quaternion v1, double t) {
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    v0.normalize();
    v1.normalize();

    // Compute the cosine of the angle between the two vectors.
    double dot = dot_product(v0, v1);

    const double DOT_THRESHOLD = 0.9995;
    if (fabs(dot) > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.

        Quaternion result = v0 + t*(v1 – v0);
        result.normalize();
        return result;
    }

    // If the dot product is negative, the quaternions
    // have opposite handed-ness and slerp won't take
    // the shorter path. Fix by reversing one quaternion.
    if (dot < 0.0f) {
        v1 = -v1;
        dot = -dot;
    }

    Clamp(dot, -1, 1);           // Robustness: Stay within domain of acos()
    double theta_0 = acos(dot);  // theta_0 = angle between input vectors
    double theta = theta_0*t;    // theta = angle between v0 and result

    Quaternion v2 = v1 – v0*dot;
    v2.normalize();              // { v0, v2 } is now an orthonormal basis

    return v0*cos(theta) + v2*sin(theta);
}*/
// t is from [0 to 1]
FORCE_INLINE Quaternion Quaternion::slerpTo(Quaternion& rhs, float t) const {
  Quaternion ret;
  float s0, s1, sinAng, ang, cosAng, absAng;
  float sinSqr;

  cosAng = Dot(rhs);
  absAng = fabsf(cosAng);

  if ((1 - absAng) > 1e-6f) {
    sinSqr = 1.0f - absAng * absAng;
    sinAng = MathUtils::broRsqrt(sinSqr);
    ang = atan2f(sinSqr * sinAng, absAng);
    s0 = sinf((1.0f - t) * ang) * sinAng;
    s1 = sinf(t * ang) * sinAng;
  }
  else {
    s0 = (1.0f - t);
    s1 = t;
  }
  s1 = (cosAng >= 0.0f) ? s1 : -s1;
  ret.x = s0 * x + s1 * rhs.x;
  ret.y = s0 * y + s1 * rhs.y;
  ret.z = s0 * z + s1 * rhs.z;
  ret.w = s0 * w + s1 * rhs.w;

  return ret;
}
















}





#endif
