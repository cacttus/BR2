
#ifndef __MATH__ALL__H__
#define __MATH__ALL__H__

#include "../math/MathHeader.h"
#include "../math/Vec2x.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../math/Box3x.h"
#include "../math/Sphere3.h"
#include "../math/Box2x.h"
#include "../math/Quad3x.h"
#include "../math/QuadPlane.h"
#include "../math/TriPlane.h"
#include "../math/Line3.h"
#include "../math/Minimax.h"
#include "../math/Random.h"


//////////////////////////////////////////////////////////////////////////
//All the stuff here is the math templates which rely on other definitions.  We don't need
//this file in MSVC because that compiler lazy evaluates template definitions but this is needed for G++.
//////////////////////////////////////////////////////////////////////////


namespace Game {
    
    
template < class Tx >
Vec2x<Tx>::Vec2x(const Vec3f& a)
{
    x = a.x;
    y = a.y;
}
template < class Tx >
Vec2x<Tx>::Vec2x(const Vec4f& a)
{
    x = a.x;
    y = a.y;
}
//Moving vec3 here because of osx compiler
template < class Tx >
Vec3x<Tx>& Vec3x<Tx>::operator*=( const Matrix3x3 &m )
{
    *this = (*this)*m;
    return *this;
}
template < class Tx >
Vec3x<Tx> Vec3x<Tx>::operator*( const Matrix3x3 &m )
{
    Vec3x<Tx> ret;
    ret.x = (Tx)(m._m11*x+m._m21*x+m._m31*x);
    ret.y = (Tx)(m._m12*y+m._m22*y+m._m32*y);
    ret.z = (Tx)(m._m13*z+m._m23*z+m._m33*z);
    return ret;
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::sphereIntersect(Sphere3f* s)
{
    return sphereIntersect(&(s->o),s->r);
}

template < class Tx >
Vec4x<Tx> Vec4x<Tx>::operator*( const Matrix4x4 &m )
{
    //**This should yield a matrix not a vector.  Throw!
    throw 0;
    //Vec4x<Tx> ret;
    //ret.x = (Tx)(m._m11*x+m._m21*x+m._m31*x+m._m41*x);
    //ret.y = (Tx)(m._m12*y+m._m22*y+m._m32*y+m._m42*y);
    //ret.z = (Tx)(m._m13*z+m._m23*z+m._m33*z+m._m43*z);
    //ret.w = (Tx)(m._m14*w+m._m24*w+m._m34*w+m._m44*w);
    //return ret;
}
 

/**
*    @fn minor()
*    @brief Returns the minor at the specified row and column.
*    @details A minor is a matrix of this matrix, minus the row and column you specify in the arguments.
*    Minors are used to compute cofactors (determinants of minors) and the classical adjunct (the transpose of the matrix of cofactors)
*    @param r - Must be 0 to 3, the row value of the matrix
*    @param c - Must be 0 to 3, the column value of the matrix.
*/
FORCE_INLINE Matrix3x3 Matrix4x4::minor(int r, int c)
{
    Matrix3x3 m;
    m.setIdentity();
    if (r < 0 || r>3 || c < 0 || c>3)
        return m;

    int ind, mind = 0;
    for (ind = 0; ind < size(); ++ind)
        if (rowNum(ind) != r && colNum(ind) != c)
            m.Mat(mind++) = Mat(ind);

    return m;
}

/**
*    @fn cofactor()
*    @brief Returns the cofactor of this matrix at the specified row and column.
*    @remarks A cofactor is the determinant of the minor of this matrix at the specified row and column.
*    @param r - The row to compute the cofactor.
*    @param c - The column to compute the cofactor.
*    @return The cofactor of this matrix at the row and column location.
*/
FORCE_INLINE float Matrix4x4::cofactor(int r, int c)
{
    // - return the determinant of the minor.
    return (float)powf(-1.00f, (float)r + (float)c)*minor(r, c).det();    // ** May be incorrect
}

FORCE_INLINE Quaternion Quaternion::operator*(Vector3 v)
{
    return Quaternion(x*v.x, y*v.y, z*v.z, w);
}
FORCE_INLINE Vector3 Quaternion::vectorPart() {
    return Vector3(x, y, z);
}
FORCE_INLINE Vector3 Quaternion::rotatePoint(Vector3& in)
{
    return ((inverse()*in)*(*this)).vectorPart();
}
FORCE_INLINE void Quaternion::getAxisAngle(vec4& v)
{
    //http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/

    if (w == 1.0f)
    {
        //Avoid divide by 0,( 1 - (cos(0) = 1)) =0
        v.x = v.z = v.w = 0;
        v.y = 1;
        return;
    }

    v.w = 2.0f * acosf(w);

    float w2 = w*w;
    float srw2_1 = 1.0f / sqrtf(1.0f - w2);
    v.x = x * srw2_1;
    v.y = y * srw2_1;
    v.z = z * srw2_1;

}
/**
*    @fn rotate()
*    @details Returns a rotation matrix, but does not alter this matrix.
*    @param a, x,y,z the axis-angle rotation.
*    @return A matrix witht the specified rotation.
*    @param a - Radians.
*/
FORCE_INLINE Matrix3x3 Matrix3x3::getRotationRad(float a, const vec3& v)
{
    return getRotationRad(a, v.x, v.y, v.z);
}
FORCE_INLINE void Matrix3x3::rotateRad(t_radians a, vec3& vec)
{
    *this *= getRotationRad(a, vec.x, vec.y, vec.z);

}
FORCE_INLINE Matrix3x3 Matrix3x3::getRotationRad(t_radians a, float x, float y, float z)
{
    // - Reference: The openGL reference.http://pyopengl.sourceforge.net/documentation/manual/reference-GL.html

    Matrix3x3 Temp;

    if (vec3(x, y, z).squaredLength() != 1.0)
    {
        vec3 v(x, y, z);
        v.normalize();
        x = v.x;
        y = v.y;
        z = v.z;
    }

    float c = cosf(a);
    float s = sinf(a);
    float nc = 1 - c;

    Temp._m11 = (x*x) * nc + c;
    Temp._m21 = (x*y) * nc + (z*s);
    Temp._m31 = (x*z) * nc - (y*s);

    Temp._m12 = (y*x) * nc - (z*s);
    Temp._m22 = (y*y) * nc + c;
    Temp._m32 = (y*z) * nc + (x*s);

    Temp._m13 = (z*x) * nc + (y*s);
    Temp._m23 = (z*y) * nc - (x*s);
    Temp._m33 = (z*z) * nc + c;

    /*Temp._m11=  (x*x) * nc + c;
    Temp._m12=  (x*y) * nc + (z*s);
    Temp._m13=  (x*z) * nc - (y*s);
    Temp._m14=   0.0f;

    Temp._m21=  (y*x) * nc - (z*s);
    Temp._m22=  (y*y) * nc + c;
    Temp._m23=  (y*z) * nc + (x*s);
    Temp._m24=   0.0f;

    Temp._m31=  (z*x) * nc + (y*s);
    Temp._m32=  (z*y) * nc - (x*s);
    Temp._m33=  (z*z) * nc + c;
    Temp._m34=  0.0f;
    */
    return Temp;
}
//
// - Times Vector3
//
FORCE_INLINE Vector3 Matrix3x3::operator*= (const Vector3& v)
{
    //_MATH_USE_ROW_VECTORS
    Vector3 tmp;
    tmp.x = v.x*_m11 + v.y*_m12 + v.z*_m13;
    tmp.y = v.x*_m21 + v.y*_m22 + v.z*_m23;
    tmp.z = v.x*_m31 + v.y*_m32 + v.z*_m33;
    return tmp;
}
//
// - Times Vector3
//
FORCE_INLINE Vector3 Matrix3x3::operator*(const Vector3& v)
{
    Matrix3x3 m = *this;
    return (m *= v);
}
template < class Tx >
Vec2x<Tx> Vec2x<Tx>::minv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b)
{
    Vec2x<Tx> out;

    out.x = MathUtils::broMin(v_a.x, v_b.x);
    out.y = MathUtils::broMin(v_a.y, v_b.y);

    return out;
}
template < class Tx >
Vec2x<Tx> Vec2x<Tx>::maxv(const Vec2x<Tx>& v_a, const Vec2x<Tx>& v_b)
{
    Vec2x<Tx> out;

    out.x = MathUtils::broMax(v_a.x, v_b.x);
    out.y = MathUtils::broMax(v_a.y, v_b.y);

    return out;
}
template < class Tx >
t_string Vec2x<Tx>::toString(int precis) const
{
    t_string tmp;
    std::ostringstream out;
    std::streamsize prec;
    if (precis == -1){
        prec = std::numeric_limits<long double>::digits10 + 1;
    }
    else{
        prec = precis;
    }

    out << std::setprecision(prec) << x << "," << y;
    return out.str();
}
template < class Tx >
t_string Vec3x<Tx>::toString(int precis) const
{
    t_string tmp;
    std::ostringstream out;
    std::streamsize prec;
    if (precis == -1){
        prec = std::numeric_limits<long double>::digits10 + 1;
    }
    else{
        prec = precis;
    }

    out << std::setprecision(prec) << x << "," << y << "," << z;
    return out.str();
}

// ------ Mem & offset --------
// vector offset to scalar offset
inline size_t vofftos(size_t row, size_t col, size_t items_per_row) {
    return (col*items_per_row + row);
}
//    return true if the (x,y) coordinate is within the grid.  
//    the left boundary being zero the right being (size-1) and all indexes positive.
inline bool in_range(size_t x, size_t y, size_t row_size, size_t col_size) {
    return (((x >= 0) && (x < row_size)) && ((y >= 0) && (y < col_size)));
}
//
// scalar offset to vector offset ( row and column)
inline size_t sofftov_row(size_t off, size_t items_per_row) {
    return (size_t)(off / items_per_row);
}
inline size_t sofftov_col(size_t off, size_t items_per_row) {
    return (size_t)(off%items_per_row);
}
inline bool fuzzyEquals(float a, float b, float e) {
    //Epsilon cusyhion equals
    return (((b) <= ((a)+(e))) && ((b) >= ((a)-(e))));
}

typedef Vec4ub Pixel4ub;


}//NS Game


#endif
