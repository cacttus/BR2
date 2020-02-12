

#include "../base/BaseAll.h"
#include "../base/OperatingSystem.h"

#ifdef BRO_OS_WINDOWS
//For opening folder
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <commdlg.h>
#endif

namespace BR2 {

int32_t OperatingSystem::getNumberOfProcessors() {
#ifdef BRO_OS_WINDOWS
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return (int32_t)si.dwNumberOfProcessors;
#elif BRO_OS_ANDROID
  return android_getCpuCount();
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::showMouseCursor() {
#ifdef BRO_OS_WINDOWS
  CURSORINFO ci;
  ci.cbSize = sizeof(CURSORINFO);
  int nFailover = 10000;
  do {
    GetCursorInfo(&ci);
    ShowCursor(true); //ShowCursor is not a boolean method - it increments/decrements a display counter
    nFailover--;
  } while ((ci.flags & CURSOR_SHOWING) == 0 && (nFailover > 0));
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::hideMouseCursor() {
#ifdef BRO_OS_WINDOWS
  CURSORINFO ci;
  ci.cbSize = sizeof(CURSORINFO);
  int nFailover = 10000;
  do {
    GetCursorInfo(&ci);
    ShowCursor(false);//ShowCursor is not a boolean method - it increments/decrements a display counter
    nFailover--;
  } while ((ci.flags & CURSOR_SHOWING) > 0 && (nFailover > 0));
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
bool OperatingSystem::getMouseCursorIsVisible() {
  bool ret = false;
#ifdef BRO_OS_WINDOWS
  CURSORINFO ci;
  ci.cbSize = sizeof(ci);
  GetCursorInfo(&ci);
  ret = (ci.flags & CURSOR_SHOWING) > 0;
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
    return ret;
}
string_t  OperatingSystem::getOperatingSystemName() {
  string_t res;
#ifdef BRO_OS_WINDOWS
  OSVERSIONINFOEX vex;
  vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionExW((OSVERSIONINFO*)&vex);

  if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 3) {
    res.append(" Windows 8.1");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 2) {
    res.append(" Windows 8");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 1) {
    res.append(" Windows 7");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 0) {
    res.append(" Windows Vista");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 2) {
    res.append(" Windows XP Pro 64 bit");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 1) {
    res.append(" Windows XP");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 0) {
    res.append(" Windows 2000");
  }
  else {
    res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
  }

  if (vex.wServicePackMajor != 0) {
    res += Stz ", Service Pack ", vex.wServicePackMajor, ".", vex.wServicePackMinor;
  }
  else {
    res += Stz ", No service pack";
  }

  if (vex.wProductType == VER_NT_DOMAIN_CONTROLLER) {
    res.append(", Domain Controller, note operating system may be incorrect as this is not supported");
  }
  else if (vex.wProductType == VER_NT_SERVER) {
    res.append(", Server, note operating system may be incorrect as this is not supported");
  }
  else if (vex.wProductType == VER_NT_WORKSTATION) {
    res.append(", Workstation");
  }

#else
  res = "Operating system name not available.";
  OS_METHOD_NOT_IMPLEMENTED
#endif
    return res;
}

string_t OperatingSystem::getUserFolderPath() {
  //returns "My Documents" on windows.
  //Linux .. ?
  string_t ret;
#ifdef BRO_OS_WINDOWS
  char lpstrPath[BRO_MAX_PATH];
  //TODO: Vista and beyond
  //SHGetKnownFolderPath
  HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, lpstrPath);

  ret = string_t(lpstrPath);

#elif BRO_OS_LINUX
  ret = t_string("/usr/");
#endif
  OperatingSystem::suppressError(ErrorNoToken, false);
  return ret;
}
string_t OperatingSystem::getGamesFolderPath() {
  // * Apparently, FOLDERID_GAMES doesn't work.
  string_t ret = "";
  /*
 // CheckOsErrorsDbg();
#ifdef BRO_OS_WINDOWS
    PWSTR lpwstrPath = NULL;
    HRESULT result;

    // Win Vista and beyond ONLY
    result = SHGetKnownFolderPath(FOLDERID_SavedGames,KF_FLAG_DONT_UNEXPAND|KF_FLAG_CREATE,NULL,&lpwstrPath);
    if(result == E_FAIL)
        BroThrowException("Folder could not be found.");
    ret = wstrToStr(lpwstrPath);

    CoTaskMemFree(static_cast<void*>(lpwstrPath));

#elif BRO_OS_UNIX

    ret = t_string("/usr/games/");
#endif
    CheckOsErrorsDbg();*/
  return ret;
}
void OperatingSystem::suppressError(OSErrorCode ec, bool bWriteMessage) {
  string_t strMsg = "";
#ifdef BRO_OS_WINDOWS
  switch (ec) {
  case OSErrorCode::FileNotFound:
    strMsg = "File Not Found";
  case OSErrorCode::PathNotFound:
    strMsg = "Path Not Found";
  case OSErrorCode::NoGuidTranslation:
    strMsg = "No GUID Translation for Security Audit";
  case OSErrorCode::ProcNotFound:
    strMsg = "Proc Not Found";
  default:
    strMsg = "Failed to register operating system error code. Code was not defined in the engine.";
  };
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif

    if (getError() == (int)ec) {
      clearAllErrors();
      if (bWriteMessage) {
        BroLogInfo(strMsg);
      }
    }
}
void OperatingSystem::showErrorDialog(string_t& str, string_t title) {
#ifdef BRO_OS_WINDOWS
  string_t dialogPath =
    FileSystem::combinePath(
      FileSystem::getExecutableDirectory(), string_t("WindowsErrorDialog.exe")
    );

  if (FileSystem::fileExists(dialogPath)) {
    string_t logDir = Gu::getLogger()->getLogPath();//FileSystem::getLogDirectory();
    string_t dialogExeCommand;
    dialogExeCommand = dialogPath;

    title = StringUtil::replaceAll(title, "\n", "\\n");
    str = StringUtil::replaceAll(str, "\n", "\\n");
    logDir = StringUtil::replaceAll(logDir, "\n", "\\n");

    title = StringUtil::replaceAll(title, "\r", "\\r");
    str = StringUtil::replaceAll(str, "\r", "\\r");
    logDir = StringUtil::replaceAll(logDir, "\r", "\\r");


    title = StringUtil::replaceAll(title, "\"", "\"\"\"");
    str = StringUtil::replaceAll(str, "\"", "\"\"\"");
    logDir = StringUtil::replaceAll(logDir, "\"", "\"\"\"");


    dialogExeCommand += Stz " \"" + title + "\"";
    dialogExeCommand += Stz " \"" + str + "\"";
    dialogExeCommand += Stz " \"" + logDir + "\"";
    system(dialogExeCommand.c_str());
  }
  else {
    MessageBoxA(NULL, str.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_OK);
  }
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::clearAllErrors() {
#ifdef BRO_OS_WINDOWS
  SetLastError(0);
#else
  throw new NotImplementedException();
#endif
}
int32_t OperatingSystem::getError() {
#ifdef BRO_OS_WINDOWS
  return GetLastError();
#else
  throw new NotImplementedException();
#endif
}

//size_t OperatingSystem::getAvailableMemory()
//{
//#ifdef BRO_OS_WINDOWS
//    MEMORYSTATUSEX mmex;
//    mmex.dwLength = sizeof(mmex);
//
//    GlobalMemoryStatusEx(&mmex);
//  //  CheckOsErrorsDbg();
//    return mmex.ullAvailPhys;
//#else
//   OS_METHOD_NOT_IMPLEMENTED
//#endif
//}
//size_t OperatingSystem::getProcessMemoryUsage()
//{
//    
//#ifdef BRO_OS_WINDOWS
//    PROCESS_MEMORY_COUNTERS pmex;
//    GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmex, sizeof(PROCESS_MEMORY_COUNTERS) );
//
//    return pmex.WorkingSetSize;
//#else
//  OS_METHOD_NOT_IMPLEMENTED
//#endif
//}

string_t OperatingSystem::getRuntimeEnvironmentStr() {
#ifdef _WIN32 
  return string_t("Windows 32 Bit");
#elif _WIN64
  return string_t("Windows 64 Bit");
#elif defined(__LP32__) || defined(_LP32)
  return string_t("Linux 32 Bit");
#elif defined(__LP64__) || defined(_LP64)
  return string_t("Linux 64 Bit");
#else
  throw new NotImplementedException();
#endif
}
int OperatingSystem::strCaseCmp(const string_t& str1, const string_t& str2) {
#ifdef BRO_OS_WINDOWS
  //win32 only
  return _stricmp(str1.c_str(), str2.c_str());
#else
  //POSIX standard
  return strcasecmp(str1.c_str(), str2.c_str());
#endif
}
void OperatingSystem::showConsole() {
#ifdef BRO_OS_WINDOWS
  ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::hideConsole() {
#ifdef BRO_OS_WINDOWS
  ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
string_t OperatingSystem::showOpenFolderDialog(string_t saved_path) {
#ifdef BRO_OS_WINDOWS
  WCHAR path[MAX_PATH];

  const char* path_param = saved_path.c_str();

  BROWSEINFO bi = { 0 };
  bi.lpszTitle = L"Browse for folder...";
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
  bi.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED) {
      std::string tmp = (const char*)lpData;
      std::cout << "path: " << tmp << std::endl;
      SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
  };
  bi.lParam = (LPARAM)path_param;

  LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

  if (pidl != 0) {
    //get the name of the folder and put it in path
    SHGetPathFromIDList(pidl, path);

    //free memory used
    IMalloc* imalloc = 0;
    if (SUCCEEDED(SHGetMalloc(&imalloc))) {
      imalloc->Free(pidl);
      imalloc->Release();
    }

    return StringUtil::wStrToStr(std::wstring(path));
  }
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif

    return "";
}
string_t OperatingSystem::showOpenFileDialog(string_t title, string_t filter, string_t defaultext, string_t basePath) {
  string_t file = "";
#ifdef BRO_OS_WINDOWS
  OPENFILENAMEW ofn = { 0 };
  WCHAR openFileNameReturnString[MAX_PATH];	// the filename will go here from the openfile dialog
  ZeroMemory(openFileNameReturnString, MAX_PATH);

  ofn.lStructSize = sizeof(OPENFILENAMEA);
  ofn.lpstrFileTitle = openFileNameReturnString;
  ofn.nMaxFileTitle = MAX_PATH;
  ofn.lpstrTitle = title.length() ? StringUtil::strToWStr(title).c_str() : 0;
  ofn.lpstrFilter = filter.length() ? StringUtil::strToWStr(filter).c_str() : 0;//"COLLADA Files (*.DAE)\0*.DAE\0\0";
  ofn.lpstrInitialDir = basePath.length() ? StringUtil::strToWStr(basePath).c_str() : 0;
  ofn.lpstrDefExt = defaultext.length() ? StringUtil::strToWStr(defaultext).c_str() : 0;//"DAE";
  ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

  if (!GetOpenFileNameW(&ofn)) {
    return "";
  }
  file = StringUtil::wStrToStr(openFileNameReturnString);
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
    return file;
}

}//ns game
