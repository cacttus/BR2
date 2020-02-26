#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/GLContext.h"
#include "../base/GraphicsWindow.h"
#include "../base/OpenGLWindow.h"
#include "../base/SDLUtils.h"
#include "../gfx/OpenGLApi.h"

namespace BR2 {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}
void OpenGLApi::cleanup() {
}

std::shared_ptr<GraphicsWindow> OpenGLApi::createWindow(std::string title) {
  //Make SDL Window and Initialize Graphics Window
  SDL_Window* win = makeSDLWindow(title, SDL_WINDOW_OPENGL);

  //Create GL Context.
  int minGLVersion;
  int minGLSubversion;
  const int c_iMax_Profs = 2;
  std::shared_ptr<GLProfile> profs[c_iMax_Profs];
  int iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
  bool bVsync = false;

#ifdef BR2_OS_IPHONE
  minGLVersion = 3;
  minGLSubversion = 0;
  iProfile = SDL_GL_CONTEXT_PROFILE_ES;
#else
#ifdef BR2_OS_WINDOWS
  minGLVersion = 3;
  minGLSubversion = 3;
  iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
#endif
#endif

  std::shared_ptr<GLContext> context = nullptr;

  profs[0] = std::make_shared<GLProfile>(32, minGLVersion, minGLSubversion, iProfile, bVsync);
  profs[1] = std::make_shared<GLProfile>(24, minGLVersion, minGLSubversion, iProfile, bVsync);

  for (int iProf = 0; iProf < c_iMax_Profs; ++iProf) {
    try {
      //This must be called before creating the window because this sets SDL's PixelFormatDescritpro
      GLContext::setWindowAndOpenGLFlags(profs[iProf]);
      SDLUtils::checkSDLErr();

      context = std::make_shared<GLContext>(getThis<GraphicsApi>(), profs[iProf], win);
      //Graphics window's creation is our error code for context creation.
      if (context->getGraphicsWindow()) {
        SDL_GL_SetSwapInterval(profs[iProf]->_bVsync ? 1 : 0);  //Vsync is automatic on IOS
        SDLUtils::checkSDLErr();

        SDLUtils::checkSDLErr();
        //Context created successfully
        this->_mainContext = context;
        _contexts.push_back(context);
        break;
      }
    }
    catch (Exception * ex) {
      BRLogError("Error during GL context creation: " + ex->what());
    }
  }

  if (context != nullptr) {
    return context->getGraphicsWindow();
  }
  return nullptr;
}
void OpenGLApi::makeCurrent(SDL_Window* win) {
  SDL_GL_MakeCurrent(win, Gu::getGraphicsContext()->getSDLGLContext());
}
void OpenGLApi::getDrawableSize(SDL_Window* win, int* w, int* h) {
  SDL_GL_GetDrawableSize(win, w, h);
}
void OpenGLApi::swapBuffers(SDL_Window* win) {
  SDL_GL_SwapWindow(win);
}




}//ns Game
