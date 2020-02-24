/**
*  @file DebugHelper.h
*  @date January 30, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __DEBUGHELPER_280862031362642290319991_H__
#define __DEBUGHELPER_280862031362642290319991_H__

#include "../base/BaseHeader.h"
namespace BR2 {
//class StackMod {
//public:
//    t_string name;
//    size_t addr;
//};
///**
//*    @class StackInfo
//*    @brief Lists the calls in a procedure.
//*
//*/
//class StackInfo /* do not inherit.  Used on exception */ {
//    //    SymbolInfo si;
//    static t_string modList();
//    static t_string callStack(t_string);
//public:
//    static t_string getStackInfo();
//    static t_string getStackInfo2();
//
//};
class StackMod {
public:
  string_t name;
  size_t addr;
};
/**
*    @class DebugHelper
*    @brief    MSVC CRT implementation of heap check debugging
*        checks consistency across memory blocks and
*        shows memory leaks.  Memory leaks are configured to
*        output to a file in the bin directory called crt.log.
*    http://blogs.msdn.com/b/webdav_101/archive/2008/03/31/detecting-application-memory-leaks-in-unmanaged-c-with-4-lines-of-code.aspx
*/
class DebugHelper : public VirtualMemory {
public:
  DebugHelper();
  virtual ~DebugHelper() override;

  static string_t getStackTrace();
  static std::vector<std::string> getCallStack(bool bIncludeFrameId = false);
  static string_t getCallingMethod();
  static void debugHeapBegin(bool bDoDebug);
  static void debugHeapEnd();
  static void setBreakAlloc(int allocNum);
  static void breakPoint();
  static void breakPoint(bool expr);
  static void debugBreak();//Breaks only on debuf
  static void checkMemory();
  static void setCheckAlways();
  static void setCheck16();
private:
  static string_t modList();
};

}//ns game



#endif
