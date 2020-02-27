/**
*  @file LightManager.h
*  @date May 6, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __LIGHTMANAGER_14940893702945849424_H__
#define __LIGHTMANAGER_14940893702945849424_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class LightManager
*  @brief Manages both lights, and shadow maps, for all objects which emit.
*/
class LightManager : public VirtualMemoryShared<LightManager> {
public:
  LightManager(std::shared_ptr<Scene> pScene);
  virtual ~LightManager() override;

  void update(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf);

  std::shared_ptr<GpuDeferredParams> getDeferredParams() { return _pGpuDeferredParams; }
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
  std::shared_ptr<Scene> getScene();

private:
  static const int c_iMaxLights = 64;
  std::shared_ptr<GpuDeferredParams> _pGpuDeferredParams = nullptr;
  std::vector<GpuPointLight> _vecGpuPointLights;
  std::vector<GpuDirLight> _vecGpuDirLights;
  std::vector<std::shared_ptr<ShadowBox>> _vecGpuShadowBoxes;
  std::vector<std::shared_ptr<ShadowFrustum>> _vecGpuShadowFrustums;
  bool _bFogDisabled = false;
  float _fLastFogBlend;
  std::shared_ptr<Scene> _pScene = nullptr;

  void updateRenderData();
  void initializeDeferredFogData();
  void setupLights(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf);    // - Sets up all the *enabled* lights for rendering.
};

}//ns Game



#endif
