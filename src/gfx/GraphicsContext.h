/**
*  @file Context.h
*  @date August 10, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __CONTEXT_15654533273259973242_H__
#define __CONTEXT_15654533273259973242_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*  @class Context
*  @brief Graphics state information.  Contains the OpenGL Context, and the shared data between contexts.
*  @note OpenGL shares some data between contexts, but does not share things like FBOs.
*/
class GraphicsContext : public VirtualMemoryShared<GraphicsContext> {
public:
  GraphicsContext();
  virtual ~GraphicsContext() override;

  void update();

  virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false) = 0;
  virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false) = 0;

  virtual void pushCullFace() = 0;
  virtual void popCullFace() = 0;
  virtual void pushBlend() = 0;
  virtual void popBlend() = 0;
  virtual void pushDepthTest() = 0;
  virtual void popDepthTest() = 0;

  virtual void enableCullFace(bool enable) = 0;
  virtual void enableBlend(bool enable) = 0;
  virtual void enableDepthTest(bool enable) = 0;

  bool isValid() { return _bValid; }

  EngineLoopState::e getLoopState() { return _eLoopState; }
  void setLoopState(EngineLoopState::e ee) { _eLoopState = ee; }

  float& getClearR() { return _fClearR; }
  float& getClearG() { return _fClearG; }
  float& getClearB() { return _fClearB; }
  float& getClearA() { return _fClearA; }

  std::shared_ptr<FpsMeter> getFpsMeter() { return _pFpsMeter; }
  std::shared_ptr<FrameSync> getFrameSync() { return _pFrameSync; }
  std::shared_ptr<Delta> getDelta() { return _pDelta; }

  std::shared_ptr<RenderPipeline> getRenderPipe() { return _pRenderPipe; }
  std::shared_ptr<Picker> getPicker() { return _pPicker; }

  // Shared managers store data accessible across ALL OpenGL graphics contexts.
  // https://stackoverflow.com/questions/51366693/are-pbos-shared-across-opengl-contexts
  std::shared_ptr<ModelCache> getModelCache() { return _pModelCache; }
  std::shared_ptr<ShaderMaker> getShaderMaker() { return _pShaderMaker; }
  std::shared_ptr<TexCache> getTexCache() { return _pTexCache; }
  std::shared_ptr<ParticleMaker> getParty() { return _pParticleMaker; }
  
protected:
  virtual bool init();

private:
  //Shared context data.
  static bool _bCreatedManagers;
  static std::shared_ptr<ModelCache> _pModelCache;
  static std::shared_ptr<TexCache> _pTexCache;
  static std::shared_ptr<ParticleMaker> _pParticleMaker;
  static std::shared_ptr<ShaderMaker> _pShaderMaker;

  //One per context, this essentially calculates glSwapBuffers performance
  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;

  EngineLoopState::e _eLoopState = EngineLoopState::Update;
  float _fClearR = 1.0f;
  float _fClearG = 1.0f;
  float _fClearB = 1.0f;
  float _fClearA = 1.0f;
  bool _bValid = false;

  //This is per context since FBOs aren't shared.
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;
  std::shared_ptr<Picker> _pPicker = nullptr;

  void makeVertexFormats();
  void createManagers();
};

}//ns Game



#endif
