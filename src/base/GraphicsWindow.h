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

namespace BR2 {
/**
*  @class GraphicsWindow
*  @brief Manages rendering to window.
*/
class GraphicsWindow_Internal;
class GraphicsWindow : public VirtualMemoryShared<GraphicsWindow> {
  friend class GraphicsApi;
  friend class OpenGLApi;
  friend class VulkanApi;
public:
  GraphicsWindow(bool ismain);
  virtual ~GraphicsWindow() override;

  virtual void create(string_t title) = 0;
  void step();
  
  void* getSDLWindow();
  std::shared_ptr<WindowViewport> getWindowViewport();
  std::shared_ptr<RenderPipe> getRenderPipe();
  std::shared_ptr<Gui2d> getGui();
  
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void printHelpfulDebug();

protected:
  void makeSDLWindow(string_t title, int rendersystem);
  void initRenderSystem();

  virtual void getDrawableSize(int* w, int* h) = 0;
  virtual void makeCurrent() = 0;
  virtual void swapBuffers() = 0;
private:
  std::unique_ptr<GraphicsWindow_Internal> _pint = nullptr;
  void createRenderPipe();
  void beginRender();
  void endRender();
};

}//ns Game



#endif
