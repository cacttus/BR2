/**
*  @file FlyingCameraControls.h
*  @date April 17, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __FLYCAM_14924442872935518543_H__
#define __FLYCAM_14924442872935518543_H__

#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"
#include "../base/CSharpScript.h"

namespace BR2 {
/**
*  @class FlyingCameraControls
*  @brief A camera that lets you fly around.  This is actually a control class for the camera and not the camera itself.
*/
class FlyingCameraControls : public CSharpScript {
public:
  FlyingCameraControls(std::shared_ptr<RenderViewport> pv, std::shared_ptr<Scene> pscene);
  virtual ~FlyingCameraControls() override;

  virtual void start() override;
  virtual void update() override;

  void moveCameraWSAD(std::shared_ptr<InputManager> pInput, float delta);
  void userZoom(float amt);
  void update(std::shared_ptr<InputManager> pInput, float dt);
  void setActive();

  void setPosAndLookAt(vec3&& pos, vec3&& lookat);
  std::shared_ptr<CameraNode> getCam() { return _pCamera; }

private:
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
  std::shared_ptr<CameraNode> _pCamera = nullptr;
  vec2 _vMousePress;
  vec3 _vCamPos;
  vec3 _vMoveVel;
  float _fMaxMoveVel = 2.0f;
  float _fMoveDamp = 9.0f;  //m/s

  vec3 _vCamNormal;
  float _fPerUnitRotate = (float)M_PI / 2000.0f;
  float _fRotationVelX = 0.0f; //current vel
  float _fRotationVelY = 0.0f; //current vel
  float _fRotationVelMin = -0.1f; //velocity min/max
  float _fRotationVelMax = 0.1f;
  float _fRotationEase = 15.50f; // % per second

  void rotateCameraNormal(float rotX, float rotY);
  void updateCameraPosition();
  void updateRotate(std::shared_ptr<InputManager> pInput);
};

}//ns Game



#endif
