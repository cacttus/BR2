#include "../base/BaseAll.h"
#include "../base/Gu.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/ForwardFramebuffer.h"
#include "../gfx/BufferRenderTarget.h"


namespace Game {
///////////////////////////////////////////////////////////////////
ForwardFramebuffer::ForwardFramebuffer(std::shared_ptr<GLContext> pc, int32_t w, int32_t h,bool bMsaa, int nMsaa, vec4& vClear) :
    FramebufferBase(pc,bMsaa,nMsaa, vClear) {
}
ForwardFramebuffer::~ForwardFramebuffer(){
}
///////////////////////////////////////////////////////////////////
GLuint ForwardFramebuffer::getGlColorBufferTexId() {
    AssertOrThrow2(_vecTargets.size()>0);
    return _vecTargets[0]->getGlTexId(); 
}
void ForwardFramebuffer::init(int32_t iWidth, int32_t iHeight, std::shared_ptr<BufferRenderTarget> sharedDepth, std::shared_ptr<BufferRenderTarget> sharedPick) {
    deleteTargets();

    Gu::getShaderMaker()->shaderBound(nullptr);
    _pContext->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _pContext->chkErrRt();
    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    _pContext->chkErrRt();

    _pContext->glGenFramebuffers(1, &_uiGlFramebufferId);
    _pContext->glBindFramebuffer(GL_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, iWidth);
    _pContext->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, iHeight);
    _pContext->chkErrRt();
    
    attachColorTargets(iWidth, iHeight);
    addTarget(sharedPick);
   // sharedPick->bind(GL_COLOR_ATTACHMENT1);
    sharedDepth->bind();
        
    checkFramebufferComplete();

    //Return to default.
    Gu::getShaderMaker()->shaderBound(nullptr);
    _pContext->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _pContext->chkErrRt();
    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);//. The value zero is reserved, but there is no default renderbuffer object. Instead, renderbuffer set to zero effectively unbinds any renderbuffer object previously bound. 
    _pContext->chkErrRt();

    _eState = FramebufferState::e::Initialized;
}

void ForwardFramebuffer::attachColorTargets(int32_t iWidth, int32_t iHeight){
    //VV < don't change "Color" Name
    addTarget("Color", GL_RGBA32F, GL_RGBA, GL_FLOAT, iWidth, iHeight, RenderTargetType::e::Color);
}
void ForwardFramebuffer::clearFb(){
    //Call this before we begin the defrred
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);//_depthRenderBufferId);
    setDrawAllTargets();

    glClearColor(getClear().x, getClear().y, getClear().z, getClear().w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void ForwardFramebuffer::beginRender() {
    if(_eState != FramebufferState::Initialized){
        BroThrowException("Framebuffer was not initialized.");
    }

    //Clear all buffers
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->glBindRenderbuffer(GL_RENDERBUFFER, 0);//_depthRenderBufferId);

    //Do not clear - previous deferred operation is in here. (clear happens in clearFb)
    //**Do not clear***
    //**Do not clear***
    //**Do not clear***
    //**Do not clear***
    //**Do not clear***
    //**Do not clear***

}
void ForwardFramebuffer::endRender() { 
    //noting
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//PickFramebuffer::PickFramebuffer(std::shared_ptr<GLContext> pc, int32_t w, int32_t h) : 
//    ForwardFramebuffer(pc, w ,h, false , 0) {
//}
//PickFramebuffer::~PickFramebuffer() {
//}
//void PickFramebuffer::attachColorTargets(int32_t iWidth, int32_t iHeight) {
//    addTarget("Color", GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, iWidth, iHeight, RenderTargetType::e::Color);
//}




}//ns Game
