/**
*  @file machineTypes.h
*  @brief Type declarations for different architectures.
*/
#pragma once
#ifndef __MACHINE__TYPES98467984679864954679654794673546__H__
#define __MACHINE__TYPES98467984679864954679654794673546__H__

#include "../base/GlobalIncludes.h"

namespace BR2 {

typedef std::string t_string;

//#endif
#define memsize_max (size_t)(-1)
typedef unsigned char       t_byte;
typedef char                t_char;

typedef uint64_t t_timeval;

// - Max and min component sizes for integeral types.
// call COMP_MAX<Tx>::m();        to get max size
template < class Tx > class COMP_MAX;
template<> class COMP_MAX<int16_t> { public: static int16_t m() { return SHRT_MAX; } };
template<> class COMP_MAX<int32_t> { public: static int32_t m() { return INT_MAX; } };
template<> class COMP_MAX<float> { public: static float m() { return FLT_MAX; } };

#define COMP_MAX(T) COMP_MAX<T>::m()
#define COMP_MIN(T) -COMP_MAX<T>::m()

#define TYPE_MIN(T) COMP_MIN(T)
#define TYPE_MAX(T) COMP_MAX(T)

}


#endif