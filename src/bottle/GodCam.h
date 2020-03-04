///**
//*  @file GodCam.h
//*  @date March 31, 2017
//*  @author MetalMario971
//*/
//#pragma once
//#ifndef __SNAPROOTER_14910183031818674424_H__
//#define __SNAPROOTER_14910183031818674424_H__
//
//#include "../bottle/BottleHeader.h"
//#include "../base/TouchInfo.h"
//
//namespace BR2 {
///**
//* @class GodCam
//* @brief An class to control a 2d-ish rotatable camaera
//* This will become a script
//*/
//class GodCam : public VirtualMemory {
//public:
//  GodCam(std::shared_ptr<GLContext> pc, std::shared_ptr<RenderViewport> pv);
//  virtual ~GodCam() override;
//
//  void moveCameraWSAD(std::shared_ptr<InputManager> pFingers, float delta);
//  void setTarget(vec3& v);
//  void update(std::shared_ptr<InputManager> pf, float dt);
//  void userZoom(float amt);
//  vec3& getCamNormal() { return _vCamNormal; }
//  vec3& getCamLookAt() { return _vLookAt; }
//  void setActive();
//
//private:
//  std::shared_ptr<CameraNode> _pCamera;
//  vec3 _vCamNormal;
//  float _fCamDist = 40;
//  float _fCamDistMax = 500;
//  float _fCamDistMin = 3;
//  vec3 _vLookAt;
//
//  float _fPerUnitRotate = (float)M_PI / 3000.0f;
//  float _fRotationVel = 0.0f; //current vel
//  float _fRotationVelMin = -0.1f; //velocity min/max
//  float _fRotationVelMax = 0.1f;
//  float _fRotationEase = 15.50f; // % per second
//
//  //float _fPerUnitZoom = 0.01f;
//  //float _fZoomVel = 0;
//  //float _fZoomVelMin = -1.1f;
//  //float _fZoomVelMax = 1.1f;
//  //float _fZoomEase = 10.724f; // % per second
//  float _fPerUnitZoom = (float)M_PI / 6000.0f;
//  float _fZoomVel = 0.0f;
//  float _fZoomVelMin = -0.01f;
//  float _fZoomVelMax = 0.01f;
//  float _fZoomEase = 15.50f; // % per second
//  float _fMaxRotZ = 0.80f; //dot product
//  bool _bUnsnapX, _bUnsnapY;
//
//
//  std::shared_ptr<GLContext> _pContext = nullptr;
//
//  TouchInfo _curTouchRmb;
//  TouchInfo _lastTouchRmb;
//  TouchInfo _curTouchLmb;
//  TouchInfo _lastTouchLmb;
//
//  void updateTouches(std::shared_ptr<InputManager> pFingers, float dt);
//  void updateCameraPosition();
//  void updateRotateAndZoom(float delta);
//  void doRotate(float dRot);
//  void doRotateZ(float dRot);
//  void doZoom(float dRot);
//  //void rotateAndZoomCamera(std::shared_ptr<InputManager> pFingers, float dt);
//  void userRotateAndZoom(vec2& v, vec2& vMouse, vec2& vLast, float perUnitRotationRads, float perUnitZoom);
//
//
//
//};
//
//}//ns Game
//
//
//
//#endif
