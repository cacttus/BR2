/**
*  @file Window.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__


#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace Game {
/**
*  @class GraphicsWindow
*  @brief Manages rendering to window.
*/
class GraphicsWindow : public VirtualMemoryShared<GraphicsWindow> {
  std::shared_ptr<WindowViewport> _pWindowViewport = nullptr;
  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  bool _bIsMainWindow = false;

  std::shared_ptr<Gui2d> _pGui = nullptr;

  friend class GraphicsApi;
  friend class OpenGLApi;
  friend class VulkanApi;


  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  void createRenderPipe();
  void beginRender();
  void endRender();

  void toggleFullscreen();
protected:
  void makeSDLWindow(t_string title, int rendersystem);
  void initRenderSystem();

  virtual void getDrawableSize(int* w, int* h) = 0;
  virtual void makeCurrent() = 0;
  virtual void swapBuffers() = 0;
public:
  GraphicsWindow(bool ismain);
  virtual ~GraphicsWindow() override;

  virtual void create(t_string title) = 0;
  void step();
  SDL_Window* getSDLWindow() { return _pSDLWindow; }
  void printHelpfulDebug();
  std::shared_ptr<WindowViewport> getWindowViewport() { return _pWindowViewport; }
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  std::shared_ptr<RenderPipe> getRenderPipe() {
    return _pRenderPipe;
  }
  std::shared_ptr<Gui2d> getGui() {
    return _pGui;
  }

};

}//ns Game



#endif
