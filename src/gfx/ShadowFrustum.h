/**
*  @file ShadowFrustum.h
*  @date January 25, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADOWFRUSTUM_15169135621311229167_H__
#define __SHADOWFRUSTUM_15169135621311229167_H__

#include "../math/MathHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"
namespace BR2 {
/**
*  @class ShadowFrustum
*  @brief Frustum for rendering shadows.
*/
class ShadowFrustum_Internal;
class ShadowFrustum : public VirtualMemoryShared<ShadowFrustum> {
public:
  ShadowFrustum(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadows);
  virtual ~ShadowFrustum() override;

  void init();
  void updateAndCullAsync(CullParams& cp);
  void endRenderShadowFrustum();
  void beginRenderShadowFrustum();
  void debugRender(RenderParams& rp);
  void renderShadows(std::shared_ptr<ShadowFrustum> pShadowFrustumMaster);

  std::shared_ptr<FrustumBase> getFrustum();
  GLint getGlTexId();
  mat4* getViewMatrixPtr();
  mat4* getProjMatrixPtr();
  mat4* getPVBPtr();
  void setChanged();

private:
  std::unique_ptr<ShadowFrustum_Internal> _pint = nullptr;
};

}//ns Game



#endif
