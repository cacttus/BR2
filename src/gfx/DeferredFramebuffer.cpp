#include "../base/BaseAll.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../math/BitHacks.h"
#include "../gfx/DeferredFramebuffer.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/Viewport.h"
#include "../gfx/RenderTarget.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderBase.h"


namespace Game {
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
DeferredFramebuffer::DeferredFramebuffer( std::shared_ptr<GLContext> pc, uint32_t iWidth, uint32_t iHeight, bool bMultisample, 
    uint32_t nSamples, vec4& vClear) :
    FramebufferBase(pc, bMultisample, nSamples, vClear) {
    _bMultisample = bMultisample;
    _nMsaaSamples = nSamples; 
    
    _pBloomVaoPass = NULL;
}
DeferredFramebuffer::~DeferredFramebuffer(){
}
///////////////////////////////////////////////////////////////////
void DeferredFramebuffer::init(int32_t w, int32_t h, std::shared_ptr<RenderTarget> pSharedDepthTarget, std::shared_ptr<RenderTarget> sharedPick) {
    deleteTargets();

    bool _bUseRenderBuffer = false;

    //TODO: later we'll create this async.
    //Gd::verifyRenderThread();
    _pContext->glGenFramebuffers(1, &_uiGlFramebufferId);
    _pContext->chkErrRt();
    _pContext->glBindFramebuffer(GL_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->chkErrRt();
    _pContext->glFramebufferParameteri(GL_FRAMEBUFFER,GL_FRAMEBUFFER_DEFAULT_WIDTH, w);
    _pContext->chkErrRt();
    _pContext->glFramebufferParameteri(GL_FRAMEBUFFER,GL_FRAMEBUFFER_DEFAULT_HEIGHT, h);
    _pContext->chkErrRt();
    _pContext->glActiveTexture(GL_TEXTURE0);
    _pContext->chkErrRt();

    // - Textures
    //Don't change the names here, we reference them elsewhere *yikes*
    addTarget("Position", GL_RGBA32F, GL_RGBA, GL_FLOAT, w, h, RenderTargetType::e::Color);//0
    addTarget("Color"   , GL_RGBA32F, GL_RGBA, GL_FLOAT, w, h, RenderTargetType::e::Color);//1
    addTarget("Normal"  , GL_RGBA32F, GL_RGBA, GL_FLOAT, w, h, RenderTargetType::e::Color);//2
    addTarget("Plane"   , GL_RGBA32F, GL_RGBA, GL_FLOAT, w, h, RenderTargetType::e::Color);//3
    addTarget(sharedPick);//4
  //  sharedPick->bind(GL_COLOR_ATTACHMENT4);

    //Depth Buffer
    pSharedDepthTarget->bind();
   // attachDepthTarget(pSharedDepthTarget);

    checkFramebufferComplete();

    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    _pContext->chkErrRt();

    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    _pContext->chkErrRt();

    _eState = FramebufferState::e::Initialized;
}
bool DeferredFramebuffer::getIsBloomEnabled()
{
    return false;
}
int DeferredFramebuffer::getNumNonDepthTargets(){
    int ret = 0;
    for(std::shared_ptr<RenderTarget> inf : _vecTargets) {
        if(inf->getTargetType() != RenderTargetType::e::Depth) {
            ret++;
        }
    }
    return ret;
}
int DeferredFramebuffer::getNumTargets() {
    return (int)_vecTargets.size();
    //uh..?
    //int ret = 0;
    //for (std::shared_ptr<RenderTarget> inf : _vecTargets) {
    //    ret++;
    //}
    //return ret;
}
std::shared_ptr<RenderTarget> DeferredFramebuffer::getTarget(RenderTargetType::e eType) {
    for (std::shared_ptr<RenderTarget> inf : _vecTargets) {
        if (inf->getTargetType() == eType) {
            return inf;
        }
    }
    return nullptr;
}

void DeferredFramebuffer::beginRender()
{
    if (_eState != FramebufferState::Initialized) {
        BroThrowException("Framebuffer was not initialized.");
    }

    //GLenum attachments[32];
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    setDrawAllTargets();
    
    //NOTE:
    //CRITICAL that clear color is zero here.
    // Otherwise the color somehow shows up in random places getting
    // blended with other colors..
    glClearColor(getClear().x, getClear().y, getClear().z, getClear().w);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    _pContext->chkErrDbg();

}
void DeferredFramebuffer::endRender() 
{
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}





}//ns Game
