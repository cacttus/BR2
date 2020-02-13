#include "../base/GLContext.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/RenderViewport.h"
#include "../base/Gu.h"

namespace BR2 {
std::shared_ptr<CameraNode> CameraNode::create(std::shared_ptr<RenderViewport> ppViewport) {
  std::shared_ptr<CameraNode> cn = std::make_shared<CameraNode>(ppViewport);
  cn->init();
  return cn;
}
CameraNode::CameraNode(std::shared_ptr<RenderViewport> ppViewport) : PhysicsNode(nullptr) {
  _pViewport = ppViewport;
  _vWorldUp.construct(0, 1, 0);

  _pMainFrustum = std::make_shared<FrustumBase>(ppViewport, _f_hfov);
  _vUp = vec3(0, 1, 0);
  _vLookAt = vec3(0, 0, 0);

  setPos(vec3(-100, -100, -100));
}
CameraNode::~CameraNode() {
  //DEL_MEM(_boundEllipsoid);
  //DEL_MEM(_pMainFrustum);
}
Ray_t CameraNode::projectPoint2(vec2& mouse) {
  Ray_t pr;
  vec2 screenPoint((float)mouse.x, (float)mouse.y);
  vec3 farPt;

  _pMainFrustum->screenPointToWorldPointNear(screenPoint, pr.Origin);
  _pMainFrustum->screenPointToWorldPointFar(screenPoint, farPt);

  pr.Dir = farPt - pr.Origin;
  pr.opt();

  return pr;
}
ProjectedRay CameraNode::projectPoint(vec2& mouse) {
  ProjectedRay pr;
  vec2 screenPoint((float)mouse.x, (float)mouse.y);
  vec3 farPt;

  _pMainFrustum->screenPointToWorldPointNear(screenPoint, pr.Origin);
  _pMainFrustum->screenPointToWorldPointFar(screenPoint, farPt);

  pr.Dir = farPt - pr.Origin;
  pr.opt();

  return pr;
}
void CameraNode::setFOV(t_radians fov) {
  _f_hfov = fov;
  _pViewport->updateChanged(true);
  _pMainFrustum->setFov(_f_hfov);
}
void CameraNode::update(float dt, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  PhysicsNode::update(dt, mapAnimators);

  //update view normal.
  vec3 vpos = getPos();
  _vViewNormal = (_vLookAt - vpos).normalize();

  //Update upd vector
  vec3 rtvn = _vWorldUp.cross(_vViewNormal);
  if (rtvn.squaredLength() == 0) {
    //Prevents teh "lock" problem.
    rtvn = vec3(0, 0, 1);
    rtvn = rtvn.cross(_vViewNormal);
  }
  _vUp = _vViewNormal.cross(rtvn).normalize();

  _vRight = _vUp.cross(_vViewNormal).normalize();

  // Viewport
  //We force true here every frame because of shadow boxes changing viewport.
  //TODO: have an "isUpdated" or some other so we can spread shadowbox viewport update over time and not update this.
  //**NOTE: 1/23/18 we changed this to refresh viewport after shadow box step in render pipe
  _pViewport->updateChanged(true);

  //**Note: This is important: sets up the projection matrix for the camera
  // either 2D 3D ortho 2point ...
  setupProjectionMatrix();
  setupViewMatrix();

  //Fix the cursor
 // fixCursor();

//  _updateFrameStamp = Globals::getTime()->getFrameStamp();
}
void CameraNode::setupProjectionMatrix() {
  _pMainFrustum->update(getViewNormal(), getPos(), _vUp, _eProjectionMode);
  _mProj = _pMainFrustum->getProjectionMatrix();
}
void CameraNode::setupViewMatrix() {
  _mView = mat4::getLookAt(getPos(), getLookAt(), _vUp);
}
vec3 CameraNode::getLookAtOffset() {
  return vec3(0, 14, 0);
}
void CameraNode::zoom(float amt) {
  vec3 newP;

  _fCurZoomDist += -amt;
  _fCurZoomDist = MathUtils::broClamp(_fCurZoomDist, _fMinZoomDist, _fMaxZoomDist);

  float f1 = _fCurZoomDist;
  float f2 = _fMinZoomDist;
  float f3 = _fMaxZoomDist;

  //Get the min PT
  vec3 ny = vec3(0, 1, 0);//This must be constant
  vec3 vp = getPos();
  vec3 del = vp - _vLookAt;
  if (false) {
    del = vec3(0, 0, 1);
    ny = vec3(0, 1, 0);
  }

  vec3 nx = ny.cross(del);
  nx.normalize();
  vec3 nz = nx.cross(ny).normalize();

  vec3 minPt, maxPt;
  minPt = (ny * _fMinZoomDist * 0.0f) + (nz * _fMinZoomDist * 9.3f);
  maxPt = (ny * _fMaxZoomDist * 2.6f) + (nz * _fMaxZoomDist * 3.8f);

  minPt += getLookAtOffset();
  maxPt += getLookAtOffset();

  //minPt = _vLookAt + _vViewNormal * -_fMinZoomDist;
  //maxPt = _vLookAt + _vViewNormal * -_fMaxZoomDist;

  float tVal = (_fCurZoomDist - _fMinZoomDist) / (_fMaxZoomDist - _fMinZoomDist);

  vec3 finalPt;
  finalPt = vec3(
    Alg::cerp_1D(minPt.x, maxPt.x, tVal),
    Alg::cerp_1D(minPt.y, maxPt.y, tVal),
    Alg::cerp_1D(minPt.z, maxPt.z, tVal)
  );

  newP = _vLookAt + finalPt;
  // newP = getPos()+ _vViewNormal * amt;

  setPos(std::move(newP));
}


}//ns game
