#include "../math/Plane3.h"
#include "../math/ProjectedRay.h"

namespace BR2 {

bool Plane3f::intersectRay(Ray_t* ray) {
  AssertOrThrow2(ray != nullptr);

  vec3 p1 = ray->getBegin();
  vec3 p2 = ray->getEnd();

  float t = -(n.dot(p1) + d) / ((p2 - p1).dot(n));
  ray->setTime(t);
  return ray->isHit();
}

}