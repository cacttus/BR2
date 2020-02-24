/**
*  @file SDLUtils.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __SDL_15655475192235209788_H__
#define __SDL_15655475192235209788_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

#include "../base/SDLIncludes.h"

namespace BR2 {
/**
*  @class SDLUtils
*  @brief Static SDLUtils interface functions.
*/
class SDLUtils : public VirtualMemory {
public:
  static void trySetWindowIcon(SDL_Window* w, t_string iconPath);
  static void checkSDLErr(bool log = true);
private:
    static void createSurfaceFromImage(const t_string strImage, std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface);
    static SDL_Surface* createSurfaceFromImage(const std::shared_ptr<Img32> bi);
};

}//ns Game



#endif
