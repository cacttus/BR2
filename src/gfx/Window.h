/**
*
*    @file Window.h
*    @date August 11, 2019
*    @author Derek Page
*
*    � 2019 
*
*
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__


#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace Game {
/**
*    @class Window
*    @brief
*/
class Window : public VirtualMemory {
    std::shared_ptr<WindowViewport> _pViewport = nullptr;
    SDL_Window* _pSDLWindow = nullptr;
    uint32_t _iLastWidth;
    uint32_t _iLastHeight;
    void makeSDLWindow(t_string title, int rendersystem);
public:
    Window(t_string title, int rendersystem);
    virtual ~Window() override;

    std::shared_ptr<WindowViewport> getWindowViewport() { return _pViewport; }

    bool _bFullscreen = false;
    void toggleFullscreen();
    void updateWidthHeight(uint32_t w, uint32_t h, bool force);


};

}//ns Game



#endif
