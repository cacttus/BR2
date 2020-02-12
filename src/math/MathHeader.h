/**
*
*    @file MathHeader.h
*    @date November 29, 2014
*    @author MetalMario971
*
*    © 2014
*
*
*/
#pragma once
#ifndef __MATHHEADER_24654626514582104228509_H__
#define __MATHHEADER_24654626514582104228509_H__

//No includes
#include "../base/BaseHeader.h"
namespace BR2 {
//////////////////////////////////////////////////////////////////////////

//define for float instead of double data structres
#define _MATH_USE_FLOAT_VECTORS 1
#define _MATH_USE_FLOAT_MATRICES 1
// - If this is defined then any undefined operations will throw an erorr.
#define _MATH_USE_ROW_VECTORS 1

#ifndef _MATH_DEFINES_DEFINED
#define _MATH_DEFINES_DEFINED
#ifndef M_E
#define M_E            2.71828182845904523536
#endif
#ifndef M_LOG2E
#define M_LOG2E        1.44269504088896340736
#endif
#ifndef M_LOG10E
#define M_LOG10E    0.434294481903251827651
#endif
#ifndef M_LN2
#define M_LN2        0.693147180559945309417
#endif
#ifndef M_LN10
#define M_LN10        2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_2PI
#define M_2PI        6.283185307179586476925286766559
#endif
#ifndef M_PI_2
#define M_PI_2        1.57079632679489661923
#endif
#ifndef M_PI_4
#define M_PI_4        0.785398163397448309616
#endif
#ifndef M_1_PI
#define M_1_PI        0.318309886183790671538
#endif
#ifndef M_2_PI
#define M_2_PI        0.636619772367581343076
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI    1.12837916709551257390
#endif
#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2    0.707106781186547524401
#endif

#define M_SIN45        0.70710678118654752440084436210485
#endif

#define PERCENT(x) ((x)*0.01f)

//*** Matrix Multiplication Order
#define MATRIX_ORDER(a,b) ((b)*(a))

#define FUZZY_EQUALS_EPSILON (0.0002f)

//See coordinate system defined in BuildConfig.h
#ifdef COORDINATE_SYSTEM_LHS
#define COORDINATE_SYSTEM COORDINATE_SYSTEM_LHS
#else
#define COORDINATE_SYSTEM COORDINATE_SYSTEM_RHS
#endif

#ifdef COORDINATE_SYSTEM_LHS
#ifdef COORDINATE_SYSTEM_RHS
    //You cannot have two coordinate systems dude
1 / 0
#endif
#endif

#define VEC3_MIN (Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX))
#define VEC3_MAX (Vec3f(FLT_MAX,FLT_MAX,FLT_MAX))
//////////////////////////////////////////////////////////////////////////

namespace CollisionResult { typedef enum { Stuck, Avoided, Collided } e; }
namespace box_point_index { typedef enum { box_nbr, box_nbl, box_ntr, box_ntl, box_fbr, box_fbl, box_ftr, box_ftl }e; }
namespace box_plane_index { typedef enum { bp_near, bp_far, bp_left, bp_right, bp_top, bp_bottom }e; }
//2018/1/22 changed thiss
//Face_Right, Face_Left, Face_Bottom, Face_Top, Face_Back, Face_Front
namespace BoxSide { typedef enum { Right, Left, Bottom, Top, Back, Front }e; }
//////////////////////////////////////////////////////////////////////////
//FWDDCL
template < class Tx >
class Vec2x;
template < class Tx >
class Vec3x;
template < class Tx >
class Vec4x;
template < class Tx >
class Box2x;
template < class Tx >
class Box3x;
template < class Tx >
class Quad3x;
template < class Tx >
class MinimaxParam;

class Matrix4x4;
class Line;
class Line3f;
class Plane3f;
class Matrix3x3;
class Polar3;
class Poly3f;
class QuadraticAttenuator;
class QuadPlane;
class Plane3f;
class ProjectedRay;
class Sphere3f;
class Hash;
class Polar3;
class Sphere3f;
class Vec3Basis;
class Quaternion;
class Noise;
class SimplexNoise;
template< class Tx >
class MatrixStack3x;
class RaycastHitList;
class v_v3n3x2;
class MpFloat;
class  MpVec3;
class MpVec3i;
class MpVec4;
class  MpUint;
class  MpInt;
//TYPEDEFS

typedef Vec2x<double> Vec2d;
typedef Vec2x<float> Vec2f;
typedef Vec2x<t_byte> Vec2ub;
typedef Vec2x<int32_t> Vec2i;
typedef Vec2x<uint32_t> Vec2ui;
typedef Vec3x<double> Vec3d;
typedef Vec3x<float> Vec3f;
typedef Vec3x<t_byte> Vec3ub;
typedef Vec3x<int32_t> Vec3i;
typedef Vec3x<uint32_t> Vec3ui;
typedef Vec4x<double> Vec4d;
typedef Vec4x<float> Vec4f;
typedef Vec4x<t_byte> Vec4ub;
typedef Vec4x<int32_t> Vec4i;
typedef Vec4x<uint32_t> Vec4ui;
typedef Box3x<int32_t> Box3i32;
typedef Box3x<float> Box3f;
typedef Box2x<float> Box2f;
typedef Box2x<int32_t> Box2i;
typedef Quad3x<float> Quad3f; //Basically this would be the same
typedef Vec3f Vector3;
typedef Vec4f Vector4;
typedef Matrix4x4 Mat44;
typedef Matrix3x3 Mat33;
typedef MatrixStack3x<Mat44> Mat4Stack;
typedef Box3i32 Box3i;
typedef Vec3f Color3f;
typedef Vec3ub Color3ub;
typedef Vec4f Color4f;
typedef Vec4ub Color4ub;
typedef Vec3f Pixel3f;
typedef Vec3ub Pixel3ub;
typedef Vec4ub Pixel4ub;
typedef float t_radian;
typedef float t_radians;
typedef float t_degree;
typedef float t_degrees;
typedef Vec2f vec2;
typedef Vec2i ivec2;
typedef Vec2ui uvec2;
typedef Vec3f vec3;
typedef Vec3i ivec3;
typedef Vec3ui uvec3;
typedef Vec4f vec4;
typedef Vec4i ivec4;
typedef Vec4ui uvec4;
typedef Mat44 mat4;
typedef Mat33 mat3;
typedef Quaternion quat;

//String conversion

std::string operator+(const std::string& str, const vec2& rhs);
std::string operator+(const std::string& str, const vec3& rhs);
std::string operator+(const std::string& str, const vec4& rhs);
std::string operator+(const std::string& str, const ivec2& rhs);
std::string operator+(const std::string& str, const ivec3& rhs);
std::string operator+(const std::string& str, const ivec4& rhs);
std::string operator+(const std::string& str, const uvec2& rhs);
std::string operator+(const std::string& str, const uvec3& rhs);
std::string operator+(const std::string& str, const uvec4& rhs);
std::string operator+(const std::string& str, const mat3& rhs);
std::string operator+(const std::string& str, const mat4& rhs);

//////////////////////////////////////////////////////////////////////////
// Syntax helper.

// Collision Results
class LineHit : public VirtualMemory {
public:
    float t;
    LineHit() { }
    OVERRIDES ~LineHit() OVERRIDE { }
};
class PlanePoint : public VirtualMemory {
public:
    float t;            // - Position along the vector that was hit
    bool contained;
    PlanePoint() { }
    OVERRIDES ~PlanePoint() OVERRIDE { }
};
class PlaneHit : public VirtualMemory {
public:
    float _fHitTime;
    bool _bContained;

    FORCE_INLINE float getHitTime() { return _fHitTime; }
    FORCE_INLINE bool getIsContained() { return _bContained; }

    void makeFar()
    {
        _bContained = false;
        _fHitTime = FLT_MAX;
    }
    PlaneHit() { }
    OVERRIDES ~PlaneHit() OVERRIDE { }
};
// for quads
class DualPlaneHit : public VirtualMemory {
public:
    bool hit;                // True if either of the planes were hit.
    PlaneHit planeHit0;
    PlaneHit planeHit1;
    DualPlaneHit() { }
    OVERRIDES ~DualPlaneHit() OVERRIDE { }
};

//////////////////////////////////////////////////////////////////////////

class MathUtils : public VirtualMemory {
public:
    //juvenile sqrt optimization - simply removes the prolog
    //approx 30% faster than sqrtf
    //#ifdef BRO_X64
    FORCE_INLINE STATIC bool isEven(int32_t i) {
        return ((i % 2) == 0);
    }
    FORCE_INLINE STATIC bool isOdd(int32_t i) {
        return ((i % 2) != 0);
    }
    FORCE_INLINE STATIC double broSqrt(double x) {
        return std::sqrt(x);
    }
    FORCE_INLINE STATIC float broSqrt(float x) {
        return std::sqrtf(x);
    }
    FORCE_INLINE STATIC float broRsqrt(float x) {
        return 1.0f / std::sqrtf(x);
    }
    FORCE_INLINE STATIC double broRsqrt(double x) {
        return 1.0 / std::sqrt(x);
    }
    FORCE_INLINE STATIC float sqrf(float x) {
        return x*x;
    }
    FORCE_INLINE STATIC double sqrd(double x) {
        return x*x;
    }
    //Round is defined in the new MSVC lib.
    FORCE_INLINE STATIC float round(float in) {
        return (in >= 0.5000f) ? (ceilf(in)) : (floorf(in));
    }
    FORCE_INLINE STATIC double round(double in) {
        return (in >= 0.5000) ? (ceil(in)) : (floor(in));
    }
    //http://stackoverflow.com/questions/3237247/c-rounding-to-the-nths-place
    FORCE_INLINE STATIC float round(float value, int precision) {
        const int adjustment = (int)pow(10, precision);
        return floorf(value*(adjustment)+0.5f) / adjustment;
    }
    FORCE_INLINE STATIC double round(double value, int precision) {
        const int adjustment = (int)pow(10, precision);
        return floor(value*(adjustment)+0.5f) / adjustment;
    }
    FORCE_INLINE STATIC float degToRad(float d) {

        return (float)((d*(float)(M_PI)) / (float)180.0);
    }
    FORCE_INLINE STATIC float radToDeg(float d) {
        return (float)((d*(float)(180.0)) / (float)M_PI);
    }
    FORCE_INLINE STATIC int32_t getNumberOfDigits(int32_t i) {
        return i > 0 ? (int)log10f((float)i) + 1 : 1;
    }
    FORCE_INLINE STATIC uint32_t getNumberOfDigits(uint32_t i) {
        return i > 0 ? (int)log10((double)i) + 1 : 1;
    }
    //btMin/max copied from bullet.
    template <class T>
    FORCE_INLINE STATIC const T& broMin(const T& a, const T& b) {
        return a < b ? a : b;
    }
    template <class T>
    FORCE_INLINE STATIC const T& broMax(const T& a, const T& b) {
        return  a > b ? a : b;
    }
    template <class T>
    FORCE_INLINE STATIC T& broMin(T& a, T& b) {
        return a < b ? a : b;
    }
    template <class T>
    FORCE_INLINE STATIC T& broMax(T& a, T& b) {
        return  a > b ? a : b;
    }
    template <class T>
    FORCE_INLINE STATIC T& broClamp(T& x, T& a, T& b) {
        return broMin(b, broMax(x, a));
    }
    template <class T>
    FORCE_INLINE STATIC const T& broClamp(const T& x, const T& a, const T& b) {
        return broMin(b, broMax(x, a));
    }
    FORCE_INLINE STATIC float broSign(const float& val) {
        return (float)((float)(0.0f < val) - (float)(val < 0.0f));
    }
    //#define clamp(x,_min,_max) (broMin((_max),broMax((x),(_min))))
    FORCE_INLINE STATIC bool fuzzyEquals(float fa, float fb, float epsilon = FUZZY_EQUALS_EPSILON) {
        float bm = fb - epsilon;
        float bp = fb + epsilon;
        return (fa > bm) && (fa < bp);
    }
    template <class T>
    FORCE_INLINE STATIC const bool broInRangeInclusive(const T& x, const T& a, const T& b) {
        return ((x >= a) && (x <= b));
    }
    template <class T>
    FORCE_INLINE STATIC const bool broInRangeInclusive(const T& x, const T& a) {
        return broInRangeInclusive(x, -a, a);
    }
    FORCE_INLINE STATIC float modulateGridValue(float f_value, float f_grid_unit, float f_1_grid_unit) {
        // - Modulate the given value.
        // Returns the fractional part of f for the given 
        // f_1_grid_unit - 1/f_grid_unit    - supply this as an optimization
        // f_grid_unit - size of grid cubes
        // f_value - value to modulate.
        f_value = fmodf(f_value, f_grid_unit)*f_1_grid_unit;
        f_value += 1.0;
        f_value = fmodf(f_value, 1.0f);// this works for -.25 etc.  see complaint comment below
        return f_value;
    }
    FORCE_INLINE STATIC int signi(float x) {
        //    Return -1, or 1 sign of x
        int a = (int)(x >= 0);
        return (a*(1)) + ((!a)*(-1));
    }
    FORCE_INLINE STATIC float signf(float fx) {
        return (float)(signi(fx));
    }
    FORCE_INLINE STATIC float ceil_signed(float x) {
        //  return ceil of a number in the given direction. 0.6 = 1.0, -0.6 = -1.0
        return ((float)ceil(fabs(x))) * (signf(x));
    }

};


}//ns game



#endif
