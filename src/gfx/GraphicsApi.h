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
    std::shared_ptr<GraphicsWindow> _pMainWindow = nullptr;
    std::vector<std::shared_ptr<GraphicsWindow>> _pvecWindows;

    void destroyMainWindow();
    void addWindow(std::shared_ptr<GraphicsWindow> w);
public:
    std::shared_ptr<GraphicsWindow> getMainWindow() { return _pMainWindow; }
    virtual std::shared_ptr<GraphicsWindow> createWindow(t_string title, bool isMain = false) = 0;
    void destroyWindow(std::shared_ptr<GraphicsWindow> w);

    //virtual void getDrawableSize(int* w, int* h) = 0;
    //virtual void swapBuffers() = 0;
    //virtual void makeCurrent() = 0;
    
    virtual void cleanup();
    virtual void createContext(std::shared_ptr<AppBase> app) = 0;

    std::vector<std::shared_ptr<GraphicsWindow>> getGraphicsWindows() { return _pvecWindows; }

    GraphicsApi();
    virtual ~GraphicsApi() override;
};

}//ns Game



#endif
