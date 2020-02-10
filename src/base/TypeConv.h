/**
*  @file TypeConv.hpp
*  @date August 23, 2010
*  @author MetalMario971
* 5/1/2017 reformatted. removed inline. made nice
*/
#pragma once
#ifndef __TYPECONV_29189136032904720213337_H__
#define __TYPECONV_29189136032904720213337_H__

#include "../base/BaseHeader.h"
#include "../base/MachineTypes.h"
#include "../base/Exception.h"
#include "../base/TypeCast.h"
namespace Game {

#define frameStampToStr(x) uint64ToStr(x)

#ifndef FORCE_INLINE
#define FORCE_INLINE inline
#endif
    
class TypeConv {
public:
    //ToVal
    STATIC bool strToBool(t_string& s, bool bThrowIfInvalid = false);
    STATIC long strToLong(t_string& s);
    STATIC int32_t strToInt(t_string& s);
    STATIC bool strToInt(t_string& s, int32_t& out);//Similar to .net' parseInt
    STATIC uint32_t strToUint(t_string& s);
    STATIC int8_t strToByte(t_string& s) ;
    STATIC float strToFloat(t_string& s);
    STATIC double strToDouble(t_string& s);

    //Silent Fail Verions
    STATIC float strToFloat(t_string& s, bool bFailed);
    STATIC double strToDouble(t_string& s, bool bFailed);

    //ToStr
    STATIC t_string intToStr(const int32_t i);
    STATIC t_string intToStr(const int64_t i);
    STATIC t_string intToStr(const uint64_t i);
    STATIC t_string intToStr(const uint32_t i);
    STATIC t_string sizetToStr(const size_t i);
    STATIC t_string longToStr(const long& i);
    STATIC t_string ulongToStr(const unsigned long& i);
    STATIC t_string int64ToStr(const int64_t& i);
    STATIC t_string uint64ToStr(const uint64_t& i);
    STATIC t_string floatToStr(const float i);
    STATIC t_string dblToStr(const double i);
    STATIC t_string wstrToStr(wchar_t* wstr);

    //Format Verions
    STATIC t_string intToStr(const  int32_t i, char* fmt);
};


}//ns game

#endif

