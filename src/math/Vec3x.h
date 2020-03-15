/**
*  @file Vec3x.h
*  @date August 24, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __VEC3I_1793212932844915478412_H__
#define __VEC3I_1793212932844915478412_H__

#include "../math/MathHeader.h"

namespace BR2 {
/**
*  @struct Vec3x
*  @brief 3 Component Generic vector
*  @details A structure to deal with vectors in 3 dimensions.
*/
template < class Tx >
class Vec3x : public PureMemory {
public:
  Tx x, y, z;

  FORCE_INLINE Vec3x<Tx>();
  FORCE_INLINE Vec3x<Tx>(const Tx& dx, const Tx& dy, const Tx& dz);
  FORCE_INLINE Vec3x<Tx>(const Vec2x<Tx>& a);
  FORCE_INLINE Vec3x<Tx>(const Vec4x<Tx>& a);
  FORCE_INLINE Vec3x<Tx>(const Vec3i& rhs);
  FORCE_INLINE Vec3x<Tx>(const Vec3ui& rhs);
  FORCE_INLINE Vec3x<Tx>(const Vec3f& rhs);
  FORCE_INLINE Vec3x<Tx>(const Vec3d& rhs);
  FORCE_INLINE Vec3x<Tx>(const Vec3ub& rhs);
  FORCE_INLINE NOT_VIRTUAL ~Vec3x<Tx>() DOES_NOT_OVERRIDE {}

  //Aliases
  FORCE_INLINE Tx& u() { return x; }
  FORCE_INLINE Tx& v() { return y; }
  FORCE_INLINE Tx& w() { return z; }
  FORCE_INLINE Tx& r() { return x; }
  FORCE_INLINE Tx& g() { return y; }
  FORCE_INLINE Tx& b() { return z; }

  FORCE_INLINE const Tx& u() const { return x; }
  FORCE_INLINE const Tx& v() const { return y; }
  FORCE_INLINE const Tx& w() const { return z; }
  FORCE_INLINE const Tx& r() const { return x; }
  FORCE_INLINE const Tx& g() const { return y; }
  FORCE_INLINE const Tx& b() const { return z; }

  //Swizzle ops
  FORCE_INLINE Vec3x<Tx> xz();
  FORCE_INLINE Vec3x<Tx> xy();
  
  FORCE_INLINE Tx length() const;
  FORCE_INLINE Tx length2() const;
  FORCE_INLINE double lengthd() const;
  FORCE_INLINE Tx squaredLength() const;
  FORCE_INLINE void len_and_norm(vec3& __out_ n, Tx& __out_ len) const;

  FORCE_INLINE Vec3x<Tx>& normalize();
  FORCE_INLINE Vec3x<Tx>& normalize(Tx len);
  FORCE_INLINE Vec3x<Tx> normalized() const;
  FORCE_INLINE Vec3x<Tx> abs() const;

  FORCE_INLINE Tx dot(const Vec3x<Tx>& v1) const;
  FORCE_INLINE Vec3x<Tx> cross(const Vec3x<Tx>& v1) const;
  FORCE_INLINE Tx distance(const Vec3x<Tx>& v1) const;
  FORCE_INLINE Tx distance2(const Vec3x<Tx>& v1) const;
  FORCE_INLINE void construct(Tx dx, Tx dy, Tx dz);
  FORCE_INLINE void construct(const Vec3x<Tx>& rhs);
  FORCE_INLINE Tx combine() const;
  FORCE_INLINE Vec3x<Tx> lerpTo(const Vec3x<Tx>& v1, const Tx t) const;
  FORCE_INLINE Vec3x<Tx> clampTo(const Vec3x<Tx>& vMin, const Vec3x<Tx>& vMax) const;

  FORCE_INLINE string_t toString(int precis = -1) const;

  FORCE_INLINE static Vec3x<Tx> zero();
  FORCE_INLINE static Vec3x<Tx> VEC3X_MIN();
  FORCE_INLINE static Vec3x<Tx> VEC3X_MAX();

  FORCE_INLINE static Vec3x<Tx> minv(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b);    // - Returns the minimum vector from the given two
  FORCE_INLINE static Vec3x<Tx> maxv(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b);    // returns maximujm vector of given t2o
  FORCE_INLINE static Vec3x<Tx> maxv_a(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b);    // returns maximujm vector of given t2o ABSOLUTE
  FORCE_INLINE static Tx maxf_a(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b); // Returns the longest component of either vector (absolute)

  FORCE_INLINE bool operator==(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool operator!=(const Vec3x<Tx>& v) const;

  FORCE_INLINE bool operator==(const Tx& f) const;
  FORCE_INLINE bool operator!=(const Tx& f) const;

  //remove unary negation
  FORCE_INLINE Vec3x<Tx> operator-() const;

  FORCE_INLINE Vec3x<Tx>& operator*=(const Matrix3x3& m);
  FORCE_INLINE Vec3x<Tx> operator*(const Matrix3x3& m);

  FORCE_INLINE Vec3x<Tx>& operator= (const Vec3x<Tx>& v);
  FORCE_INLINE Vec3x<Tx>& operator+=(const Vec3x<Tx>& v);
  FORCE_INLINE Vec3x<Tx>& operator-=(const Vec3x<Tx>& v);
  FORCE_INLINE Vec3x<Tx>& operator*=(const Vec3x<Tx>& v);
  FORCE_INLINE Vec3x<Tx>& operator/=(const Vec3x<Tx>& v);

  FORCE_INLINE Vec3x<Tx>& operator= (const Tx& f);
  FORCE_INLINE Vec3x<Tx>& operator+=(const Tx& f);
  FORCE_INLINE Vec3x<Tx>& operator-=(const Tx& f);
  FORCE_INLINE Vec3x<Tx>& operator*=(const Tx& f);
  FORCE_INLINE Vec3x<Tx>& operator/=(const Tx& f);

  FORCE_INLINE bool operator>(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool operator>=(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool operator<(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool operator<=(const Vec3x<Tx>& v) const;

  FORCE_INLINE bool operator>(const Tx& f) const;
  FORCE_INLINE bool operator>=(const Tx& f) const;
  FORCE_INLINE bool operator<(const Tx& f) const;
  FORCE_INLINE bool operator<=(const Tx& f) const;

  //TODO:
  //const Vec3x<Tx> operator*(const Mat3<Tx> &m) const;

  //For some reason it MSVC doesn't like the scope resolution operator in front of templated methods with teh
  // same return type
  FORCE_INLINE Vec3x<Tx> operator+(const Vec3x<Tx>& v) const;
  FORCE_INLINE Vec3x<Tx> operator-(const Vec3x<Tx>& v) const;
  FORCE_INLINE Vec3x<Tx> operator*(const Vec3x<Tx>& v) const;
  FORCE_INLINE Vec3x<Tx> operator/(const Vec3x<Tx>& v) const;

  FORCE_INLINE Vec3x<Tx> operator+(const Tx& f) const;
  FORCE_INLINE Vec3x<Tx> operator-(const Tx& f) const;
  FORCE_INLINE Vec3x<Tx> operator*(const Tx& f) const;
  FORCE_INLINE Vec3x<Tx> operator/(const Tx& f) const;

  //From Color3
  FORCE_INLINE uint32_t toUint();
  FORCE_INLINE void fromUint(const uint32_t& i);

  FORCE_INLINE bool compareTo(const Vec3x<Tx>* rhs) const; //Binary compare two vectors and return the closer in the sort order.

  //std::Map sort Comparator
  struct Vec3xCompLess {
    FORCE_INLINE bool operator() (const Vec3x<Tx>* lhs, const Vec3x<Tx>* rhs) const {
      return lhs->compareTo(rhs);
    }
  };

  // - Vector shorthands
  FORCE_INLINE STATIC Vec3x<Tx> normalize(const Vec3x<Tx>& v1);
  FORCE_INLINE STATIC Vec3x<Tx> cross(const Vec3x<Tx>& v1, const Vec3x<Tx>& v2);
  //   FORCE_INLINE STATIC Vec3x<Tx> rotate(const Vec3x<Tx>& v1, const float& angle, const Vec3x<Tx>& aboutNormal);
  FORCE_INLINE STATIC Tx dot(const Vec3x<Tx>& v1, const Vec3x<Tx>& v2);
  FORCE_INLINE STATIC void bilinear_interpolate(const Vec3x<Tx>& a,
    const Vec3x<Tx>& b,
    const Vec3x<Tx>& c,
    const Vec3x<Tx>& d,
    Vec3x<Tx>& __out_ avg,
    float pct = 0.5f
  );
  FORCE_INLINE STATIC void reflect(const Vec3x<Tx>& v, const Vec3x<Tx>& n, Vec3x<Tx>& __out_ v_r);


  void checkNormalOrZero(); // Set number to zero if it is not normal (component-wise)
  void checkNormalOrZeroAndLimitVector(float fMaxVectorLength, bool bShowWarningMessage); // formats the number to zero if it is not normal, also truncates the vector if it's too long. 
  bool isNormalFloat(); //throw if the number is not normal
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template < class Tx >
Vec3x<Tx>::Vec3x() {
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Tx& dx, const Tx& dy, const Tx& dz) {
  x = (Tx)dx;
  y = (Tx)dy;
  z = (Tx)dz;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec3ui& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec3i& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec3f& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec3d& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec3ub& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec2x<Tx>& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = 0; // This isn't correct. The problem is we get auto casted when we add.
}
template < class Tx >
Vec3x<Tx>::Vec3x(const Vec4x<Tx>& rhs) {
  x = (Tx)rhs.x;
  y = (Tx)rhs.y;
  z = (Tx)rhs.z;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::minv(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b) {
  Vec3x<Tx> out;

  out.x = MathUtils::brMin(v_a.x, v_b.x);
  out.y = MathUtils::brMin(v_a.y, v_b.y);
  out.z = MathUtils::brMin(v_a.z, v_b.z);

  return out;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::maxv(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b) {
  Vec3x<Tx> out;

  out.x = MathUtils::brMax(v_a.x, v_b.x);
  out.y = MathUtils::brMax(v_a.y, v_b.y);
  out.z = MathUtils::brMax(v_a.z, v_b.z);

  return out;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::maxv_a(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b) {
  Vec3x<Tx> out;

  out.x = max(fabs(v_a.x), fabs(v_b.x));
  out.y = max(fabs(v_a.y), fabs(v_b.y));
  out.z = max(fabs(v_a.z), fabs(v_b.z));

  return out;
}
template < class Tx >
Tx Vec3x<Tx>::maxf_a(const Vec3x<Tx>& v_a, const Vec3x<Tx>& v_b) {
  Vec3x<Tx> tmp = maxv_a(v_a, v_b);
  return max(fabs(tmp.x), max(fabs(tmp.y), fabs(tmp.z)));
}
template < class Tx >
Vec3x<Tx>    Vec3x<Tx>::xz() {
  return Vec3x<Tx>(x, 0, z);
}
template < class Tx >
Vec3x<Tx>    Vec3x<Tx>::xy() {
  return Vec3x<Tx>(x, y, 0);
}

template < class Tx >
Vec3x<Tx> Vec3x<Tx>::VEC3X_MIN() {
  return Vec3x<Tx>(-COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m());
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::VEC3X_MAX() {
  return Vec3x<Tx>(COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m());
}
//#define VEC3_MIN (Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX))
//#define VEC3_MAX (Vec3f(FLT_MAX,FLT_MAX,FLT_MAX))

template < class Tx >
Tx Vec3x<Tx>::length() const {
  return MathUtils::brSqrt(x * x + y * y + z * z);
}
template < class Tx >
double Vec3x<Tx>::lengthd() const {
  double dx = (double)x;
  double dy = (double)y;
  double dz = (double)z;
  
  return MathUtils::brSqrt(dx * dx + dy * dy + dz * dz);
}
template < class Tx >
Tx    Vec3x<Tx>::length2() const {
  return (x * x + y * y + z * z);
}
template < class Tx >
Tx Vec3x<Tx>::squaredLength() const {
  return length2();
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::normalize() {
  //we may be able to get away with rsqrt here...
  //but maybe not.
  Tx a = length();
  return normalize(a);

  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::normalize(Tx len) {
  //we may be able to get away with rsqrt here...
  //but maybe not.
  // We should allow the float to hit infinity if we try to divide zero
  if (len != 0) {
    Tx a1 = 1.0f / len;
    x *= a1;
    y *= a1;
    z *= a1;
  }
  else {
    x = y = z = 0;
  }
  return *this;
}
template < class Tx >
void Vec3x<Tx>::len_and_norm(vec3& __out_ n, Tx& __out_ len) const {
  //Computes length and normal to avoid having do do len() then norm()
  len = length();
  n = *this;
  n.normalize(len);
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::normalized() const {
  Vec3x<Tx> ret = *this;
  return ret.normalize();
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::abs() const {
  Vec3x<Tx> ret = *this;
  ret.x = fabsf(ret.x);
  ret.y = fabsf(ret.y);
  ret.z = fabsf(ret.z);
  return ret;
}
template < class Tx >
Tx Vec3x<Tx>::dot(const Vec3x<Tx>& v) const {
  return (x * v.x + y * v.y + z * v.z);
}
template < class Tx >
Tx Vec3x<Tx>::distance(const Vec3x<Tx>& v1) const {
  return ((*this) - v1).length();
}
template < class Tx >
Tx Vec3x<Tx>::distance2(const Vec3x<Tx>& v1) const {
  return ((*this) - v1).length2();
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::cross(const Vec3x<Tx>& v1) const {
  Vec3x<Tx> vt;
  vt.x = (y * v1.z) - (v1.y * z);
  vt.y = (z * v1.x) - (v1.z * x);
  vt.z = (x * v1.y) - (v1.x * y);

  return vt;
}
template < class Tx >
Vec3x<Tx>  Vec3x<Tx>::lerpTo(const Vec3x<Tx>& v1, const Tx t) const {
  vec3 ret = *this + (v1 - *this) * t;
  return ret;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::clampTo(const Vec3x<Tx>& vMin, const Vec3x<Tx>& vMax) const {
  //Technically we can just use the #define for clamp() to get the same result.
  //but my brain isn't working right now and i want to see this line for line
  Vec3x<Tx> out = *this;

  if (out.x < vMin.x) { out.x = vMin.x; }
  if (out.y < vMin.y) { out.y = vMin.y; }
  if (out.z < vMin.z) { out.z = vMin.z; }

  if (out.x > vMax.x) { out.x = vMax.x; }
  if (out.y > vMax.y) { out.y = vMax.y; }
  if (out.z > vMax.z) { out.z = vMax.z; }

  return out;
}

template < class Tx >
Tx Vec3x<Tx>::combine() const {
  return x + y + z;
}
//Operators
template < class Tx >
bool Vec3x<Tx>::operator==(const Vec3x<Tx>& v) const {
  return(x == v.x ? (y == v.y ? (z == v.z ? 1 : 0) : 0) : 0);
}
template < class Tx >
bool Vec3x<Tx>::operator!=(const Vec3x<Tx>& v) const {
  return(x != v.x ? 1 : (y != v.y ? 1 : (z != v.z ? 1 : 0)));
}


template < class Tx >
bool Vec3x<Tx>::operator==(const Tx& f) const {
  return(x == f ? (y == f ? (z == f ? 1 : 0) : 0) : 0);
}
template < class Tx >
bool Vec3x<Tx>::operator!=(const Tx& f) const {
  return(x != f ? 1 : (y != f ? 1 : (z != f ? 1 : 0)));
}

template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator-() const {
  return Vec3x<Tx>(-x, -y, -z);
}
//

//
// row vector times matrix
//
//template < class Tx >
//Vec3x<Tx> & Vec3x<Tx>::operator*=( const Mat3<Tx>& m )
//{
//    x=m._m11*x + m._m12*y + m._m13*z;
//    y=m._m21*x + m._m22*y + m._m23*z;
//    z=m._m31*x + m._m32*y + m._m33*z;
//    return *this;
//}
//
//
//
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator=(const Vec3x<Tx>& v) {
  //Check for self-reassignment
  if (this == &v) return *this;

  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator+=(const Vec3x<Tx>& v) {
  if (this == &v) return *this;
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator-=(const Vec3x<Tx>& v) {
  if (this == &v) return *this;
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator*=(const Vec3x<Tx>& v) {
  if (this == &v) return *this;
  x *= v.x;
  y *= v.y;
  z *= v.z;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator/=(const Vec3x<Tx>& v) {
  if (this == &v) return *this;
  x /= v.x;
  y /= v.y;
  z /= v.z;
  return *this;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator=(const Tx& f) {
  x = f;
  y = f;
  z = f;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator+=(const Tx& f) {
  x += f;
  y += f;
  z += f;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator-=(const Tx& f) {
  x -= f;
  y -= f;
  z -= f;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator*=(const Tx& f) {
  x *= f;
  y *= f;
  z *= f;
  return *this;
}
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator/=(const Tx& f) {
  x /= f;
  y /= f;
  z /= f;
  return *this;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator+(const Vec3x<Tx>& v) const {
  Vec3x<Tx> tmp = *this;
  tmp += v;
  return tmp;
}

template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator-(const Vec3x<Tx>& v) const {
  Vec3x<Tx> tmp = *this;
  tmp -= v;
  return tmp;
}

template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator*(const Vec3x<Tx>& v) const {
  Vec3x<Tx> tmp = *this;
  tmp *= v;
  return tmp;
}

template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator/(const Vec3x<Tx>& v) const {
  Vec3x<Tx> tmp = *this;
  tmp /= v;
  return tmp;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator+(const Tx& f) const {
  Vec3x<Tx> tmp = *this;
  tmp += f;
  return tmp;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator-(const Tx& f) const {
  Vec3x<Tx> tmp = *this;
  tmp -= f;
  return tmp;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator*(const Tx& f) const {
  Vec3x<Tx> tmp = *this;
  tmp *= f;
  return tmp;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator/(const Tx& f) const {
  Vec3x<Tx> tmp = *this;
  tmp /= f;
  return tmp;
}

//
//
//
template < class Tx >
bool Vec3x<Tx>::operator>(const Vec3x<Tx>& v) const {
  return (x > v.x&& y > v.y&& z > v.z);
}
template < class Tx >
bool Vec3x<Tx>::operator>=(const Vec3x<Tx>& v) const {
  return (x >= v.x && y >= v.y && z >= v.z);
}
template < class Tx >
bool Vec3x<Tx>::operator<(const Vec3x<Tx>& v) const {
  return (x < v.x && y < v.y && z < v.z);
}
template < class Tx >
bool Vec3x<Tx>::operator<=(const Vec3x<Tx>& v) const {
  return (x <= v.x && y <= v.y && z <= v.z);
}

//
//
//
template < class Tx >
bool Vec3x<Tx>::operator>(const Tx& f) const {
  return (x > f&& y > f&& z > f);
}
template < class Tx >
bool Vec3x<Tx>::operator>=(const Tx& f) const {
  return (x >= f && y >= f && z >= f);
}
template < class Tx >
bool Vec3x<Tx>::operator<(const Tx& f) const {
  return (x < f && y < f && z < f);
}
template < class Tx >
bool Vec3x<Tx>::operator<=(const Tx& f) const {
  return (x <= f && y <= f && z <= f);
}



// Constructors
template < class Tx >
void Vec3x<Tx>::construct(const Vec3x<Tx>& rhs) {
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
}
template < class Tx >
void Vec3x<Tx>::construct(Tx dx, Tx dy, Tx dz) {
  x = dx;
  y = dy;
  z = dz;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::zero() {
  return Vec3x<Tx>(0, 0, 0);
}

template < class Tx >
FORCE_INLINE uint32_t Vec3x<Tx>::toUint() {
  uint32_t ret = (uint32_t)(
    ((uint32_t)0 << 16) |
    ((uint32_t)r() << 16) |
    ((uint32_t)g() << 8) |
    ((uint32_t)b())
    );
  return ret;
}
template < class Tx >
FORCE_INLINE void Vec3x<Tx>::fromUint(const uint32_t& i) {
  r() = (i >> 16) & 0xFF;
  g() = (i >> 8) & 0xFF;
  b() = (i) & 0xFF;
}
template < class Tx >
bool Vec3x<Tx>::compareTo(const Vec3x<Tx>* rhs) const {
  const Vec3x<Tx>* lhs = this;

  if (lhs->x < rhs->x)
    return true;
  else if (lhs->x > rhs->x)
    return false;
  else {
    if (lhs->y < rhs->y)
      return true;
    else if (lhs->y > rhs->y)
      return false;
    else {
      if (lhs->z < rhs->z)
        return true;
      else //if(lhs->z > rhs->z) 
        return false;
    }
  }
}

//template < class Tx >
//Vec3x<Tx>::Vec3x( const Vec3d& rhs )
//{
//    x = (Tx)rhs.x; 
//    y = (Tx)rhs.y; 
//    z = (Tx)rhs.z;
//}

// - Vector shorthands
template < typename Tx >
Vec3x<Tx> Vec3x<Tx>::normalize(const Vec3x<Tx>& v1) {
  return Vec3x<Tx>(v1).normalize();
}
template < typename Tx >
Vec3x<Tx> Vec3x<Tx>::cross(const Vec3x<Tx>& v1, const Vec3x<Tx>& v2) {
  return Vec3x<Tx>(v1).cross(Vec3x<Tx>(v2));
}
//template < typename Tx >
//Vec3x<Tx> Vec3x<Tx>::rotate(const Vec3x<Tx>& v1, const float& angle, const Vec3x<Tx>& normal)
//{
//    //TODO: test.
//    mat3 m = mat3::getRotationRad(angle, normal);
//    Vec3x<Tx> ret = v1;
//    ret = ret*m;
//    return ret;
//}
template < typename Tx >
Tx Vec3x<Tx>::dot(const Vec3x<Tx>& v1, const Vec3x<Tx>& v2) {
  return Vec3x<Tx>(v1).dot(Vec3x<Tx>(v2));
}
template < typename Tx >
void Vec3x<Tx>::bilinear_interpolate(
  const Vec3x<Tx>& a,
  const Vec3x<Tx>& b,
  const Vec3x<Tx>& c,
  const Vec3x<Tx>& d,
  Vec3x<Tx>& __out_ avg,
  float pct
) {
  Vec3x<Tx> v1, v2, v3;
  v1 = a + (b - a) * pct;
  v2 = c + (d - c) * pct;
  avg = v1 + (v2 - v1) * pct;
}
template < typename Tx >
void Vec3x<Tx>::reflect(const Vec3x<Tx>& v, const Vec3x<Tx>& n, Vec3x<Tx>& __out_ v_r) {
  v_r = v - (n * n.dot(v)) * 2.0f;
}
template < typename Tx >
void Vec3x<Tx>::checkNormalOrZero() {
  //Make sure the number is a normal FP number
  int cx = std::fpclassify(x);
  int cy = std::fpclassify(y);
  int cz = std::fpclassify(z);
  if (cx != FP_ZERO && cx != FP_NORMAL)
    x = 0.0f;
  if (cy != FP_ZERO && cy != FP_NORMAL)
    y = 0.0f;
  if (cz != FP_ZERO && cz != FP_NORMAL)
    z = 0.0f;
}
template < typename Tx >
bool Vec3x<Tx>::isNormalFloat() {
  bool b = true;

  //Make sure the number is a normal FP number
  int cx = std::fpclassify(x);
  int cy = std::fpclassify(y);
  int cz = std::fpclassify(z);
  //NAN
  if (cx == FP_NAN) { b = false; }
  if (cy == FP_NAN) { b = false; }
  if (cz == FP_NAN) { b = false; }
  ////DEN
  //If the number is too small who cares. Let it round to zero.
  //AssertOrThrow2(cx!= FP_SUBNORMAL);
  //AssertOrThrow2(cy!= FP_SUBNORMAL);
  //AssertOrThrow2(cz!= FP_SUBNORMAL);
  //INF
  if (cx == FP_INFINITE) { b = false; }
  if (cy == FP_INFINITE) { b = false; }
  if (cz == FP_INFINITE) { b = false; }

  return b;
}
template < typename Tx >
void Vec3x<Tx>::checkNormalOrZeroAndLimitVector(float fMaxLength, bool bShowWarningMessage) {
  //Normalize number
  checkNormalOrZero();

  // Make sure the vector length isn't too big.
  if (squaredLength() >= (fMaxLength * fMaxLength)) {
    if (bShowWarningMessage == true)
      BRLogWarn("Object has launched into orbit: v=("
        , x, " "
        , y, " "
        , z, ")");
    *this = normalized() * fMaxLength;
  }
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
STATIC FORCE_INLINE string_t tstr(const vec3& x) { return x.toString(); }
STATIC FORCE_INLINE string_t tstr(const ivec3& x) { return x.toString(); }
STATIC FORCE_INLINE string_t tstr(const uvec3& x) { return x.toString(); }



}//ns game


#endif
