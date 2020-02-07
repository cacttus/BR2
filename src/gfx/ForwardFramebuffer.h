/**
*
*    @file ForwardFramebuffer.h
*    @date May 26, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __FORWARDFRAMEBUFFER_14958495541236280687_H__
#define __FORWARDFRAMEBUFFER_14958495541236280687_H__

#include "../gfx/FramebufferBase.h"

namespace Game {
/**
*    @class ForwardFramebuffer
*    @brief
*
*/
class ForwardFramebuffer : public FramebufferBase {
public:
    ForwardFramebuffer(std::shared_ptr<GLContext> pc, int32_t w, int32_t h, bool bMsaa, int nMsaa, vec4& vClear);
    virtual ~ForwardFramebuffer() override;
    virtual void init(int32_t iWidth, int32_t iHeight, std::shared_ptr<RenderTarget> sharedDepth, std::shared_ptr<RenderTarget> sharedPick) override;
    virtual void beginRender() override;
    virtual void endRender() override;

    virtual void attachColorTargets(int32_t iWidth, int32_t iHeight);  //Adds the targets - overridable for the pick buffer.
    GLuint getGlColorBufferTexId();
    void clearFb();
};
//
//class PickFramebuffer : public ForwardFramebuffer {
//public:
//    PickFramebuffer(std::shared_ptr<GLContext> pc, int32_t w, int32_t h);
//    virtual ~PickFramebuffer() override;
//    virtual void attachColorTargets(int32_t iWidth, int32_t iHeight) override;
//
//};

}//ns Game



#endif
