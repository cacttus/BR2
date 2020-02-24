/**
*
*    @file GlobalIncludes.h
*    @date November 12, 2016
*    @author MetalMario971
*
*    � 2016
*
*
*/
#pragma once
#ifndef __GLOBALINCLUDES_14789744473709794265_H__
#define __GLOBALINCLUDES_14789744473709794265_H__

#include "../base/BuildConfig.h"

//
//TODO: move all the include files into their respective sources and headers to speed up compilation
//
#define WIN32_LEAN_AND_MEAN

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

//STL
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdarg.h>
//needed for FLT_MAX
#include <float.h>

//for fpclassify
#include <cmath>

//for std::mutex
#include <mutex>

//For High resolution clock
#include <chrono>

//for std::atomic_bool
#include <atomic>

//std::future
#include <future>

//std::array
#include <array>

#include <bitset>

//for mkdir
#include <direct.h>

//for opendir
#include "../ext/dirent.h"

//For std::setprecision
#include <iomanip>

//C++17 Used for some Vulkan init stuff
#include <optional>
//C++17 For path
#include <filesystem>

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
#include "../ext/lodepng.h"

//XML files.
#include "../ext/pugixml/pugixml.hpp"


////////////
//Windows
#ifdef _WIN32
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

#include <Windows.h>

//Winsock (old)
//#include <ws2tcpip.h>
//Reinclude if we can't get sdl net to work
//#include <winsock2.h>
//Note to link with ws2_32.lib and wsock32.lib
#endif

#endif
