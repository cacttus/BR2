/**
*
*    @file LightNode.h
*    @date October 4, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#pragma once
#ifndef __LIGHTNODE_115916903195042306423126_H__
#define __LIGHTNODE_115916903195042306423126_H__


#include "../world/PhysicsNode.h"
#include "../display/DisplayHeader.h"
#include "../math/MathAll.h"

namespace Game {

class LightNodeBase : public PhysicsNode {
    bool _bEnableShadows = false;
protected:
    Color4f _color;
    //float _fSpecExp = 2.0f;
    //vec3 _vSpecColor;
    vec3 _vGpuBufferedPosition; // Temporary storage for the GPU - DO NOT CHANGE
protected:
    virtual void init() override;
public:
    LightNodeBase(bool bShadow  ) ;
    virtual ~LightNodeBase() override ;

    Color4f* getColorPtr() { return &_color; }
    Color4f& getColor() { return _color; }
    void setLightColor(const Color4f&& c) { _color = c; _color.clampValues(); }
    // vec3& getSpecColor() { return _vSpecColor; }
    // float& getSpecExp() { return _fSpecExp; }
    virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;
    vec3* getFinalPosPtr();
    bool shadowsEnabled();
};

class LightNodeDir : public LightNodeBase {
    vec3 _vLookAt;
    vec3 _vDir;
    vec3 _vUp;
    vec3 _vRight;
    std::shared_ptr<GpuDirLight> _pGpuLight = nullptr;
    std::shared_ptr<ShadowFrustum> _pShadowFrustum = nullptr;
protected:
    virtual void init() override;
public:
    LightNodeDir(bool bShadow);
    static std::shared_ptr<LightNodeDir> LightNodeDir::create(bool bShadow);
    virtual ~LightNodeDir() override;
    const vec3& getLookAt() { return _vLookAt;}
    const vec3& getDir() { return _vDir; }
    const vec3& getUp () { return _vUp; }
    void setLookAt(const vec3& v);
    bool renderShadows(std::shared_ptr<ShadowFrustum> pf);
    virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;
    virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
    std::shared_ptr<ShadowFrustum> getShadowFrustum() { return _pShadowFrustum;}
    std::shared_ptr<GpuDirLight> getGpuLight() { return _pGpuLight; }
    void setMaxDistance(float f);
};
class LightNodePoint : public LightNodeBase {

    float _cInfiniteLightRadius = FLT_MAX;
    float _fRadius = 20.0f; // maximum radius of the light
    float _f_1_Radius_2 = 0.0f;    // 1/(r*r)
    float _attenuation = 0.5f;

    bool _bFlickerEnabled = false;
    float _fFlickerValue = 0;
    float _fFlickerAddRadius = 0;
    float _fFlickerMaxRadius = 1.0f;
    float _fLastRandomValue = 0.0f;
    float _fNextRandomValue = 0.0f;

    std::shared_ptr<ShadowBox> _pShadowBox = nullptr;

    std::shared_ptr<GpuPointLight> _pGpuLight = nullptr;

    void updateFlicker();
protected:
    virtual void init() override;
public:
    LightNodePoint(bool bhasShadowBox);
    static std::shared_ptr<LightNodePoint> create(bool bhasShadowBox);
    virtual ~LightNodePoint() override;
    std::shared_ptr<ShadowBox> getShadowBox() { return _pShadowBox; }

    virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;

    std::shared_ptr<GpuPointLight> LightNodePoint::getGpuLight() {return _pGpuLight;}
    bool renderShadows(std::shared_ptr<ShadowBox> pf); // - Called by lightmanager to set up the light
    void setEnableFlicker(bool b){ _bFlickerEnabled = b; if(_bFlickerEnabled==FALSE) _fFlickerAddRadius=0.0f; }
    float getLightAttenuation(){ return _attenuation; }
    void setLightAttenuation(float f){ _attenuation = f; }
    
    float getLightRadius1_2(){ return _f_1_Radius_2; }
    float getLightRadius();
    void setLightRadius(float r);
    virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
};

}//ns game



#endif
