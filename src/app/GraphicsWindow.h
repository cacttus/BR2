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

namespace Game {
//Pass Scene + RenderTarget to the RenderPipe
class RenderTarget : public HasGraphicsContext<RenderTarget> {
public:
  std::shared_ptr<RenderViewport> getViewport() { return _pViewport; }
private:
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
};

/**
*  @class GraphicsWindow
*  @brief Window to draw to.
*/
class GraphicsWindow : public RenderTarget {
public:
  GraphicsWindow(std::shared_ptr<GraphicsContext> context, bool ismain, t_string title, RenderSystem::e sys);
  virtual ~GraphicsWindow() override;

  void init();
  void step();
  SDL_Window* getSDLWindow() { return _pSDLWindow; }
  void printHelpfulDebug();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void checkErrorsDbg();
  void checkErrorsRt();

  std::shared_ptr<Scene> getScene() { return _pScene; }
  void setScene(std::shared_ptr<Scene> s) { _pScene = s; }
  std::shared_ptr<FpsMeter> getFpsMeter() { return _pFpsMeter; }
  std::shared_ptr<FrameSync> getFrameSync() { return _pFrameSync; }
  std::shared_ptr<Delta> getDelta() { return _pDelta; }

protected:
  void makeSDLWindow(t_string title, int rendersystem);
  void initRenderSystem();
  void swapBuffers();
  void makeCurrent();
  void getDrawableSize(int* w, int* h);

private:
  t_string _title = "";
  std::shared_ptr<Scene> _pScene = nullptr; //The scene we render.

  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<GLContext> _pContext = nullptr;
  //std::shared_ptr<CameraNode> _pCamera = nullptr;//this is part of scene.

  //Renderpipe should be on the window, or at least, on a renderable "target"
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;

  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  bool _bIsMainWindow = false;

  void createRenderPipe();
  void beginRender();
  void endRender();
  void toggleFullscreen();
  void createManagers();
  void createSDL_OpenGLWindow(t_string title);
};

}//ns Game



#endif
