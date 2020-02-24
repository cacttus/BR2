#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/Exception.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/FramebufferBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/RenderTarget.h"
#include "../model/ModelHeader.h"
#include "../model/VertexFormat.h"
#include "../model/MeshNode.h"
#include "../model/VboData.h"
#include "../model/VaoDataGeneric.h"

namespace BR2 {
FramebufferBase::FramebufferBase(std::shared_ptr<GLContext> pc, bool bMsaa, int nMsaa, vec4& vClear) :
    _pContext(pc), _bMsaaEnabled(bMsaa), _nMsaaSamples(nMsaa){
    _vClear = vClear;
}
FramebufferBase::~FramebufferBase(){ 
    deleteTargets();

}
std::shared_ptr<RenderTarget> FramebufferBase::getTargetByName(std::string name){
    for(std::shared_ptr<RenderTarget> rt : _vecTargets){
        if(rt->getName() == name){
            return rt;
        }
    }
    return nullptr;
}
void FramebufferBase::checkFramebufferComplete() {
    _pContext->chkErrRt();
    _pContext->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _uiGlFramebufferId);
    _pContext->chkErrRt();

    attachAllTargets();
    setDrawAllTargets();

    GLenum Status = _pContext->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    _pContext->chkErrRt();

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        if (Status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        {
            BRLogError("Framebuffer is not complete.  Multisampling error.  Make sure that you enable " +
                "multisampling on ALL textures, additionally make sure all textures have the same setting for FIXED_SAMPLE_LOCATIONS");
        }
        _pContext->chkErrRt();
        BRThrowException("Failed to create framebuffer.");
    }
}
void FramebufferBase::attachAllTargets() {
    for (std::shared_ptr<RenderTarget> inf : _vecTargets) {
        inf->bind();
        //_pContext->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, inf->getAttachment(), inf->getTextureTarget(), inf->getTexId(), 0);
        //_pContext->chkErrRt();
    }
}
void FramebufferBase::setDrawAllTargets(){
    const int c_iMaxAttachments = 64;
    GLenum attachments[c_iMaxAttachments];
    int iCount = 0;
    for(int i=0; i<c_iMaxAttachments; ++i) {
        if(i<(int)_vecTargets.size()){
            if(_vecTargets[i]->getTargetType() == RenderTargetType::e::Color ||
                _vecTargets[i]->getTargetType() == RenderTargetType::e::Pick ||
                _vecTargets[i]->getTargetType() == RenderTargetType::e::Shadow 
                ) {
                attachments[i] = _vecTargets[i]->getAttachment();
                iCount++;
            }
        }
        else {
            attachments[i] = 0;
        }
    }

    _pContext->glDrawBuffers(iCount, attachments);
    _pContext->chkErrDbg();

}
//void FramebufferBase::attachDepthTarget(std::shared_ptr<RenderTarget> pSharedDepth){
//    if (pSharedDepth->getMsaaEnabled()) {
//        std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, pSharedDepth->getGlTexId(), 0);
//    }
//    else {
//        std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pSharedDepth->getGlTexId(), 0);
//    }
//    Gu::getGraphicsContext()->chkErrRt();
//}
void FramebufferBase::addTarget(t_string strName, GLenum internalFormat, GLenum texFormat,
    GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType) {
    int iIndex = (int)_vecTargets.size();

    std::shared_ptr<RenderTarget> inf = createTarget(strName, internalFormat, texFormat, dataType, w, h, 
        eTargetType, iIndex, _bMsaaEnabled, _nMsaaSamples);
    _vecTargets.push_back(inf);
}
void FramebufferBase::addTarget(std::shared_ptr<RenderTarget> other) {
    int iIndex = (int)_vecTargets.size();

    std::shared_ptr<RenderTarget> inf = std::make_shared<RenderTarget>(true);
    inf->_strName = other->_strName;
    inf->_iLayoutIndex = iIndex;
    inf->_eTextureTarget = other->_eTextureTarget;
    inf->_eAttachment = GL_COLOR_ATTACHMENT0 + iIndex;
    inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
    inf->_eBlitBit = GL_COLOR_BUFFER_BIT;
    inf->_eTargetType = other->_eTargetType;
    inf->_iGlTexId = other->_iGlTexId;

    _vecTargets.push_back(inf);
}
std::shared_ptr<RenderTarget> FramebufferBase::createTarget(t_string strName, GLenum internalFormat, GLenum texFormat,
    GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType, int32_t iIndex, bool bMsaaEnabled, int32_t nMsaaSamples) {

    std::shared_ptr<RenderTarget> inf = std::make_shared<RenderTarget>(false);
    inf->_strName = strName;
    inf->_iLayoutIndex = iIndex;
    inf->_eTextureTarget = GL_TEXTURE_2D;
    inf->_eAttachment = GL_COLOR_ATTACHMENT0 + iIndex;
    inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
    inf->_eBlitBit = GL_COLOR_BUFFER_BIT;
    inf->_eTargetType = eTargetType;

    GLint maxAttach = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
    if(iIndex > maxAttach) {
        BRThrowException("GPU Does not support enough color attachments, wanted: " + iIndex + " max supported: " + maxAttach);
    }

    makeRenderTexture(&inf->_iGlTexId,
        inf->_eAttachment, internalFormat, texFormat, dataType, w, h,
        &inf->_eTextureTarget, bMsaaEnabled, nMsaaSamples);

    return inf;
}
std::shared_ptr<RenderTarget> FramebufferBase::createDepthTarget(t_string strName, int32_t w, int32_t h, int iIndex, bool bMsaaEnabled, int32_t nMsaaSamples) {
    std::shared_ptr<RenderTarget> inf = std::make_shared<RenderTarget>(true);
    inf->_strName = strName;
    //**Note: index doesn't matter for depth target since we simply bind it to GL_Depth_attachment.  It confused the fuck out of me. 2/9/18
    inf->_iLayoutIndex = iIndex;
    if (bMsaaEnabled) {
        inf->_eTextureTarget = GL_TEXTURE_2D_MULTISAMPLE;
    }
    else {
        inf->_eTextureTarget = GL_TEXTURE_2D;
    }
    inf->_eAttachment = GL_DEPTH_ATTACHMENT; //GL_COLOR_ATTACHMENT0 + iLayoutIndex;
    inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
    inf->_eBlitBit = GL_DEPTH_BUFFER_BIT;
    inf->_eTargetType = RenderTargetType::e::Depth;

    //This will cycle through depth formats and choose the most precise.
    RenderUtils::createDepthTexture(&inf->_iGlTexId, w, h, bMsaaEnabled, nMsaaSamples, GL_DEPTH_COMPONENT32F);

    return inf;
}
void FramebufferBase::deleteTargets()
{
    _pContext->glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    
    if(_uiGlFramebufferId >0) {
        _pContext->glDeleteFramebuffers(1, &_uiGlFramebufferId);
    }

    for (size_t i = 0; i<_vecTargets.size(); ++i) {
        _vecTargets[i] = nullptr;
    }
    _vecTargets.resize(0);
}
//void FramebufferBase::resizeScreenBuffers(int32_t w, int32_t h, std::shared_ptr<RenderTarget> pSharedDepthTarget)
//{
//    deleteTargets();
//    init(w, h, pSharedDepthTarget);
//}
void FramebufferBase::makeRenderTexture(GLuint* iTexId, GLenum eAttachment, GLenum eInternalFormat
    , GLenum eTextureFormat, GLenum eDataType, int32_t iWidth, int32_t iHeight,
    GLenum* eOutTarget, bool bMultisample, int32_t nSamples) {

    glGenTextures(1, iTexId);
    Gu::getGraphicsContext()->chkErrRt();

    if (bMultisample)
    {

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *iTexId);
        Gu::getGraphicsContext()->chkErrRt();

        //if (Gu::GetEngineDisplayParams()->getEnableAnisotropicFiltering())
        //{
        //    //CHANGED FROM GL_TEXTURE_2D **MIGHT BE WRONG
        //    glTexParameterf(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_ANISOTROPY_EXT, Gu::GetEngineDisplayParams()->getTextureAnisotropyLevel());
        //    CheckGpuErrorsDbg();
        //}
        std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, nSamples, eInternalFormat, iWidth, iHeight, GL_TRUE);
        Gu::getGraphicsContext()->chkErrRt();

        if (eOutTarget != NULL)
            *eOutTarget = GL_TEXTURE_2D_MULTISAMPLE;
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *iTexId);
        Gu::getGraphicsContext()->chkErrRt();
        //if (Gu::GetEngineDisplayParams()->getEnableAnisotropicFiltering())
        //{
        //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Gu::GetEngineDisplayParams()->getTextureAnisotropyLevel());
        //    Gu::getGraphicsContext()->chkErrRt();
        //}
        glTexImage2D(GL_TEXTURE_2D, 0, eInternalFormat, iWidth, iHeight, 0, eTextureFormat, eDataType, nullptr);
        Gu::getGraphicsContext()->chkErrRt();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        Gu::getGraphicsContext()->chkErrRt();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        Gu::getGraphicsContext()->chkErrRt();

        if (eOutTarget != nullptr) {
            *eOutTarget = GL_TEXTURE_2D;
        }
    }
    glDisable(GL_DITHER);//Dithering gets enabled for some reason

    Gu::getGraphicsContext()->chkErrRt();

}

}//ns Game
