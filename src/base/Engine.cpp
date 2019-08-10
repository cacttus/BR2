#include "../base/BaseAll.h"
#include "../base/Engine.h"
#include "../base/AppBase.h"
#include "../base/FrameSync.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../base/Stopwatch.h"
#include "../base/Net.h"

#include "../math/MathAll.h"

#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Atlas.h"
#include "../gfx/HappySky.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/CameraNode.h"
#include "../gfx/Viewport.h"
#include "../gfx/Picker.h"
#include "../gfx/Gui2d.h"
#include "../gfx/GraphicsApi.h"

#include "../model/MeshNode.h"
#include "../model/MeshUtils.h"
#include "../model/MeshSpec.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"

//#include "SDL_shaders_gles2.h"

namespace Game {
///////////////////////////////////////////////////////////////////
Engine::Engine(std::shared_ptr<AppBase> rb, std::shared_ptr<GraphicsApi> re, int xw, int xh) : _pApp(rb)
{
    AssertOrThrow2(rb != nullptr);
    _pGraphicsApi = re;
    _pViewport = std::make_shared<Viewport>(xw, xh);
}
Engine::~Engine()
{
   // DEL_MEM(_pViewport);
}
///////////////////////////////////////////////////////////////////

void Engine::init() {
    _pipeBits.set();

    _pRenderPipe = std::make_shared<RenderPipe>();
    _pRenderPipe->init(_pViewport->getWidth(), _pViewport->getHeight(), _pApp->makeAssetPath(_pApp->getEnvTexturePath()));
    Gu::setRenderPipe(_pRenderPipe);

    //Gui
    Gu::getGui()->init();

    //Net
    _pNet = std::make_shared<Net>();

    //Make Room
    _pApp->setup(_pViewport);

    //*Set room width / height
    _iLastWidth = _pApp->getStartWidth();
    _iLastHeight = _pApp->getStartHeight();

    Gu::SDLTrySetWindowIcon(_pGraphicsApi->getWindow(), _pApp->getIconFullPath());

    if(_pApp->getForceAspectRatio()){
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
            BroLogError("SDL_GetDesktopDisplayMode failed: " + SDL_GetError());
        }
        else {
            float ar = (float)dm.h / (float)dm.w;
            int newHeight = (int)((float)_iLastWidth * ar);
            if(newHeight != _iLastHeight){
                BroLogInfo("Adjusting window dims from " + _iLastWidth + " x " + _iLastHeight + " to " + _iLastWidth + " x " + newHeight);
                _iLastHeight = newHeight;
            }
        }
    }

    BroLogInfo("Setting window size to, " + _iLastWidth + " x " + _iLastHeight);
    SDL_SetWindowSize(_pGraphicsApi->getWindow(), _iLastWidth, _iLastHeight);
    updateWidthHeight(_iLastWidth, _iLastHeight, true);

    if (_pApp->getStartFullscreen() == true) {
        BroLogInfo("Setting window fullscreen.");
        toggleFullscreen();
    }

}
void Engine::updateTouch(std::shared_ptr<Fingers> pFingers) {
    if (pFingers->keyPress(SDL_SCANCODE_F11)) {
       toggleFullscreen();
    }

    _pApp->updateTouch(pFingers, getFrameDelta());
}
void Engine::toggleFullscreen() {
    if (_bFullscreen == false) {

        //get the fullscreen resolution
        int32_t iFsW = Gu::getEngineConfig()->getFullscreenWidth();
        int32_t iFsH = Gu::getEngineConfig()->getFullscreenHeight();
        if(iFsW <=0 || iFsH <=0){
            SDL_DisplayMode DM;
            SDL_GetCurrentDisplayMode(0, &DM);
            iFsW = DM.w;
            iFsH = DM.h;
        }
        //Save pre-fullscreen width/height
        _iFullscreenToggleWidth = _iLastWidth;
        _iFullscreenToggleHeight = _iLastHeight;
        SDL_SetWindowSize(_pGraphicsApi->getWindow(), iFsW, iFsH);

        if (SDL_SetWindowFullscreen(_pGraphicsApi->getWindow(), SDL_WINDOW_FULLSCREEN) != 0) {
            BroLogError("Failed to go fullscreen.");
        }
        else {
            _bFullscreen = true;
            _pApp->screenChanged(iFsW, iFsH, _bFullscreen);
        }
    }
    else {
        if(_iFullscreenToggleWidth >0 && _iFullscreenToggleHeight > 0){
            //Restore pre-fullscreen width/height
            SDL_SetWindowSize(_pGraphicsApi->getWindow(), _iFullscreenToggleWidth, _iFullscreenToggleHeight);
        }
        if (SDL_SetWindowFullscreen(_pGraphicsApi->getWindow(), 0) != 0) {
            BroLogError("Failed to exit fullscreen.");
        }
        else {
            _bFullscreen = false;
            _pApp->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
        }
    }

}
void Engine::userZoom(int iAmount){
    _pApp->userZoom(iAmount);
}
void Engine::updateDelta() { 
    NOW = SDL_GetPerformanceCounter();
    double delta = (double)((NOW - LAST));
    delta = delta / SDL_GetPerformanceFrequency();
    LAST = NOW;
    _fDelta = delta;
}

void Engine::step(int w, int h)
{
    Gu::pushPerf();    

    //Debug:Inc/Dec render ipe
    if(Gu::getFingers()->keyPress(SDL_SCANCODE_F8)) {
        Gu::incrementEnum<PipeBit::e>(_ePipeBit, PipeBit::e::MaxPipes);
        if(_ePipeBit == PipeBit::e::Full){
            _pipeBits.set();
        }
        else{
            _pipeBits.reset();
            _pipeBits.set(_ePipeBit);
        }
    }

    _pNet->update();

    Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncBegin);
    Gu::getFrameSync()->syncBegin();
    {
        Gu::getGraphicsContext()->setLoopState(EngineLoopState::Update);
        Gu::getGraphicsContext()->update(getFrameDelta());

        updateWidthHeight(w, h, false);
        _pApp->step((float)_fDelta);

        Gu::getGraphicsContext()->setLoopState(EngineLoopState::Render);
        
        _pRenderPipe->renderScene(_pipeBits, _pApp);
    }
    Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncEnd);
    Gu::getFrameSync()->syncEnd();

    Gu::popPerf();
}


void Engine::cleanup()
{
//    DEL_MEM(_pApp);
}

void Engine::updateWidthHeight(uint32_t w, uint32_t h, bool bForce){
    //update view/cam
    if (_iLastWidth != w || bForce) {
        _pViewport->setWidth(w);
    }
    if (_iLastHeight != h || bForce) {
        _pViewport->setHeight(h);
    }
    if (_iLastHeight != h || _iLastWidth != w || bForce) {
        if(_pRenderPipe != nullptr) {
            _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
        }
        _pApp->screenChanged(w, h, _bFullscreen);
        Gu::getGui()->screenChanged(w, h, _bFullscreen);
    }
    _iLastWidth = w;
    _iLastHeight = h;


}


}//ns Game

