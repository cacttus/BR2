#include "../math/Minimax.h"
#include "../math/Random.h"
namespace BR2 {

float MpFloat::next() {
  checkWasSet();
  return Random::nextFloat(_min, _max);
}
vec3 MpVec3::next() {
  checkWasSet();
  return Random::nextVec3(_min, _max);
}
ivec3 MpVec3i::next() {
  checkWasSet();
  return Random::nextIVec3(_min, _max);
}
vec4 MpVec4::next() {
  checkWasSet();
  return Random::nextVec4(_min, _max);
}
uint32_t MpUint::next() {
  checkWasSet();
  return Random::nextUint32(_min, _max);
}
int32_t MpInt::next() {
  checkWasSet();
  return Random::nextInt32(_min, _max);
}




}

