/**
*  @file DeferredFramebuffer.h
*  @date May 28, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __DEFERREDFRAMEBUFFER_1464419631987791295_H__
#define __DEFERREDFRAMEBUFFER_1464419631987791295_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/FramebufferBase.h"
#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class DeferredFramebuffer
*  @brief Deferred render target. This can be either a direct framebuffer & textures OR a multisample framebuffer.
*/
class DeferredFramebuffer : public FramebufferBase {
public:
  DeferredFramebuffer(std::shared_ptr<GLContext> pContext, uint32_t iWidth, uint32_t iHeight, bool bMultisample, uint32_t nSamples, vec4& vClear);
  virtual ~DeferredFramebuffer() override;

  virtual void init(int32_t iWidth, int32_t iHeight, std::shared_ptr<BufferRenderTarget> sharedDepth, std::shared_ptr<BufferRenderTarget> sharedPick) override;
  std::shared_ptr<BufferRenderTarget> getTarget(RenderTargetType::e eType);
  virtual void beginRender();
  virtual void endRender();
  int getNumNonDepthTargets();

private:
  bool _bMultisample = false;
  uint32_t _nMsaaSamples = 2;
  bool _bUf0 = false;
  bool _bUf1 = false; //For the offscreen stuff (later)
  std::shared_ptr<VaoDataGeneric> _pBloomVaoPass;
  int getNumTargets();
  bool getIsBloomEnabled();
};

}//ns BR2



#endif
