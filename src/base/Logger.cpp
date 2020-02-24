#include "../base/Logger.h"
#include "../base/FileSystem.h"

#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../base/AppBase.h"
#include "../base/GLContext.h"
#include "../base/FpsMeter.h"

#include <mutex>
#include <atomic>
#include <fstream>

namespace BR2 {
class Logger_Internal {
public:
  typedef enum { Debug, Info, Warn, Error, }LogLevel;
  string_t _logDir;
  string_t _logFileName;
  int32_t _nMsg = 0;    // - Profiling variables.  We increment them when we log.

  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogWriteMutex;

  bool _bEnabled = false;
  bool _bLogToFile = false;
  bool _bLogToConsole = false;

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
    std::lock_guard<std::mutex> guard(_mtLogWriteMutex);

    string_t m = header + " " + msg + (e != nullptr ? (", Exception: " + e->what()) : "") + "\n";

    if (_bLogToConsole) {
      Gu::print(m);
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
        _fileHandle.write(m.c_str(), (std::streamsize)m.length());
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

    _nMsg++;

    SetLoggerColor_Info();
  }

};

//////////////////////////////////////////////////////////////////////////

Logger::Logger() {
  _pint = std::make_unique<Logger_Internal>();
}
Logger::~Logger() {
  _pint = nullptr;
}
void Logger::init(std::shared_ptr<AppBase> rb) {
  _pint->_logFileName = "Log.txt";
  _pint->_logDir = rb->makeAssetPath(rb->getCacheDir(), _pint->_logFileName);

  //*Note: do not call the #define shortcuts here.
  logInfo(Stz  "Logger Initializing " + DateTime::dateTimeToStr(DateTime::getDateTime()));
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
  SetLoggerColor_Debug();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logDebug(string_t msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Debug();
  _pint->addLineFileToMsg(msg, line, file);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logError(string_t msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Error();

  msg = _pint->addStackTrace(msg);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Error), e);
}
void Logger::logError(string_t msg, int line, char* file, BR2::Exception* e, bool hideStackTrace) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Error();
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
  SetLoggerColor_Info();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logInfo(string_t msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Info();
  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logWarn(string_t msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Warn();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarn(string_t msg, int line, char* file, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Warn();
  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarnCycle(string_t msg, int line, char* file, BR2::Exception* e, int iCycle) {
  //prevents per-frame logging conundrum
  if (Gu::getGraphicsContext() != nullptr) {
    if (Gu::getFpsMeter() != nullptr) {
      if (Gu::getFpsMeter()->frameMod(iCycle)) {
        logWarn(msg, line, file, e);
      }
    }
  }
}
void Logger::logErrorCycle(string_t msg, int line, char* file, BR2::Exception* e, int iCycle) {
  //prevents per-frame logging conundrum
  if (Gu::getGraphicsContext() != nullptr) {
    if (Gu::getFpsMeter() != nullptr) {
      if (Gu::getFpsMeter()->frameMod(iCycle)) {
        logError(msg, line, file, e);
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
