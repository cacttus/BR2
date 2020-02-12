/**
*  @file OperatingSystem.h
*  @date November 27, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __OPERATINGSYSTEM_30289979527355844012131_H__
#define __OPERATINGSYSTEM_30289979527355844012131_H__

#include "../base/OperatingSystemHeader.h"
#include "../base/BaseHeader.h"

namespace Game {
/**
*  @class OperatingSystem
*  @brief Wraps OS kernel calls. Provides OS information. Put all OS methods in this class.
*/
class OperatingSystem : public VirtualMemory {
public:
  enum ErrorCode {
    //Note:Remember to add these codes to suppressError()
#ifdef BRO_OS_WINDOWS
    NoError = 0,
    FileNotFound = 2,
    PathNotFound = 3,
    InsufficientBuffer = ERROR_INSUFFICIENT_BUFFER,
    ProcNotFound = 127,
    NoGuidTranslation = 560,
#elif BRO_OS_ANDROID
    //TODO: fix this
    FileNotFound = 2,
    PathNotFound = 3,
    ProcNotFound = 127,
    NoGuidTranslation = 560,
#else

#endif
  };
  //  static RuntimeEnvironment getRuntimeEnvironment();// Returns windows/linux 32/64 environment.
  static string_t getRuntimeEnvironmentStr();// Returns windows/linux 32/64 environment.
 // static void suspendThread(uint32_t millis); // Suspends the currend thread this method is called in for the specified number of milliseconds.
//  static uint32_t getCurrentThreadId();
 // static ThreadHandle createThread(ThreadCallbackMethod myCallback, void* parameter=NULL, uint32_t* threadId=NULL);
 // static void terminateThread(ThreadHandle handle);
 // static OsWindowMessage processThreadMessages();    // Win32 message pump.
 // static OsWindowMessage processThreadMessagesIgnore();    // Win32 message pump.
//  static int64_t getTickCount64();
 // static size_t getAvailableMemory();
 // static size_t getProcessMemoryUsage();
 // static int32_t getError(); // gets the current operating system error code.
  //static void osed( int line, char* file, bool bThrow = false);    // Error debug.  Removed in release versions.
//   static void oser( int line, char* file, bool bThrow = false);    // Error debug.  Removed in release versions.
 //  static void clearAllErrors();
 //  static void suppressError(int error, const char* message, bool bWriteMessage=true); //Suppress the given operating system error code and write this message to the log.
 //  static void suppressError(int error, t_string& message, bool bWriteMessage=true); //Suppress the given operating system error code and write this message to the log.
 //  static void suppressError(ErrorCode, bool bWriteMessage=true); //Suppress the given operating system error code and write this message to the log.
 //  static void getScreenDims(Quad2f*); // - Get the window dimensions.
  static void showErrorDialog(string_t& str, string_t title = string_t("Error"));
  // static t_string getUserFolderPath();
  static string_t getGamesFolderPath();
  static void createDirectory(string_t& dir);
  static void showMouseCursor();
  static void hideMouseCursor();
  static bool getMouseCursorIsVisible();
  static int32_t getNumberOfProcessors();
  static bool isVistaOrGreater();
  static string_t  getOperatingSystemName();
  static void showConsole();
  static void hideConsole();
  static int strCaseCmp(const string_t& str1, const string_t& str2);
  //  static bool keyDown(int code);
};


}//ns game



#endif
