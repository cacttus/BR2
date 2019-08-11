/**
*
*    @file Engine.h
*    @date November 5, 2016
*    @author Derek Page
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __ENGINE_14783644163757421765_H__
#define __ENGINE_14783644163757421765_H__

#include "../base/BaseHeader.h"

#include "../math/Vec2x.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"
#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace Game {

/**
*    @class Engine
*    @brief
*
*/
class Engine : public GameMemory {
private:
    //std::shared_ptr<Viewport> _pViewport = nullptr;
    std::shared_ptr<GraphicsApi> _pGraphicsApi = nullptr;
    std::shared_ptr<AppBase> _pApp = nullptr;
    std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;

    Uint64 NOW=0, LAST=0;
    GLuint vbuf, ibuf, vao;
    PipeBit::e _ePipeBit = PipeBit::e::Full;
    std::bitset<8> _pipeBits;

    std::shared_ptr<Net> _pNet;

    double _fDelta = 0;

public:
    Engine(std::shared_ptr<AppBase> rb, std::shared_ptr<GraphicsApi> re, int xw, int xh);
    virtual ~Engine();
   
//    std::shared_ptr<Viewport> getViewport() { return _pViewport; }

    void init();
    void updateDelta(); //**Call before step()
    void step();
    void cleanup();
    float getFrameDelta() { return (float)_fDelta; }
    GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
    void makeTestVao();
    void userZoom(int iAmount);
    void updateTouch(std::shared_ptr<Fingers> pFingers);
    
};

}//ns Game



#endif
