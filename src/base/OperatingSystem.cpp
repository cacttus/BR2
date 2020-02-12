

#include "../base/BaseAll.h"
#include "../base/OperatingSystem.h"

#ifdef BRO_OS_WINDOWS
//For opening folder
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <commdlg.h>
#endif

namespace Game {
int32_t OperatingSystem::getNumberOfProcessors() {
#ifdef BRO_OS_WINDOWS
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return (int32_t)si.dwNumberOfProcessors;
#elif BRO_OS_ANDROID
  return android_getCpuCount();
#else
#error "Operating System Error"
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
#error "Operating System Error"
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
#error "Operating System Error"
#endif
}
bool OperatingSystem::getMouseCursorIsVisible() {
#ifdef BRO_OS_WINDOWS
  CURSORINFO ci;
  ci.cbSize = sizeof(ci);
  GetCursorInfo(&ci);
  return (ci.flags & CURSOR_SHOWING) > 0;
#else

#error "Operating System Error"
#endif
}
string_t  OperatingSystem::getOperatingSystemName() {
  string_t res;
  //#ifdef BRO_OS_WINDOWS
  //    OSVERSIONINFOEX vex;
  //    vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  //    GetVersionExA((OSVERSIONINFO*)&vex);
  //
  //   // CheckOsErrorsDbg();
  //
  //    if(vex.dwMajorVersion==6 && vex.dwMinorVersion==3)
  //    {
  //        res.append(" Windows 8.1");
  //    }
  //    else if(vex.dwMajorVersion==6 && vex.dwMinorVersion==2)
  //    {
  //        res.append(" Windows 8");
  //    }
  //    else if(vex.dwMajorVersion==6 && vex.dwMinorVersion==1)
  //    {
  //        res.append(" Windows 7");
  //    }
  //    else if(vex.dwMajorVersion==6 && vex.dwMinorVersion==0)
  //    {
  //        res.append(" Windows Vista");
  //    }
  //    else if(vex.dwMajorVersion==5 && vex.dwMinorVersion==2)
  //    {
  //        res.append(" Windows XP Pro 64 bit");
  //    }
  //    else if(vex.dwMajorVersion==5 && vex.dwMinorVersion==1)
  //    {
  //        res.append(" Windows XP");
  //    }
  //    else if(vex.dwMajorVersion==5 && vex.dwMinorVersion==0)
  //    {
  //        res.append(" Windows 2000");
  //    }
  //    else
  //    {
  //        res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
  //    }
  //
  //    if(vex.wServicePackMajor != 0)
  //        res += TStr(", Service Pack ",vex.wServicePackMajor,".",vex.wServicePackMinor);
  //    else
  //        res += TStr(", No service pack");
  //
  //    if( vex.wProductType== VER_NT_DOMAIN_CONTROLLER )
  //        res.append(", Domain Controller, note operating system may be incorrect as this is not supported");
  //    else if( vex.wProductType== VER_NT_SERVER )
  //        res.append(", Server, note operating system may be incorrect as this is not supported");
  //    else if( vex.wProductType== VER_NT_WORKSTATION )
  //        res.append(", Workstation");
  //
  //#else
  //    #error "Operating System Error"
  //#endif
  //    CheckOsErrorsDbg();
  return res;
}
void OperatingSystem::createDirectory(string_t& dir) {
  /*
      //S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
  //TODO: File Permissions and directory permissions.
  //Should be POSIX compliant
  //int ret;
  //
  //do
  //{
  //    ret = _mkdir(dirName.c_str());
  //    if(ret==-1)
  //        BroThrowException(TStr("Mkdir failed for directory ",dirName));
  //    if(ret==EEXIST)
  //    {
  //    }
  //    if(ret==ENOENT)
  //    {
  //        BroThrowException(TStr("Failed to make directory ",dirName," the parameters to mkdir did not create the full hierarchy before calling"));
  //    }
#ifdef BRO_OS_WINDOWS
    //SECURITY_ATTRIBUTES sc;
    //sc.nLength = sizeof(sc);
    //sc.bInheritHandle = false;
    //sc.lpSecurityDescriptor = NULL;

    BOOL b = CreateDirectory(dir.c_str(),NULL);
#else
    // - Open / Make directory
    if(opendir(dir.c_str())==NULL)
    {
        if(_mkdir(dir.c_str())==-1)
            throw new Exception("Could not make log directory.",__LINE__,__FILE__);
    }
    //use mkdir
#endif
    OperatingSystem::suppressError(ERROR_ALREADY_EXISTS,t_string(""),false);
    CheckOsErrorsDbg();
}
t_string OperatingSystem::getUserFolderPath()
{
    t_string ret;
    CheckOsErrorsDbg();
#ifdef BRO_OS_WINDOWS
    char lpstrPath[BRO_MAX_PATH];
    //TODO: Vista and beyond
    //SHGetKnownFolderPath
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, lpstrPath);

    ret = t_string(lpstrPath);

#elif BRO_OS_UNIX

    ret = t_string("/usr/");
#endif
    OperatingSystem::suppressError(1008,NULL,false);
    CheckOsErrorsDbg();
    return ret;*/
}
bool OperatingSystem::isVistaOrGreater() {
  bool ret = false;
#ifdef BRO_OS_WINDOWS
  //if( IsWindowsVistaOrGreater(void) )
  //    ret = true;
#endif
  return ret;
}
/**
*    @fn
*    @brief This is shit.
*    isn't used by anybody
*        use "My Games" instaed.
*    FOLDERID_Games doesn't work.
*/
string_t OperatingSystem::getGamesFolderPath() {

  string_t ret;
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
//void OperatingSystem::suppressError(int error, t_string& message,bool bWriteMessage)
//{
//    if(getError()==error)
//    {
//        clearAllErrors();
//        if(bWriteMessage)
//            BroLogInfo(message,__LINE__,__FILE__);
//    }
//}
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
#error "Operating System Error"
#endif
}
//void OperatingSystem::getScreenDims(Quad2f* ret)
//{
//
//#ifdef BRO_OS_WINDOWS
//
//    RECT screen;
//    HWND hwnd;
//    hwnd = GetDesktopWindow();
//    GetWindowRect(hwnd, &screen);
//    CheckOsErrorsDbg();
//    ret->_p0.x = 0.0;
//    ret->_p0.y = 0.0;
//    ret->_p1.x = (float)screen.right;
//    ret->_p1.y = (float)screen.bottom;
//#else
//    #error "Operating System Error"
//#endif
//    
//}
//void OperatingSystem::suppressError(int error, const char* message, bool bWriteMessage)
//{
//    t_string msg;
//    
//    //Do this before we construct string so we don't have further errors
//    if(getError()==error)
//        clearAllErrors();
//
//    if(message!=NULL)
//        msg.assign(message);
//
//    suppressError(error,t_string(msg),bWriteMessage);
//}
//void OperatingSystem::suppressError(ErrorCode ec, bool bWriteMessage)
//{
//    t_string strMsg="";
//#ifdef BRO_OS_WINDOWS
//    switch(ec)
//    {
//        case FileNotFound:
//            strMsg = "File Not Found";
//        case PathNotFound:
//            strMsg = "Path Not Found";
//        case NoGuidTranslation:
//            strMsg = "No GUID Translation for Security Audit";
//        case ProcNotFound:
//            strMsg = "Proc Not Found";
//    };
//#else
//    #error "Operating System Error"
//#endif
//    
//    suppressError(ec, strMsg, bWriteMessage);
//}
//void OperatingSystem::clearAllErrors()
//{
//#ifdef BRO_OS_WINDOWS
//    SetLastError(0);
//#else
//    throw new NotImplementedException();
//#endif
//}
/// - Runtime version of getting errors.
//void OperatingSystem::oser(int line, char* file, bool bThrow)
//{
//    int x=0;
//#ifdef BRO_OS_WINDOWS
//    while(x=getError()!=ErrorCode::NoError)
//    {
//#ifdef BRO_OS_WINDOWS
//        if(x==1)
//        {
//            clearAllErrors();
//            continue;    //Windows error 1 for some reason pollutes the window.
//        }
//#endif
//        if(x==122)
//        {
//            int nn=0;
//            nn++;
//        }
//        clearAllErrors();
//        BroLogError(TStr("OS ERROR ",x, " Line: ", line, ", File: ", file));
//        //throw new Exception(TStr(" [OS] Operating system error: ",x),__LINE__,__FILE__);
//    }
//#else
//    #error "Operating System Error"
//#endif
//}
//void OperatingSystem::osed(int line, char* file, bool bThrow)
//{
//#ifdef _DEBUG
//    SystemErrHandler::oser(line, file, bThrow);
//#endif
//}
//int32_t OperatingSystem::getError()
//{
//#ifdef BRO_OS_WINDOWS
//    return GetLastError();
//#else
//    throw new NotImplementedException();
//#endif
//}

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
//    #error "Operating System Error"
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
//    #error "Operating System Error"
//#endif
//}
//void OperatingSystem::terminateThread(ThreadHandle handle)
//{
//#ifdef BRO_OS_WINDOWS
//    TerminateThread(handle,0);
//#else
//    #error "Operating System Error"
//#endif
//    CheckOsErrorsDbg();
//}
//int64_t Gu::getMilliSeconds()
//{
//    int64_t ret;
//    std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
//    ret = std::chrono::duration_cast<std::chrono::milliseconds>(ns).count();
//    return ret;
//}
//OsWindowMessage OperatingSystem::processThreadMessages()
//{
//    OsWindowMessage osmsg = OS_MSG_NONE;
//
//#ifdef BRO_OS_WINDOWS
//    MSG msg;
//    BOOL bHasMessage;
//    bHasMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
//    CheckOsErrorsDbg();
//    
//    while(bHasMessage)
//    {
//        if(msg.message == WM_QUIT)
//        {
//            return OS_MSG_QUIT;
//        } 
//        else  
//        {
//            TranslateMessage( &msg );
//            CheckOsErrorsDbg();
//            DispatchMessage( &msg );
//            //see http://stackoverflow.com/questions/8750899/invalid-window-handle-after-getmessage-loop
//            //OperatingSystem::suppressError(1400,TStr("DispatchMessage Returned invalid window handle.  Suppressing the error."),true);
//            CheckOsErrorsDbg();
//        }
//        bHasMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
//        CheckOsErrorsDbg();
//    }
//#else
//    #error "Operating System Error"
//#endif
//    return osmsg;
//}
//OsWindowMessage OperatingSystem::processThreadMessagesIgnore()
//{
//    OsWindowMessage osmsg = OS_MSG_NONE;
//
//#ifdef BRO_OS_WINDOWS
//    MSG msg;
//    while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
//    {
//        TranslateMessage( &msg );
//        DispatchMessage( &msg );
//    }
//#else
//    #error "Operating System Error"
//#endif
//    CheckOsErrorsDbg();
//    return osmsg;
//}
//ThreadHandle OperatingSystem::createThread(ThreadCallbackMethod myCallback, void* parameter, uint32_t* threadId)
//{
//    ThreadHandle ret;
//#ifdef BRO_OS_WINDOWS
//     ret = CreateThread(
//    NULL,
//    NULL,
//    myCallback,
//    parameter,
//    NULL,
//    (LPDWORD)threadId);
//
//    CheckOsErrorsDbg();
//#else
//    #error "Operating System Error"
//#endif
//    return ret;
//}
//uint32_t OperatingSystem::getCurrentThreadId()
//{
//    uint32_t threadId;
//#ifdef BRO_OS_WINDOWS
//    //TODO: std::this_thread::get_id()
//    threadId = (uint32_t) GetCurrentThreadId();
//    OperatingSystem::osed(__LINE__, __FILE__);
//#else
//    #error "Operating System Error"
//#endif
//    return threadId;
//}
//RuntimeEnvironment OperatingSystem::getRuntimeEnvironment()
//{
//#ifdef _WIN32 
//    return RuntimeEnvironment::RE_WIN32;
//#elif _WIN64
//    return RuntimeEnvironment::RE_WIN64;
//#elif defined(__LP32__) || defined(_LP32)
//    return RuntimeEnvironment::RE_LINUX32;
//#elif defined(__LP64__) || defined(_LP64)
//    return RuntimeEnvironment::RE_LINUX64;
//#else
//    throw new NotImplementedException();
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
//void OperatingSystem::suspendThread(uint32_t millis)
//{
//#ifdef BRO_OS_WINDOWS
//    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
//    CheckOsErrorsDbg();
//#else
//    #error "Operating System Error"
//#endif
//}
//void OperatingSystem::debugBreak()
//{
//#ifdef BRO_OS_WINDOWS
//#ifdef BRO_X86
//    __asm {
//        int 3
//    }
//#else
//    __debugbreak();
//#endif
//    //or DebugBreak()
//#else
//    // I'm not sure if unix uses inline asm.  
//    // int 3 - interrupt 3 = compiler debug interrrupt
//    __asm { int 3 }
//#endif
//}
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
#error "Operating System Error"
#endif
}
void OperatingSystem::hideConsole() {
#ifdef BRO_OS_WINDOWS
  ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
#error "Operating System Error"
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
#endif

  return "";
}

string_t OperatingSystem::showOpenFileDialog(string_t title, string_t filter, string_t defaultext, string_t basePath) {
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
  string_t converted = StringUtil::wStrToStr(openFileNameReturnString);
  return converted;
}
//bool OperatingSystem::keyDown(int code)
//{
//#ifdef BRO_OS_WINDOWS
//    return GetAsyncKeyState(code) & 0x8000;
//#else
//    #error "Operating System Error"
//#endif
//}

}//ns game
