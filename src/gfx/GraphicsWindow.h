/**
*  @file GraphicsWindow.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"
#include "../base/SDLIncludes.h"

namespace BR2 {

class RenderTarget : public VirtualMemoryShared<RenderTarget> {
public:
  RenderTarget() {}
  virtual ~RenderTarget() override {}

  virtual int32_t getWidth() = 0;
  virtual int32_t getHeight() = 0;
  std::shared_ptr<GLContext> getContext() { return _pContext; }
protected:
  std::shared_ptr<GLContext> _pContext = nullptr;
};
/**
*  @class GraphicsWindow
*  @brief Operating system window that is the final render output.  All rendering-surface related data is contained in the window.
*/
class GraphicsWindow : public RenderTarget {
public:
  GraphicsWindow(bool ismain, string_t title, RenderSystem::e sys);
  virtual ~GraphicsWindow() override;

  void init();
  void step() ;
  void idle(int64_t us);

  virtual int32_t getWidth() override;
  virtual int32_t getHeight() override;

  SDL_Window* getSDLWindow() { return _pSDLWindow; }
  void printHelpfulDebug();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);

  void setScene(std::shared_ptr<Scene> s) { _pScene=s; }
  std::shared_ptr<Scene> getScene() { return _pScene; }
  std::shared_ptr<FrameSync> getFrameSync() { return _pFrameSync; }
  std::shared_ptr<RenderPipeline> getRenderPipeline() { return _pRenderPipe; }

protected:
  void makeSDLWindow(string_t title, int rendersystem);
  void initRenderSystem();
  void swapBuffers();
  void makeCurrent();
  void getDrawableSize(int* w, int* h);

private:
  string_t _title = "";
  std::shared_ptr<Scene> _pScene = nullptr; //The scene we render.
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;

  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  int32_t _iLastWidth = 0;
  int32_t _iLastHeight = 0;
  int32_t _iFullscreenToggleWidth = 0;
  int32_t _iFullscreenToggleHeight = 0;

  bool _bIsMainWindow = false;

  void createRenderPipe();
  void beginRender();
  void endRender();
  void toggleFullscreen();
  void createManagers();
  void createSDL_OpenGLWindow(string_t title);
};

}//ns BR2



#endif
