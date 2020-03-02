#include "../gfx/GraphicsApi.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"
#include "../base/DebugHelper.h"
#include "../base/Delta.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../base/GLContext.h"
#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderPipe.h"

namespace BR2 {

GraphicsApi::GraphicsApi() {
}
GraphicsApi::~GraphicsApi() {
  cleanup();
}
void GraphicsApi::updateLoop() {
#ifdef __WINDOWS__
  SDL_ShowCursor(SDL_DISABLE);
#endif
  if (_pCoreContext == nullptr) {
    BRLogError("No main window was created entering game loop.  Cannot continue");
    return;
  }

  while (true) {
    Perf::beginPerf();
    Perf::pushPerf();
    {
      if (handleSDLEvents() == true) {
        break;//SDL_QUIT
      }

      Gu::checkErrorsDbg();

      Gu::getInputManager()->preUpdate();

      Gu::checkErrorsDbg();

      Gu::updateManagers();

      for (std::shared_ptr<GraphicsContext> ct : _contexts) {
        std::shared_ptr<GraphicsWindow> w = ct->getGraphicsWindow();
        w->step();
      }

      //Update all button states.
      Gu::getInputManager()->postUpdate();
    }
    Perf::popPerf();
    Perf::endPerf();
    DebugHelper::checkMemory();

    //**End of loop error -- Don't Remove** 
    _pCoreContext->chkErrRt();
    //**End of loop error -- Don't Remove** 
  }

  DebugHelper::checkMemory();
}
bool GraphicsApi::handleSDLEvents() {
  SDL_Event event;
  bool done = false;
  while (SDL_PollEvent(&event)) {
    if (handleEvents(&event) == false) {
      done = true;
    }
  }
  return done;
}
bool GraphicsApi::handleEvents(SDL_Event* event) {
  static SDL_MouseMotionEvent lastEvent;
  int n = 0;
  vec2 delta;
  SDL_Scancode keyCode;
  std::shared_ptr<InputManager> pFingers = Gu::getInputManager();

  if (event == nullptr) {
    return true;
  }

  switch (event->type) {
  case SDL_MOUSEMOTION:
    //Don't use this.  Because of "SDL_warpMouse" which will mess stuff up.
    break;
  case SDL_KEYDOWN:
    keyCode = event->key.keysym.scancode;
    pFingers->setKeyDown(keyCode);
    break;
  case SDL_KEYUP:
    keyCode = event->key.keysym.scancode;
    pFingers->setKeyUp(keyCode);
    break;
  case SDL_MOUSEBUTTONDOWN:
    switch (event->button.button) {
    case SDL_BUTTON_LEFT:
      pFingers->setLmbState(ButtonState::Press);
      break;
    case SDL_BUTTON_MIDDLE:
      break;
    case SDL_BUTTON_RIGHT:
      pFingers->setRmbState(ButtonState::Press);
      break;
    }
    break;
  case SDL_MOUSEBUTTONUP:
    switch (event->button.button) {
    case SDL_BUTTON_LEFT:
      pFingers->setLmbState(ButtonState::Release);
      break;
    case SDL_BUTTON_MIDDLE:
      break;
    case SDL_BUTTON_RIGHT:
      pFingers->setRmbState(ButtonState::Release);
      break;
    }
    break;
  case SDL_WINDOWEVENT:
    switch (event->window.event) {
    case SDL_WINDOWEVENT_CLOSE:
      return false;
      break;
    }
    break;
  case SDL_MOUSEWHEEL:
    if (event->wheel.y != 0) {

      int n = MathUtils::brMin(10, MathUtils::brMax(-10, event->wheel.y));

      for (std::shared_ptr<GraphicsContext> ct : _contexts) {
        std::shared_ptr<GraphicsWindow> w = ct->getGraphicsWindow();
        w->mouseWheel(n);
      }

    }
    if (event->wheel.x != 0) {
      n++;
    }
    break;
  case SDL_QUIT:
    return false;
    break;
  }

  return true;
}
void GraphicsApi::destroyWindow(std::shared_ptr<GraphicsWindow> w) {
  w = nullptr;
  SDL_DestroyWindow(w->getSDLWindow());
  //std::vector<std::shared_ptr<GraphicsWindow>>::iterator it = std::find(_pvecWindows.begin(), _pvecWindows.end(), w);
  //if (it != _pvecWindows.end()) {
  //  _pvecWindows.erase(it);
  //}
}
SDL_Window* GraphicsApi::makeSDLWindow(string_t windowTitle, int render_system, bool show) {
  string_t title;
  bool bFullscreen = false;
  SDL_Window* ret = nullptr;

  int x = 0, y = 0, w = 800, h = 600, flags = 0;
#ifdef BR2_OS_IPHONE
  x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
  title = "";
#else
#ifdef BR2_OS_WINDOWS

  //SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN;

  if (bFullscreen) {
    x = 0; y = 0;
    w = 1920; h = 1080;
    flags = render_system;
  }
  else {
    x = 100, y = 100, w = 800, h = 600, flags = (show ? SDL_WINDOW_SHOWN : SDL_WINDOW_HIDDEN) | render_system | SDL_WINDOW_RESIZABLE;
  }
  title = windowTitle;
#else
  OS_NOT_SUPPORTED_ERROR
#endif
#endif

    //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
    ret = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
  SDLUtils::checkSDLErr();

  //Fullscreen nonsense
  if (bFullscreen) {
    SDL_SetWindowFullscreen(ret, SDL_WINDOW_FULLSCREEN);
  }
  SDLUtils::checkSDLErr();

  return ret;
}



}//ns Game
