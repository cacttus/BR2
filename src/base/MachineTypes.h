/**
*    @file machineTypes.h
*    @brief Type declarations for different architectures.
*
*    @desc MetalMario971
*    @date May 5, 2008
*
*    Update:
*        May 23, 2008    - added void
*        May 24, 2008    - added standard pointer
*        May 25, 2008    - added signed words
*        June 15, 2008    - added qwords
*        June 21, 2008    - added memmove
*        October 2, 2008 - added __int32, __int64
*        November 1, 2008 - re-added __int32???
*        November 22, 2008 - Added true, false
*         Februrary 29, 2011    - added t_char8
*         20140630 added double
*    20151115 fixed stl header include
*   082019 g'bye
*/
#pragma once
#ifndef __MACHINE__TYPES98467984679864954679654794673546__H__
#define __MACHINE__TYPES98467984679864954679654794673546__H__

#include "../base/GlobalIncludes.h"

namespace Game {

typedef std::string string_t;
typedef std::wstring wstring_t;

//#endif
#define memsize_max (size_t)(-1)
typedef unsigned char       t_byte;
typedef char                t_char;

typedef uint64_t t_timeval;

// - Max and min component sizes for integeral types.
// call COMP_MAX<Tx>::m();        to get max size
template < class Tx > class COMP_MAX;
template<> class COMP_MAX<int16_t> {public: static int16_t m(){ return SHRT_MAX; } };
template<> class COMP_MAX<int32_t> {public: static int32_t m(){ return INT_MAX; } };
template<> class COMP_MAX<float> {public: static float m(){ return FLT_MAX; } };

#define COMP_MAX(T) COMP_MAX<T>::m()
#define COMP_MIN(T) -COMP_MAX<T>::m()

#define TYPE_MIN(T) COMP_MIN(T)
#define TYPE_MAX(T) COMP_MAX(T)

}


#endif