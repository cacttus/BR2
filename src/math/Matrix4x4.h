/**
*  @file Matrix4x4.h
*  @brief Column-major matrix
*  @author MetalMario971
*  @date 2007, 2008, 2009, 2015
*/

#pragma once
#ifndef __MAT__4__X__4__H__
#define __MAT__4__X__4__H__

#include "../math/Vec4x.h"
#include "../math/Vec3x.h"
#include "../math/Matrix3x3.h"
#include "../base/TypeConv.h"

namespace BR2 {
/**
*    @stuct Matrix4x4
*    @brief Row Major 4x4 matrix.
*/
class Matrix4x4 : public PureMemory {
public:
  float
    _m11, _m12, _m13, _m14,
    _m21, _m22, _m23, _m24,
    _m31, _m32, _m33, _m34,
    _m41, _m42, _m43, _m44;

  FORCE_INLINE Matrix4x4();
  FORCE_INLINE Matrix4x4(float* mat);
  FORCE_INLINE NOT_VIRTUAL ~Matrix4x4() DOES_NOT_OVERRIDE {}

  FORCE_INLINE  float& Mat(size_t i) { return (float&)*((float*)(this) + i); }
  const FORCE_INLINE float& Mat(size_t i) const { return (float&)*((float*)(this) + i); }
  FORCE_INLINE int32_t nRows() { return 4; }
  FORCE_INLINE int32_t nCols() { return 4; }
  FORCE_INLINE int32_t size() { return 16; }     // - Returns the number of entries in this matrix.
  FORCE_INLINE static Matrix4x4 identity() {
    float m[] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1 };
    return Matrix4x4(m);
  }

  FORCE_INLINE STATIC void copy(Matrix4x4* to, Matrix4x4* from);
  FORCE_INLINE void values(float* m);
  FORCE_INLINE Matrix4x4& setTranslation(float x, float y, float z);
  FORCE_INLINE Matrix4x4 operator+(const Matrix4x4& m) const;
  FORCE_INLINE Quaternion getQuaternion();

  FORCE_INLINE void lookAt(const vec3& eye, const vec3& center, const vec3& up);
  FORCE_INLINE Matrix4x4& translate(float x, float y, float z);
  FORCE_INLINE Matrix4x4& translate(const vec3& v);
  FORCE_INLINE void scale(float x, float y, float z);
  FORCE_INLINE void rotateDeg(t_degrees a, float x, float y, float z);
  FORCE_INLINE void rotateRad(t_radians a, float x, float y, float z);
  FORCE_INLINE Matrix4x4& transpose();
  FORCE_INLINE Matrix4x4 transposed();
  FORCE_INLINE Vector3 getVec3() const;
  FORCE_INLINE Vector4 getVec4() const;
  FORCE_INLINE void clear();
  FORCE_INLINE void setIdentity();
  FORCE_INLINE float at(int row, int col) const;
  FORCE_INLINE int32_t colNum(int ind);
  FORCE_INLINE int32_t rowNum(int ind);
  FORCE_INLINE float det();
  FORCE_INLINE Matrix3x3 minor(int r, int c);
  FORCE_INLINE float cofactor(int r, int c);
  FORCE_INLINE Matrix4x4 adj();
  FORCE_INLINE Matrix4x4 invert();
  FORCE_INLINE Matrix4x4 inverseOf();
  FORCE_INLINE Vector4 row(int row);
  FORCE_INLINE Vector4 col(int col);
  FORCE_INLINE Matrix4x4& projection(float n, float f, float l, float r, float t, float b); // set up a projection matrix.

  FORCE_INLINE string_t toString() const;

  FORCE_INLINE Vec3f getTranslation() const;
  FORCE_INLINE void setTranslation(const Vec3f& vec);
  FORCE_INLINE void setTranslationX(float x);
  FORCE_INLINE void setTranslationY(float y);
  FORCE_INLINE void setTranslationZ(float z);
  FORCE_INLINE float getTranslationX() const;
  FORCE_INLINE float getTranslationY() const;
  FORCE_INLINE float getTranslationZ() const;
  FORCE_INLINE Vector4 getTranslationVector() const { return Vector4(_m41, _m42, _m43, _m44); }

#pragma region Operators
  FORCE_INLINE float& operator[](const size_t& i);
  FORCE_INLINE float         operator[](const size_t& i) const;
  FORCE_INLINE Matrix4x4& operator*=(const Matrix4x4& m);
  FORCE_INLINE const Vector4   operator*(const Vector4& v) const;
  FORCE_INLINE Matrix4x4& operator=(const float f);
  FORCE_INLINE bool          operator==(const Matrix4x4& rhs) const;
  FORCE_INLINE bool          operator!=(const Matrix4x4& rhs) const;
  FORCE_INLINE Matrix4x4& operator=(const Matrix4x4& m);
  FORCE_INLINE const Matrix4x4 operator*(const Matrix4x4& m) const;
#pragma endregion

#pragma region Static Matrix Constructors
  FORCE_INLINE STATIC Matrix4x4 getIdentity();

  FORCE_INLINE STATIC Matrix4x4 getTranslation(vec3& vTrans);
  FORCE_INLINE STATIC Matrix4x4 getTranslation(float x, float y, float z);

  FORCE_INLINE STATIC Matrix4x4 getRotationDeg(t_degrees a, vec3& vAxis);
  FORCE_INLINE STATIC Matrix4x4 getRotationRad(t_radians a, vec3& vAxis);
  FORCE_INLINE STATIC Matrix4x4 getRotationDeg(t_degrees a, float x, float y, float z);
  FORCE_INLINE STATIC Matrix4x4 getRotationRad(t_radians a, float x, float y, float z);

  FORCE_INLINE STATIC Matrix4x4 getRotationToVector(vec3 v, vec3 up);

  FORCE_INLINE STATIC Matrix4x4 getScale(vec3& vScale);
  FORCE_INLINE STATIC Matrix4x4 getScale(float x, float y, float z);

  FORCE_INLINE STATIC Matrix4x4 getProjection(float n, float f, float l, float r, float t, float b);
  FORCE_INLINE STATIC Matrix4x4 getOrientToVector(vec3 v, vec3 up);
  FORCE_INLINE STATIC Matrix4x4 getLookAt(const vec3& eye, const vec3& center, const vec3& up);
  FORCE_INLINE STATIC Matrix4x4 getOrtho(float left, float right, float top, float bottom, float neard, float fard);
  FORCE_INLINE STATIC bool parse(string_t, Matrix4x4& mOut);
#pragma endregion


  FORCE_INLINE void decompose(vec4& pos, mat4& rot, vec4& scale) const;
  FORCE_INLINE void decompose(vec4& pos, vec4& rot, vec4& scale, bool bDegreeRotation = false) const;

};

FORCE_INLINE Matrix4x4::Matrix4x4() {
}
FORCE_INLINE Matrix4x4::Matrix4x4(float* mat) {
  // error
  //(t_float*)Mat=mat; ////// just getting this to build... fix me... TODO: BUG: DEBUG:
  for (int i = 0; i < 16; ++i) {
    Mat(i) = mat[i];
  }
}
FORCE_INLINE void Matrix4x4::copy(Matrix4x4* to, Matrix4x4* from) {
  if (!(to && from)) {
    throw new Exception("runtime assertion", __LINE__, __FILE__);
  }
  memcpy((void*)to, (void*)from, sizeof(Matrix4x4));
}
FORCE_INLINE void Matrix4x4::values(float* m) {
  int x = 0;
  m[x++] = _m11;
  m[x++] = _m21;
  m[x++] = _m31;
  m[x++] = _m41;

  m[x++] = _m12;
  m[x++] = _m22;
  m[x++] = _m32;
  m[x++] = _m42;

  m[x++] = _m13;
  m[x++] = _m23;
  m[x++] = _m33;
  m[x++] = _m43;

  m[x++] = _m14;
  m[x++] = _m24;
  m[x++] = _m34;
  m[x++] = _m44;
}
FORCE_INLINE Matrix4x4& Matrix4x4::setTranslation(float x, float y, float z) {
  _m41 = x;
  _m42 = y;
  _m43 = z;

  return *this;
}
FORCE_INLINE Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m) const {
  Matrix4x4 ret;
  for (int i = 0; i < 16; ++i)
    ret[i] = this->Mat(i) + m[i];
  return ret;
}
/**
*    @fn getQuaternion
*    @brief Intel's version of matrix to a quaternion.
*    (http://cache-www.intel.com/cd/00/00/29/37/293748_293748.pdf)
*/
FORCE_INLINE Quaternion Matrix4x4::getQuaternion() {
  double s0, s1, s2;
  int32_t k0, k1, k2, k3;
  float  q[4];
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
  float t = (float)(s0 * _m11 + s1 * _m22 + s2 * _m33 + 1.0f);
  //assert(t>=0.0);
  //if( t==0.0 ) t=1e-10f;
  float s = (float)(MathUtils::broRsqrt(t) * 0.5f);/*reciprocalSqrt(t)*0.5f;*/

  q[k0] = s * t;

  q[k1] = (float)((_m12 - s2 * _m21) * s);
  q[k2] = (float)((_m31 - s1 * _m13) * s);
  q[k3] = (float)((_m23 - s0 * _m32) * s);

  Quaternion ret = *((Quaternion*)q);
  return ret;
}
/**
*    getIdentity()
*
*
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getIdentity() {
  Matrix4x4 m;
  m.setIdentity();
  return m;
}
/**
*    @fn translate()
*    @details Returns a translation matrix, but does not alter this matrix.
*    @param x,y,z the translation value.
*    @return A matrix witht the specified translation.
*
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getTranslation(vec3& vTrans) {
  return getTranslation(vTrans.x, vTrans.y, vTrans.z);
}
FORCE_INLINE Matrix4x4 Matrix4x4::getTranslation(float x, float y, float z) {
  Matrix4x4 m = Matrix4x4::identity();

  m._m41 = x;
  m._m42 = y;
  m._m43 = z;

  return m;
}
/**
*    @fn rotate()
*    @details Returns a rotation matrix, but does not alter this matrix.
*    @param a, x,y,z the axis-angle rotation.
*    @return A matrix witht the specified rotation.
*    @param a - ANGLE IN DEGREES!
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getRotationDeg(t_degrees a, vec3& vAxis) {
  return getRotationDeg(a, vAxis.x, vAxis.y, vAxis.z);
}
FORCE_INLINE Matrix4x4 Matrix4x4::getRotationDeg(t_degrees a, float x, float y, float z) {
  // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html
  a = (float)((double)((double)a * (M_PI)) * (double)(1.0f / 180.0f));
  return getRotationRad(a, x, y, z);
}
/**
*    @fn rotate()
*    @details Returns a rotation matrix, but does not alter this matrix.
*    @param a, x,y,z the axis-angle rotation.
*    @return A matrix witht the specified rotation.
*    @param a - Radians.
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getRotationRad(t_radians a, vec3& vAxis) {
  return getRotationRad(a, vAxis.x, vAxis.y, vAxis.z);
}
FORCE_INLINE Matrix4x4 Matrix4x4::getRotationRad(t_radians a, float x, float y, float z) {
  // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html

  Matrix4x4 Temp = Matrix4x4::identity();

  float c = cosf(a);
  float s = sinf(a);
  float nc = 1 - c;
  // row matrix

  Temp._m11 = (x * x) * nc + c;
  Temp._m12 = (x * y) * nc + (z * s);
  Temp._m13 = (x * z) * nc - (y * s);
  Temp._m14 = 0.0f;

  Temp._m21 = (y * x) * nc - (z * s);
  Temp._m22 = (y * y) * nc + c;
  Temp._m23 = (y * z) * nc + (x * s);
  Temp._m24 = 0.0f;

  Temp._m31 = (z * x) * nc + (y * s);
  Temp._m32 = (z * y) * nc - (x * s);
  Temp._m33 = (z * z) * nc + c;
  Temp._m34 = 0.0f;

  Temp._m41 = 0.0f;
  Temp._m42 = 0.0f;
  Temp._m43 = 0.0f;
  Temp._m44 = 1.0f;

  return Temp;
}
/**
*    @fn getRotationToVector()
*    @brief WORKS.  Returns a matrix that would, when multiplied by a vector, rotate that vector to align with this input vector v. (and it would be a different length)
*    @remarks I don't know if the "scuz" part works completely but for now it seems to work in every case.
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getRotationToVector(Vector3 v, Vector3 up) {
  if (v.x + v.y + v.z != 1.0)
    v.normalize();
  if (up.x + up.y + up.z != 1.0)
    up.normalize();

  float ang = acosf(v.dot(up));

  if (ang == 0.0f)
    // no need for rotation
    return Matrix4x4::identity();

  Vector3 perp = up.cross(v);

  if (perp.x + perp.y + perp.z == 0.0) // vectors are direct opposites.
  {
    Vector3 scuz(up.y, -up.z, up.x);
    scuz.normalize();
    perp = scuz.cross(v);

  }

  //TODO: possible error may rotate opposite. (would need to do opposite cross of up.cross(v) scuz cross v

  return Matrix4x4::getRotationRad(ang, perp.x, perp.y, perp.z);
}
/**
*    @fn scale()
*    @details Returns a scale matrix, but does not alter this matrix.
*    @param x,y,z the scaling value.
*    @return A matrix witht the specified scaling.
*
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getScale(vec3& vScale) {
  return getScale(vScale.x, vScale.y, vScale.z);
}
FORCE_INLINE Matrix4x4 Matrix4x4::getScale(float x, float y, float z) {
  Matrix4x4 m = Matrix4x4::identity();
  m._m11 = x;
  m._m22 = y;
  m._m33 = z;

  return m;
}
/*
*    This is a projection matrix
*    see: http://www.opengl.org/sdk/docs/man/xhtml/glFrustum.xml
*/
FORCE_INLINE Matrix4x4 Matrix4x4::getProjection(float fNearClip, float fFarClip, float fLeft, float fRight, float fTop, float fBottom) {
  Matrix4x4 m;
  m.projection(fNearClip, fFarClip, fLeft, fRight, fTop, fBottom);
  return m;
}
/*
*    This is a projection matrix
*    see: http://www.opengl.org/sdk/docs/man/xhtml/glFrustum.xml
*/
FORCE_INLINE Matrix4x4& Matrix4x4::projection(float n, float f, float l, float r, float t, float b) {
  _m11 = (2 * n) / (r - l);
  _m21 = 0;
  _m31 = 0;
  _m41 = 0;

  _m12 = 0;
  _m22 = (2 * n) / (t - b);// *-1.0f; // we added a neagtive here because IDK WHY this is not right
  _m32 = 0;
  _m42 = 0;

  _m13 = (r + l) / (r - l);
  _m23 = (t + b) / (t - b);
  _m33 = -(f + n) / (f - n);
  _m43 = -1;

  _m14 = 0;
  _m24 = 0;
  _m34 = -(2 * f * n) / (f - n);
  _m44 = 0;

#ifdef COORDINATE_SYSTEM_RHS
  //m = m.transposed(); 
  transpose();
#endif

  return *this;
}

//untested
//http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml
FORCE_INLINE Matrix4x4 Matrix4x4::getOrientToVector(Vector3 v, Vector3 up) {
  Matrix4x4 m = Matrix4x4::identity();

  v = v.normalize();

  if (up.x + up.y + up.z != 1.0)
    up = up.normalize();

  Vector3 s = v.cross(up);
  Vector3 u = s.cross(v);

  m._m11 = s.x;
  m._m21 = u.x;
  m._m31 = -v.x;

  m._m12 = s.y;
  m._m22 = u.y;
  m._m32 = -v.y;

  m._m13 = s.z;
  m._m23 = u.z;
  m._m33 = -v.z;

  return m;
}
FORCE_INLINE Matrix4x4 Matrix4x4::getLookAt(const vec3& eye, const vec3& center, const vec3& up) {
  Matrix4x4 m;
  m.lookAt(eye, center, up);
  return m;
}
FORCE_INLINE void Matrix4x4::lookAt(const vec3& eye, const vec3& center, const vec3& up) {
  // also see 
  //http://www-01.ibm.com/support/knowledgecenter/ssw_aix_53/com.ibm.aix.opengl/doc/openglrf/gluLookAt.htm%23b5c874e426rree
  /*
      Let E be the 3d column vector(eyeX, eyeY, eyeZ).
      Let C be the 3d column vector(centerX, centerY, centerZ).
      Let U be the 3d column vector(upX, upY, upZ).
      Compute L = C - E.
      Normalize L.
      Compute S = L x U.
      Normalize S.
      Compute U' = S x L.
      (S, 0), (U', 0), (-L, 0), (-E, 1)  (all column vectors)
  */
  Vec3f L = center - eye;
  L.normalize();
  Vec3f S = L.cross(up);
  S.normalize();
  Vec3f D = S.cross(L);
  Vec3f E = eye;

  L = L * -1;
  E = E;

  Matrix4x4 mm;
  // It seems to be an exact transpose.
  mm._m11 = S.x, mm._m12 = S.y, mm._m13 = S.z, mm._m14 = 0,
    mm._m21 = D.x, mm._m22 = D.y, mm._m23 = D.z, mm._m24 = 0,
    mm._m31 = L.x, mm._m32 = L.y, mm._m33 = L.z, mm._m34 = 0,
    mm._m41 = 0, mm._m42 = 0, mm._m43 = 0, mm._m44 = 1;

  // Not sure if this is right. seems to mimic the opengl matrix correctly
  //mm._m11 = S.x, mm._m12 = D.x, mm._m13 = L.x, mm._m14 =   0,
  //mm._m21 = S.y, mm._m22 = D.y, mm._m23 = L.y, mm._m24 =   0,
  //mm._m31 = S.z, mm._m32 = D.z, mm._m33 = L.z, mm._m34 =   0,
  //mm._m41 =   0, mm._m42 =   0, mm._m43 =   0, mm._m44 =   1;

  Matrix4x4 mmTrans;
  mmTrans.setIdentity();
  mmTrans._m14 = -E.x;
  mmTrans._m24 = -E.y;
  mmTrans._m34 = -E.z;

  Matrix4x4 ret = mm * mmTrans;
#ifdef COORDINATE_SYSTEM_RHS
  ret = ret.transposed();
#endif
  (*this) = ret;
}
/**
*    @fn transCat()
*    @details Translates the values of this matrix.
*    @param x,y,z the translation value.
*
*/
FORCE_INLINE Matrix4x4& Matrix4x4::translate(float x, float y, float z) {
  return (*this *= getTranslation(x, y, z));
}
FORCE_INLINE Matrix4x4& Matrix4x4::translate(const vec3& v) {
  return (*this *= getTranslation(v.x, v.y, v.z));
}

/**
*    @fn scaleCat()
*    @details Scales the values of this matrix.
*    @param x,y,z the scale value.
*
*/
FORCE_INLINE void Matrix4x4::scale(float x, float y, float z) {
  *this *= getScale(x, y, z);
}
/**
*    @fn rotateCat()
*    @details Rotates the values of this matrix.
*    @param a, x,y,z the rotation value in axis-angle form.
*    a is in degrees
*/
FORCE_INLINE void Matrix4x4::rotateDeg(t_degrees a, float x, float y, float z) {
  *this *= getRotationDeg(a, x, y, z);

}
/**
*    @fn rotateCat()
*    @details Rotates the values of this matrix.
*    @param a, x,y,z the rotation value in axis-angle form.
*    a is in degrees
*/
FORCE_INLINE void Matrix4x4::rotateRad(t_radians a, float x, float y, float z) {
  *this *= getRotationRad(a, x, y, z);
}
/**
*    @fn transpose()
*    @details Transpose the matrix.
*/
FORCE_INLINE Matrix4x4& Matrix4x4::transpose() {
  float f[] = {
  _m11, _m21, _m31, _m41,
  _m12, _m22, _m32, _m42,
  _m13, _m23, _m33, _m43,
  _m14, _m24, _m34, _m44
  };

  *this = f;

  return *this;
}
FORCE_INLINE Matrix4x4 Matrix4x4::transposed() {
  Matrix4x4 ret = *this;
  ret.transpose();
  return ret;
}
/**
*    @fn getVector3()
*    @return: Returns the origin translation of a vector by multiplying a zero vector by this matrix.
*/
FORCE_INLINE Vector3 Matrix4x4::getVec3() const {
  Vector4 v(0, 0, 0, 0);
  v = v * (*this);

  return Vector3(v.x, v.y, v.z);
}
/**
*    @fn getVector4()
*    @return: returns a zero vector multiplied by this matrix
*/
FORCE_INLINE Vector4 Matrix4x4::getVec4() const {
  return Vector4(0, 0, 0, 0) * (*this);
}
/**
*    @fn clear
*    @details Clear the matrix.
*/
FORCE_INLINE void Matrix4x4::clear() {
  for (uint32_t n = 0; n < 16; ++n)
    Mat(n) = (float)0.0;
}
/**
*    setIdentity()
*        Sets the identity for this matrix.
*        Works on row matrices as well.
*/
FORCE_INLINE void Matrix4x4::setIdentity() {
  _m11 = _m22 = _m33 = _m44 = 1.0;

  _m21 = _m31 = _m41 = _m12 =
    _m32 = _m42 = _m13 = _m23 =
    _m43 = _m14 = _m24 = _m34 = 0.0;
}
/**
*    @fn at()
*    @brief Returns the value at the given coordinate in the matrix.
*    @remarks Only use 0-3 for indexes.
*/
FORCE_INLINE float Matrix4x4::at(int row, int col) const {
  return Mat(row * col + row);
}
/**
*    @fn colNum
*    @brief Returns the zero-based index of the column in which the supplied index lies.
*/
FORCE_INLINE int32_t Matrix4x4::colNum(int ind) {
  return (int32_t)floor((double)((double)ind / 4.0));
}
/**
*    @fn rowNum
*    @brief Returns the zero-based index of the row in which the supplied index lies.
*/
FORCE_INLINE int32_t Matrix4x4::rowNum(int ind) {
  return ind % 4;
}
/**
*    @fn det() (determinant)
*    @brief Compute the determinant (|M|).
*    @remarks If the determinant of a matrix is 0, then the matrix is singular, and it has no inverse,
*    thus, the inverse() function will return an identity matrix.
*    The determinant can also be computed by multiplying all of the minors by all of the cofactors, or by
*    pivoting a single element.
*/
FORCE_INLINE float Matrix4x4::det() {

  /*
      0    4    8    12
      1    5    9    13
      2    6    10    14
      3    7    11    15

      11    21    31    41
      12    22    32    42
      13    23    33    43
      14    24    34    44
  */

  //TODO:Optimize later.
  return(
    _m11 * (
    (_m22 * _m33 * _m44)
      + (_m23 * _m34 * _m42)
      + (_m24 * _m32 * _m43)
      - (_m22 * _m34 * _m43)
      - (_m23 * _m32 * _m44)
      - (_m24 * _m33 * _m42)
      )
    - _m12 * (
    (_m21 * _m33 * _m44)
      + (_m23 * _m34 * _m41)
      + (_m24 * _m31 * _m43)
      - (_m21 * _m34 * _m43)
      - (_m23 * _m31 * _m44)
      - (_m24 * _m33 * _m41)
      )
    + _m13 * (
    (_m21 * _m32 * _m44)
      + (_m22 * _m34 * _m41)
      + (_m24 * _m31 * _m42)
      - (_m21 * _m34 * _m42)
      - (_m22 * _m31 * _m44)
      - (_m24 * _m32 * _m41)
      )
    - _m14 * (
    (_m21 * _m32 * _m43)
      + (_m22 * _m33 * _m41)
      + (_m23 * _m31 * _m42)
      - (_m21 * _m33 * _m42)
      - (_m22 * _m31 * _m43)
      - (_m23 * _m32 * _m41)
      )
    );

}


/**
*    @fn adj()
*    @brief returns the Adjoint of this matrix.
*/
FORCE_INLINE Matrix4x4 Matrix4x4::adj() {
  //TODO:Optimize (transpose)
  float f[] = {
  cofactor(0,0), cofactor(0,1), cofactor(0,2), cofactor(0,3),
  cofactor(1,0), cofactor(1,1), cofactor(1,2), cofactor(1,3),
  cofactor(2,0), cofactor(2,1), cofactor(2,2), cofactor(2,3),
  cofactor(3,0), cofactor(3,1), cofactor(3,2), cofactor(3,3)
  };
  return Matrix4x4(f);//.transpose();
}
/**
*    @fn getInverse()
*    @brief Returns the inverse of this matrix.
*    @remarks The inverse is the classical adjoint divided by the determinant of the adjoint
*/
FORCE_INLINE Matrix4x4 Matrix4x4::invert() {
  //**Note:
  //Transpose of an orthogonal matrix is it's inverse
  //If we're orthogonal return the transpose.
 //     return this->transposed();

  // - Convert the matrix to Reduced RE form
  Matrix4x4 m;
  // - If the determinant is zero, return m.
  if (!det())
    return m;
  // - Classical adjoint is favored here over Gaussian and reduced row-echlon form.
  m = adj();
  float d = m.det();
  for (int i = 0; i < 16; ++i)
    m.Mat(i) /= d;

  *this = m;

  return *this;
}
FORCE_INLINE Matrix4x4 Matrix4x4::inverseOf() {
  Matrix4x4 ret = *this;
  ret.invert();
  return ret;
}
/**
*    @fn row()
*    @brief Returns a row in the matrix.
*    @remarks Only use 0-3 for indexes
TODO:ERROR - this may be a column and row interchange problem.
*/
FORCE_INLINE Vector4 Matrix4x4::row(int row) {
  return Vector4(Mat(row), Mat(row + 4), Mat(row + 8), Mat(row + 12));
}
/**
*    @fn col()
*    @brief Returns a col in the matrix.
*    @remarks Only use 0-3 for indexes
*/
FORCE_INLINE Vector4 Matrix4x4::col(int col) {
  return Vector4(Mat(col * 4 + 0), Mat(col * 4 + 1), Mat(col * 4 + 2), Mat(col * 4 + 3));
}
/**
*    @fn operator[]
*
*
*/
FORCE_INLINE float& Matrix4x4::operator[](const size_t& i) {
  return Mat(i);
}
FORCE_INLINE float  Matrix4x4::operator[](const size_t& i) const {
  return Mat(i);
}
/**
*    @fn Multiplication Operator.
*    @details Pre-multiplies this matrix by another matrix.
*    @param m: the matrix to multiply by.
*/
FORCE_INLINE Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m) {
  if (this == &m) return *this;

  Matrix4x4 tMat;

  //64 mul
  //48 add
  tMat._m11 = (_m11 * m._m11) + (_m12 * m._m21) + (_m13 * m._m31) + (_m14 * m._m41);
  tMat._m21 = (_m21 * m._m11) + (_m22 * m._m21) + (_m23 * m._m31) + (_m24 * m._m41);
  tMat._m31 = (_m31 * m._m11) + (_m32 * m._m21) + (_m33 * m._m31) + (_m34 * m._m41);
  tMat._m41 = (_m41 * m._m11) + (_m42 * m._m21) + (_m43 * m._m31) + (_m44 * m._m41);

  tMat._m12 = (_m11 * m._m12) + (_m12 * m._m22) + (_m13 * m._m32) + (_m14 * m._m42);
  tMat._m22 = (_m21 * m._m12) + (_m22 * m._m22) + (_m23 * m._m32) + (_m24 * m._m42);
  tMat._m32 = (_m31 * m._m12) + (_m32 * m._m22) + (_m33 * m._m32) + (_m34 * m._m42);
  tMat._m42 = (_m41 * m._m12) + (_m42 * m._m22) + (_m43 * m._m32) + (_m44 * m._m42);

  tMat._m13 = (_m11 * m._m13) + (_m12 * m._m23) + (_m13 * m._m33) + (_m14 * m._m43);
  tMat._m23 = (_m21 * m._m13) + (_m22 * m._m23) + (_m23 * m._m33) + (_m24 * m._m43);
  tMat._m33 = (_m31 * m._m13) + (_m32 * m._m23) + (_m33 * m._m33) + (_m34 * m._m43);
  tMat._m43 = (_m41 * m._m13) + (_m42 * m._m23) + (_m43 * m._m33) + (_m44 * m._m43);

  tMat._m14 = (_m11 * m._m14) + (_m12 * m._m24) + (_m13 * m._m34) + (_m14 * m._m44);
  tMat._m24 = (_m21 * m._m14) + (_m22 * m._m24) + (_m23 * m._m34) + (_m24 * m._m44);
  tMat._m34 = (_m31 * m._m14) + (_m32 * m._m24) + (_m33 * m._m34) + (_m34 * m._m44);
  tMat._m44 = (_m41 * m._m14) + (_m42 * m._m24) + (_m43 * m._m34) + (_m44 * m._m44);

  *this = tMat;

  return *this;
}
/**
*    Multiply this matrix by a vector.
*    Vector*Matrix always yields a vector
*/
FORCE_INLINE const Vector4 Matrix4x4::operator*(const Vector4& v) const {
  // #if using row vectors, alert user, quit.
  Matrix4x4 m = *this;
  Vector4 vret;

  m._m11 *= v.x;
  m._m12 *= v.x;
  m._m13 *= v.x;
  m._m14 *= v.x;

  m._m21 *= v.y;
  m._m22 *= v.y;
  m._m23 *= v.y;
  m._m24 *= v.y;

  m._m31 *= v.z;
  m._m32 *= v.z;
  m._m33 *= v.z;
  m._m34 *= v.z;

  m._m41 *= v.w;
  m._m42 *= v.w;
  m._m43 *= v.w;
  m._m44 *= v.w;

  vret.x = m._m11 + m._m21 + m._m31 + m._m41;
  vret.y = m._m12 + m._m22 + m._m32 + m._m42;
  vret.z = m._m13 + m._m23 + m._m33 + m._m43;
  vret.w = m._m14 + m._m24 + m._m34 + m._m44;

  return vret;
}
FORCE_INLINE Matrix4x4& Matrix4x4::operator=(const float f) {
  for (int i = 0; i < 16; ++i) {
    Mat(i) = f;
  }

  return *this;
}
FORCE_INLINE bool Matrix4x4::operator==(const Matrix4x4& rhs) const {
  for (int i = 0; i < 16; ++i)
    if (Mat(i) != rhs.Mat(i))
      return false;
  return true;
}
FORCE_INLINE bool Matrix4x4::operator!=(const Matrix4x4& rhs) const {
  for (int i = 0; i < 16; ++i)
    if (Mat(i) != rhs.Mat(i))
      return true;
  return false;
}
FORCE_INLINE Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m) {
  for (int i = 0; i < 16; ++i)
    Mat(i) = m.Mat(i);

  return *this;
}
FORCE_INLINE const Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const {
  Matrix4x4 tmp = *this;
  tmp *= m;
  return tmp;
}
FORCE_INLINE string_t Matrix4x4::toString() const {
  return
    TypeConv::floatToStr(_m11) + "," +
    TypeConv::floatToStr(_m12) + "," +
    TypeConv::floatToStr(_m13) + "," +
    TypeConv::floatToStr(_m14) + "\n" +
    TypeConv::floatToStr(_m21) + "," +
    TypeConv::floatToStr(_m22) + "," +
    TypeConv::floatToStr(_m23) + "," +
    TypeConv::floatToStr(_m24) + "\n" +
    TypeConv::floatToStr(_m31) + "," +
    TypeConv::floatToStr(_m32) + "," +
    TypeConv::floatToStr(_m33) + "," +
    TypeConv::floatToStr(_m34) + "\n" +
    TypeConv::floatToStr(_m41) + "," +
    TypeConv::floatToStr(_m42) + "," +
    TypeConv::floatToStr(_m43) + "," +
    TypeConv::floatToStr(_m44)
    ;
}
FORCE_INLINE Vec3f Matrix4x4::getTranslation() const {
  Vec3f ret;
  ret.x = _m41;
  ret.y = _m42;
  ret.z = _m43;
  return ret;
}
FORCE_INLINE void Matrix4x4::setTranslation(const Vec3f& vec) {
  _m41 = vec.x;
  _m42 = vec.y;
  _m43 = vec.z;
}
FORCE_INLINE void Matrix4x4::setTranslationX(float x) {
  _m41 = x;
}
FORCE_INLINE void Matrix4x4::setTranslationY(float y) {
  _m42 = y;
}
FORCE_INLINE void Matrix4x4::setTranslationZ(float z) {
  _m43 = z;
}
FORCE_INLINE float Matrix4x4::getTranslationX() const {
  return _m41;
}
FORCE_INLINE float Matrix4x4::getTranslationY() const {
  return _m42;
}
FORCE_INLINE float Matrix4x4::getTranslationZ() const {
  return _m43;
}
FORCE_INLINE Matrix4x4 Matrix4x4::getOrtho(float left, float right, float top,
  float bottom, float neard, float fard) {
  Matrix4x4 mm;

  float a1 = 2.0f / (right - left);
  float a2 = 2.0f / (top - bottom);  //IDK WY
  float a3 = -2.0f / (fard - neard); //IDK WY
  float t1 = (right + left) / (right - left) * -1.0f;
  float t2 = (top + bottom) / (top - bottom) * -1.0f;
  float t3 = (fard + neard) / (fard - neard) * -1.0f;


  //Row major order version
  //mm._m11 =a1, mm._m12 = 0, mm._m13 = 0, mm._m14 =t1,
  //mm._m21 = 0, mm._m22 =a2, mm._m23 = 0, mm._m24 =t2,
  //mm._m31 = 0, mm._m32 = 0, mm._m33 =a3, mm._m34 =t3,
  //mm._m41 = 0, mm._m42 = 0, mm._m43 = 0, mm._m44 = 1;

  // ** OpenGL version - the transpose of the former.
  mm._m11 = a1, mm._m12 = 0, mm._m13 = 0, mm._m14 = 0,
    mm._m21 = 0, mm._m22 = a2, mm._m23 = 0, mm._m24 = 0,
    mm._m31 = 0, mm._m32 = 0, mm._m33 = a3, mm._m34 = 0,
    mm._m41 = t1, mm._m42 = t2, mm._m43 = t3, mm._m44 = 1;

  return mm;
}

FORCE_INLINE void Matrix4x4::decompose(vec4& pos, mat4& rot, vec4& scale) const {
  //http://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
  //11  21  31  41 << Don't use gl order
  //12  22  32  42
  //13  23  33  43
  //14  24  34  44
  pos = getTranslationVector();

  scale.x = vec3(_m11, _m21, _m31).length();
  scale.y = vec3(_m12, _m22, _m32).length();
  scale.z = vec3(_m13, _m23, _m33).length();

  rot.setIdentity();
  rot._m11 = _m11 / scale.x;
  rot._m21 = _m21 / scale.x;
  rot._m31 = _m31 / scale.x;

  rot._m12 = _m12 / scale.y;
  rot._m22 = _m22 / scale.y;
  rot._m32 = _m32 / scale.y;

  rot._m13 = _m13 / scale.z;
  rot._m23 = _m23 / scale.z;
  rot._m33 = _m33 / scale.z;
}
FORCE_INLINE void Matrix4x4::decompose(vec4& pos, vec4& rot, vec4& scale, bool bDegreeRotation) const {
  mat4 mOut;
  decompose(pos, mOut, scale);

  Quaternion q = mOut.getQuaternion();

  q.getAxisAngle(rot);
  if (bDegreeRotation) {
    rot.w = MathUtils::radToDeg(rot.w);
  }

}
FORCE_INLINE bool Matrix4x4::parse(string_t tok, Matrix4x4& mOut) {
  // - Parse csv matrix string.

  size_t n = 0;
  char c;
  float mat[16];
  string_t val = "";
  int mat_ind = 0;

  while (n < tok.length()) {
    c = tok[n++];
    if (c == ',' || c == '\n' || n == tok.length()) {
      mat[mat_ind++] = TypeConv::strToFloat(val);
      val = "";
    }
    else if (isalnum(c) || c == '-' || c == '.' || c == '+' || c == 'e') {
      val += c;
    }
  }

  mOut = mat;

  return true;
}

STATIC FORCE_INLINE string_t tstr(const mat4& x) { return x.toString(); }



}

#endif