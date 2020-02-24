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

namespace BR2 {
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

  void makeVertexFormats();

protected:
  virtual bool init();


};

}//ns Game



#endif
