#include "../base/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../gfx/CameraNode.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/FreeCameraScript.h"
#include "../model/SceneNode.h"
#include "../world/Scene.h"

namespace BR2 {
FreeCameraScript::FreeCameraScript() {
}
FreeCameraScript::~FreeCameraScript() {
}
void FreeCameraScript::onStart() {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (cam == nullptr) {
    BRLogError("Script could not start, object node was not set.");
    Gu::debugBreak();
    return;
  }

  cam->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  

  std::shared_ptr<LightNodePoint> p = LightNodePoint::create("camLight", vec3(0, 1, 0), 10, vec4(1, 1, 0.5, 1), "", true);
  cam->attachChild(std::dynamic_pointer_cast<TreeNode>(p));
}
void FreeCameraScript::onUpdate(float delta) {
  std::shared_ptr<CameraNode> cn = getNode<CameraNode>();
  if (cn != nullptr) {
    std::shared_ptr<InputManager> im = cn->getInput();
    if (im) {
      if (cn->active() == true) {
        update(im, delta);
        moveCameraWSAD(im, delta);
      }
    }
    else {
      BRLogErrorCycle("Node input could not be found.");
    }
  }
  else {
    BRLogErrorCycle("Node not found.");
  }
}
void FreeCameraScript::onExit() {
}
void FreeCameraScript::moveCameraWSAD(std::shared_ptr<InputManager> pInput, float delta) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  float strafeAmt = 10.0; 

  if (pInput->shiftHeld()) {
    strafeAmt *= 5;
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
  cam->setVelocity(std::move(cam->getVelocity() + vel));
}
void FreeCameraScript::update(std::shared_ptr<InputManager> pInput, float dt) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  //Capture & u8pdate input
  ButtonState::e eLmb = pInput->getLmbState();
  vec2 vLast = pInput->getLastMousePos_Relative();
  t_timeval us = Gu::getMicroSeconds();

  updateRotate(pInput);

  //set cam pos
  cam->setPos(std::move(cam->getPos() + cam->getVelocity() * dt));// += _vMoveVel;

  //dampen the velocity
  vec3 v_n;
  float v_len;
  cam->getVelocity().len_and_norm(v_n, v_len);
  float v_new_len = v_len - v_len * _fMoveDamp * dt;
  v_new_len = MathUtils::brClamp(v_new_len, 0.0f, _fMaxMoveVel);
  cam->setVelocity(std::move(v_n * v_new_len));

  RenderParams rp;
  rp.setCamera(cam);
}
void FreeCameraScript::updateRotate(std::shared_ptr<InputManager> pInput) {
  vec2 vMouse = pInput->getMousePos_Relative();
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

  if (vDelta.x != 0 || vDelta.y != 0) {
    rotateCameraNormal(_fPerUnitRotate * vDelta.x, _fPerUnitRotate * -vDelta.y);
  }
}
void FreeCameraScript::rotateCameraNormal(float rotX, float rotY) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  // mat4 rot = mat4::getRotationRad(dRot, vec3(0, 1, 0));
  vec3 camPos = getNode()->getScene()->getActiveCamera()->getPos();
  vec3 rt = cam->getRightNormal();
  vec3 up = cam->getUpNormal();

  vec3 vNewCamNormal = (cam->getViewNormal() + rt * rotX + up * rotY).normalize();

  float e = 0.0003;
#define N_INR(a, b) ((a >= (b-e)) && (a <= (b+e)))

  if (N_INR(vNewCamNormal.y, -1.0) || N_INR(vNewCamNormal.y, 1.0)) {
    //Keep vCamNormal - don't let the user rotate to -1, or 1
  }
  else {
    cam->setViewNormal(vNewCamNormal);
  }

  vec3 vn = cam->getViewNormal();

  //Additional checking to make sure we don't flip the UI
  if (vn.y == 1.0f) {
    vn.y = 0.98f;
    vn.x = 0.01f;
    vn.z = 0.01f;
    vn.normalize();
  }
  if (vn.y == -1.0f) {
    vn.y = -0.98f;
    vn.x = -0.01f;
    vn.z = -0.01f;
    vn.normalize();
  }
  cam->setViewNormal(vn);
}
void FreeCameraScript::setPosAndLookAt(vec3&& pos, vec3&& lookat) {
  std::shared_ptr<CameraNode> cam = getNode<CameraNode>();
  if (!cam) {
    return;
  }

  cam->setPos(std::move(pos));
  cam->setViewNormal(std::move(lookat - pos));
}

}//ns BR2
