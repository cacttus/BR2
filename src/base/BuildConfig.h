/**
*  @file BuildConfig.h
*  @date April 3, 2016
*  @author MetalMario971
*    Contains all Build based defines.
*/
#pragma once
#ifndef __BUILDCONFIG_14597134672299218799_H__
#define __BUILDCONFIG_14597134672299218799_H__

//////////////////////////////////////////////////////////////////////////
/// Only put *#define* in this file!  Nothing else please.
/// This header must come before all other header files.
//////////////////////////////////////////////////////////////////////////

#define OS_NOT_SUPPORTED_ERROR error "Operating system not supported"

//////////////////////////////////////////////////////////////////////////
//Project defines
#define MINEGAME_PHASE1
#define ENGINE_REBUILD

#ifdef _DEBUG
//Uncomment this to debug pure header files
#define __SK_DEBUG_INCLUDES__ 1
#endif

//////////////////////////////////////////////////////////////////////////
//G++ / CLANG - NDEBUG is enabled by DEFAULT messing up builds.
#ifdef _DEBUG
#ifdef NDEBUG
#undef NDEBUG
#endif
#ifdef _NDEBUG
#undef _NDEBUG
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// OPERATING SYSTEM
#ifdef _WIN32  
//Note Win64 is defined WITH win32
#define BR2_OS_WINDOWS
//#define BRO_USE_DIRECTX
//We're not using winsock anymore due to SDL_Net
//#define BRO_USE_WINSOCK
#elif BR2_OS_ANDROID
//**Bro OS Android MUST BE DEFINED in the android makefile.
#ifndef 
#error "Operating System Error"
#endif
#elif __APPLE__
#ifdef TARGET_OS_IPHONE
#define BR2_OS_IOS 1
#endif
#else
#pragma error Error: Incorrect operating system, or undefined system.
#endif

//Needed for Filesystem
#ifdef BR2_OS_WINDOWS
typedef unsigned short mode_t;
#endif

//////////////////////////////////////////////////////////////////////////
// - Undefine to enable runtime debugging which enables 
//     RELEASE BUILD ONLY
//    GetLastError() and GlGetError() 
//    all over the place (HUGE PERFORMANCE HIT)
//#define _ENABLE_RUNTIME_DEBUG_CHECKS

//////////////////////////////////////////////////////////////////////////
// - Undefine to use the the BRO wrapper for std::allocator. 
//     This forces the STL to allocate all std classes such as std::vetor, std::map, etc
//   with the game's memory manager. Commenting this out the STL will allocate via the
//   operating system's memory.
//#define USE_BRO_STL_ALLOCATOR_WRAPPER

//////////////////////////////////////////////////////////////////////////
// - Comment/Uncomment to use system memory instead of the game memory manager.
#define _MM_USE_SYSTEM_MEMORY

//////////////////////////////////////////////////////////////////////////
// - MEMORY DEBUGGING
#ifdef _DEBUG
// - **VERY SLOW** Uncomment to make sure that blocks are refreenced/destroyed in their respective threads.
//#define _MM_DO_THREAD_DESTROY_ACCESS_CHECK
// - **NOT SHOW** Verify magic number at head of block (helps find overwrites)
#define _MM_DO_VERIFY
// - **FAIRLY SLOW** Enable this to check for block validity within threads.  Moderate performance hit
#define _MM_DEBUG_CHECK_INDIVIDUAL_BLOCKS
// - **VERY SLOW** Uncomment to enable access violation checking. This is WAY SLOW
//#define _MM_DEBUG_CHECK_BLOCKS
// - **VERY SLOW** Checks that the cache is sorted properly
//#define _MM_DEBUG_CHECK_SORT_ORDER
// - **VERY SLOW** If this is defined new blocks (actually, all blocks returned by get_block()) will be zeroed.  this helps find memory overwrites **VERY SLOW**
//#define _MM_DEBUG_ZERO_NEW_BLOCKS
// - **VERY SLOW** USe this to fill all blocks with memory access errors
//#define _MM_DEBUG_INVALIDATE_NEW_BLOCKS
// - **VERY SLOW** Check integrity of the heap
// THis is critical to ensure heap size has not changed. ** This has to add all blocks in the memory manager every time - twice - very slow **
//#define _MM_DEBUG_CHECK_HEAP_SIZE_INTEGRITY
#else
// - Uncomment this to verify the magic number of memory blocks.
#define _MM_DO_VERIFY
#endif

//////////////////////////////////////////////////////////////////////////
// - This constant is used in the master block list to reserve blocks .
// this is purely for performance.  Size is arbitrary
#define MM_MEMORY_BLOCK_STATIC_RESERVE_COUNT (0)

//////////////////////////////////////////////////////////////////////////
// Memory Management Caches.
//        USE BINARY SEARCH CACHE - USEs an O(log(n)) cache.
#define _MM_USE_MANUAL_BCACHE 1
//        Use STD::VECTOR FOR CACHE - Forces the memory manager to use a linear O(n) cache and operations rather than a binary searchable O(logn)cache.
//        Useful for debugging 
//#define _MM_USE_SLOW_CACHE 1
//        USE STD::MAP FOR CACHE - Uses the system defined std::map cache.
//#define _MM_USE_MAP_CACHE 1

//////////////////////////////////////////////////////////////////////////
//COORDINATE SYSTEM
//   Don't change this or it will screw the whole engine up.
//   RHS - looking down the positive z axis with the y axis pointing up, 
//   the X axis points to the left instead of the right.
#define COORDINATE_SYSTEM_RHS

//////////////////////////////////////////////////////////////////////////
// ANIMATION AND SKIN
#ifdef _DEBUG
// * Undefine this in order to  NOT copy the SPEC mesh to the BACK gpu buffer and thus 
// produce artifacts in the back buffer since all data is not copied on the GPU per frame.
// This inevitably shows whether there is an issue swapping buffers
//#define GFX_ANIMATION_TEST_GPU_BUFFER_SWAP

// - Uncomment to log some verbose buffer data to the logfile when skin is loaded
//#define DEBUG_LOG_VERBOSE_SKIN_DATA

// - Uncomment to test skins (dumps a ton of stuff to console)
//#define DEBUG_ANIMATION_SKIN_TEST
#endif

//////////////////////////////////////////////////////////////////////////
//IPHONE BUILD CONFIG
//Some OS flags.
#ifdef __IPHONEOS__
//#include "CGGeometry.h"
//#define FLT_MAX CGFLOAT_MAX
#endif

//////////////////////////////////////////////////////////////////////////
//WINDOWS BUILD CONFIG
//**NOTE: Security Checks are a performance pitfall.  You should disable them for maximum performance (but it's a security exploit unfortunately.)
//    https://preshing.com/20110807/the-cost-of-buffer-security-checks-in-visual-c/
//
//CL, annoying errors.  This must come before we include the actual windows stuff.
#ifdef _WIN32

//Disable annoying warnings.
//https://stackoverflow.com/questions/46916437/itoa-the-posix-name-for-this-item-is-deprecated
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

//So I should use Windows in place of a simple string conversion routine? Please.
//  warning C4996 : 'std::wstring_convert<convert_type,wchar_t,std::allocator<wchar_t>,std::allocator<char>>' : warning STL4017 : std::wbuffer_convert, std::wstring_convert, and the <codecvt>
//  header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. (The std::codecvt class template is NOT
//  deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> 
//  instead. You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

//disable wctombs_s
//#pragma warning(disable:4996)
//not all control paths return a value - must be an error.
#pragma warning(error:4715)
//'==': operator has no effect; did you intend '='?
//#pragma warning(error:4553)
//warning C4002: too many arguments for function-like macro invocation 'BroLogWarn'
#pragma warning(error:4002)
//recursive on all control paths
#pragma warning(error:4717)

//Winsock (old)
//#include <ws2tcpip.h>
//Reinclude if we can't get sdl net to work
//#include <winsock2.h>
//Note to link withws2_32.lib and wsock32.lib
#endif



//End guard
#endif


