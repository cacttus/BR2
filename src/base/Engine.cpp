#include "../base/BaseAll.h"
#include "../base/Engine.h"
#include "../base/AppBase.h"
#include "../base/FrameSync.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../base/Stopwatch.h"
#include "../base/Net.h"
#include "../base/Window.h"

#include "../math/MathAll.h"

#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Atlas.h"
#include "../gfx/HappySky.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/CameraNode.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/Picker.h"
#include "../gfx/Gui2d.h"
#include "../gfx/GraphicsApi.h"

#include "../model/MeshNode.h"
#include "../model/MeshUtils.h"
#include "../model/MeshSpec.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"

namespace Game {
Engine::Engine() {
}
Engine::~Engine() {
}
void Engine::init() {
    _pipeBits.set();

    //Deferred Renderer
    _pRenderPipe = std::make_shared<RenderPipe>();
    _pRenderPipe->init(Gu::getViewport()->getWidth(), Gu::getViewport()->getHeight(), Gu::getApp()->makeAssetPath(Gu::getApp()->getEnvTexturePath()));
    Gu::setRenderPipe(_pRenderPipe);

    //Gui
    if (Gu::getGui()) {
        Gu::getGui()->init();
    }

    //Network
    _pNet = std::make_shared<Net>();
}
void Engine::updateTouch(std::shared_ptr<Fingers> pFingers) {
    if (pFingers->keyPress(SDL_SCANCODE_F11)) {
        Gu::getWindow()->toggleFullscreen();
    }

    Gu::getApp()->updateTouch(pFingers, getFrameDelta());
}
void Engine::userZoom(int iAmount) {
    Gu::getApp()->userZoom(iAmount);
}
void Engine::updateDelta() {
    _iDeltaNow = SDL_GetPerformanceCounter();
    double delta = (double)((_iDeltaNow - _iDeltaLast));
    delta = delta / SDL_GetPerformanceFrequency();
    _iDeltaLast = _iDeltaNow;
    _fDelta = delta;
}
void Engine::step() {
    Gu::pushPerf();
    {
        //Debug:Inc/Dec render ipe
        if (Gu::getFingers()->keyPress(SDL_SCANCODE_F8)) {
            Gu::incrementEnum<PipeBit::e>(_ePipeBit, PipeBit::e::MaxPipes);
            if (_ePipeBit == PipeBit::e::Full) {
                _pipeBits.set();
            }
            else {
                _pipeBits.reset();
                _pipeBits.set(_ePipeBit);
            }
        }

        _pNet->update();

        Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncBegin);
        Gu::getFrameSync()->syncBegin();
        {
            Gu::getGraphicsContext()->setLoopState(EngineLoopState::Update);
            Gu::update(getFrameDelta());

            Gu::getApp()->step((float)_fDelta);

            Gu::getGraphicsContext()->setLoopState(EngineLoopState::Render);

            _pRenderPipe->renderScene(_pipeBits, Gu::getApp());
        }
        Gu::getGraphicsContext()->setLoopState(EngineLoopState::SyncEnd);
        Gu::getFrameSync()->syncEnd();
    }
    Gu::popPerf();
}


void Engine::cleanup() {
}
void Engine::updateWidthHeight(int w, int h) {
    _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);

}


}//ns Game

