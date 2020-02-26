/**
*  @file OpenGLApi.h
*  @date August 5, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __OPENGLGraphicsApi_15650573213471462786_H__
#define __OPENGLGraphicsApi_15650573213471462786_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/GraphicsApi.h"

namespace BR2 {
/**
*  @class OpenGLApi
*  @brief Handles the OpenGL API.
*/
class OpenGLApi : public GraphicsApi {
public:
  OpenGLApi();
  virtual ~OpenGLApi() override;

  virtual std::shared_ptr<GraphicsWindow> createWindow(std::string title) override;
  virtual void cleanup() override;
  std::shared_ptr<GLContext> getContext() { return _mainContext; }

  virtual void makeCurrent(SDL_Window* win) override;
  virtual void getDrawableSize(SDL_Window* win, int* w, int* h) override;
  virtual void swapBuffers(SDL_Window* win)  override;

private:
  std::shared_ptr<GLContext> _mainContext = nullptr;
};



}//ns Game



#endif
