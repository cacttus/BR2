/**
*
*    @file SDL.h
*    @date August 11, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __SDL_15655475192235209788_H__
#define __SDL_15655475192235209788_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace Game {
/**
*    @class SDL
*    @brief Static SDL interface functions.
*
*/
class SDL : public VirtualMemory {
public:
    static void trySetWindowIcon(SDL_Window* w, t_string iconPath);
    static void createSurfaceFromImage(const t_string strImage, std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface);
    static void checkSDLErr(bool log = true);

};

}//ns Game



#endif
