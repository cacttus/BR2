/**
*  @file RenderHeader.h
*  @date September 1, 2019
*  @author Derek Page
*/
#pragma once
#ifndef __RENDERINCLUDES_15673689883087018270_H__
#define __RENDERINCLUDES_15673689883087018270_H__

#include "../base/GlobalIncludes.h"

//SDL
#include <SDL.h>

//Including both APIs.  In the future we abstract the API calls to interface and move the headers.
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <SDL_vulkan.h>

//Needed to get the main window handle so we can set the main window handle.
#include <SDL_syswm.h>

//Networking
#include <SDL_net.h>

//Vulkan
#include <vulkan/vulkan.h>


namespace BR2 {


}//ns Game



#endif
