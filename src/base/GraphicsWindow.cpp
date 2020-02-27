#include "../base/OperatingSystemHeader.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../base/AppBase.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"
#include "../base/Delta.h"
#include "../base/FrameSync.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../gfx/UiControls.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderPipe.h"
#include "../base/SDLIncludes.h"
#include "../world/Scene.h"

namespace BR2 {
#pragma region GraphicsWindow_Internal
class GraphicsWindow_Internal {
public:
  std::unique_ptr<GraphicsWindow> _cont = nullptr;
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr;
  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  std::shared_ptr<GraphicsApi> _pApi = nullptr;

  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  void printHelpfulDebug();
  void beginRender();
  void endRender();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void toggleFullscreen();

};
void GraphicsWindow_Internal::toggleFullscreen() {
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


void GraphicsWindow_Internal::updateWidthHeight(uint32_t w, uint32_t h, bool bForce) {
  //update view/cam
  if (_iLastWidth != w || bForce) {
    _pViewport->setWidth(w);
    if (_iLastHeight != h || bForce) {
      _pViewport->setHeight(h);
    }
    if (_iLastHeight != h || _iLastWidth != w || bForce) {
      if (_pRenderPipe != nullptr) {
        _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
      }
      //   _pApp->screenChanged(w, h, _bFullscreen);

      //TODO: one Gui per window.
      if (_pScene != nullptr) {
        _pScene->updateWidthHeight(w, h, bForce);
      }
    }
    _iLastWidth = w;
    _iLastHeight = h;
  }
}
void GraphicsWindow_Internal::printHelpfulDebug() {
  int dw, dh;
  SDL_DisplayMode mode;
  SDL_Window* win = _pSDLWindow;

  SDL_GetCurrentDisplayMode(0, &mode);
  BRLogInfo("Screen BPP    : " + SDL_BITSPERPIXEL(mode.format));
  BRLogInfo("Swap Interval : " + SDL_GL_GetSwapInterval());
  SDL_GetWindowSize(win, &dw, &dh);
  BRLogInfo("Initial Window Size   : " + dw + "x" + dh);
  SDL_GL_GetDrawableSize(win, &dw, &dh);
  BRLogInfo("Draw Size     : " + dw + "x" + dh);

  SDLUtils::checkSDLErr();
}

void GraphicsWindow_Internal::beginRender() {

  //Make this window current *critical*
  //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
  _pApi->makeCurrent(_pSDLWindow);

  //Update the widnow size
  int w, h;
  _pApi->getDrawableSize(_pSDLWindow, &w, &h);
  updateWidthHeight(w, h, false);

  Perf::pushPerf();//**BEGIN PERF 
}
void GraphicsWindow_Internal::endRender() {
  _pApi->swapBuffers(_pSDLWindow);

  Perf::popPerf();//**END PERF
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////

#pragma region GraphicsWindow
//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLContext> ct, SDL_Window* win) : RenderTarget(ct) {
  _pint = std::make_unique<GraphicsWindow_Internal>();
  _pint->_pSDLWindow = win;
  _pint->_pApi = api;
  _pint->_iLastWidth = Gu::getConfig()->getDefaultScreenWidth();
  _pint->_iLastHeight = Gu::getConfig()->getDefaultScreenHeight();
  _pint->_pViewport = std::make_shared<RenderViewport>(_pint->_iLastWidth, _pint->_iLastHeight);
}
GraphicsWindow::~GraphicsWindow() {
  if (_pint->_pSDLWindow != nullptr) {
    _pint->_pSDLWindow = nullptr;
  }
  _pint = nullptr;
}
int32_t GraphicsWindow::getWidth() { return _pint->_iLastWidth; }
int32_t GraphicsWindow::getHeight() { return _pint->_iLastHeight; }
SDL_Window* GraphicsWindow::getSDLWindow() { return _pint->_pSDLWindow; }
std::shared_ptr<RenderViewport> GraphicsWindow::getViewport() { return _pint->_pViewport; }
std::shared_ptr<RenderPipe> GraphicsWindow::getRenderPipe() { return _pint->_pRenderPipe; }
std::shared_ptr<Scene> GraphicsWindow::getScene() { return _pint->_pScene; }
void GraphicsWindow::setScene(std::shared_ptr<Scene> scene) { 
  scene->setWindow(getThis<GraphicsWindow>());
  _pint->_pScene = scene;
  scene->afterAttachedToWindow();
}
void GraphicsWindow::step() {
  _pint->beginRender();
  {
    if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F11)) {
      _pint->toggleFullscreen();
    }

    Gu::getCoreContext()->setLoopState(EngineLoopState::SyncBegin);
    Gu::getFrameSync()->syncBegin();
    {
      Gu::getCoreContext()->setLoopState(EngineLoopState::Update);

      //...?

      //App Update steps.
     // Gu::getApp()->step((float)_fDelta);

      Gu::getCoreContext()->setLoopState(EngineLoopState::Render);
      
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      //TEST: HACK: TODO:
      if (_pint->_pScene == nullptr) {
        setScene(Scene::create());
      }

      if (getScene() != nullptr) {
        //Main Render
        PipeBits pbs;
        pbs.set();
        _pint->_pRenderPipe->renderScene(getScene(), getScene()->getActiveCamera(), getScene()->getLightManager(), pbs);
      }
      else {
        BRLogErrorCycle("Scene was not set on graphics window " + getTitle());
      }
    }
    Gu::getCoreContext()->setLoopState(EngineLoopState::SyncEnd);
    Gu::getFrameSync()->syncEnd();
  }

  _pint->endRender();
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
  _pint->updateWidthHeight(_pint->_iLastWidth, _pint->_iLastHeight, true);

  if (Gu::getConfig()->getStartFullscreen() == true) {
    BRLogInfo("Setting window fullscreen.");
    _pint->toggleFullscreen();
  }

  _pint->_pRenderPipe = std::make_shared<RenderPipe>(getContext(), getThis<GraphicsWindow>());
  _pint->_pRenderPipe->init(getViewport()->getWidth(), getViewport()->getHeight(), Gu::getApp()->makeAssetPath(Gu::getApp()->getEnvTexturePath()));

  _pint->printHelpfulDebug();
}
void GraphicsWindow::mouseWheel(int amount) {
  if (_pint->_pScene != nullptr) {
    _pint->_pScene->mouseWheel(amount);
  }
}
string_t GraphicsWindow::getTitle() {
  string_t st(SDL_GetWindowTitle(_pint->_pSDLWindow));
  return st;
}


#pragma endregion
}//ns Game
