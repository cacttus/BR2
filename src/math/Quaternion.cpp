#include "../math/MathAll.h"

namespace Game {
/**
*    @fn toMat4
*    @brief Fairly quick operation to turn from Quat to Matrix
*/
Matrix4x4 Quaternion::toMat4() {
  Matrix4x4 m;
  m._m11 = 1 - 2 * y * y - 2 * z * z;        m._m12 = 2 * x * y + 2 * w * z;        m._m13 = 2 * x * z - 2 * w * y;        m._m14 = 0;
  m._m21 = 2 * x * y - 2 * w * z;        m._m22 = 1 - 2 * x * x - 2 * z * z;    m._m23 = 2 * y * z + 2 * w * x;        m._m24 = 0;
  m._m31 = 2 * x * z + 2 * w * y;        m._m32 = 2 * y * z - 2 * w * x;        m._m33 = 1 - 2 * x * x - 2 * y * y;    m._m34 = 0;
  m._m41 = 0;                    m._m42 = 0;                m._m43 = 0;                m._m44 = 1;

  return m;
}
/**
*    @fn
*    @brief Quickly go from mat3 to mat4
*/
Matrix3x3 Quaternion::toMat3() {
  Matrix3x3 m;
  m._m11 = 1 - 2 * y * y - 2 * z * z;    m._m12 = 2 * x * y + 2 * w * z;    m._m13 = 2 * x * z - 2 * w * y;
  m._m21 = 2 * x * y - 2 * w * z;        m._m22 = 1 - 2 * x * x - 2 * z * z; m._m23 = 2 * y * z + 2 * w * x;
  m._m31 = 2 * x * z + 2 * w * y;        m._m32 = 2 * y * z - 2 * w * x;    m._m33 = 1 - 2 * x * x - 2 * y * y;

  return m;
}



}

