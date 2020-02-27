#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../gfx/CameraNode.h"
#include "../base/GLContext.h"
#include "../world/PhysicsGridAwareness.h"
#include "../world/NodeUtils.h"


namespace BR2 {
PhysicsGridAwareness::PhysicsGridAwareness(std::shared_ptr<PhysicsWorld> pw, MpFloat rxz, float incXZ, MpFloat ry, float incY) {
  _pPhysics = pw;
  _vLastAwarenessPos = vec3(0, 0, 0);// getAwarenessPos();
  _mpXz = rxz;
  _mpY = ry;
  _incY = incY;
  _incXz = incXZ;
  _fAwarenessRadiusXZ = rxz.getMin();
  _fAwarenessRadiusY = ry.getMin();
}
PhysicsGridAwareness::~PhysicsGridAwareness() {
}
void PhysicsGridAwareness::update(float dt) {
  updateAwarenessSpheroidAxis(_fAwarenessRadiusY,
    _mpY.getMin(),
    _mpY.getMax(),
    _incY);
  updateAwarenessSpheroidAxis(_fAwarenessRadiusXZ,
    _mpXz.getMin(),
    _mpXz.getMax(),
    _incXz
  );
}
void PhysicsGridAwareness::updateAwarenessSpheroidAxis(float& fAwareness, float minR, float maxR, float increment) {
  std::shared_ptr<CameraNode> cam = NodeUtils::getActiveCamera(_pPhysics);

  float fLen = (_vLastAwarenessPos - cam->getPos()).squaredLength();
  //Reset awareness when we move some small value.
  if (fLen > 0.05) {
    fAwareness = minR;
    _vLastAwarenessPos = cam->getPos();
  }

  // float aMax = CongaUtils::getBvhAwarenessMaxRadius();
  // float aMin = CongaUtils::getBvhAwarenessMinRadius();
  fAwareness = MathUtils::brClamp(fAwareness, minR, maxR);

  //TODO: fix this
  //if (_setGenStage1.size() == 0 && _setGenStage2.size() == 0) {
  fAwareness += increment;//CongaUtils::getBvhAwarenessIncrement();
//}
}
void PhysicsGridAwareness::getAwarenessBox(Box3f& c) {
  return getAwarenessBox(c, _fAwarenessRadiusXZ, _fAwarenessRadiusY);
}
void PhysicsGridAwareness::getAwarenessBox(Box3f& c, float rxz, float ry) {
  vec3 vb = getAwarenessPos();//getRaycastViewCenter(); //getContext()->getCamera()->getPos();
  c._min.x = vb.x - rxz;
  c._min.y = vb.y - ry;
  c._min.z = vb.z - rxz;
  c._max.x = vb.x + rxz;
  c._max.y = vb.y + ry;
  c._max.z = vb.z + rxz;
}
vec3 PhysicsGridAwareness::getAwarenessPos() {
  //***Don't dork with this because we use it to collect visible nodes and
  // all nodes now contain positions.
  //This is a debatable position.  If we use the "raycast" version we end up
  //creating / deleting tons of cells. 
  std::shared_ptr<CameraNode> cam = NodeUtils::getActiveCamera(_pPhysics);
  if (cam == nullptr) {
    BRLogWarn("Awareness didn't have a camera (may not be a bug)");
    return vec3(0, 0, 0);
  }
  else {
    vec3 vp = cam->getPos();//getRaycastViewCenter(); //->getCamera()->getPos();//getProjectedViewCenter();
    return vp;
  }
}








}//ns Game
