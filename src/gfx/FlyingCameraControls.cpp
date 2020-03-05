#include "../base/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/FlyingCameraControls.h"
#include "../model/SceneNode.h"
#include "../world/Scene.h"

namespace BR2 {
FlyingCameraControls::FlyingCameraControls() {
}
FlyingCameraControls::~FlyingCameraControls() {
}
void FlyingCameraControls::onStart() {

  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (cam == nullptr) {
    BRLogError("Script could not start, object node was not set.");
    Gu::debugBreak();
    return;
  }
  // cam = std::make_shared<CameraNode>();// CameraNode::create(pv, pscene);
  cam->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  
  updateCameraPosition();
  _vMoveVel.construct(0, 0, 0);
  cam->setPos(vec3(0, 0, 0));

  //cam->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());//Make sure to create the frustum.
  _vCamNormal = cam->getViewNormal();
  _vCamPos = cam->getPos();
}
void FlyingCameraControls::onUpdate(float delta) {
  std::shared_ptr<InputManager> im = getNode()->getInput();
  if (im) {
    update(im, delta);
    moveCameraWSAD(im, delta);
  }
  else {
    BRLogErrorCycle("Node input could not be found.");
  }
}
void FlyingCameraControls::onExit() {
}
void FlyingCameraControls::moveCameraWSAD(std::shared_ptr<InputManager> pInput, float delta) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  //Damp Slows us a bit when we zoom out.
 // float damp = fabsf(_fCamDist)*0.001f;
  float strafeAmt = 1.05; //fabsf(_fCamDist) / (CongaUtils::getNodeWidth() + damp) * factor;


  if (pInput->shiftHeld()) {
    strafeAmt = 2.1f;
  }

  strafeAmt *= delta;
  vec3 vel(0, 0, 0);

  if (pInput->keyPressOrDown(SDL_SCANCODE_W)) {
    vel += cam->getViewNormal() * strafeAmt;
  }
  if (pInput->keyPressOrDown(SDL_SCANCODE_S)) {
    vel += cam->getViewNormal() * -strafeAmt;
  }
  if (pInput->keyPressOrDown(SDL_SCANCODE_A)) {
    vel += cam->getRightNormal() * -strafeAmt;
  }
  if (pInput->keyPressOrDown(SDL_SCANCODE_D)) {
    vel += cam->getRightNormal() * strafeAmt;
  }
  _vMoveVel += vel;
  updateCameraPosition();
}
void FlyingCameraControls::updateCameraPosition() {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }
  if (_vCamNormal.squaredLength() == 0.0f) {
    //We have an error wit the camera normal because we're using the same normal from the
    //camera to do calculations.  This is a HACK:
    _vCamNormal.construct(1.0f, 0, 0);
    Gu::debugBreak();
  }
  vec3 vLookat = _vCamPos + _vCamNormal;
  cam->setPos(std::move(_vCamPos));
  cam->setLookAt(std::move(vLookat));
}
void FlyingCameraControls::update(std::shared_ptr<InputManager> pInput, float dt) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  //Capture & u8pdate input
  ButtonState::e eLmb = pInput->getLmbState();
  vec2 vLast = pInput->getLastMousePos();
  t_timeval us = Gu::getMicroSeconds();

  updateRotate(pInput);

  //set cam pos
  //vec3 pos = cam->getPos();
  _vCamPos += _vMoveVel;
  updateCameraPosition();

  //dampen the velocity
  vec3 v_n;
  float v_len;
  _vMoveVel.len_and_norm(v_n, v_len);
  float v_new_len = v_len - v_len * _fMoveDamp * dt;
  v_new_len = MathUtils::brClamp(v_new_len, 0.0f, _fMaxMoveVel);
  _vMoveVel = v_n * v_new_len;

  RenderParams rp;
  rp.setCamera(cam);

  //Finalluy update camera

  //**TODO: this should likely be done on the scene automatically.
  //This would run as a CSHarpScript (in the future) and get its update() method called.
  //cam->update(dt, std::map<Hash32, std::shared_ptr<Animator>>());
}
void FlyingCameraControls::updateRotate(std::shared_ptr<InputManager> pInput) {
  vec2 vMouse = pInput->getMousePos();
  ButtonState::e eRmb = pInput->getRmbState();
  vec2 vDelta(0, 0);
  if (eRmb == ButtonState::Press) {
    _vMousePress = vMouse;
  }
  if (eRmb == ButtonState::Press || eRmb == ButtonState::Down) {
    vDelta = vMouse - _vMousePress;
    pInput->warpMouse((int)_vMousePress.x, (int)_vMousePress.y);
  }
  else if (eRmb == ButtonState::Release) {
  }

  //Update Rotate
  //float stop_e = 0.000001f;
  //_fRotationVelX += _fPerUnitRotate * vDelta.x;
  //_fRotationVelY += _fPerUnitRotate * -vDelta.y;
  //_fRotationVelX = clamp(_fRotationVelX, _fRotationVelMin, _fRotationVelMax);
  //_fRotationVelY = clamp(_fRotationVelY, _fRotationVelMin, _fRotationVelMax);
  //if (_fRotationVelX*_fRotationVelX + _fRotationVelY*_fRotationVelY > 0.0f) {
  //    _fRotationVelX = _fRotationVelX - (_fRotationVelX * _fRotationEase );
  //    _fRotationVelY = _fRotationVelY - (_fRotationVelY * _fRotationEase );
  //    _fRotationVelX = clamp(_fRotationVelX, _fRotationVelMin, _fRotationVelMax);
  //    _fRotationVelY = clamp(_fRotationVelY, _fRotationVelMin, _fRotationVelMax);
  //
  //    if (_fRotationVelX*_fRotationVelX < stop_e) {
  //        _fRotationVelX = 0.0f;
  //    }
  //    if (_fRotationVelY*_fRotationVelY < stop_e) {
  //        _fRotationVelY = 0.0f;
  //    }
  //    rotateCameraNormal(_fRotationVelX, _fRotationVelY);
  //}
  if (vDelta.x != 0 || vDelta.y != 0) {
    rotateCameraNormal(_fPerUnitRotate * vDelta.x, _fPerUnitRotate * -vDelta.y);
  }
}
void FlyingCameraControls::rotateCameraNormal(float rotX, float rotY) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  // mat4 rot = mat4::getRotationRad(dRot, vec3(0, 1, 0));
  vec3 camPos = getNode()->getScene()->getActiveCamera()->getPos();

  // _vCamNormal = _pCamera->getViewNormal();

   //vec4 mul =
   //    vec4(_vCamNormal.x,
   //        _vCamNormal.y,
   //        _vCamNormal.z,
   //        1.0f);
   //vec4 res = rot * mul;

  vec3 rt = cam->getRightNormal();
  vec3 up = cam->getUpNormal();

  vec3 vNewCamNormal = (_vCamNormal + rt * rotX + up * rotY).normalize();

  float e = 0.0003;
#define N_INR(a, b) ((a >= (b-e)) && (a <= (b+e)))

  if (N_INR(vNewCamNormal.y, -1.0) || N_INR(vNewCamNormal.y, 1.0)) {
    //Keep vCamNormal - don't let the user rotate to -1, or 1
  }
  else {
    _vCamNormal = vNewCamNormal;
  }

  //Additional checking to make sure we don't flip the UI
  if (_vCamNormal.y == 1.0f) {
    _vCamNormal.y = 0.98f;
    _vCamNormal.x = 0.01f;
    _vCamNormal.z = 0.01f;
    _vCamNormal.normalize();
  }
  if (_vCamNormal.y == -1.0f) {
    _vCamNormal.y = -0.98f;
    _vCamNormal.x = -0.01f;
    _vCamNormal.z = -0.01f;
    _vCamNormal.normalize();
  }

  //_vCamNormal = vec3(res.x, res.y, res.z);
 // vec3 lookAt = camPos + _vCamNormal;

  updateCameraPosition();
}
void FlyingCameraControls::setPosAndLookAt(vec3&& pos, vec3&& lookat) {
  _vCamPos = pos;
  _vCamNormal = (lookat - pos).normalize();

  updateCameraPosition();
}

}//ns BR2
