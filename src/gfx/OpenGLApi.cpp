//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_opengl_glext.h>

#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/GLContext.h"
#include "../base/GraphicsWindow.h"
#include "../base/OpenGLWindow.h"
#include "../gfx/OpenGLApi.h"

namespace BR2 {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}
void OpenGLApi::cleanup() {
}

std::shared_ptr<GraphicsWindow> OpenGLApi::createWindow(std::string title, bool isMain) {
  std::shared_ptr<OpenGLWindow> w = std::make_shared<OpenGLWindow>(isMain);
  if (isMain) {
    if (_pMainWindow != nullptr) {
      BRThrowException("Main window already set.");
    }
    _pMainWindow = w;
  }

  w->create(title);

  _pvecWindows.push_back(w);

  return w;
}
void OpenGLApi::createRenderer() {
  std::dynamic_pointer_cast<OpenGLWindow>(_pMainWindow)->createRenderer();
}
std::shared_ptr<GLContext> OpenGLApi::makeContext(std::shared_ptr<GraphicsWindow> w, GLProfile& p) {
  std::shared_ptr<GLContext> ctx = std::make_shared<GLContext>();
  if (ctx->create(w, p) == false) {
    ctx = nullptr;
    return nullptr;
  }

  _pContext = ctx;
  return ctx;
}



}//ns Game
