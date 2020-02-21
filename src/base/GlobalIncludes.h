/**
*  @file GlobalIncludes.h
*  @date November 12, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GLOBALINCLUDES_14789744473709794265_H__
#define __GLOBALINCLUDES_14789744473709794265_H__

#include "../base/BuildConfig.h"

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
//#include <vulkan/vulkan.h>

//STL
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

#ifdef __IPHONEOS__
//#include "CGGeometry.h"
//#define FLT_MAX CGFLOAT_MAX
#endif

#ifdef main
#  undef main
#endif /* main */

////////////
//Externals

//PNG files.
//#include "../ext/lodepng.h"

//XML files.
//#include "../ext/pugixml/pugixml.hpp"


//CL, annoying errors.  This must come before we include the actual windows stuff.
#ifdef __WINDOWS__
//disable wctombs_s
#pragma warning(disable:4996)
//not all control paths return a value - must be an error.
#pragma warning(error:4715)
//'==': operator has no effect; did you intend '='?
#pragma warning(error:4553)
//warning C4002: too many arguments for function-like macro invocation 'BroLogWarn'
#pragma warning(error:4002)
//recursive on all control paths
#pragma warning(error:4717)


//Winsock (old)
//#include <ws2tcpip.h>
//Reinclude if we can't get sdl net to work
//#include <winsock2.h>
//Note to link with ws2_32.lib and wsock32.lib
#endif

#endif
