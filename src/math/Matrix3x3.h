/**
*  @file Matrix3x3.cpp
*  @author MetalMario971
*  @date 9 / 7 / 2009
*/
#pragma once
#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
#include "../math/Quaternion.h"
#include "../base/TypeConv.h"
namespace BR2 {
/**
*  @class Matrix3x3
*  @brief A 3x3 Matrix. Column major.
*/
class Matrix3x3 : public PureMemory {
public:
  float
    _m11, _m12, _m13,
    _m21, _m22, _m23,
    _m31, _m32, _m33;

  FORCE_INLINE Matrix3x3();
  FORCE_INLINE Matrix3x3(float(&f)[9]);
  FORCE_INLINE NOT_VIRTUAL ~Matrix3x3() DOES_NOT_OVERRIDE;

  FORCE_INLINE float& Mat(size_t i) { return (float&)*((float*)(this) + i); }
  FORCE_INLINE const float& Mat(size_t i) const { return (float&)*((float*)(this) + i); }

  FORCE_INLINE void scale(float x, float y, float z);    // - Scales this matrix by some amount
  FORCE_INLINE void rotate(t_degrees a, float x, float y, float z); // - Rotates this matrix.
  FORCE_INLINE void rotateRad(t_radians a, float x, float y, float z); // - Rotates this matrix.
  FORCE_INLINE void rotateRad(t_radians a, vec3& vec); // - Rotates this matrix.

  FORCE_INLINE Matrix3x3 inverse();
  FORCE_INLINE float det();    // determinant
  FORCE_INLINE Matrix3x3 adj(); // adjoint

  FORCE_INLINE float& operator[](const size_t i);

  FORCE_INLINE Matrix3x3& operator*=(const Matrix3x3& rhs);
  FORCE_INLINE Matrix3x3 operator*(const Matrix3x3& rhs);

  FORCE_INLINE Matrix3x3& operator=(const Matrix3x3& rhs);
  Matrix3x3& operator=(const Matrix4x4& rhs);

  FORCE_INLINE Vector3 operator*= (const Vector3& v);
  FORCE_INLINE Vector3 operator*(const Vector3& v);

  FORCE_INLINE void setIdentity();    // - Sets this matrix to it's identity.
  Matrix4x4 getMat4();
  FORCE_INLINE Quaternion getQuaternion();    // - Returns the quaternion of this matrix

  FORCE_INLINE STATIC Matrix3x3 getScale(float x, float y, float z);        // - Returns a scale matrix of x,y,z.
  FORCE_INLINE STATIC Matrix3x3 getRotation(t_degrees a, float x, float y, float z);    // - Returns a rotation matrix from degrees.
  FORCE_INLINE STATIC Matrix3x3 getRotationRad(t_radians a, float x, float y, float z);    // - Returns a rotation matrix in radians.
  FORCE_INLINE STATIC Matrix3x3 getRotationRad(float a, const vec3& v);    // - Returns a rotation matrix in radians.
  FORCE_INLINE t_string toString() const;

};
FORCE_INLINE Matrix3x3::Matrix3x3() {
}
FORCE_INLINE Matrix3x3::Matrix3x3(float(&f)[9]) {
  memcpy((float*)(&_m11), f, sizeof(float) * 9);
}
FORCE_INLINE NOT_VIRTUAL Matrix3x3::~Matrix3x3() {
}

// - The expanded cofactor adjoint.
FORCE_INLINE Matrix3x3 Matrix3x3::adj() {
  Matrix3x3 m;
  m._m11 = 0.0f * ((_m22 * _m33) - (_m23 * _m32));
  m._m12 = 1.0f * ((_m21 * _m33) - (_m23 * _m31));
  m._m13 = 0.5f * ((_m21 * _m32) - (_m22 * _m31));
  m._m21 = 1.0f * ((_m12 * _m33) - (_m13 * _m32));
  m._m22 = 0.5f * ((_m11 * _m33) - (_m13 * _m31));
  m._m23 = powf(-1.00f, 2 + 1) * ((_m12 * _m31) - (_m11 * _m32));
  m._m31 = 0.5f * ((_m12 * _m23) - (_m13 * _m22));
  m._m32 = powf(-1.00f, 2 + 1) * ((_m11 * _m23) - (_m13 * _m21));
  m._m33 = 0.25f * ((_m11 * _m22) - (_m12 * _m21));
  return m;
}
FORCE_INLINE float Matrix3x3::det() {
  return (
    _m11 * _m22 * _m33 +
    _m21 * _m32 * _m13 +
    _m12 * _m23 * _m31
    - (_m13 * _m22 * _m31)
    - (_m12 * _m21 * _m33)
    - (_m23 * _m32 * _m11)
    );
}
FORCE_INLINE Matrix3x3 Matrix3x3::inverse() {
  Matrix3x3 m = adj();

  float d = m.det();
  for (int i = 0; i < 9; ++i)
    m.Mat(i) /= d;
  return m;
}

FORCE_INLINE Quaternion Matrix3x3::getQuaternion() {
  float s0, s1, s2;
  int32_t k0, k1, k2, k3;
  float q[4];
  if (_m11 + _m22 + _m33 > 0.0f) {
    k0 = 3;
    k1 = 2;
    k2 = 1;
    k3 = 0;
    s0 = 1.0f;
    s1 = 1.0f;
    s2 = 1.0f;
  }
  else if (_m11 + _m22 && _m11 > _m33) {
    k0 = 0;
    k1 = 1;
    k2 = 2;
    k3 = 3;
    s0 = 1.0f;
    s1 = -1.0f;
    s2 = -1.0f;
  }
  else if (_m22 > _m33) {
    k0 = 1;
    k1 = 0;
    k2 = 3;
    k3 = 2;
    s0 = -1.0f;
    s1 = 1.0f;
    s2 = -1.0f;
  }
  else {
    k0 = 2;
    k1 = 3;
    k2 = 0;
    k3 = 1;
    s0 = -1.0f;
    s1 = -1.0f;
    s2 = 1.0f;
  }
  float t = s0 * _m11 + s1 * _m22 + s2 * _m33 + 1.0f;
  float s = MathUtils::brRsqrt(t) * 0.5f;//reciprocalSqrt(t)*0.5f;

  q[k0] = s * t;
  q[k1] = (_m12 - s2 * _m21) * s;
  q[k2] = (_m31 - s1 * _m13) * s;
  q[k3] = (_m23 - s0 * _m32) * s;

  Quaternion ret = *((Quaternion*)q);
  return ret;
}
/**
*    @fn setIdentity()
*    @brief Sets the identity of this matrix.
*/
FORCE_INLINE void Matrix3x3::setIdentity() {
  /*
      0  3  6
      1  4  7
      2  5  8
      11  21  31
      12  22  32
      13  23  33
  */
  _m11 = _m22 = _m33 = 1.0;
  _m12 = _m13 = _m21 = _m23 = _m31 = _m32 = 0.0;
  //Mat(0)=Mat(4)=Mat(8)=1.0;
  //Mat(3)=Mat[6]=Mat[1]=Mat[7]=Mat[2]=Mat[5]=0.0;
}


/**
*    @fn scaleCat()
*    @details Scales the values of this matrix.
*    @param x,y,z the scale value.
*
*/
FORCE_INLINE void Matrix3x3::scale(float x, float y, float z) {
  *this *= getScale(x, y, z);
}

/**
*    @fn rotateCat()
*    @details Rotates the values of this matrix.
*    @param a, x,y,z the rotation value in axis-angle form.
*    a is in degrees
*/
FORCE_INLINE void Matrix3x3::rotate(t_degrees a, float x, float y, float z) {
  *this *= getRotation(a, x, y, z);

}
/**
*    @fn rotateCat()
*    @details Rotates the values of this matrix.
*    @param a, x,y,z the rotation value in axis-angle form.
*    a is in degrees
*/
FORCE_INLINE void Matrix3x3::rotateRad(t_radians a, float x, float y, float z) {
  *this *= getRotationRad(a, x, y, z);
}

/**
*    @fn rotate()
*    @details Returns a rotation matrix, but does not alter this matrix.
*    @param a, x,y,z the axis-angle rotation.
*    @return A matrix witht the specified rotation.d
*    @param a - ANGLE IN DEGREES!
*/
FORCE_INLINE Matrix3x3 Matrix3x3::getRotation(t_degrees a, float x, float y, float z) {
  // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html
  a = (float)(double)(a * (M_PI)) / 180.0f;
  return getRotationRad(a, x, y, z);
}
/**
*    @fn scale()
*    @details Returns a scale matrix, but does not alter this matrix.
*    @param x,y,z the scaling value.
*    @return A matrix witht the specified scaling.
*
*/
FORCE_INLINE Matrix3x3 Matrix3x3::getScale(float x, float y, float z) {
  Matrix3x3 m;
  m._m11 = x;
  m._m22 = y;
  m._m33 = z;
  //m.Mat[0 ] = x;
  //m.Mat[5 ] = y;
  ///m._m33 = z;

  return m;
}
/**
*    @fn operator[]
*
*
*/
FORCE_INLINE float& Matrix3x3::operator[](const size_t i) {
  return Mat(i);
}



FORCE_INLINE Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& m) {
  Matrix3x3 tMat;
  tMat._m11 = (_m11 * m._m11) + (_m12 * m._m21) + (_m13 * m._m31);
  tMat._m21 = (_m21 * m._m11) + (_m22 * m._m21) + (_m23 * m._m31);
  tMat._m31 = (_m31 * m._m11) + (_m32 * m._m21) + (_m33 * m._m31);

  tMat._m12 = (_m11 * m._m12) + (_m12 * m._m22) + (_m13 * m._m32);
  tMat._m22 = (_m21 * m._m12) + (_m22 * m._m22) + (_m23 * m._m32);
  tMat._m32 = (_m31 * m._m12) + (_m32 * m._m22) + (_m33 * m._m32);

  tMat._m13 = (_m11 * m._m13) + (_m12 * m._m23) + (_m13 * m._m33);
  tMat._m23 = (_m21 * m._m13) + (_m22 * m._m23) + (_m23 * m._m33);
  tMat._m33 = (_m31 * m._m13) + (_m32 * m._m23) + (_m33 * m._m33);
  *this = tMat;
  return *this;
}
FORCE_INLINE Matrix3x3 Matrix3x3::operator*(const Matrix3x3& rhs) {
  Matrix3x3 m;
  m *= rhs;
  return m;
}
FORCE_INLINE Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs) {
  _m11 = rhs._m11;
  _m12 = rhs._m12;
  _m13 = rhs._m13;

  _m21 = rhs._m21;
  _m22 = rhs._m22;
  _m23 = rhs._m23;

  _m31 = rhs._m31;
  _m32 = rhs._m32;
  _m33 = rhs._m33;

  return *this;
}
FORCE_INLINE t_string Matrix3x3::toString() const {
  return
    TypeConv::floatToStr(_m11) + "," +
    TypeConv::floatToStr(_m12) + "," +
    TypeConv::floatToStr(_m13) + "\n" +
    TypeConv::floatToStr(_m21) + "," +
    TypeConv::floatToStr(_m22) + "," +
    TypeConv::floatToStr(_m23) + "\n" +
    TypeConv::floatToStr(_m31) + "," +
    TypeConv::floatToStr(_m32) + "," +
    TypeConv::floatToStr(_m33) + "\n"
    ;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
STATIC FORCE_INLINE t_string tstr(const mat3& x) { return x.toString(); }


}



#endif
