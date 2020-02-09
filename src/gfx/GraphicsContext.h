/**
*
*    @file Context.h
*    @date August 10, 2019
*    @author MetalMario971
*
*    © 2019
*
*
*/
#pragma once
#ifndef __CONTEXT_15654533273259973242_H__
#define __CONTEXT_15654533273259973242_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class Context
*    @brief Graphics state information.
*/
class GraphicsContext : public VirtualMemoryShared<GLContext> {
public:
  GraphicsContext();
  virtual ~GraphicsContext() override;

  virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false) = 0;
  virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false) = 0;

  std::shared_ptr<LightManager> getLightManager() { return _pLightManager; }
  std::shared_ptr<ModelCache> getModelCache() { return _pModelCache; }
  std::shared_ptr<RenderPipeline> getRenderPipe() { return _pRenderPipe; }
  std::shared_ptr<ShaderMaker> getShaderMaker() { return _pShaderMaker; }
  std::shared_ptr<TexCache> getTexCache() { return _pTexCache; }
  std::shared_ptr<ParticleMaker> getParty() { return _pParticleMaker; }
  std::shared_ptr<Picker> getPicker() { return _pPicker; }

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


private:
  EngineLoopState::e _eLoopState = EngineLoopState::Update;
  float _fClearR = 1.0f;
  float _fClearG = 1.0f;
  float _fClearB = 1.0f;
  float _fClearA = 1.0f;
  bool _bValid = false;

  std::shared_ptr<FlyCam> _pFlyCam = nullptr;

  std::shared_ptr<LightManager> _pLightManager = nullptr;
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;
  std::shared_ptr<ModelCache> _pModelCache = nullptr;
  std::shared_ptr<TexCache> _pTexCache = nullptr;
  std::shared_ptr<ParticleMaker> _pParticleMaker = nullptr;
  std::shared_ptr<ShaderMaker> _pShaderMaker = nullptr;
  std::shared_ptr<Picker> _pPicker = nullptr;

  void makeVertexFormats();

protected:
  virtual bool init();


};

}//ns Game



#endif
