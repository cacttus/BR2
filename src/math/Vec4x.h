/**
*    @file Vec4x.h
*    @date November 30, 2008
*    @date November 27, 2015 - Vector4 is now Vec4x - templated
*    @author MetalMario971
*/
#pragma once
#ifndef __VEC4X_282629732193852125132500_H__
#define __VEC4X_282629732193852125132500_H__

#include "../math/MathHeader.h"

namespace Game {
/**
*    @struct Vec4x
*    @brief 4D vector class.
*/
template < class Tx >
class Vec4x : public PureMemory {
public:

    Tx x, y, z, w;

    FORCE_INLINE Vec4x<Tx>();
    FORCE_INLINE Vec4x<Tx>(const Tx& dx, const Tx& dy, const Tx& dz, const Tx& dw);
    //FORCE_INLINE Vec4x<Tx>( const Vec2x<Tx>& a );//DO NOT ALLOW IMPLICIT VEC4 CASTS
    //FORCE_INLINE Vec4x<Tx>( const Vec3x<Tx>& a );//DO NOT ALLOW IMPLICIT VEC4 CASTS
    FORCE_INLINE Vec4x<Tx>(const Vec2x<Tx>& a, const Tx& dz, const Tx& dw);
    FORCE_INLINE Vec4x<Tx>(const Vec2x<Tx>& a, const Vec2x<Tx>& b);
    FORCE_INLINE Vec4x<Tx>(const Vec3x<Tx>& a, const Tx& dw);
    FORCE_INLINE Vec4x<Tx>(const Vec4i& rhs);
    FORCE_INLINE Vec4x<Tx>(const Vec4ui& rhs);
    FORCE_INLINE Vec4x<Tx>(const Vec4f& rhs);
    FORCE_INLINE NOT_VIRTUAL ~Vec4x<Tx>() DOES_NOT_OVERRIDE {}

    //Aliases
    FORCE_INLINE Tx& r() { return x; }
    FORCE_INLINE Tx& g() { return y; }
    FORCE_INLINE Tx& b() { return z; }
    FORCE_INLINE Tx& a() { return w; }

    FORCE_INLINE const Tx& r() const { return x; }
    FORCE_INLINE const Tx& g() const { return y; }
    FORCE_INLINE const Tx& b() const { return z; }
    FORCE_INLINE const Tx& a() const { return w; }

    //Swizzle ops
    //FORCE_INLINE Vec4x<Tx>    xz();
    //FORCE_INLINE Vec4x<Tx>    xy();
    //
    FORCE_INLINE Tx            length() const;
    FORCE_INLINE Tx            length2() const;
    FORCE_INLINE Tx            squaredLength() const;

    FORCE_INLINE Vec4x<Tx>&   normalize();
    FORCE_INLINE Vec4x<Tx>    normalized();

    FORCE_INLINE Vec3x<Tx> xyz() const { return Vec3x<Tx>(x, y, z); }

    FORCE_INLINE Tx       dot(const Vec4x<Tx>& v1) const;
    FORCE_INLINE Tx       distance(const Vec4x<Tx>& v1) const;
    FORCE_INLINE Tx       distance2(const Vec4x<Tx>& v1) const;
    FORCE_INLINE void     construct(const Tx& dx, const Tx& dy, const Tx& dz, const Tx& dw);
    FORCE_INLINE Tx       combine() const;
    FORCE_INLINE string_t toString() const;

    FORCE_INLINE static Vec4x<Tx> zero();
    FORCE_INLINE static Vec4x<Tx> Vec4x_MIN();
    FORCE_INLINE static Vec4x<Tx> Vec4x_MAX();

    FORCE_INLINE static Vec4x<Tx> minv(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b);     // - Returns the minimum vector from the given two
    FORCE_INLINE static Vec4x<Tx> maxv(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b);     // returns maximujm vector of given t2o
    FORCE_INLINE static Vec4x<Tx> maxv_a(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b);   // returns maximujm vector of given t2o ABSOLUTE
    FORCE_INLINE static Tx maxf_a(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b);          // Returns the longest component of either vector (absolute)

    FORCE_INLINE bool operator==(const Vec4x<Tx> &v);
    FORCE_INLINE bool operator!=(const Vec4x<Tx> &v);

    FORCE_INLINE bool operator==(const Tx &f);
    FORCE_INLINE bool operator!=(const Tx &f);

    //remove unary negation
    FORCE_INLINE Vec4x<Tx> operator-() const;

    FORCE_INLINE Vec4x<Tx>& operator*=(const Matrix4x4 &m);
    FORCE_INLINE Vec4x<Tx> operator*(const Matrix4x4 &m);

    FORCE_INLINE Vec4x<Tx> & operator= (const Vec4x<Tx> &v);
    FORCE_INLINE Vec4x<Tx> & operator+=(const Vec4x<Tx> &v);
    FORCE_INLINE Vec4x<Tx> & operator-=(const Vec4x<Tx> &v);
    FORCE_INLINE Vec4x<Tx> & operator*=(const Vec4x<Tx> &v);
    FORCE_INLINE Vec4x<Tx> & operator/=(const Vec4x<Tx> &v);

    FORCE_INLINE Vec4x<Tx> & operator= (const Tx &f);
    FORCE_INLINE Vec4x<Tx> & operator+=(const Tx &f);
    FORCE_INLINE Vec4x<Tx> & operator-=(const Tx &f);
    FORCE_INLINE Vec4x<Tx> & operator*=(const Tx &f);
    FORCE_INLINE Vec4x<Tx> & operator/=(const Tx &f);

    FORCE_INLINE bool operator>(const Vec4x<Tx>& v);
    FORCE_INLINE bool operator>=(const Vec4x<Tx>& v);
    FORCE_INLINE bool operator<(const Vec4x<Tx>& v);
    FORCE_INLINE bool operator<=(const Vec4x<Tx>& v);

    FORCE_INLINE bool operator>(const Tx& f);
    FORCE_INLINE bool operator>=(const Tx& f);
    FORCE_INLINE bool operator<(const Tx& f);
    FORCE_INLINE bool operator<=(const Tx& f);

    //For some reason it MSVC doesn't like the scope resolution operator in front of templated methods with teh same return type
    FORCE_INLINE Vec4x<Tx> operator+(const Vec4x<Tx> &v) const;
    FORCE_INLINE Vec4x<Tx> operator-(const Vec4x<Tx> &v) const;
    FORCE_INLINE Vec4x<Tx> operator*(const Vec4x<Tx> &v) const;
    FORCE_INLINE Vec4x<Tx> operator/(const Vec4x<Tx> &v) const;

    FORCE_INLINE Vec4x<Tx> operator+(const Tx &f) const;
    FORCE_INLINE Vec4x<Tx> operator-(const Tx &f) const;
    FORCE_INLINE Vec4x<Tx> operator*(const Tx &f) const;
    FORCE_INLINE Vec4x<Tx> operator/(const Tx &f) const;

    //From Color4
    FORCE_INLINE Vec4x<Tx>& clampValues(const Tx& minVal = 0.0f, const Tx& maxVal = 1.0f);
    FORCE_INLINE Vec4x<Tx>& saturate();

    //std::Map sort Comparator
    struct Vec4xCompLess {
        FORCE_INLINE bool operator() (const Vec4x<Tx>* lhs, const Vec4x<Tx>* rhs) const {
            if (lhs->x < rhs->x) {
                return true;
            }
            else if (lhs->x > rhs->x) {
                return false;
            }
            else {
                if (lhs->y < rhs->y) {
                    return true;
                }
                else if (lhs->y > rhs->y) {
                    return false;
                }
                else {
                    if (lhs->z < rhs->z) {
                        return true;
                    }
                    else if (lhs->z > rhs->z) {
                        return false;
                    }
                    else {
                        if (lhs->w < rhs->w) {
                            return true;
                        }
                        else { //if(lhs->w > rhs->w) 
                            return false;
                        }
                    }
                }
            }
        }
    };

};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template < class Tx >
Vec4x<Tx>::Vec4x()
{
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Tx& dx, const Tx& dy, const Tx& dz, const Tx& dw)
{
    x = dx;
    y = dy;
    z = dz;
    w = dw;
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec4ui& rhs)
{
    x = (Tx)rhs.x;
    y = (Tx)rhs.y;
    z = (Tx)rhs.z;
    w = (Tx)rhs.w;
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec4i& rhs)
{
    x = (Tx)rhs.x;
    y = (Tx)rhs.y;
    z = (Tx)rhs.z;
    w = (Tx)rhs.w;
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec4f& rhs)
{
    x = (Tx)rhs.x;
    y = (Tx)rhs.y;
    z = (Tx)rhs.z;
    w = (Tx)rhs.w;
}
//DO NOT ALLOW IMPLICIT VEC4 CASTS
//template < class Tx >
//Vec4x<Tx>::Vec4x( const Vec2x<Tx>& a ){
//    x = (Tx)a.x; 
//    y = (Tx)a.y;
//}
//template < class Tx >
//Vec4x<Tx>::Vec4x( const Vec3x<Tx>& a ){
//    x = (Tx)a.x; 
//    y = (Tx)a.y; 
//    z = (Tx)a.z;
//}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec3x<Tx>& a, const Tx& dw) {
    x = (Tx)a.x;
    y = (Tx)a.y;
    z = (Tx)a.z;
    w = (Tx)dw;
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec2x<Tx>& a, const Vec2x<Tx>& b) {
    x = (Tx)a.x;
    y = (Tx)a.y;
    z = (Tx)b.x;
    w = (Tx)b.y;
}
template < class Tx >
Vec4x<Tx>::Vec4x(const Vec2x<Tx>& a, const Tx& dz, const Tx& dw) {
    x = (Tx)a.x;
    y = (Tx)a.y;
    z = (Tx)dz;
    w = (Tx)dw;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::minv(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b)
{
    Vec4x<Tx> out;

    out.x = min(v_a.x, v_b.x);
    out.y = min(v_a.y, v_b.y);
    out.z = min(v_a.z, v_b.z);
    out.w = min(v_a.w, v_b.w);

    return out;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::maxv(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b)
{
    Vec4x<Tx> out;

    out.x = max(v_a.x, v_b.x);
    out.y = max(v_a.y, v_b.y);
    out.z = max(v_a.z, v_b.z);
    out.w = max(v_a.w, v_b.w);

    return out;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::maxv_a(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b)
{
    Vec4x<Tx> out;

    out.x = max(fabs(v_a.x), fabs(v_b.x));
    out.y = max(fabs(v_a.y), fabs(v_b.y));
    out.z = max(fabs(v_a.z), fabs(v_b.z));
    out.w = max(fabs(v_a.w), fabs(v_b.w));

    return out;
}
template < class Tx >
Tx Vec4x<Tx>::maxf_a(const Vec4x<Tx>& v_a, const Vec4x<Tx>& v_b)
{
    Vec4x<Tx> tmp = maxv_a(v_a, v_b);
    return max(fabs(tmp.x),
        max(fabs(tmp.y),
            max(fabs(tmp.z), fabs(tmp.w))
        )
    );
}
//template < class Tx >
//Vec4x<Tx> Vec4x<Tx>::xz()
//{ 
//    return Vec4x<Tx>(x,0,z,0); 
//}
//template < class Tx >
//Vec4x<Tx> Vec4x<Tx>::xy()
//{ 
//    return Vec4x<Tx>(x,y,0,0); 
//}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::Vec4x_MIN()
{
    return Vec4x<Tx>(-COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m(), -COMP_MAX<Tx>::m());
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::Vec4x_MAX()
{
    return Vec4x<Tx>(COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m(), COMP_MAX<Tx>::m());
}
template < class Tx >
Tx    Vec4x<Tx>::length() const
{
    return MathUtils::broSqrt(x*x + y*y + z*z + w*w);
}
template < class Tx >
Tx    Vec4x<Tx>::length2() const
{
    return (x*x + y*y + z*z + w*w);
}
template < class Tx >
Tx Vec4x<Tx>::squaredLength() const
{
    return length2();
}
template < class Tx >
Vec4x<Tx>& Vec4x<Tx>::normalize()
{
    //we may be able to get away with rsqrt here...
    //but maybe not.
    Tx a = length();
    if (a != 0)
    {
        Tx a1 = 1.0f / a;
        x *= a1;
        y *= a1;
        z *= a1;
        w *= a1;
    }
    else
    {
        x = y = z = w = 0;
    }
    return *this;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::normalized()
{
    Vec4x<Tx> ret = *this;
    return ret.normalize();
}
template < class Tx >
Tx Vec4x<Tx>::dot(const Vec4x<Tx>& v) const
{
    return (x*v.x + y*v.y + z*v.z + w*v.w);
}
template < class Tx >
Tx Vec4x<Tx>::distance(const Vec4x<Tx>& v1) const
{
    return ((*this) - v1).length();
}
template < class Tx >
Tx Vec4x<Tx>::distance2(const Vec4x<Tx>& v1) const
{
    return ((*this) - v1).length2();
}
template < class Tx >
Tx Vec4x<Tx>::combine() const
{
    return x + y + z + w;
}
//Operators
template < class Tx >
bool Vec4x<Tx>::operator==(const Vec4x<Tx> &v)
{
    return(x == v.x ? (y == v.y ? (z == v.z ? (w == v.w ? 1 : 0) : 0) : 0) : 0);
}
template < class Tx >
bool Vec4x<Tx>::operator!=(const Vec4x<Tx> &v)
{
    return(x != v.x ? 1 : (y != v.y ? 1 : (z != v.z ? 1 : (w != v.w ? 1 : 0))));
}
template < class Tx >
bool Vec4x<Tx>::operator==(const Tx &f)
{
    return(x == f ? (y == f ? (z == f ? (w == f ? 1 : 0) : 0) : 0) : 0);
}
template < class Tx >
bool Vec4x<Tx>::operator!=(const Tx &f)
{
    return(x != f ? 1 : (y != f ? 1 : (z != f ? 1 : (w != f ? 1 : 0))));
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator-() const
{
    return Vec4x<Tx>(-x, -y, -z, -w);
}
template < class Tx >
Vec4x<Tx>& Vec4x<Tx>::operator*=(const Matrix4x4 &m)
{
    *this = (*this)*m;
    return *this;
}

template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator=(const Vec4x<Tx> &v)
{
    //Check for self-reassignment
    if (this == &v)
        return *this;
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator+=(const Vec4x<Tx> &v)
{
    if (this == &v)
        return *this;
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator-=(const Vec4x<Tx> &v)
{
    if (this == &v)
        return *this;
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator*=(const Vec4x<Tx> &v)
{
    if (this == &v)
        return *this;
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator/=(const Vec4x<Tx> &v)
{
    if (this == &v)
        return *this;
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
    return *this;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator=(const Tx &f)
{
    x = f;
    y = f;
    z = f;
    w = f;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator+=(const Tx &f)
{
    x += f;
    y += f;
    z += f;
    w += f;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator-=(const Tx &f)
{
    x -= f;
    y -= f;
    z -= f;
    w -= f;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator*=(const Tx &f)
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}
template < class Tx >
Vec4x<Tx> & Vec4x<Tx>::operator/=(const Tx &f)
{
    x /= f;
    y /= f;
    z /= f;
    w /= f;
    return *this;
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator+(const Vec4x<Tx> &v) const
{
    Vec4x<Tx> tmp = *this;
    tmp += v;
    return tmp;
}

template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator-(const Vec4x<Tx> &v) const
{
    Vec4x<Tx> tmp = *this;
    tmp -= v;
    return tmp;
}

template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator*(const Vec4x<Tx> &v) const
{
    Vec4x<Tx> tmp = *this;
    tmp *= v;
    return tmp;
}

template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator/(const Vec4x<Tx> &v) const
{
    Vec4x<Tx> tmp = *this;
    tmp /= v;
    return tmp;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator+(const Tx &f) const
{
    Vec4x<Tx> tmp = *this;
    tmp += f;
    return tmp;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator-(const Tx &f) const
{
    Vec4x<Tx> tmp = *this;
    tmp -= f;
    return tmp;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator*(const Tx &f) const
{
    Vec4x<Tx> tmp = *this;
    tmp *= f;
    return tmp;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator/(const Tx &f) const
{
    Vec4x<Tx> tmp = *this;
    tmp /= f;
    return tmp;
}

//
//
//
template < class Tx >
bool Vec4x<Tx>::operator>(const Vec4x<Tx>& v)
{
    return (x > v.x && y > v.y && z > v.z && w > v.w);
}
template < class Tx >
bool Vec4x<Tx>::operator>=(const Vec4x<Tx>& v)
{
    return (x >= v.x && y >= v.y && z >= v.z && w >= v.w);
}
template < class Tx >
bool Vec4x<Tx>::operator<(const Vec4x<Tx>& v)
{
    return (x < v.x && y < v.y && z < v.z && w < v.w);
}
template < class Tx >
bool Vec4x<Tx>::operator<=(const Vec4x<Tx>& v)
{
    return (x <= v.x && y <= v.y && z <= v.z && w <= v.w);
}

//
//
// 
template < class Tx >
bool Vec4x<Tx>::operator>(const Tx& f)
{
    return (x > f&&y > f&&z > f&&w > f);
}
template < class Tx >
bool Vec4x<Tx>::operator>=(const Tx& f)
{
    return (x >= f&&y >= f&&z >= f&&w >= f);
}
template < class Tx >
bool Vec4x<Tx>::operator<(const Tx& f)
{
    return (x < f&&y < f&&z < f&&w < f);
}
template < class Tx >
bool Vec4x<Tx>::operator<=(const Tx& f)
{
    return (x <= f&&y <= f&&z <= f&&w <= f);
}


template < class Tx >
string_t Vec4x<Tx>::toString() const
{
    return Stz x + "," + y + "," + z + "," + w;
}

// Constructors
template < class Tx >
void Vec4x<Tx>::construct(const Tx& dx, const Tx& dy, const Tx& dz, const Tx& dw)
{
    x = dx;
    y = dy;
    z = dz;
    w = dw;
}
template < class Tx >
Vec4x<Tx> Vec4x<Tx>::zero()
{
    return Vec4x<Tx>(0, 0, 0, 0);
}
template < class Tx >
Vec4x<Tx>& Vec4x<Tx>::clampValues(const Tx& minVal, const Tx& maxVal)
{
    r() = MathUtils::broClamp(r(), minVal, maxVal);
    g() = MathUtils::broClamp(g(), minVal, maxVal);
    b() = MathUtils::broClamp(b(), minVal, maxVal);
    a() = MathUtils::broClamp(a(), minVal, maxVal);

    return *this;
}
template < class Tx >
Vec4x<Tx>& Vec4x<Tx>::saturate()
{
    float cr = (float)r();
    float cg = (float)g();
    float cb = (float)b();

    float* ff;
    if (cr > cg)
        ff = &cr;
    else
        ff = &cg;

    if (cg > cb)
        ff = &cg;
    else
        ff = &cb;

    float diff = 1.0f - (*ff);
    r() = broMin(1.0f, cr + diff);
    g() = broMin(1.0f, cg + diff);
    b() = broMin(1.0f, cb + diff);

    return *this;
}



// - Vector shorthands

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
STATIC FORCE_INLINE string_t tstr(const vec4& x) { return x.toString(); }
STATIC FORCE_INLINE string_t tstr(const ivec4& x) { return x.toString(); }
STATIC FORCE_INLINE string_t tstr(const uvec4& x) { return x.toString(); }
//FORCE_INLINE Vec4f normalize(const Vec4f& v1) {return Vec4f(v1).normalize();}
//FORCE_INLINE float dot(const Vec4f& v1, const Vec4f& v2) {return Vec4f(v1).dot(Vec4f(v2));}
//



}//ns agme

#endif
