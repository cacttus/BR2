/**
*  @file ShadowMapFrustum.h
*  @date April 25, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADOWMAPFRUSTUM_14616351763757858195_H__
#define __SHADOWMAPFRUSTUM_14616351763757858195_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class PointLightShadowBox
*  @brief Shadow map box for a point light.  A collection of 6 @cShadowFrustum.
*/
class ShadowBox_Internal;
class ShadowBox : public VirtualMemoryShared<ShadowBox> {
public:
  ShadowBox(std::shared_ptr<LightNodePoint> refLightSource, int32_t iWidth, int32_t iHeight);
  virtual ~ShadowBox() override;
  void init();
  void updateAndCullAsync(CullParams& rp);
  void renderShadows(std::shared_ptr<ShadowBox> pMaster);
  void debugRender(RenderParams& rp);
  void copyAndBlendToShadowMap(std::shared_ptr<ShadowBox> pBox);
  void beginRenderShadowBox();
  void beginRenderSide(BoxSide::e side);
  void endRenderSide();
  void endRenderShadowBox();
  void updateScreenQuad();

  std::shared_ptr<LightNodePoint> getLightSource();
  float& getSmallBoxSize();
  uint32_t getFboWidth();
  uint32_t getFboHeight();
  GLuint getGlTexId();

private:
  std::unique_ptr<ShadowBox_Internal> _pint = nullptr;
};

}//ns Game



#endif
