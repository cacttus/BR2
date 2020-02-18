
#include "../base/BaseAll.h"
#include "../math/MathAll.h"

namespace BR2 {

vec3 ProjectedRay::getHitPoint(PlaneHit& ph) const {
  vec3 ret = getBegin() + getDir() * ph.getHitTime();
  return ret;
}

}