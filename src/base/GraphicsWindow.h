/**
*  @file Window.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__


#include "../gfx/RenderTarget.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class GraphicsWindow
*  @brief Manages rendering to window.
*/
class GraphicsWindow_Internal;
class GraphicsWindow : public RenderTarget {
  friend class GraphicsApi;
  friend class OpenGLApi;
  friend class VulkanApi;
public:
  GraphicsWindow(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLContext> ct, SDL_Window* win);
  virtual ~GraphicsWindow() override;

  static std::shared_ptr<GraphicsWindow> create(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLContext> ct, SDL_Window* win);
  void init();
  void step();
  void idle(int64_t us);

  void initRenderSystem();
  virtual int32_t getWidth() override;
  virtual int32_t getHeight() override;
  string_t getTitle();
  
  SDL_Window* getSDLWindow();
  std::shared_ptr<RenderViewport> getViewport();
  std::shared_ptr<RenderPipe> getRenderPipe();
  std::shared_ptr<Scene> getScene();
  std::shared_ptr<FrameSync> getFrameSync();
  std::shared_ptr<Delta> getDelta();
  std::shared_ptr<FpsMeter> getFpsMeter();
  uint64_t getFrameNumber();
  std::shared_ptr<InputManager> getInput();

  void setScene(std::shared_ptr<Scene> scene);
private:
  std::unique_ptr<GraphicsWindow_Internal> _pint = nullptr;
};

}//ns Game



#endif
