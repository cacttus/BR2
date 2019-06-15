/**
*
*    @file GlobalIncludes.h
*    @date November 12, 2016
*    @author Derek Page
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __GLOBALINCLUDES_14789744473709794265_H__
#define __GLOBALINCLUDES_14789744473709794265_H__
//#include "../ext/glad.h"
//#define COORDINATE_SYSTEM_RHS 1


// 2019 03 15 - Changing SDL to be a library not a built project
//#include "../../../include/SDL.h"
//#include "../../../include/SDL_opengl.h"
//#include "../../../include/SDL_opengl_glext.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
//Needed to get the main window handle so we can set the main window handle.
#include <SDL_syswm.h>

#include <SDL_net.h>

//Include for SDL_Mixer which also includes ffmpeg..yikes
//#include "../../../SDL2_mixer-2.0.1/SDL_mixer.h"

//#include <GL/gl.h>
/* GL and GLES2 headers conflict on Linux 32 bits */
//#if SDL_VIDEO_OPENGL_ES2 && !SDL_VIDEO_OPENGL
//#include "../../../include/SDL_opengles2.h"
//#endif /* SDL_VIDEO_OPENGL_ES2 && !SDL_VIDEO_OPENGL */


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
#include <future>

//std::array
#include <array>

#include <bitset>
//#include <SDL.h>
//Note about these includes
//**For IOS SDL_Opengl will comment itself out
//**For PC - PC Should comment out opengl ES...
//#define GLEW_STATIC 1
//#include <GL/glew.h>

//for mkdir
#include <direct.h>
//for opendir
#include "../ext/dirent.h"

//For std::setprecision
#include <iomanip>

#ifdef __IPHONEOS__
//#include "CGGeometry.h"
//#define FLT_MAX CGFLOAT_MAX
#endif
//#include "../ext/glew.h"

//
//#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#ifdef main
#  undef main
#endif /* main */



////#include "SDL_opengl.h"
//#else
////IPHONE
//#include "SDL.h"
//#ifdef main
//#  undef main
//#endif /* main */
//
//
//#include "SDL_opengles2.h"
//#endif
#include "../ext/lodepng.h"

#ifdef _WIN32
//wctombs_s fucking
#pragma warning(disable:4996)
//not all control paths return a value - must be an error.
#pragma warning(error:4715)
//'==': operator has no effect; did you intend '='?
#pragma warning(error:4553)
////Disable "throw list" warning
//#pragma warning( disable:4290 ) 
////Disable warning C4091: 'typedef ': ignored on left of '' when no variable is declared
//#pragma warning( disable:4091 ) 
//// - Disable "nonstandard enum extension"
//#pragma warning( disable:4482 )
////Disable the annoying "unsafe mix of bool and bool" when comparing a bool to true.
//#pragma warning( disable:4805 )
////warning C4996: 'wcstombs': This function or variable may be unsafe. Consider using wcstombs_s instead.
////Microsoft's recommendation is'nt cross-platform
//#pragma warning( disable:4996 )

#include <Windows.h>
//#include <Windows.h>
//#include <ws2tcpip.h>
//Reinclude if we can't get sdl net to work
//#include <winsock2.h>

//
//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "wsock32.lib")
#endif

//#define CC_REF_LEAK_DETECTION 1




//#include "cocos2d.h"

//#define CC_REF_LEAK_DETECTION 1

#endif
