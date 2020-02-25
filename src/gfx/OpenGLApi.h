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

  virtual std::shared_ptr<GraphicsWindow> createWindow(std::string title, bool isMain = false) override;
  virtual void cleanup() override;
  std::shared_ptr<GLContext> getContext() { return _pContext; }
  std::shared_ptr<GLContext> makeContext(std::shared_ptr<GraphicsWindow> w, GLProfile& p);

  virtual void createRenderer();

private:
  std::shared_ptr<GLContext> _pContext = nullptr;
};



}//ns Game



#endif
