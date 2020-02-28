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
  GraphicsContext(std::shared_ptr<GraphicsApi> api);
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

  std::shared_ptr<GraphicsWindow> getGraphicsWindow();
  std::shared_ptr<CameraNode> getActiveCamera();
  std::shared_ptr<GraphicsApi> getGraphicsApi() { return _pGraphicsApi; }

  float& getClearR() { return _fClearR; }
  float& getClearG() { return _fClearG; }
  float& getClearB() { return _fClearB; }
  float& getClearA() { return _fClearA; }

protected:
  SDL_Window* _pSDLWindow = nullptr;

private:
  std::shared_ptr<GraphicsWindow> _pWindow = nullptr;
  std::shared_ptr<GraphicsApi> _pGraphicsApi = nullptr;
  float _fClearR = 1.0f;
  float _fClearG = 1.0f;
  float _fClearB = 1.0f;
  float _fClearA = 1.0f;
};

}//ns Game



#endif
