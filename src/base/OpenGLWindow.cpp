#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../gfx/GraphicsContext.h"
#include "../base/OpenGLWindow.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/OpenGLApi.h"

namespace BR2 {
OpenGLWindow::OpenGLWindow(bool is_main) : GraphicsWindow(is_main) {
}
OpenGLWindow::~OpenGLWindow() {
}
void OpenGLWindow::create(string_t windowTitle) {

  int minGLVersion;
  int minGLSubversion;
  const int c_iMax_Profs = 2;
  GLProfile profs[c_iMax_Profs];
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

  // if (getMainWindow() == nullptr) {
  profs[0].make(32, minGLVersion, minGLSubversion, iProfile, bVsync);
  profs[1].make(24, minGLVersion, minGLSubversion, iProfile, bVsync);

  for (int iProf = 0; iProf < c_iMax_Profs; ++iProf) {
    //This must be called before creating the window because this sets SDL's PixelFormatDescritpro
    GLContext::setWindowAndOpenGLFlags(profs[iProf]);

    makeSDLWindow(windowTitle, SDL_WINDOW_OPENGL);

    //Unfortunately because SDL needs a window, we need to wait to create the context.
    if (Gu::getGraphicsContext() == nullptr) {
      std::shared_ptr<GLContext> ct = std::dynamic_pointer_cast<OpenGLApi>(Gu::getGraphicsApi())->makeContext(getThis<GraphicsWindow>(), profs[iProf]);
      if (ct != nullptr) {
        //Context created successfully
        setContext(ct);
        break;
      }
    }
  }


}
void OpenGLWindow::createRenderer() {
  //We moved this here instead of in the init because we need to init all managers after creating the window.

  //***************INIT ORDER**************
  //Setup Ogl
  //create window
  //init opengl context & procs
  //init managers
  //init render system

  //Create the rendering system
  initRenderSystem();
}


void OpenGLWindow::makeCurrent() {
  SDL_GL_MakeCurrent((SDL_Window*)getSDLWindow(), Gu::getGraphicsContext()->getSDLGLContext());
}
void OpenGLWindow::getDrawableSize(int* w, int* h) {
  SDL_GL_GetDrawableSize((SDL_Window*)getSDLWindow(), w, h);
}
void OpenGLWindow::swapBuffers() {
  SDL_GL_SwapWindow((SDL_Window*)getSDLWindow());
}









}//ns Game
