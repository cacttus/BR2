//#ifdef _DEBUG
#include "../base/TypeConv.h"
#include "../base/StringUtil.h"

#include "../base/Logger.h"
//#endif
namespace Game {
//Explicit versions
bool TypeConv::strToBool(t_string& s1, bool bThrowIfInvalid) {
  t_string s = StringUtil::lowercase(s1);

  if (StringUtil::equals(s, "1"))
    return true;
  else if (StringUtil::equals(s, "0"))
    return false;
  else if (StringUtil::equals(s, "true"))
    return true;
  else if (StringUtil::equals(s, "false"))
    return false;
  else if (StringUtil::equals(s, "t"))
    return true;
  else if (StringUtil::equals(s, "f"))
    return false;
  else if (StringUtil::equals(s, "y"))
    return true;
  else if (StringUtil::equals(s, "n"))
    return false;
  else if (StringUtil::equals(s, "yes"))
    return true;
  else if (StringUtil::equals(s, "no"))
    return false;

  if (bThrowIfInvalid) {
    BRThrowException("Invalid boolean cast, tried to cast " + s1 + " to boolean.");
  }

  return false;
}
long TypeConv::strToLong(t_string& s) {
  long ret;
  try {
    ret = std::stol(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToLong: Invalid parameter" + s + ". ex: " + ia.what());

    throw ia;
  }
  return ret;
}
bool TypeConv::strToInt(t_string& s, int32_t& out) {
  try {
    out = std::stoi(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToInt: Invalid parameter" + s + ". ex: " + ia.what());
    return false;
  }
  return true;
}
int32_t TypeConv::strToInt(t_string& s) {
  int32_t ret;
  try {
    ret = std::stoi(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToInt: Invalid argument '" + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
float TypeConv::strToFloat(t_string& s) {
  float ret;
  try {
    ret = std::stof(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToFloat: Invalid argument '" + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
uint32_t TypeConv::strToUint(t_string& s) {
  uint32_t ret;
  try {
    ret = (uint32_t)std::stoi(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToUint: Invalid argument " + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
double TypeConv::strToDouble(t_string& s) {
  double ret;
  try {
    ret = std::stod(s);
  }
  catch (const std::invalid_argument & ia) {
    BRLogError("strToDouble: Invalid argument '" + s + "'.");

    throw ia;
    return 0;
  }
  return ret;
}

int8_t TypeConv::strToByte(t_string& s) {
  int32_t i = strToInt(s);
  return (t_byte)i;
}

//int32_t strToInt( t_string& s, bool bFailed ){
//    char* endPtr;
//    int32_t ret = (int32_t)strtol(s.c_str(), &endPtr, 10);
//    bFailed = (bool)(*endPtr!='\0');
//    return ret;
//}

//////////////////////////////////////////////////////////////////////////
//Silent Fail Versions
float TypeConv::strToFloat(t_string& s, bool bFailed) {
  char* pend;
  float ret = (float)strtod(s.c_str(), &pend);
  bFailed = (bool)(*pend != '\0');
  return ret;
}
double TypeConv::strToDouble(t_string& s, bool bFailed) {
  char* pend;
  double ret = strtod(s.c_str(), &pend);
  bFailed = (bool)(*pend != '\0');
  return ret;
}

//////////////////////////////////////////////////////////////////////////
//ToStr
t_string TypeConv::intToStr(const int32_t i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::intToStr(const uint32_t i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::intToStr(const int64_t i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::intToStr(const uint64_t i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::TypeConv::floatToStr(const float i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::dblToStr(const double i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::sizetToStr(const size_t i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::longToStr(const long& i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::ulongToStr(const unsigned long& i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::int64ToStr(const int64_t& i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}
t_string TypeConv::uint64ToStr(const uint64_t& i) {
  t_string ret = t_string(std::to_string(i));
  return ret;
}


t_string TypeConv::wstrToStr(wchar_t* wstr) {
  t_string ret;

  char str[BRO_MAX_PATH];
  wcstombs(str, wstr, BRO_MAX_PATH);
  ret.assign(str);

  return ret;
}

//////////////////////////////////////////////////////////////////////////
//Format Versions
//t_string TypeConv::uintToStr(const  uint32_t i) {
//    uint32_t cpy = i;
//    int8_t buf[32];
//    //_itoa_s( cpy, (char*)buf, 32, 10 );
//    snprintf((char*)buf, 32, "%u", i);
//    return t_string((char*)buf);
//}

t_string TypeConv::intToStr(const  int32_t i, char* fmt) {
  int32_t cpy = i;
  char buf[32];
  snprintf((char*)buf, 32, fmt, i);

  return t_string((char*)buf);
}



}