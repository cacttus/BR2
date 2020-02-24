/**
*  @file GlobalIncludes.h
*  @date November 12, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GLOBALINCLUDES_14789744473709794265_H__
#define __GLOBALINCLUDES_14789744473709794265_H__

#include "../base/BuildConfig.h"

//////////////////////////////////////////////////////////////////////////
//
// This is the root of all *system and api* include files.
// Note: This file is included everywhere, so we must spare the heavy includes!
//
//////////////////////////////////////////////////////////////////////////

// Engine Rebuild.
// Removing Windows, SDL, and Vulkan from the global includes, to increase compile time.

//SDL

//Including both APIs.  In the future we abstract the API calls to interface and move the headers.

//**Leaving SDL_openGL since we literally use OpenGL everywhere. In the future we may be able to abstract this header.

//#include <SDL_opengl.h>
//#include <SDL_opengl_glext.h>

//#include <SDL_vulkan.h>
//Needed to get the main window handle so we can set the main window handle.


//Networking
//#include <SDL_net.h>

//Vulkan
//#include <SDL_vulkan.h>
//#include <vulkan/vulkan.h>

//STL
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <bitset>
//#include <set>
//#include <iostream>
//#include <stack>
//#include <algorithm>
//#include <fstream>
//#include <sstream>
//#include <stdarg.h>
////needed for FLT_MAX
//#include <float.h>

////for fpclassify
//#include <cmath>
//
////for std::mutex
//#include <mutex>
//
////For High resolution clock
//#include <chrono>
//
////for std::atomic_bool
//#include <atomic>
//
////std::future
//#include <future>
//
////std::array
//#include <array>
//
//
////for mkdir
//#include <direct.h>

//for opendir
//**NOTE: this drags WINNT into the mix.  Inlcude teh WindowsHeader for this file.
//

////For std::setprecision
//#include <iomanip>
//
////C++17 Used for some Vulkan init stuff
//#include <optional>
////C++17 For path
//#include <filesystem>

//Mono
//#include <mono/jit/jit.h>
//#include <mono/metadata/assembly.h>

//TODO

////////////
//Externals

//PNG files.
//#include "../ext/lodepng.h"

//XML files.
//#include "../ext/pugixml/pugixml.hpp"


#endif