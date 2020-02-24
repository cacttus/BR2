#include "../base/DebugHelper.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../base/StringUtil.h"
#include "../base/WindowsIncludes.h"


#include <iostream>
#ifdef BR2_OS_WINDOWS
#include <DbgHelp.h>
#include <TlHelp32.h>
#endif


namespace BR2 {
HANDLE hCrtLog;
bool _bDoDebug = false;

void staticDebugBreak(t_string str) {
  BRLogError(str);
  Gu::debugBreak();
}
void runtimeAssertion(t_string str) {
  BRThrowException(str);
}

DebugHelper::DebugHelper() {
}
DebugHelper::~DebugHelper() {
}
void DebugHelper::breakPoint() {
  Gu::debugBreak();
}
void DebugHelper::breakPoint(bool expr) {
#ifdef _DEBUG
  if (expr) {
    //Gu
    //AsmCpuDebugBreak();
  }

#endif
}
void DebugHelper::debugBreak() {
#ifdef _DEBUG
  //  DebugValidate(NULL);    // asm int 3
#endif
}
void DebugHelper::setCheckAlways() {
  std::cout << ("****SETTING CHECK ALWAYS***") << std::endl;;
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  //clear
  flags &= ~_CRTDBG_CHECK_ALWAYS_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_16_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_128_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_1024_DF;
  //set
  flags |= _CRTDBG_CHECK_ALWAYS_DF;
  _CrtSetDbgFlag(flags);
}
void DebugHelper::setCheck16() {
  std::cout << ("****SETTING CHECK 16***") << std::endl;;
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  //clear
  flags &= ~_CRTDBG_CHECK_ALWAYS_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_16_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_128_DF;
  flags &= ~_CRTDBG_CHECK_EVERY_1024_DF;
  //set
  flags |= _CRTDBG_CHECK_EVERY_16_DF;
  _CrtSetDbgFlag(flags);
}
void DebugHelper::debugHeapBegin(bool bDoDebug) {
  _bDoDebug = bDoDebug;
  if (_bDoDebug == false)
    return;

#ifdef _DEBUG
#ifdef BR2_OS_WINDOWS

  //This chekcs every block for problems
#define VERBOSITY 
    //every 16 blocks
    // 
    //
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  //https://msdn.microsoft.com/en-us/library/5at7yxcs.aspx
  //flags |= _CRTDBG_CHECK_ALWAYS_DF;
  //flags |= _CRTDBG_CHECK_CRT_DF;
  flags |= _CRTDBG_CHECK_EVERY_16_DF;
  flags |= _CRTDBG_ALLOC_MEM_DF;
  flags |= _CRTDBG_LEAK_CHECK_DF;
  flags |= _CRTDBG_CHECK_CRT_DF;
  // flags |= _CRTDBG_DELAY_FREE_MEM_DF;//Don't actually free blocks.

  _CrtSetDbgFlag(flags);
  hCrtLog = CreateFile(TEXT("crt.log"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  SetLastError(0);
  if (hCrtLog != INVALID_HANDLE_VALUE) {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hCrtLog);
    _CrtSetReportFile(_CRT_ERROR, hCrtLog);
  }
  else {
    //_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    //_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
  }
#else
#error "Operating System Error"
#endif
#endif
}
void DebugHelper::setBreakAlloc(int allocNum) {
  if (_bDoDebug == false) {
    return;
  }
  _CrtSetBreakAlloc(allocNum);
}

void DebugHelper::debugHeapEnd() {
  if (_bDoDebug == false) {
    return;
  }
#ifdef _DEBUG
#ifdef BR2_OS_WINDOWS
  _CrtCheckMemory();
  //LEAK_CHECK_DF flag will call this 
  _CrtDumpMemoryLeaks();

  //http://avid-insight.co.uk/2008/02/tracing-memory-leaks-in-c-microsoft-specific/

  if (hCrtLog != INVALID_HANDLE_VALUE) {
    CloseHandle(hCrtLog);
  }
#else
#error "Operating System Error"
#endif
#endif
}
void DebugHelper::checkMemory() {
#ifdef _DEBUG
#ifdef BR2_OS_WINDOWS
  _CrtCheckMemory();
#else
#error "Operating System Error"
#endif
#endif
}

t_string DebugHelper::modList() {
  t_string ret = "";
#ifdef BR2_OS_WINDOWS

  MODULEENTRY32 me32;
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  DWORD procid = GetCurrentProcessId();
  hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procid);

  // - Get gay snapshot.
  if (hModuleSnap == INVALID_HANDLE_VALUE) {
    ret = "Failed to get module snapshot.";
    return ret;
  }

  me32.dwSize = sizeof(MODULEENTRY32);

  // - Get first module
  if (!Module32First(hModuleSnap, &me32)) {
    ret = "Failed to get the module for call stack analysis.";
    CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
    return ret;
  }

  t_string app = "";
  // - Loop through all modules.
  do {
    ret += app;
    //TDOO: check this for errors 8/11/19
    std::wstring ws = std::wstring(me32.szModule);
    ret += StringUtil::wStrToStr(ws);
    app = ",";
    //me32.
  } while (Module32Next(hModuleSnap, &me32));

  // - Close up shop.
  CloseHandle(hModuleSnap);

#else
  ret = "No module support for OS.";
#endif
  return ret;
}
std::vector<std::string> DebugHelper::getCallStack(bool bIncludeFrameId) {
  std::vector<std::string> callStack;
#ifdef BR2_OS_WINDOWS
  //Code copied from Msdn.
  uint32_t i;
  void* stack[512];
  unsigned short frames;
  SYMBOL_INFO* symbol;
  HANDLE process;

  process = GetCurrentProcess();
  SymInitialize(process, NULL, TRUE);
  frames = CaptureStackBackTrace(0, 100, stack, NULL);
  symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
  symbol->MaxNameLen = 255;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

  for (i = 0; i < frames; i++) {
    SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

    if (bIncludeFrameId) {
      callStack.push_back(StringUtil::format("[%i] %s", frames - i - 1, symbol->Name));
    }
    else {
      callStack.push_back(StringUtil::format("%s", symbol->Name));
    }
  }

  free(symbol);
#else
  ret = "No Stack Info for current OS.  Derek PUT IT IN.";
#endif
  return callStack;
}
t_string DebugHelper::getStackTrace() {
  //How to keep debug info in release builds:
  //*https://msdn.microsoft.com/en-us/library/fsk896zz.aspx
  t_string ret = "";
  std::vector<std::string> cs = getCallStack(true);
  for (std::string s : cs) {
    ret.append(s);
    ret.append("\n");
  }
  return ret;
}
t_string DebugHelper::getCallingMethod() {
  //Returns the method that called the current one.
  std::vector<std::string> str = getCallStack(false);
  if (str.size() < 4) {
    return "";
  }
  else {
    return str[3];
  }

}



}//ns game
