/**
*
*    @file FlyCam.h
*    @date April 17, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __FLYCAM_14924442872935518543_H__
#define __FLYCAM_14924442872935518543_H__

#include "../display/DisplayHeader.h"

namespace Game {
/**
*    @class FlyCam
*    @brief
*
*/
class FlyCam : public VirtualMemory {
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<Viewport> _pViewport = nullptr;
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

  //  TouchInfo _curTouchRmb;
   // TouchInfo _lastTouchRmb;


    void rotateCameraNormal(float rotX, float rotY);
    void updateCameraPosition();
    void updateRotate(std::shared_ptr<Fingers> pFingers);
public:
    void moveCameraWSAD(std::shared_ptr<Fingers> pFingers, float delta);
    void userZoom(float amt);
    void update(std::shared_ptr<Fingers> pFingers, float dt);
    void setActive();

    void setPosAndLookAt(vec3&& pos, vec3&& lookat);
    std::shared_ptr<CameraNode> getCam() { return _pCamera; }
    FlyCam(std::shared_ptr<GLContext> ct, std::shared_ptr<Viewport> pv);
    virtual ~FlyCam() override;

};

}//ns Game



#endif
