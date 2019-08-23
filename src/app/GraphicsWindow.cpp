#include "../base/Base.h"
#include "../app/AppBase.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../app/GraphicsWindow.h"
#include "../base/Delta.h"
#include "../base/FrameSync.h"
#include "../base/Perf.h"
#include "../base/Fingers.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/Gui2d.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderPipe.h"

namespace Game {

//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(bool ismain) {
    this->_bIsMainWindow = ismain;
}
void GraphicsWindow::makeSDLWindow(t_string windowTitle, int render_system) {
    t_string title;
    bool bFullscreen = false;

    int x, y, w, h, flags;
#ifdef BRO_OS_IPHONE
    x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
    title = "";
#elif BRO_OS_WINDOWS

    //int render_system = SDL_WINDOW_OPENGL;
    //if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::e::Vulkan) {
    //    render_system = SDL_WINDOW_VULKAN;
    //}
    if (bFullscreen) {
        x = 0; y = 0;
        w = 1920; h = 1080;
        flags = render_system;
}
    else {
        x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | render_system | SDL_WINDOW_RESIZABLE;
    }
    title = windowTitle;
#endif

    //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
    _pSDLWindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    SDLUtils::checkSDLErr();

    //Fullscreen nonsense
    if (bFullscreen) {
        SDL_SetWindowFullscreen(_pSDLWindow, SDL_WINDOW_FULLSCREEN);
    }
    SDLUtils::checkSDLErr();

#ifdef BRO_OS_WINDOWS
    BroLogError("We are not making the window icon because there's an error somewhere in SDL here.");
    //**There is an error here
  //  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pApp->getIconFullPath());
#endif
}
void GraphicsWindow::initRenderSystem() {
    if (_pSDLWindow == nullptr) {
        BroThrowException("You need to make the SDL window before initializing render system.");
    }

    SDLUtils::trySetWindowIcon(_pSDLWindow, Gu::getApp()->getIconFullPath());

    //Note we may need to adjust for actual start width/height if start width is too large , etc.
    //*Set room width / height
    _iLastWidth = Gu::getConfig()->getDefaultScreenWidth();
    _iLastHeight = Gu::getConfig()->getDefaultScreenHeight();

    _pWindowViewport = std::make_shared<WindowViewport>(_iLastWidth, _iLastHeight);

    if (Gu::getConfig()->getForceAspectRatio()) {
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
            BroLogError("SDL_GetDesktopDisplayMode failed: " + SDL_GetError());
        }
        else {
            float ar = (float)dm.h / (float)dm.w;
            int newHeight = (int)((float)_iLastWidth * ar);
            if (newHeight != _iLastHeight) {
                BroLogInfo("Adjusting window dims from " + _iLastWidth + " x " + _iLastHeight + " to " + _iLastWidth + " x " + newHeight);
                _iLastHeight = newHeight;
            }
        }
    }

    BroLogInfo("Setting window size to, " + _iLastWidth + " x " + _iLastHeight);
    SDL_SetWindowSize(_pSDLWindow, _iLastWidth, _iLastHeight);
    updateWidthHeight(_iLastWidth, _iLastHeight, true);

    if (Gu::getConfig()->getStartFullscreen() == true) {
        BroLogInfo("Setting window fullscreen.");
        toggleFullscreen();
    }

    createRenderPipe();

    _pGui = std::make_shared<Gui2d>();
    _pGui->init();
}
GraphicsWindow::~GraphicsWindow() {
    if (_pSDLWindow != nullptr) {
        SDL_DestroyWindow(_pSDLWindow);
        _pSDLWindow = nullptr;
    }
}
void GraphicsWindow::updateWidthHeight(uint32_t w, uint32_t h, bool bForce) {
    //update view/cam
    if (_iLastWidth != w || bForce) {
        _pWindowViewport->setWidth(w);
        if (_iLastHeight != h || bForce) {
            _pWindowViewport->setHeight(h);
        }
        if (_iLastHeight != h || _iLastWidth != w || bForce) {
            if (_pRenderPipe != nullptr) {
                _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
            }
            //   _pApp->screenChanged(w, h, _bFullscreen);

            //TODO: one Gui per window.
            if (_pGui != nullptr) {
                _pGui->screenChanged(w, h, _bFullscreen);
            }
        }
        _iLastWidth = w;
        _iLastHeight = h;
    }
}
void GraphicsWindow::toggleFullscreen() {
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
            BroLogError("Failed to go fullscreen.");
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
            BroLogError("Failed to exit fullscreen.");
        }
        else {
            _bFullscreen = false;
           //_pApp->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
        }
    }

}

void GraphicsWindow::printHelpfulDebug() {
    int dw, dh;
    SDL_DisplayMode mode;
    SDL_Window* win = _pSDLWindow;

    SDL_GetCurrentDisplayMode(0, &mode);
    BroLogInfo("Screen BPP    : " + SDL_BITSPERPIXEL(mode.format));
    BroLogInfo("Swap Interval : " + SDL_GL_GetSwapInterval());
    SDL_GetWindowSize(win, &dw, &dh);
    BroLogInfo("Initial Window Size   : " + dw + "x" + dh);
    SDL_GL_GetDrawableSize(win, &dw, &dh);
    BroLogInfo("Draw Size     : " + dw + "x" + dh);

    int tmp = 0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &tmp);
    BroLogInfo("SDL_GL_DOUBLEBUFFER: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &tmp);
    BroLogInfo("SDL_GL_BUFFER_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);

    BroLogInfo("SDL_GL_DEPTH_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &tmp);
    BroLogInfo("SDL_GL_STENCIL_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &tmp);
    BroLogInfo("SDL_GL_ACCELERATED_VISUAL: " + tmp);

    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &tmp);
    BroLogInfo("SDL_GL_RED_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &tmp);
    BroLogInfo("SDL_GL_GREEN_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &tmp);
    BroLogInfo("SDL_GL_BLUE_SIZE: " + tmp);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &tmp);
    BroLogInfo("SDL_GL_ALPHA_SIZE: " + tmp);

    SDLUtils::checkSDLErr();
}
void GraphicsWindow::createRenderPipe() {
    //Deferred Renderer
    _pRenderPipe = std::make_shared<RenderPipe>(getThis<GraphicsWindow>());
    _pRenderPipe->init(Gu::getViewport()->getWidth(), Gu::getViewport()->getHeight(), Gu::getApp()->makeAssetPath(Gu::getApp()->getEnvTexturePath()));
    // Gu::setRenderPipe(_pRenderPipe);

    _pRenderPipe->getPipeBits().set();
}
void GraphicsWindow::beginRender() {
    
    //Make this window current *critical*
    //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
    Gu::getGraphicsApi()->makeCurrent();

    //Update the widnow size
    int w, h;
    Gu::getGraphicsApi()->getDrawableSize(&w, &h);
    Gu::getMainWindow()->updateWidthHeight(w, h, false);

    Perf::pushPerf();
}
void GraphicsWindow::endRender() {
    Gu::getGraphicsApi()->swapBuffers();

    Perf::popPerf();
}
void GraphicsWindow::step() {
    beginRender();
    {
        if (Gu::getFingers()->keyPress(SDL_SCANCODE_F11)) {
            toggleFullscreen();
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
            _pRenderPipe->renderScene(Gu::getApp());
        }
        Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncEnd);
        Gu::getFrameSync()->syncEnd();
    }

    endRender();
}


}//ns Game
