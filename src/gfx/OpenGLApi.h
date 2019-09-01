/**
*
*    @file OpenGLApi.h
*    @date August 5, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __OPENGLGraphicsApi_15650573213471462786_H__
#define __OPENGLGraphicsApi_15650573213471462786_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/GraphicsApi.h"

namespace Game {
/**
*    @class OpenGLApi
*    @brief
*
*/
class OpenGLApi : public GraphicsApi {
    std::shared_ptr<GLContext> _pContext=nullptr;
public:

    virtual std::shared_ptr<GraphicsWindow> createWindow(std::string title, bool isMain = false) override;
    virtual void cleanup() override;
    std::shared_ptr<GLContext> getContext() { return _pContext; }
    std::shared_ptr<GLContext> makeContext(std::shared_ptr<GraphicsWindow> w, GLContext::GLProfile& p);

    virtual void createRenderer();

    OpenGLApi();
    virtual ~OpenGLApi() override;
};



}//ns Game



#endif
