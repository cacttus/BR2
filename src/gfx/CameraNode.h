/**
*    @file BaseCamera.h
*    @author MetalMario971
*
*    © 2011 
*    @date january 2010
*    @date aug 2010 - moved src to cpp file
*/
#pragma once
#ifndef __BASE_CAMERA_H__
#define __BASE_CAMERA_H__


#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"
#include "../world/PhysicsNode.h"

namespace Game {
/**
*    @class BaseCamera
*    @brief Base class for cameras in the GL.
*
*    Superclass of @cBaseCamera2D, @cBaseCamera3D
*/
class CameraNode  : public PhysicsNode {
protected:
    std::shared_ptr<WindowViewport> _pViewport = nullptr;        // - Viewport is a class because the values might change.
    float _f_hfov = 60;            // - Field of view.

    std::shared_ptr<FrustumBase> _pMainFrustum = nullptr;
    mat4 _mView;
    mat4 _mProj;
    vec3 _vLookAt;
    vec3 _vUp;
    vec3 _vRight;

    float _fMinZoomDist = 20.0f;
    float _fMaxZoomDist = 500.0f;
    float _fCurZoomDist = 100.0f;

    vec3 _vWorldUp;
    ProjectionMode::e _eProjectionMode = ProjectionMode::e::Perspective;

public:
    CameraNode(std::shared_ptr<WindowViewport> ppViewport);
    static std::shared_ptr<CameraNode> create(std::shared_ptr<WindowViewport> ppViewport);
    virtual ~CameraNode() override;

    void zoom(float amt);
    const vec3& getLookAt() { return _vLookAt ; }
    void setLookAt(vec3&& v) {_vLookAt = v;}
    const mat4& getView() {return _mView;}
    const mat4& getProj() {return _mProj;}
    void setWorldUp(vec3&& v){ _vWorldUp = v; }
    void setProjectionMode(ProjectionMode::e eMode) { _eProjectionMode = eMode; }
    vec3 getLookAtOffset();

    virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;            // - Main update function, must be called if you override it.
    void setupProjectionMatrix();// - Manipulate the view projection matrix to project points 
    void setupViewMatrix();    // - Manipulate the modelview matrix to orient the camera
    ProjectedRay projectPoint(vec2& mouse);    // - Project a ray into the screen.
    Ray_t projectPoint2(vec2& mouse);
    void setFOV(t_radians fov);        // - Set Field of View
    float getFOV() const {  return _f_hfov;  }            
    std::shared_ptr<WindowViewport> getViewport() { return _pViewport; }
    const vec3& getRightNormal() { return _vRight;  }
    const vec3& getUpNormal() { return _vUp;  }
    std::shared_ptr<FrustumBase> getFrustum() {return _pMainFrustum; } 


};

}//namespace Game
#endif