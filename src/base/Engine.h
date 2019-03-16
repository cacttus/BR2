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
#include "../display/DisplayHeader.h"
#include "../model/ModelHeader.h"

namespace Game {

/**
*    @class Engine
*    @brief
*
*/
class Engine : public GameMemory {
private:
    std::shared_ptr<Viewport> _pViewport = nullptr;
    Uint64 NOW=0, LAST=0;
    SDL_Window* _pWindow = nullptr;
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<RoomBase> _pRoomBase = nullptr;
    //PipeMode::e _ePipeMode = PipeMode::e::Full; //Debug turns off render pipes.

    GLuint vbuf, ibuf, vao;
    std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
    //FramebufferMultisampleDeferred* _pFramebuffer = nullptr;
    PipeBit::e _ePipeBit = PipeBit::e::Full;
    std::bitset<8> _pipeBits;

    uint32_t _iLastWidth = 0;
    uint32_t _iLastHeight = 0;
    uint32_t _iFullscreenToggleWidth  = 0;
    uint32_t _iFullscreenToggleHeight = 0;

    double _fDelta = 0;
    bool _bFullscreen = false;

    void toggleFullscreen();
    void updateWidthHeight(uint32_t iw, uint32_t ih, bool bForce);
public:
    Engine(std::shared_ptr<GLContext> ct, std::shared_ptr<RoomBase> rb, SDL_Window* w, int xw, int xh);
    virtual ~Engine();
   
    void init();
    void updateDelta(); //**Call before step()
    void step(int w, int h);
    void cleanup();
    float getFrameDelta() { return (float)_fDelta; }
    GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
    void makeTestVao();
    void userZoom(int iAmount);
    void updateTouch(std::shared_ptr<Fingers> pFingers);
    
};

}//ns Game



#endif
