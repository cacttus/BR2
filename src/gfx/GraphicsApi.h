/**
*  @file GraphicsApi.h
*  @date August 5, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __GraphicsApi_1565057965514614365_H__
#define __GraphicsApi_1565057965514614365_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
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

    virtual std::shared_ptr<GraphicsWindow> createWindow(string_t title, bool isMain = false) = 0;

    void destroyWindow(std::shared_ptr<GraphicsWindow> w);
    virtual void cleanup() ;

    std::vector<std::shared_ptr<GraphicsWindow>> getGraphicsWindows() { return _pvecWindows; }

    virtual void createRenderer() = 0;

    GraphicsApi();
    virtual ~GraphicsApi() override;
};

}//ns Game



#endif
