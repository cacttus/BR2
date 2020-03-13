/**
*  @file RTSCamScript.h
*  @date March 31, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __RTSCAMSCRIPT_14910183031818674424_H__
#define __RTSCAMSCRIPT_14910183031818674424_H__

#include "../bottle/BottleHeader.h"
#include "../base/TouchInfo.h"
#include "../world/Script.h"

namespace BR2 {
/**
*  @class RTSCamScript
*  @brief An script to control a 2d-ish rotatable camera for viewing the world as an RTS.
*  This class has 2 modes: Focus mode, and Free Mode.  Free mode is a camera that roams based on the current view layer.  Focus mode is when the camera is focused on an object in the world (for viewing closer).
*  Controls: RMB+Drag = Left/Right rotates the camera left and right.  Up/Down changes the height of the camera.
*/
class RTSCamScript : public Script {
public:
  RTSCamScript(std::shared_ptr<World25> pw );
  virtual ~RTSCamScript() override;


  virtual void onStart() override;
  virtual void onUpdate(float dt) override;
  virtual void onExit() override;

  void setFocusObject(std::shared_ptr<SceneNode> s);

private:
  std::shared_ptr<SceneNode> _pFocusObject = nullptr;

  float _fViewAngle = (float)M_PI * 0.25f;
  float _fZoomDist=50;
  float _fZoomHMax=100;
  float _fZoomHMin=10;
  vec3 _vLookAt;
  vec3 _vCamNormal;

  float _fPerUnitRotate = (float)M_PI / 3000.0f;
  float _fRotationVel = 0.0f; //current vel
  float _fRotationVelMin = -0.1f; //velocity min/max
  float _fRotationVelMax = 0.1f;
  float _fRotationEase = 15.50f; // % per second

  float _fPerUnitZoom = (float)M_PI / 6000.0f;
  float _fZoomVel = 0.0f;
  float _fZoomVelMin = -0.01f;
  float _fZoomVelMax = 0.01f;
  float _fZoomEase = 15.50f; // % per second
  float _fMaxRotZ = 0.80f; //dot product
  bool _bUnsnapX, _bUnsnapY;

  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<World25> _pWorld25 = nullptr;

  TouchInfo _curTouchRmb;
  TouchInfo _lastTouchRmb;
  TouchInfo _curTouchLmb;
  TouchInfo _lastTouchLmb;

  std::shared_ptr<Path> _pPath;

  bool focusMode();
  void updateTouches(std::shared_ptr<CameraNode> cam, std::shared_ptr<InputManager> pFingers, float dt);
  void updateCameraPosition(std::shared_ptr<CameraNode> cam);
  void doRotate(std::shared_ptr<CameraNode> cam, float dRot);
  void doRotateZ(std::shared_ptr<CameraNode> cam, float dRot);
  void doZoom(std::shared_ptr<CameraNode> cam, float dRot);
  void userRotateAndZoom(std::shared_ptr<CameraNode> cam, vec2& v, vec2& vMouse, vec2& vLast, float perUnitRotationRads, float perUnitZoom);
  void updateRotateAndZoom(std::shared_ptr<CameraNode> cam, float delta);
  void moveCameraWSAD(std::shared_ptr<CameraNode> cam, std::shared_ptr<InputManager> pFingers, float delta);
  void setTarget(std::shared_ptr<CameraNode> cam, vec3& v);
  vec3& getCamNormal() { return _vCamNormal; }
  vec3& getCamLookAt() { return _vLookAt; }
};

}//ns Game



#endif
