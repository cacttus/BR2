#include "../base/BaseAll.h"
#include "../base/Engine.h"
#include "../base/RoomBase.h"
#include "../base/FrameSync.h"
#include "../base/oglErr.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../base/Stopwatch.h"
#include "../base/Net.h"

#include "../math/MathAll.h"

#include "../display/ShaderMaker.h"
#include "../display/RenderUtils.h"
#include "../display/ShaderBase.h"
#include "../display/Atlas.h"
#include "../display/HappySky.h"
#include "../display/RenderPipe.h"
#include "../display/CameraNode.h"
#include "../display/Viewport.h"
#include "../display/Picker.h"
#include "../display/Gui2d.h"

#include "../model/MeshNode.h"
#include "../model/MeshUtils.h"
#include "../model/MeshSpec.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"

//#include "SDL_shaders_gles2.h"

namespace Game {
///////////////////////////////////////////////////////////////////
Engine::Engine(std::shared_ptr<GLContext> ct, std::shared_ptr<RoomBase> rb, SDL_Window* w, int xw, int xh) : _pContext(ct), _pRoomBase(rb)
{
    AssertOrThrow2(rb != nullptr);
    AssertOrThrow2(ct != nullptr);
    _pWindow = w;
    _pViewport = std::make_shared<Viewport>(xw, xh);
}
Engine::~Engine()
{
   // DEL_MEM(_pViewport);
}
///////////////////////////////////////////////////////////////////

void Engine::init() {
    _pipeBits.set();

    _pRenderPipe = std::make_shared<RenderPipe>(_pContext);
    _pRenderPipe->init(_pViewport->getWidth(), _pViewport->getHeight(), _pRoomBase->makeAssetPath(_pRoomBase->getEnvTexturePath()));
    _pContext->setRenderPipe(_pRenderPipe);

    //Enable some stuff.
    #ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    #endif
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    //Gui
    Gu::getContext()->getGui()->init();

    //Net
    _pNet = std::make_shared<Net>();

    //Make Room
    _pRoomBase->setup(_pViewport, _pContext);

    //*Set room width / height
    _iLastWidth = _pRoomBase->getStartWidth();
    _iLastHeight = _pRoomBase->getStartHeight();

    Gu::SDLTrySetWindowIcon(_pWindow, _pRoomBase->getIconFullPath());


    if(_pRoomBase->getForceAspectRatio()){
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
    SDL_SetWindowSize(_pWindow, _iLastWidth, _iLastHeight);
    updateWidthHeight(_iLastWidth, _iLastHeight, true);

    if (_pRoomBase->getStartFullscreen() == true) {
        BroLogInfo("Setting window fullscreen.");
        toggleFullscreen();
    }

}
void Engine::updateTouch(std::shared_ptr<Fingers> pFingers) {
    if (pFingers->keyPress(SDL_SCANCODE_F11)) {
       toggleFullscreen();
    }

    _pRoomBase->updateTouch(pFingers, getFrameDelta());
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
        SDL_SetWindowSize(_pWindow, iFsW, iFsH);

        if (SDL_SetWindowFullscreen(_pWindow, SDL_WINDOW_FULLSCREEN) != 0) {
            BroLogError("Failed to go fullscreen.");
        }
        else {
            _bFullscreen = true;
            _pRoomBase->screenChanged(iFsW, iFsH, _bFullscreen);
        }
    }
    else {
        if(_iFullscreenToggleWidth >0 && _iFullscreenToggleHeight > 0){
            //Restore pre-fullscreen width/height
            SDL_SetWindowSize(_pWindow, _iFullscreenToggleWidth, _iFullscreenToggleHeight);
        }
        if (SDL_SetWindowFullscreen(_pWindow, 0) != 0) {
            BroLogError("Failed to exit fullscreen.");
        }
        else {
            _bFullscreen = false;
            _pRoomBase->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
        }
    }

}
void Engine::userZoom(int iAmount){
    _pRoomBase->userZoom(iAmount);
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
    if(Gu::getContext()->getFingers()->keyPress(SDL_SCANCODE_F8)) {
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

    _pContext->setLoopState(EngineLoopState::SyncBegin);
    _pContext->getFrameSync()->syncBegin();
    {
        _pContext->setLoopState(EngineLoopState::Update);
        _pContext->update(getFrameDelta());

        updateWidthHeight(w, h, false);
        _pRoomBase->step((float)_fDelta);

        _pContext->setLoopState(EngineLoopState::Render);
        
        _pRenderPipe->renderScene(_pipeBits, _pRoomBase);
    }
    _pContext->setLoopState(EngineLoopState::SyncEnd);
    _pContext->getFrameSync()->syncEnd();

    Gu::popPerf();
}


void Engine::cleanup()
{
//    DEL_MEM(_pRoomBase);
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
        _pRoomBase->screenChanged(w, h, _bFullscreen);
        Gu::getContext()->getGui()->screenChanged(w, h, _bFullscreen);
    }
    _iLastWidth = w;
    _iLastHeight = h;


}


}//ns Game

