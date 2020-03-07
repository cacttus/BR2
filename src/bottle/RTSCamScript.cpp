#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/InputManager.h"
#include "../base/GraphicsWindow.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../base/TouchInfo.h"
#include "../bottle/RTSCamScript.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../bottle/BottleUtils.h"
#include "../world/Scene.h"

namespace BR2 {
RTSCamScript::RTSCamScript() {
}
RTSCamScript::~RTSCamScript() {
}
void RTSCamScript::onStart() {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (cam == nullptr) {
    BRLogError("Script could not start, object node was not set.");
    Gu::debugBreak();
    return;
  }
  cam->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.
  _vCamNormal.construct(-.6, 1, -.6);
  _vCamNormal.normalize();
  _vLookAt.construct(0, 0, 0);
  updateCameraPosition(cam);
}
void RTSCamScript::onUpdate(float dt) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    BRLogErrorCycle("Node not found.");
    return;
  }
  if (cam->active()) {
    updateRotateAndZoom(cam, dt);
    std::shared_ptr<InputManager> im = getNode()->getInput();
    if (im) {
      updateTouches(cam, getNode()->getInput(), dt);
      moveCameraWSAD(cam, im, dt);
    }
    else {
      BRLogErrorCycle("Node input could not be found.");
    }
  }
}
void RTSCamScript::onExit() {
}
void RTSCamScript::updateTouches(std::shared_ptr<CameraNode> cam, std::shared_ptr<InputManager> pFingers, float dt) {
  ButtonState::e eLmb = pFingers->getLmbState();
  ButtonState::e eRmb = pFingers->getRmbState();
  vec2 vMouse = pFingers->getMousePos_Relative();
  vec2 vLast = pFingers->getLastMousePos_Relative();
  t_timeval us = Gu::getMicroSeconds();

  //RMB
  if (eRmb == ButtonState::e::Press) {
    _lastTouchRmb = _curTouchRmb;
    _curTouchRmb.touchBegin(vMouse, eRmb);
    _bUnsnapY = _bUnsnapX = false;
    vec2 delta = vMouse - vLast;
    userRotateAndZoom(cam, delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
  }
  else if (eRmb == ButtonState::e::Down) {
    vec2 delta = vMouse - vLast;
    userRotateAndZoom(cam, delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
    //     pFingers->warpMouse(_pCamera->getViewport()->getWidth()*0.5, _pCamera->getViewport()->getHeight()*0.5 );
  }
  else if (eRmb == ButtonState::e::Release) {
    _curTouchRmb.touchEnd(vMouse, eRmb);
    _bUnsnapY = _bUnsnapX = false;
  }

  else if (eRmb == ButtonState::e::Up) {
    if (_curTouchRmb._eState == ButtonState::e::Press || _curTouchRmb._eState == ButtonState::e::Down) {
      _curTouchRmb.touchEnd(vMouse, eRmb);
    }
    _bUnsnapY = _bUnsnapX = false;
  }

  if (pFingers->getMouseWheel() != 0) {
    doZoom(cam, pFingers->getMouseWheel());
  }
}
//void SnapRooter::rotateAndZoomCamera(std::shared_ptr<InputManager> pFingers, float dt) {
//    vec2 vMouse = pFingers->getMousePos();
//    vec2 vLast = pFingers->getLastMousePos();
//
//    ButtonState eRmb = pFingers->getRmbState();
//    if (eRmb == ButtonState::Press) {
//        _lastTouchRmb = _curTouchRmb;
//        _curTouchRmb.touchBegin(vMouse, eRmb);
//        _bUnsnapY = _bUnsnapX = false;
//        vec2 delta = vMouse - vLast;
//        userRotateAndZoom(delta, vMouse,vLast, _fPerUnitRotate, _fPerUnitZoom);
//    }
//    else if (eRmb == ButtonState::Down) {
//        vec2 delta = vMouse - vLast;
//        userRotateAndZoom(delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
//    }
//    else if (eRmb == ButtonState::Release) {
//        _curTouchRmb.touchEnd(vMouse, eRmb);
//        _bUnsnapY = _bUnsnapX = false;
//    }
//}
void RTSCamScript::userRotateAndZoom(std::shared_ptr<CameraNode> cam, vec2& v, vec2& vMouse, vec2& vLast, float perUnitRotationRads, float perUnitZoom) {
  //Calculate snap controls - so that clicking the user doesn't incidedntally rotate/zoom the camera.
  vec2 vdelta = _curTouchRmb.getMousePosPress() - vMouse;
  float fZoomControlY = cam->getViewport()->getHeight() * 0.07f; //1%
  if (fabsf(vdelta.y) > fZoomControlY) {
    _bUnsnapY = true;
  }
  float fZoomControlX = cam->getViewport()->getWidth() * 0.023f; //1%
  if (fabsf(vdelta.x) > fZoomControlX) {
    _bUnsnapX = true;
  }

  //*Note: we apply rotate/zoom in the update loop in order to smooth out the velocity.

  //Rotate (X)
  if (_bUnsnapX) {
    _fRotationVel += perUnitRotationRads * v.x;
    _fRotationVel = MathUtils::brClamp(_fRotationVel, _fRotationVelMin, _fRotationVelMax);
  }

  //Zoom (Y)
  if (_bUnsnapY) {
    _fZoomVel += perUnitZoom * v.y;
    _fZoomVel = MathUtils::brClamp(_fZoomVel, _fZoomVelMin, _fZoomVelMax);
  }
}
void RTSCamScript::updateRotateAndZoom(std::shared_ptr<CameraNode> cam, float delta) {
  float stop_e = 0.000001f;

  if (_fRotationVel * _fRotationVel > 0.0f) {
    doRotate(cam, _fRotationVel);
    _fRotationVel = _fRotationVel - (_fRotationVel * _fRotationEase * delta);
    _fRotationVel = MathUtils::brClamp(_fRotationVel, _fRotationVelMin, _fRotationVelMax);

    if (_fRotationVel * _fRotationVel < stop_e) {
      _fRotationVel = 0.0f;
    }
  }

  if (_fZoomVel * _fZoomVel > 0.0f) {
    //doZoom(_fZoomVel);
    doRotateZ(cam, _fZoomVel);
    _fZoomVel = _fZoomVel - (_fZoomVel * _fZoomEase * delta);
    _fZoomVel = MathUtils::brClamp(_fZoomVel, _fZoomVelMin, _fZoomVelMax);
    if (_fZoomVel * _fZoomVel < stop_e) {
      _fZoomVel = 0.0f;
    }
  }
}
void RTSCamScript::doRotate(std::shared_ptr<CameraNode> cam, float dRot) {
  mat4 rot = mat4::getRotationRad(dRot, vec3(0, 1, 0));

  vec3 origPos = cam->getPos();

  vec4 mul =
    vec4(_vCamNormal.x,
      _vCamNormal.y,
      _vCamNormal.z,
      1.0f);
  vec4 res = rot * mul;

  _vCamNormal = vec3(res.x, res.y, res.z);
  updateCameraPosition(cam); // Recalc the camera position.
  //updateCameraFollow();
}
void RTSCamScript::doRotateZ(std::shared_ptr<CameraNode> cam, float dRot) {
  vec3 rn = cam->getRightNormal();

  mat4 rot = mat4::getRotationRad(dRot, rn);

  vec3 origPos = cam->getPos();

  vec4 mul =
    vec4(_vCamNormal.x,
      _vCamNormal.y,
      _vCamNormal.z,
      1.0f);
  vec4 res = rot * mul;
  int n = 0;
  _vCamNormal = vec3(res.x, res.y, res.z);
  updateCameraPosition(cam); // Recalc the camera position.
                          //updateCameraFollow();
}
void RTSCamScript::doZoom(std::shared_ptr<CameraNode> cam, float amt) {
  float fZoomVal = 3.0;
  _fCamDist -= amt * fZoomVal;

  updateCameraPosition(cam); // Recalc the camera position.
}
void RTSCamScript::updateCameraPosition(std::shared_ptr<CameraNode> cam) {
  if (_fCamDist >= _fCamDistMax) {
    _fCamDist = _fCamDistMax;
  }
  if (_fCamDist < _fCamDistMin) {
    _fCamDist = _fCamDistMin;
  }
  vec3 cp = _vLookAt + (_vCamNormal * _fCamDist);

  cam->setPos(std::move(cp));
  cam->setLookAt(std::move(_vLookAt));
}

void RTSCamScript::moveCameraWSAD(std::shared_ptr<CameraNode> cam, std::shared_ptr<InputManager> pFingers, float delta) {
  //**Camera
  //Damp Slows us a bit when we zoom out.
  float damp = fabsf(_fCamDist) * 0.001f;
  float factor = 4.0f;
  if (pFingers->shiftHeld()) {
    factor = 12.0f;
  }
  float strafeAmt = fabsf(_fCamDist) / (BottleUtils::getNodeWidth() + damp) * factor * delta;

  std::weak_ptr<CameraNode> cam_w = cam;
  std::function<void()> moveUp = [this, strafeAmt, cam_w]() {
    if (std::shared_ptr<CameraNode> cam_lock = cam_w.lock()) {
      vec3 dir = cam_lock->getViewNormal().xz().normalized();
      _vLookAt += dir * strafeAmt;
    }
  };
  std::function<void()> moveDown = [this, strafeAmt, cam_w]() {
    if (std::shared_ptr<CameraNode> cam_lock = cam_w.lock()) {
      vec3 dir = cam_lock->getViewNormal().xz().normalized();
      _vLookAt += dir * -strafeAmt;
    }
  };
  std::function<void()> moveLeft = [this, strafeAmt, cam_w]() {
    if (std::shared_ptr<CameraNode> cam_lock = cam_w.lock()) {
      vec3 dir = cam_lock->getRightNormal();
      _vLookAt += dir * -strafeAmt;
    }
  };
  std::function<void()> moveRight = [this, strafeAmt, cam_w]() {
    if (std::shared_ptr<CameraNode> cam_lock = cam_w.lock()) {
      vec3 dir = cam_lock->getRightNormal();
      _vLookAt += dir * strafeAmt;
    }
  };

  //Mouse scroll
  float amt = 0.03f;
  float vw = (float)cam->getViewport()->getWidth();
  float vh = (float)cam->getViewport()->getHeight();
  float wa = vw * amt;
  float ha = vh * amt;

  vec2 mp = pFingers->getMousePos_Relative();

  if (getScene()->getWindow()->containsPoint_Global2D(std::move(pFingers->getMousePos_Global()))) {
    if (mp.x < wa) {
      moveLeft();
    }
    if (mp.y < ha) {
      moveUp();
    }
    if (mp.x > vw - wa) {
      moveRight();
    }
    if (mp.y > vh - ha) {
      moveDown();
    }
  }

  //Keyboard
  if (pFingers->keyPressOrDown(SDL_SCANCODE_W)) {
    moveUp();
  }
  if (pFingers->keyPressOrDown(SDL_SCANCODE_S)) {
    moveDown();
  }
  if (pFingers->keyPressOrDown(SDL_SCANCODE_A)) {
    moveLeft();
  }
  if (pFingers->keyPressOrDown(SDL_SCANCODE_D)) {
    moveRight();
  }
  updateCameraPosition(cam);
}
void RTSCamScript::setTarget(std::shared_ptr<CameraNode> cam, vec3& v) {
  _vLookAt = v;
  updateCameraPosition(cam);
}
}//ns Game