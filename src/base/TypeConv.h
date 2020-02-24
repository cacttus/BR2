/**
*  @file TypeConv.hpp
*  @date August 23, 2010
*  @author MetalMario971
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
  static bool strToBool(t_string& s, bool bThrowIfInvalid = false);
  static long strToLong(t_string& s);
  static int32_t strToInt(t_string& s);
  static bool strToInt(t_string& s, int32_t& out);//Similar to .net' parseInt
  static uint32_t strToUint(t_string& s);
  static int8_t strToByte(t_string& s);
  static float strToFloat(t_string& s);
  static double strToDouble(t_string& s);

  //Silent Fail Verions
  static float strToFloat(t_string& s, bool bFailed);
  static double strToDouble(t_string& s, bool bFailed);

  //ToStr
  static t_string intToStr(const int32_t i);
  static t_string intToStr(const int64_t i);
  static t_string intToStr(const uint64_t i);
  static t_string intToStr(const uint32_t i);
  static t_string sizetToStr(const size_t i);
  static t_string longToStr(const long& i);
  static t_string ulongToStr(const unsigned long& i);
  static t_string int64ToStr(const int64_t& i);
  static t_string uint64ToStr(const uint64_t& i);
  static t_string floatToStr(const float i);
  static t_string dblToStr(const double i);
  static t_string wstrToStr(wchar_t* wstr);

  //Format Verions
  static t_string intToStr(const  int32_t i, char* fmt);
};


}//ns game

#endif

