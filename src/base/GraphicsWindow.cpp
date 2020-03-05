#include "../base/OperatingSystemHeader.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../base/ApplicationPackage.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"
#include "../base/Delta.h"
#include "../base/FpsMeter.h"
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
#include "../bottle/BottleScript.h"


namespace BR2 {
#pragma region GraphicsWindow_Internal
class GraphicsWindow_Internal {
public:
  std::unique_ptr<GraphicsWindow> _cont = nullptr;
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr;
  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  std::shared_ptr<GraphicsApi> _pApi = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<InputManager> _pInput = nullptr;

  FrameState _eFrameState = FrameState::None;

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
  void setFrameState(FrameState e) { _eFrameState = e; }
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

  Gu::checkErrorsDbg();
  //Make this window current *critical*
  //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
  _pApi->makeCurrent(_pSDLWindow);
  Gu::checkErrorsDbg();

  //Update the widnow size
  int w, h;
  _pApi->getDrawableSize(_pSDLWindow, &w, &h);
  Gu::checkErrorsDbg();
  updateWidthHeight(w, h, false);
  Gu::checkErrorsDbg();

  Perf::pushPerf();//**BEGIN PERF 
}
void GraphicsWindow_Internal::endRender() {
  _pApi->swapBuffers(_pSDLWindow);

  Perf::popPerf();//**END PERF
}
#pragma endregion


#pragma region GraphicsWindow
//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLContext> ct, SDL_Window* win) : RenderTarget(ct) {
  _pint = std::make_unique<GraphicsWindow_Internal>();
  _pint->_pSDLWindow = win;
  _pint->_pApi = api;
}
GraphicsWindow::~GraphicsWindow() {
  if (_pint->_pSDLWindow != nullptr) {
    _pint->_pSDLWindow = nullptr;
  }
  _pint = nullptr;
}
void GraphicsWindow::init() {
  _pint->_iLastWidth = Gu::getConfig()->getDefaultScreenWidth();
  _pint->_iLastHeight = Gu::getConfig()->getDefaultScreenHeight();
  _pint->_pViewport = std::make_shared<RenderViewport>(_pint->_iLastWidth, _pint->_iLastHeight);
  _pint->_pFrameSync = std::make_shared<FrameSync>(getThis<GraphicsWindow>());
  _pint->_pDelta = std::make_shared<Delta>();
  _pint->_pFpsMeter = std::make_shared<FpsMeter>();
  _pint->_pInput = std::make_shared<InputManager>();
}
void GraphicsWindow::initRenderSystem() {
  if (_pint->_pSDLWindow == nullptr) {
    BRThrowException("You need to make the SDL window before initializing render system.");
  }

  SDLUtils::trySetWindowIcon(_pint->_pSDLWindow, Gu::getPackage()->getIconPath());

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
  _pint->_pRenderPipe->init(getViewport()->getWidth(), getViewport()->getHeight(), Gu::getPackage()->makeAssetPath(Gu::getPackage()->getEnvTextureFolder()));

  _pint->printHelpfulDebug();
}
void GraphicsWindow::step() {
  _pint->_pDelta->update();
  _pint->_pFpsMeter->update();

  if (_pint->_pScene == nullptr) {
    std::shared_ptr<Scene> pscene = Scene::create();
    pscene->addComponent(std::make_shared<BottleScript>());
    setScene(pscene);
  }

  _pint->beginRender();
  {
    if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F11)) {
      _pint->toggleFullscreen();
    }

    _pint->setFrameState(FrameState::SyncBegin);
    _pint->_pFrameSync->syncBegin();
    {
      if (getScene() != nullptr) {
        _pint->setFrameState(FrameState::Update);
        _pint->_pScene->update(_pint->_pDelta->get());

        _pint->setFrameState(FrameState::Render);
        PipeBits p;
        p.set();
        _pint->_pRenderPipe->renderScene(getScene(), getScene()->getActiveCamera(), getScene()->getLightManager(), p);
      }
      else {
        BRLogErrorCycle("Scene was not set on graphics window " + getTitle());
      }
    }
    _pint->setFrameState(FrameState::SyncEnd);
    _pint->_pFrameSync->syncEnd();
  }

  _pint->endRender();
}
void GraphicsWindow::idle(int64_t us) {
  if (_pint->_pScene != nullptr) {
    _pint->_pScene->idle(us);
  }
}
string_t GraphicsWindow::getTitle() {
  string_t st(SDL_GetWindowTitle(_pint->_pSDLWindow));
  return st;
}
std::shared_ptr<GraphicsWindow> GraphicsWindow::create(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLContext> ct, SDL_Window* win) {
  std::shared_ptr<GraphicsWindow> w = std::make_shared<GraphicsWindow>(api, ct, win);
  w->init();
  return w;
}
uint64_t GraphicsWindow::getFrameNumber() { return _pint->_pFpsMeter->getFrameNumber(); }
int32_t GraphicsWindow::getWidth() { return _pint->_iLastWidth; }
int32_t GraphicsWindow::getHeight() { return _pint->_iLastHeight; }
SDL_Window* GraphicsWindow::getSDLWindow() { return _pint->_pSDLWindow; }
std::shared_ptr<RenderViewport> GraphicsWindow::getViewport() { return _pint->_pViewport; }
std::shared_ptr<RenderPipe> GraphicsWindow::getRenderPipe() { return _pint->_pRenderPipe; }
std::shared_ptr<Scene> GraphicsWindow::getScene() { return _pint->_pScene; }
std::shared_ptr<FrameSync> GraphicsWindow::getFrameSync() { return _pint->_pFrameSync; }
std::shared_ptr<Delta> GraphicsWindow::getDelta() { return _pint->_pDelta; }
std::shared_ptr<FpsMeter> GraphicsWindow::getFpsMeter() { return _pint->_pFpsMeter; }
std::shared_ptr<InputManager> GraphicsWindow::getInput() { return _pint->_pInput; }

void GraphicsWindow::setScene(std::shared_ptr<Scene> scene) {
  scene->setWindow(getThis<GraphicsWindow>());
  _pint->_pScene = scene;
  scene->afterAttachedToWindow();
}



#pragma endregion
}//ns Game
