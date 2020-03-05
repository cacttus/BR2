#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../base/GraphicsWindow.h"
#include "../base/GLContext.h"
#include "../base/FpsMeter.h"
#include "../base/ConsoleColors.h"
#include <mutex>
#include <atomic>
#include <fstream>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>

#define SetLoggerColor_Error() ConsoleColorRed()
#define SetLoggerColor_Info() ConsoleColorGray()
#define SetLoggerColor_Debug() ConsoleColorCyan()
#define SetLoggerColor_Warn() ConsoleColorYellow()

namespace BR2 {
class Logger_Internal {
public:
  enum class LogLevel { Debug, Info, Warn, Error, };
  string_t _logDir;
  string_t _logFileName;
  std::atomic_bool _bAsync = false;

  std::vector<std::string> _toLog;

  std::atomic_bool _bEnabled = true;
  std::atomic_bool _bLogToFile = true;
  std::atomic_bool _bLogToConsole = true;

  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogStackAccess;
  std::atomic_bool _kill;

  string_t addStackTrace(string_t msg);
  void addLineFileToMsg(string_t msg, int line, char* file);
  string_t createMessageHead(LogLevel level);
  void log(string_t msg, string_t header, BR2::Exception* e);
  void processLogs_Async();
};

string_t Logger_Internal::addStackTrace(string_t msg) {
  msg += "\r\n";
  msg += DebugHelper::getStackTrace();
  return msg;
}
void Logger_Internal::addLineFileToMsg(string_t msg, int line, char* file) {
  if (_bSuppressLineFileDisplay == false) {
    msg = msg + "  (" + FileSystem::getFileNameFromPath(file) + " : " + line + ")";
  }
}
string_t Logger_Internal::createMessageHead(LogLevel level) {
  string_t str;
  if (level == LogLevel::Debug) {
    str = "DBG";
  }
  else if (level == LogLevel::Info) {
    str = "INF";
  }
  else if (level == LogLevel::Warn) {
    str = "WRN";
  }
  else if (level == LogLevel::Error) {
    str = "ERR";
  }
  return Stz "" + DateTime::timeToStr(DateTime::getTime()) + " " + str + " ";
}
void Logger_Internal::log(string_t msg, string_t header, BR2::Exception* e) {
  string_t m = header + " " + msg + (e != nullptr ? (", Exception: " + e->what()) : "") + "\n";

  if (_bAsync) {
    _mtLogStackAccess.lock();
    {
      _toLog.push_back(m);
    }
    _mtLogStackAccess.unlock();
  }
  else {
    _toLog.push_back(m);
    processLogs_Async();
  }
}
void Logger_Internal::processLogs_Async() {
  std::vector<string_t> logs;
  if (_bAsync) {
    _mtLogStackAccess.lock();
    {
      logs.swap(_toLog);
    }
    _mtLogStackAccess.unlock();
  }
  else {
    logs.swap(_toLog);
  }

  string_t appended = "";
  for (string_t m : logs) {
    appended += m;

    if (_bLogToConsole) {
      if (m.find(" DBG ") != std::string::npos) {
        SetLoggerColor_Debug();
      }
      else if (m.find(" ERR ") != std::string::npos) {
        SetLoggerColor_Error();
      }
      else if (m.find(" WRN ") != std::string::npos) {
        SetLoggerColor_Warn();
      }
      else {
        SetLoggerColor_Info();
      }
      std::cout << m;
    }
  }
  if (_bLogToConsole) {
    SetLoggerColor_Info();
  }

  if (_bLogToFile) {
    if (!FileSystem::fileExists(_logDir)) {
      FileSystem::createDirectoryRecursive(FileSystem::getPathFromPath(_logDir));
      FileSystem::createFile(_logDir, false, false);
    }
    //  OperatingSystem::suppressError(183,"Suppressing windows dumb 'append' error 183",false);
    std::ofstream _fileHandle;
    _fileHandle.open(_logDir.c_str(), std::ofstream::app);
    if (_fileHandle.fail() == false) {
      _fileHandle.write(appended.c_str(), (std::streamsize)appended.length());
      _fileHandle.close();
    }
    else {
      //Ignore log writes.  Not app critical
      //Debug only . This is not necessarily an error.
      //We could wait for the file to become available by write-checking
      //it a couple of times
      Gu::debugBreak();
    }
  }

  logs.clear();
}

//////////////////////////////////////////////////////////////////////////

Logger::Logger(bool async, bool disabled) {
  _pint = new Logger_Internal();
  _pint->_bAsync = async;
  _pint->_bEnabled = !disabled;

}
Logger::~Logger() {
  //kill thread.
  _pint->_kill = true;
  _pint = nullptr;
}
void Logger::init(string_t cache) {
  _pint->_logFileName = "Log.txt";
  string_t cache_rooted = FileSystem::getRootedPath(cache);
  _pint->_logDir = FileSystem::combinePath(cache_rooted, _pint->_logFileName);

  //Run async, if applicable
  if (_pint->_bAsync) {
    //https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/

    Logger_Internal* li = _pint;

    std::thread th([li]() {
      while (true) {
        li->processLogs_Async();
        if (li->_kill) {
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      //delete in this thread.
      delete li;
      });
    th.detach();
  }

  //*Note: do not call the #define shortcuts here.
  logInfo(Stz(_pint->_bAsync ? "Async " : "") + "Logger Initializing " + DateTime::dateTimeToStr(DateTime::getDateTime()));
}
string_t Logger::getLogPath() { return _pint->_logDir; }
void Logger::logDebug(const char* msg) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logDebug(string_t(msg));
}
void Logger::logInfo(const char* msg) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logInfo(string_t(msg));
}
void Logger::logError(const char* msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logError(string_t(msg), e);
}
void Logger::logWarn(const char* msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logWarn(string_t(msg), e);
}
void Logger::logDebug(string_t msg) {
  if (_pint->_bEnabled == false) {
    return;
  }
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logDebug(string_t msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }
  if (_pint->_bLogToConsole) {
    SetLoggerColor_Debug();
  }
  _pint->addLineFileToMsg(msg, line, file);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logError(string_t msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  if (_pint->_bLogToConsole) {
    SetLoggerColor_Error();
  }

  msg = _pint->addStackTrace(msg);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Error), e);
}
void Logger::logError(string_t msg, int line, char* file, BR2::Exception* e, bool hideStackTrace) {
  if (_pint->_bEnabled == false) {
    return;
  }

  _pint->addLineFileToMsg(msg, line, file);

  if (hideStackTrace == false) {
    msg = _pint->addStackTrace(msg);
  }

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Error), e);
}
void Logger::logInfo(string_t msg) {
  if (_pint->_bEnabled == false) {
    return;
  }

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logInfo(string_t msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }

  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logWarn(string_t msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarn(string_t msg, int line, char* file, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarnCycle(string_t msg, int line, char* file, BR2::Exception* e, int iCycle, bool force) {
  //prevents per-frame logging conundrum
  if (Gu::getCoreContext() != nullptr) {
    if (Gu::getCoreContext()->getGraphicsWindow() != nullptr) {
      if (Gu::getCoreContext()->getGraphicsWindow()->getFpsMeter()) {
        if (force || Gu::getCoreContext()->getGraphicsWindow()->getFpsMeter()->frameMod(iCycle)) {
          logWarn(msg, line, file, e);
        }
      }
    }
  }
}
void Logger::logErrorCycle(string_t msg, int line, char* file, BR2::Exception* e, int iCycle, bool force) {
  //prevents per-frame logging conundrum
  if (Gu::getCoreContext() != nullptr) {
    if (Gu::getCoreContext()->getGraphicsWindow() != nullptr) {
      if (Gu::getCoreContext()->getGraphicsWindow()->getFpsMeter()) {
        if (force || Gu::getCoreContext()->getGraphicsWindow()->getFpsMeter()->frameMod(iCycle)) {
          logError(msg, line, file, e);
        }
      }
    }
  }
}
void Logger::enableLogToConsole(bool bLogToConsole) {
  _pint->_bLogToConsole = bLogToConsole;
  _pint->_bEnabled = _pint->_bLogToConsole || _pint->_bLogToFile;
}
void Logger::enableLogToFile(bool bLogToFile) {
  _pint->_bLogToFile = bLogToFile;
  _pint->_bEnabled = _pint->_bLogToConsole || _pint->_bLogToFile;
}
}//ns game