/**
*  @file Framebuffer2.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __FRAMEBUFFER2_14939705342264419216_H__
#define __FRAMEBUFFER2_14939705342264419216_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace BR2 {
class DOFFbo : public VirtualMemoryShared<DOFFbo> {
public:
  DOFFbo(std::shared_ptr<GLContext> ct, int32_t w, int32_t h);
  virtual ~DOFFbo();
  GLuint _uiDOFFboId;
  GLuint _uiTexId0;
  std::shared_ptr<GLContext> getContext() { return _pContext; }
private:
  std::shared_ptr<GLContext> _pContext = nullptr;
};
class RenderPass : public VirtualMemoryShared<RenderPass> {
public:
  //Something to consider in the future.
private:
};
/**
*  @class RenderPipe
*  @brief Manages the render pipeline.
*  TODO: Each render pass should be a set of classes, giving us the ability to swap render passes in/out of the render pipeline.
*/
class RenderPipeline : public VirtualMemoryShared<RenderPipeline> {
public:
  RenderPipeline(std::shared_ptr<GraphicsWindow> w);
  virtual ~RenderPipeline() override;

  void init(int32_t iWidth, int32_t iHeight, string_t strEnvTexturePath);
  std::shared_ptr<DeferredFramebuffer> getBlittedDeferred() { return _pBlittedDeferred; }
  void renderSceneTexture(PipeBits _pipeBits);
  void renderScene(std::shared_ptr<Scene> pScene, PipeBits pipeBits);
  void resizeScreenBuffers(int32_t   w, int32_t h);
  std::shared_ptr<Img32> getResultAsImage();
  const vec4& getClear();
  void setClear(vec4& v);
  std::shared_ptr<Picker> getPicker() { return _pPicker; }
  std::shared_ptr<GLContext> getContext();
  std::shared_ptr<GraphicsWindow> getWindow() { return _pWindow; }
  int32_t getBufferWidth() { return _iLastWidth; }
  int32_t getBufferHeight() { return _iLastHeight; }
private:
  bool _bMsaaEnabled = false;
  uint32_t _nMsaaSamples = 0;
  std::shared_ptr<MeshNode> _pQuadMesh = nullptr;
  std::shared_ptr<ShaderBase> _pDeferredShader = nullptr;
  std::shared_ptr<ShaderBase> _pForwardShader = nullptr;

  int32_t _iLastWidth, _iLastHeight;  //Last weidth/height gotten from the screen manager.

  std::shared_ptr<BufferRenderTarget> _pMsaaDepth = nullptr;
  std::shared_ptr<BufferRenderTarget> _pBlittedDepth = nullptr;
  std::shared_ptr<BufferRenderTarget> _pPick = nullptr;
  std::shared_ptr<DeferredFramebuffer> _pMsaaDeferred = nullptr; //If no multisampling is enabled this is equal to the blittedFramebuffer object
  std::shared_ptr<DeferredFramebuffer> _pBlittedDeferred = nullptr;
  std::shared_ptr<ForwardFramebuffer> _pMsaaForward = nullptr;
  std::shared_ptr<ForwardFramebuffer> _pBlittedForward = nullptr;
  std::shared_ptr<ShadowBox> _pShadowBoxFboMaster = nullptr;
  std::shared_ptr<ShadowFrustum> _pShadowFrustumMaster = nullptr;
  std::shared_ptr<DOFFbo> _pDOFFbo = nullptr;
  std::shared_ptr<Texture2DSpec> _pEnvTex = nullptr;//Enviro map - for mirrors (coins)
  std::shared_ptr<Picker> _pPicker = nullptr;
  std::shared_ptr<GraphicsWindow> _pWindow = nullptr;
  vec4 _vClear;
  bool _bRenderInProgress = false;

  void blitDeferredRender(std::shared_ptr<Scene> pScene, std::shared_ptr<CameraNode> cam);
  void createQuadMesh(int w, int h);
  void checkMultisampleParams();
  void checkDeviceCaps(int iWidth, int iHeight);

  void saveScreenshot();
  void copyMsaaSamples(std::shared_ptr<FramebufferBase> msaa, std::shared_ptr<FramebufferBase> blitted);
  void debugForceSetPolygonMode();
  void releaseFbosAndMesh();
  void setShadowUf();

  void beginRenderDeferred();
  void endRenderDeferred();
  void beginRenderForward();
  void endRenderForward();
  void renderShadows(std::shared_ptr<CameraNode> cam);
  void beginRenderShadows();
  void endRenderShadows();

  void endRenderAndBlit(std::shared_ptr<CameraNode> pCam);

  void postProcessDOF(std::shared_ptr<CameraNode> cam);
  void enableDisablePipeBits();
};


}//ns BR2



#endif
