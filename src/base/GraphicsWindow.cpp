#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../gfx/UiControls.h"
#include "../base/AppBase.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"
#include "../base/Delta.h"
#include "../base/FrameSync.h"
#include "../base/Perf.h"
#include "../base/Fingers.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderPipe.h"
#include "../base/OperatingSystemHeader.h"

namespace BR2 {
class GraphicsWindow_Internal {
public:
  std::unique_ptr<GraphicsWindow> _cont = nullptr;

  std::shared_ptr<WindowViewport> _pWindowViewport = nullptr;
  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  bool _bIsMainWindow = false;

  std::shared_ptr<Gui2d> _pGui = nullptr;

  //GraphicsWindow_Internal(std::unique_ptr<GraphicsWindow> cont) {
  //  _cont = cont;
  //}

  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  void toggleFullscreen() {
    if (_bFullscreen == false) {
      //get the fullscreen resolution
      int32_t iFsW = Gu::getEngineConfig()->getFullscreenWidth();
      int32_t iFsH = Gu::getEngineConfig()->getFullscreenHeight();
      if (iFsW <= 0 || iFsH <= 0) {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        iFsW = DM.w;
        iFsH = DM.h;
      }
      //Save pre-fullscreen width/height
      _iFullscreenToggleWidth = _iLastWidth;
      _iFullscreenToggleHeight = _iLastHeight;
      SDL_SetWindowSize(_pSDLWindow, iFsW, iFsH);

      if (SDL_SetWindowFullscreen(_pSDLWindow, SDL_WINDOW_FULLSCREEN) != 0) {
        BRLogError("Failed to go fullscreen.");
      }
      else {
        _bFullscreen = true;
        //_pApp->screenChanged(iFsW, iFsH, _bFullscreen);
      }
    }
    else {
      if (_iFullscreenToggleWidth > 0 && _iFullscreenToggleHeight > 0) {
        //Restore pre-fullscreen width/height
        SDL_SetWindowSize(_pSDLWindow, _iFullscreenToggleWidth, _iFullscreenToggleHeight);
      }
      if (SDL_SetWindowFullscreen(_pSDLWindow, 0) != 0) {
        BRLogError("Failed to exit fullscreen.");
      }
      else {
        _bFullscreen = false;
        //_pApp->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
      }
    }

  }


};
//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(bool ismain) {
  _pint = std::make_unique<GraphicsWindow_Internal>();
  _pint->_bIsMainWindow = ismain;
}
GraphicsWindow::~GraphicsWindow() {
  if (_pint->_pSDLWindow != nullptr) {
    SDL_DestroyWindow(_pint->_pSDLWindow);
    _pint->_pSDLWindow = nullptr;
  }
  _pint = nullptr;
}

void* GraphicsWindow::getSDLWindow() { return _pint->_pSDLWindow; }
std::shared_ptr<WindowViewport> GraphicsWindow::getWindowViewport() { return _pint->_pWindowViewport; }
std::shared_ptr<RenderPipe> GraphicsWindow::getRenderPipe() { return _pint->_pRenderPipe; }
std::shared_ptr<Gui2d> GraphicsWindow::getGui() { return _pint->_pGui; }

void GraphicsWindow::makeSDLWindow(string_t windowTitle, int render_system) {
  string_t title;
  bool bFullscreen = false;

  int x=0, y=0, w=800, h=600, flags=0;
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
    x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | render_system | SDL_WINDOW_RESIZABLE;
  }
  title = windowTitle;
#else
  OS_NOT_SUPPORTED_ERROR
#endif
#endif

  //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
  _pint->_pSDLWindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
  SDLUtils::checkSDLErr();

  //Fullscreen nonsense
  if (bFullscreen) {
    SDL_SetWindowFullscreen(_pint->_pSDLWindow, SDL_WINDOW_FULLSCREEN);
  }
  SDLUtils::checkSDLErr();

  //Note we may need to adjust for actual start width/height if start width is too large , etc.
  //*Set room width / height
  _pint->_iLastWidth = Gu::getConfig()->getDefaultScreenWidth();
  _pint->_iLastHeight = Gu::getConfig()->getDefaultScreenHeight();
  _pint->_pWindowViewport = std::make_shared<WindowViewport>(_pint->_iLastWidth, _pint->_iLastHeight);

#ifdef BR2_OS_WINDOWS
  BRLogError("We are not making the window icon because there's an error somewhere in SDL here.");
  //**There is an error here
//  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pApp->getIconFullPath());
#endif
}
void GraphicsWindow::initRenderSystem() {
  if (_pint->_pSDLWindow == nullptr) {
    BRThrowException("You need to make the SDL window before initializing render system.");
  }

  SDLUtils::trySetWindowIcon(_pint->_pSDLWindow, Gu::getApp()->getIconFullPath());

  if (Gu::getConfig()->getForceAspectRatio()) {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
      BRLogError("SDL_GetDesktopDisplayMode failed: " + SDL_GetError());
    }
    else {
      float ar = (float)dm.h / (float)dm.w;
      int newHeight = (int)((float)_pint->_iLastWidth * ar);
      if (newHeight != _pint->_iLastHeight) {
        BRLogInfo("Adjusting window dims from " + _pint->_iLastWidth + " x " + _pint->_iLastHeight + " to " + _pint->_iLastWidth + " x " + newHeight);
        _pint->_iLastHeight = newHeight;
      }
    }
  }

  BRLogInfo("Setting window size to, " + _pint->_iLastWidth + " x " + _pint->_iLastHeight);
  SDL_SetWindowSize(_pint->_pSDLWindow, _pint->_iLastWidth, _pint->_iLastHeight);
  updateWidthHeight(_pint->_iLastWidth, _pint->_iLastHeight, true);

  if (Gu::getConfig()->getStartFullscreen() == true) {
    BRLogInfo("Setting window fullscreen.");
    _pint->toggleFullscreen();
  }

  createRenderPipe();

  _pint->_pGui = std::make_shared<Gui2d>();
  _pint->_pGui->init();
}

void GraphicsWindow::updateWidthHeight(uint32_t w, uint32_t h, bool bForce) {
  //update view/cam
  if (_pint->_iLastWidth != w || bForce) {
    _pint->_pWindowViewport->setWidth(w);
    if (_pint->_iLastHeight != h || bForce) {
      _pint->_pWindowViewport->setHeight(h);
    }
    if (_pint->_iLastHeight != h || _pint->_iLastWidth != w || bForce) {
      if (_pint->_pRenderPipe != nullptr) {
        _pint->_pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
      }
      //   _pApp->screenChanged(w, h, _bFullscreen);

      //TODO: one Gui per window.
      if (_pint->_pGui != nullptr) {
        _pint->_pGui->screenChanged(w, h, _pint->_bFullscreen);
      }
    }
    _pint->_iLastWidth = w;
    _pint->_iLastHeight = h;
  }
}

void GraphicsWindow::printHelpfulDebug() {
  int dw, dh;
  SDL_DisplayMode mode;
  SDL_Window* win = _pint->_pSDLWindow;

  SDL_GetCurrentDisplayMode(0, &mode);
  BRLogInfo("Screen BPP    : " + SDL_BITSPERPIXEL(mode.format));
  BRLogInfo("Swap Interval : " + SDL_GL_GetSwapInterval());
  SDL_GetWindowSize(win, &dw, &dh);
  BRLogInfo("Initial Window Size   : " + dw + "x" + dh);
  SDL_GL_GetDrawableSize(win, &dw, &dh);
  BRLogInfo("Draw Size     : " + dw + "x" + dh);


  SDLUtils::checkSDLErr();
}

void GraphicsWindow::step() {
  beginRender();
  {
    if (Gu::getFingers()->keyPress(SDL_SCANCODE_F11)) {
      _pint->toggleFullscreen();
    }

    Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncBegin);
    Gu::getFrameSync()->syncBegin();
    {
      Gu::getGraphicsContext()->setLoopState(EngineLoopState::Update);

      //...?

      //App Update steps.
     // Gu::getApp()->step((float)_fDelta);

      Gu::getGraphicsContext()->setLoopState(EngineLoopState::Render);

      //Main Render
      _pint->_pRenderPipe->renderScene(Gu::getApp());
    }
    Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncEnd);
    Gu::getFrameSync()->syncEnd();
  }

  endRender();
}

void GraphicsWindow::createRenderPipe() {
  //Deferred Renderer
  _pint->_pRenderPipe = std::make_shared<RenderPipe>(getThis<GraphicsWindow>());
  _pint->_pRenderPipe->init(Gu::getViewport()->getWidth(), Gu::getViewport()->getHeight(), Gu::getApp()->makeAssetPath(Gu::getApp()->getEnvTexturePath()));
  // Gu::setRenderPipe(_pRenderPipe);

  _pint->_pRenderPipe->getPipeBits().set();
}
void GraphicsWindow::beginRender() {

  //Make this window current *critical*
  //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
  makeCurrent();

  //Update the widnow size
  int w, h;
  getDrawableSize(&w, &h);
  updateWidthHeight(w, h, false);

  Perf::pushPerf();
}
void GraphicsWindow::endRender() {
  swapBuffers();

  Perf::popPerf();
}


}//ns Game
