/**
*
*    @file Window.h
*    @date August 11, 2019
*    @author MetalMario971
*
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class GraphicsWindow
*    @brief Window to draw to.
*/
class GraphicsWindow : public VirtualMemoryShared<GraphicsWindow> {
public:
  GraphicsWindow(bool ismain, t_string title, RenderSystem::e sys);
  virtual ~GraphicsWindow() override;

  t_string _title;

  void init();
  void step();
  SDL_Window* getSDLWindow() { return _pSDLWindow; }
  void printHelpfulDebug();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void checkErrorsDbg();
  void checkErrorsRt();

  std::shared_ptr<Scene> getScene() {  return _pScene; }
  void setScene(std::shared_ptr<Scene> s) { _pScene = s; }

  std::shared_ptr<WindowViewport> getWindowViewport() { return _pWindowViewport; }

  std::shared_ptr<UiScreen> getUiScreen() { return _pScreen; }
  std::shared_ptr<FpsMeter> getFpsMeter() { return _pFpsMeter; }
  std::shared_ptr<FrameSync> getFrameSync() { return _pFrameSync; }

  std::shared_ptr<Delta> getDelta() { return _pDelta; }
  std::shared_ptr<GLContext> getGraphicsContext() { return _pContext; }

  //camera should be part of scene & physicsworld
  std::shared_ptr<CameraNode> getCamera() { return _pCamera; }
  void setCamera(std::shared_ptr<CameraNode> pc) { _pCamera = pc; }
  //Should be part of camera, not graphicswindow
  std::shared_ptr<WindowViewport> getViewport() { return _pWindowViewport; }
  std::shared_ptr<PhysicsWorld> getPhysicsWorld() { return _pPhysicsWorld; }
  void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { _pPhysicsWorld = p; }

protected:
  void makeSDLWindow(t_string title, int rendersystem);
  void initRenderSystem();

  void swapBuffers();
  void makeCurrent();
  void getDrawableSize(int* w, int* h);
private:
  std::shared_ptr<Scene> _pScene = nullptr; //The scene we render.
  std::shared_ptr<WindowViewport> _pWindowViewport = nullptr;

  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<CameraNode> _pCamera = nullptr;//this is part of scene.
  std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
  std::shared_ptr<UiScreen> _pScreen = nullptr;

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
