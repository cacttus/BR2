/**
*    @file TypeConv.hpp
*    @date August 23, 2010
*    @author Derek Page
*
*    � 2011 
* 5/1/2017 reformatted. removed inline. made nice
*/
#pragma once
#ifndef __TYPECONV_29189136032904720213337_H__
#define __TYPECONV_29189136032904720213337_H__

#include "../base/BaseHeader.h"
#include "../base/MachineTypes.h"
#include "../base/Exception.h"
#include "../base/TypeCast.h"
namespace Game {;

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
    STATIC t_byte strToByte(t_string& s) ;
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

//Some awesome string concat business
class _TemplateDummy {};
static FORCE_INLINE t_string tstr(_TemplateDummy t) { return t_string(""); }
static FORCE_INLINE t_string tstr(char t)
{
    char cc[2];
    cc[0] = t;
    cc[1] = 0;
    return t_string((char*)cc); 
}
// String Shorthand overloads.
STATIC FORCE_INLINE t_string tstr(const char* x){ return t_string(x); }
STATIC FORCE_INLINE t_string tstr(const t_string& x){ return x; }
STATIC FORCE_INLINE t_string tstr(const t_int16 x){ return TypeConv::intToStr((t_int16)x); }
STATIC FORCE_INLINE t_string tstr(const int32_t x){ return TypeConv::intToStr((int32_t)x); }
STATIC FORCE_INLINE t_string tstr(const t_int64 x) { return TypeConv::intToStr((t_int64)x); }
STATIC FORCE_INLINE t_string tstr(const t_uint64 x){ return TypeConv::intToStr((t_uint64)x); }
STATIC FORCE_INLINE t_string tstr(const t_uint32 x){ return TypeConv::intToStr((t_uint32)x); }
STATIC FORCE_INLINE t_string tstr(const float x){ return TypeConv::floatToStr((float)x); }
STATIC FORCE_INLINE t_string tstr(const double x){ return TypeConv::dblToStr((double)x); }

//static FORCE_INLINE std::string tstr(x) {
//    std::ostringstream stream;
//    stream << "Some Text";
//    std::string str = stream.str();
//    const char* chr = str.c_str();
//}

#ifdef BRO_OS_WINDOWS
//Unix freaks out.  I think because of size_t being integral.
static FORCE_INLINE t_string tstr(const t_long x){ return TypeConv::longToStr((t_long)x); }
//Unix freaks out.  I think because of size_t being integral.
//static FORCE_INLINE t_string tstr(const size_t x){ return sizetToStr((size_t)x); }
//static FORCE_INLINE t_string tstr(const void* x){ return TypeConv::intToStr((t_ptr)x); }
#endif


}//ns game

//#include "../base/TStr.h"
//#include "../base/TypeCast.h"

#endif

