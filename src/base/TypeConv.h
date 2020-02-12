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
namespace BR2 {

#define frameStampToStr(x) uint64ToStr(x)

#ifndef FORCE_INLINE
#define FORCE_INLINE inline
#endif
    
class TypeConv {
public:
    //ToVal
    static bool strToBool(string_t& s, bool bThrowIfInvalid = false);
    static long strToLong(string_t& s);
    static int32_t strToInt(string_t& s);
    static bool strToInt(string_t& s, int32_t& out);//Similar to .net' parseInt
    static uint32_t strToUint(string_t& s);
    static int8_t strToByte(string_t& s) ;
    static float strToFloat(string_t& s);
    static double strToDouble(string_t& s);

    //Silent Fail Verions
    static float strToFloat(string_t& s, bool bFailed);
    static double strToDouble(string_t& s, bool bFailed);

    //ToStr
    static string_t intToStr(const int32_t i);
    static string_t intToStr(const int64_t i);
    static string_t intToStr(const uint64_t i);
    static string_t intToStr(const uint32_t i);
    static string_t sizetToStr(const size_t i);
    static string_t longToStr(const long& i);
    static string_t ulongToStr(const unsigned long& i);
    static string_t int64ToStr(const int64_t& i);
    static string_t uint64ToStr(const uint64_t& i);
    static string_t floatToStr(const float i);
    static string_t dblToStr(const double i);
    static string_t wstrToStr(wchar_t* wstr);

    //Format Verions
    static string_t intToStr(const  int32_t i, char* fmt);
};


}//ns game

#endif

