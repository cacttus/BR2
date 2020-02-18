/**
*  @file BaseHeader.h
*  @date November 12, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __BASEHEADER_1478973884162863251_H__
#define __BASEHEADER_1478973884162863251_H__

#include "../base/BaseDefines.h"
#include "../base/ConsoleColors.h"

namespace BR2 {

/************************************************************************/
/* String                                                               */
/************************************************************************/
//String
#ifdef USE_STD_STRING
typedef std::string string_t;
#else
#include "../base/StringWrapper.h"
#endif
typedef std::wstring wstring_t;

//Helper string since we can't cast char* to std::string automatically.
#define Stz std::string("")+
//String Extensions  See MathHeader for math string extensions.
std::string operator+(const std::string& str, const char& rhs);
std::string operator+(const std::string& str, const int8_t& rhs);
std::string operator+(const std::string& str, const int16_t& rhs);
std::string operator+(const std::string& str, const int32_t& rhs);
std::string operator+(const std::string& str, const int64_t& rhs);
std::string operator+(const std::string& str, const uint8_t& rhs);
std::string operator+(const std::string& str, const uint16_t& rhs);
std::string operator+(const std::string& str, const uint32_t& rhs);
std::string operator+(const std::string& str, const uint64_t& rhs);
std::string operator+(const std::string& str, const double& rhs);
std::string operator+(const std::string& str, const float& rhs);


/************************************************************************/
/* Exceptions & Errors                                                  */
/************************************************************************/
#define NotImplementedException() Exception("The method is not implemented.",__LINE__,__FILE__)
#define DeprecatedException() Exception("The method has been deprecated and removed.  Please see the method notes to use the correct replacement.",__LINE__,__FILE__)

void staticDebugBreak(string_t str);
void runtimeAssertion(string_t str);
#define TIME_T_MIN (0)
#ifdef _DEBUG
#define AssertOrThrow2(x) do{ if(!(x)) { staticDebugBreak(Stz "Debug Time Assertion: '" + #x + "'. "); } }while(0)
#else
#define AssertOrThrow2(x) do{ if(!(x)) { runtimeAssertion(Stz "Runtime Assertion: '" + #x + "'. "); } }while(0)
#endif

//GCC NOTE: GCC says allowing the use of an undeclared name is deprecated which would make us have to move TStr up 
//above BaseHeader.
//if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated
#define Br2ThrowException(x) throw new Exception(Stz x,__LINE__,__FILE__)
#define Br2ThrowNotImplementedException() throw new NotImplementedException()
#define BroThrowDeprecatedException() throw new DeprecatedException()
#define VerifyOrThrow(expr,x) do { if(!(expr)) Br2ThrowException(x); } while(0)
#define CheckGpuErrorsDbg() Gu::checkErrors()

#define ShowMessageBoxOnce(msg) { \
static bool __show=false; \
if(__show==false) { \
do{ \
Gu::showMessageBox(msg, "Error"); \
}while(0); \
}; \
__show=true; \
}


class GLFramework : public VirtualMemoryShared<GLFramework> {
public:
  GLFramework(std::shared_ptr<GLContext> ct) {
    _pContext = ct;
  }
  virtual ~GLFramework() override { _pContext = nullptr; }
  std::shared_ptr<GLContext> getContext() { return _pContext; }
private:
  std::shared_ptr<GLContext> _pContext = nullptr;
};

}//ns BR2



#endif






