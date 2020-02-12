#include "../base/BaseAll.h"
#include "../math/MathAll.h"

namespace BR2 {

Matrix4x4 Matrix3x3::getMat4()
{
    Matrix4x4 mat;
    mat._m11 = _m11;
    mat._m12 = _m12;
    mat._m13 = _m13;
    mat._m21 = _m21;
    mat._m22 = _m22;
    mat._m23 = _m23;
    mat._m31 = _m31;
    mat._m32 = _m32;
    mat._m33 = _m33;
    //mat._m41=mat._m42=mat._m43=0.0;
    //mat._m44=1.0;

    return mat;
}
Matrix3x3& Matrix3x3::operator=( const Matrix4x4& rhs ) {
    
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

}