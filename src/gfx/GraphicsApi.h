/**
*  @file GraphicsApi.h
*  @date August 5, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __GraphicsApi_1565057965514614365_H__
#define __GraphicsApi_1565057965514614365_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class GraphicsApi
*  @brief Base class for rendering system.
*/
class GraphicsApi : public VirtualMemoryShared<GraphicsApi> {
public:
  GraphicsApi();
  virtual ~GraphicsApi() override;

  void updateLoop();

  virtual std::shared_ptr<GraphicsWindow> createWindow(string_t title) = 0;
  void destroyWindow(std::shared_ptr<GraphicsWindow> w);
  virtual void makeCurrent(SDL_Window* win) = 0;
  virtual void getDrawableSize(SDL_Window* win, int* w, int* h) = 0;
  virtual void swapBuffers(SDL_Window* win) = 0;
  virtual void cleanup() {}

  std::shared_ptr<GraphicsContext> getCoreContext() { return _pCoreContext; }

protected:
  SDL_Window* makeSDLWindow(string_t windowTitle, int render_system, bool show);
  void setMainContext(std::shared_ptr<GraphicsContext> ct) { _pCoreContext = ct; }
  std::vector<std::shared_ptr<GraphicsContext>>& getContexts() { return _contexts; }
private:
  std::vector<std::shared_ptr<GraphicsContext>> _contexts;
  std::shared_ptr<GraphicsContext> _pCoreContext = nullptr;
  bool handleSDLEvents();
  bool handleEvents(SDL_Event* event);

  std::shared_ptr<InputManager> getInputForWindow(uint32_t sdl_windowId);

};

}//ns Game



#endif
