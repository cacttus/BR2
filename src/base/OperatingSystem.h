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

namespace BR2 {
/**
*  @class OperatingSystem
*  @brief Wraps OS kernel calls. Provides OS information. Put all OS methods in this class.
*/
class OperatingSystem : public VirtualMemory {
public:
  static string_t getRuntimeEnvironmentStr();
  static int32_t getError();
  static void clearAllErrors();
  static void suppressError(OSErrorCode, bool bWriteMessage = true);
  static void showErrorDialog(string_t& str, string_t title = string_t("Error"));
  static string_t getUserFolderPath();
  static string_t getGamesFolderPath();
  static void showMouseCursor();
  static void hideMouseCursor();
  static bool getMouseCursorIsVisible();
  static int32_t getNumberOfProcessors();
  static string_t  getOperatingSystemName();
  static void showConsole();
  static void hideConsole();
  static int strCaseCmp(const string_t& str1, const string_t& str2);
  static string_t showOpenFolderDialog(string_t baseFolder);
  static string_t showOpenFileDialog(string_t title, string_t filter, string_t defaultext, string_t basePath);
  // static size_t getAvailableMemory();
// static size_t getProcessMemoryUsage();
};


}//ns BR2



#endif
