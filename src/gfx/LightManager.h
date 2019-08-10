/**
*
*    @file LightManager.h
*    @date May 6, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __LIGHTMANAGER_14940893702945849424_H__
#define __LIGHTMANAGER_14940893702945849424_H__

#include "../gfx/DisplayHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
class LightManager : public VirtualMemory {
    static const int c_iMaxLights = 64;
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<GpuDeferredParams> _pGpuDeferredParams = nullptr;
    std::vector<GpuPointLight> _vecGpuPointLights;
    std::vector<GpuDirLight> _vecGpuDirLights;
    std::vector<std::shared_ptr<ShadowBox>> _vecGpuShadowBoxes;
    std::vector<std::shared_ptr<ShadowFrustum>> _vecGpuShadowFrustums;
    bool _bFogDisabled = false;
    float _fLastFogBlend;
    
    void updateRenderData();
    void initializeDeferredFogData();
    void setupLights(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf);    // - Sets up all the *enabled* lights for rendering.
public:
    LightManager(std::shared_ptr<GLContext> tc);
    virtual ~LightManager() override;

    void update(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf);

    std::shared_ptr<GpuDeferredParams> getDeferredParams() { return _pGpuDeferredParams; }
    //std::vector<LightSourcePoint*>& getActiveLights() { return _vecActivePointLights; }
    void* getGpuPointLightBuffer() { return _vecGpuPointLights.data(); }
    void* getGpuDirLightBuffer() { return _vecGpuDirLights.data(); }
    void setFogDisabled(bool b) { _bFogDisabled = b; }
    bool getFogDisabled() { return _bFogDisabled; }
    void setHdrToneMapExponent(float val);
    void setHdrSampleSpaceExponent(float val);

    std::vector<std::shared_ptr<ShadowBox>>& getGpuShadowBoxes() { return  _vecGpuShadowBoxes; }
    std::vector<std::shared_ptr<ShadowFrustum>>& getGpuShadowFrustums() { return  _vecGpuShadowFrustums; }
    std::vector<std::shared_ptr<ShadowBox>> getAllShadowBoxes();
    std::vector<std::shared_ptr<ShadowFrustum>> getAllShadowFrustums();

};

}//ns Game



#endif
