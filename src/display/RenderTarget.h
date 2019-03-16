/**
*
*    @file RenderTarget.h
*    @date May 27, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __RENDERTARGET_149586780355650870_H__
#define __RENDERTARGET_149586780355650870_H__

#include "../display/DisplayHeader.h"
namespace Game {
/**
*    @class RenderTarget
*    @brief
*
*/
//Storage class for OpenGL render target data.
class RenderTarget : public VirtualMemory {
    friend class FramebufferBase;
private:
    t_string _strName;
    GLuint _iGlTexId;    // Texture Id
    GLenum _eTextureTarget; //GL_TEXTURE_2D, or other
    GLenum _eAttachment;//GL_COLORATTACHMENT_0 + n
    GLint _iLayoutIndex;// The (layout = 0).. in the shader
    GLenum _eTextureChannel;//GL_TEXTURE0 +..
    GLenum _eBlitBit; // GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT
    RenderTargetType::e _eTargetType;
    bool _bShared = false;
public:
    RenderTarget(bool bShared) : _bShared(bShared) { }
    virtual ~RenderTarget() override;

    bool getShared() {return _bShared; }
    GLuint getGlTexId() { return _iGlTexId; }
    t_string getName() { return _strName; }
    GLenum getTextureChannel() { return _eTextureChannel; }
    GLenum getAttachment() { return _eAttachment; }
    GLenum getTextureTarget() { return _eTextureTarget; }
    GLuint getTexId() { return _iGlTexId; }
    GLint getLayoutIndex() { return _iLayoutIndex; }
    RenderTargetType::e getTargetType() { return _eTargetType; }
    GLenum getBlitBit() { return _eBlitBit; }
    bool getMsaaEnabled();

    void bind(GLenum eAttachment = 0);
};


}//ns Game



#endif
