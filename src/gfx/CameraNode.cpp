#include "../base/GLContext.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/RenderViewport.h"
#include "../base/Gu.h"
#include "../world/Scene.h"
#include "../model/MeshNode.h"
#include "../model/UtilMeshInline.h"
#include "../model/MeshSpec.h"

namespace BR2 {
CameraNode::CameraNode(string_t name, std::shared_ptr<RenderViewport> ppViewport) : PhysicsNode(name, nullptr) {
  _pViewport = ppViewport;
  _vWorldUp.construct(0, 1, 0);
  _pMainFrustum = std::make_shared<FrustumBase>(ppViewport, _f_hfov);
  _vUp = vec3(0, 1, 0);
  _vLookAt = vec3(0, 0, 0);
  setPos(vec3(-100, -100, -100));
}
CameraNode::~CameraNode() {
  //DEL_MEM(_pMainFrustum);
}
std::shared_ptr<CameraNode> CameraNode::create(string_t name, std::shared_ptr<RenderViewport> ppViewport) {
  std::shared_ptr<CameraNode> cn = std::make_shared<CameraNode>(name, ppViewport);
  cn->init();
  return cn;
}
void CameraNode::init() {
  PhysicsNode::init();
//  TODO:
}
void CameraNode::drawForwardDebug(RenderParams& rp) {
  Gu::getCoreContext()->setLineWidth(3.0f);

  std::shared_ptr<FrustumBase> pf = _pMainFrustum;
  UtilMeshInline mi(Gu::getCoreContext());

  //Limit the distance of the frustum to fixed units
  float limit_length = 1;
  float depth = (pf->getZFar() - pf->getZNear());
  float lengthmuil = limit_length / (depth - limit_length);
  vec3 ftl = pf->PointAt(fpt_ntl) + (pf->PointAt(fpt_ftl) - pf->PointAt(fpt_ntl)) * lengthmuil;
  vec3 ftr = pf->PointAt(fpt_ntr) + (pf->PointAt(fpt_ftr) - pf->PointAt(fpt_ntr)) * lengthmuil;
  vec3 fbl = pf->PointAt(fpt_nbl) + (pf->PointAt(fpt_fbl) - pf->PointAt(fpt_nbl)) * lengthmuil;
  vec3 fbr = pf->PointAt(fpt_nbr) + (pf->PointAt(fpt_fbr) - pf->PointAt(fpt_nbr)) * lengthmuil;

  Color4f c4(1, 1, 0, 1);
  mi.begin(GL_LINES);
  {
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(ftl, c4));// Edges
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbr), c4), v_v3c4(fbr, c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbl), c4), v_v3c4(fbl, c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntr), c4), v_v3c4(ftr, c4));
    mi.vt2(v_v3c4(ftl, c4), v_v3c4(ftr, c4));// Far Quad
    mi.vt2(v_v3c4(fbl, c4), v_v3c4(fbr, c4));
    mi.vt2(v_v3c4(ftl, c4), v_v3c4(fbl, c4));
    mi.vt2(v_v3c4(ftr, c4), v_v3c4(fbr, c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(pf->PointAt(fpt_ntr), c4));// Near Quad
    mi.vt2(v_v3c4(pf->PointAt(fpt_nbl), c4), v_v3c4(pf->PointAt(fpt_nbr), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntl), c4), v_v3c4(pf->PointAt(fpt_nbl), c4));
    mi.vt2(v_v3c4(pf->PointAt(fpt_ntr), c4), v_v3c4(pf->PointAt(fpt_nbr), c4));
  }
  mi.endAndDraw(getThis<CameraNode>());
  Gu::getCoreContext()->setLineWidth(1.0f);
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
  //_pViewport->updateChanged(true);
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
  //_pViewport->updateChanged(true);

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
  //This is useless now that we have scripts, however, this is a cool function that zooms camera with mouse wheel, so we could
  //use it in a script somewhere.
  vec3 newP;

  _fCurZoomDist += -amt;
  _fCurZoomDist = MathUtils::brClamp(_fCurZoomDist, _fMinZoomDist, _fMaxZoomDist);

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

  float tVal = (_fCurZoomDist - _fMinZoomDist) / (_fMaxZoomDist - _fMinZoomDist);

  vec3 finalPt;
  finalPt = vec3(
    Alg::cerp_1D(minPt.x, maxPt.x, tVal),
    Alg::cerp_1D(minPt.y, maxPt.y, tVal),
    Alg::cerp_1D(minPt.z, maxPt.z, tVal));

  newP = _vLookAt + finalPt;

  setPos(std::move(newP));
}


}//ns game
