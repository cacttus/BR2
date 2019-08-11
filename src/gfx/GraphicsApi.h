/**
*
*    @file GraphicsApi.h
*    @date August 5, 2019
*    @author Derek Page
*
*    © 2019
*
*
*/
#pragma once
#ifndef __GraphicsApi_1565057965514614365_H__
#define __GraphicsApi_1565057965514614365_H__

#include "../gfx/GfxHeader.h"

namespace Game {
/**
*    @class GraphicsApi
*    @brief
*/
class GraphicsApi : public VirtualMemory {
protected:
    std::shared_ptr<Window> _pWindow = nullptr;

    void makeWindow();
public:
    std::shared_ptr<Window> getWindow() { return _pWindow; }
    virtual void createWindow(t_string title) = 0;
    virtual void getDrawableSize(int* w, int* h) = 0;
    virtual void swapBuffers() = 0;
    virtual void makeCurrent() = 0;
    
    virtual void cleanup();
    virtual void createContext(std::shared_ptr<AppBase> app) = 0;

    GraphicsApi();
    virtual ~GraphicsApi() override;
};

}//ns Game



#endif
