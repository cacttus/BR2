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
    STATIC bool strToBool(string_t& s, bool bThrowIfInvalid = false);
    STATIC long strToLong(string_t& s);
    STATIC int32_t strToInt(string_t& s);
    STATIC bool strToInt(string_t& s, int32_t& out);//Similar to .net' parseInt
    STATIC uint32_t strToUint(string_t& s);
    STATIC int8_t strToByte(string_t& s) ;
    STATIC float strToFloat(string_t& s);
    STATIC double strToDouble(string_t& s);

    //Silent Fail Verions
    STATIC float strToFloat(string_t& s, bool bFailed);
    STATIC double strToDouble(string_t& s, bool bFailed);

    //ToStr
    STATIC string_t intToStr(const int32_t i);
    STATIC string_t intToStr(const int64_t i);
    STATIC string_t intToStr(const uint64_t i);
    STATIC string_t intToStr(const uint32_t i);
    STATIC string_t sizetToStr(const size_t i);
    STATIC string_t longToStr(const long& i);
    STATIC string_t ulongToStr(const unsigned long& i);
    STATIC string_t int64ToStr(const int64_t& i);
    STATIC string_t uint64ToStr(const uint64_t& i);
    STATIC string_t floatToStr(const float i);
    STATIC string_t dblToStr(const double i);
    STATIC string_t wstrToStr(wchar_t* wstr);

    //Format Verions
    STATIC string_t intToStr(const  int32_t i, char* fmt);
};


}//ns game

#endif

