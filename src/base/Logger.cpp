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
  t_string _logDir;
  t_string _logFileName;
  int32_t _nMsg = 0;    // - Profiling variables.  We increment them when we log.

  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogWriteMutex;

  bool _bEnabled = false;
  bool _bLogToFile = false;
  bool _bLogToConsole = false;

  t_string Logger_Internal::addStackTrace(t_string msg) {
    msg += "\r\n";
    msg += DebugHelper::getStackTrace();
    return msg;
  }
  void Logger_Internal::addLineFileToMsg(t_string msg, int line, char* file) {
    if (_bSuppressLineFileDisplay == false) {
      msg = msg + "  (" + FileSystem::getFileNameFromPath(file) + " : " + line + ")";
    }
  }
  t_string Logger_Internal::createMessageHead(LogLevel level) {
    t_string str;
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
  void Logger_Internal::log(t_string msg, t_string header, BR2::Exception* e) {
    std::lock_guard<std::mutex> guard(_mtLogWriteMutex);

    t_string m = header + " " + msg + (e != nullptr ? (", Exception: " + e->what()) : "") + "\n";

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
t_string Logger::getLogPath() { return _pint->_logDir; }
void Logger::logDebug(const char* msg) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logDebug(t_string(msg));
}
void Logger::logInfo(const char* msg) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logInfo(t_string(msg));
}
void Logger::logError(const char* msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logError(t_string(msg), e);
}
void Logger::logWarn(const char* msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }

  logWarn(t_string(msg), e);
}
void Logger::logDebug(t_string msg) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Debug();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logDebug(t_string msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Debug();
  _pint->addLineFileToMsg(msg, line, file);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Debug), NULL);
}
void Logger::logError(t_string msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Error();

  msg = _pint->addStackTrace(msg);

  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Error), e);
}
void Logger::logError(t_string msg, int line, char* file, BR2::Exception* e, bool hideStackTrace) {
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
void Logger::logInfo(t_string msg) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Info();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logInfo(t_string msg, int line, char* file) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Info();
  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Info), NULL);
}
void Logger::logWarn(t_string msg, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Warn();
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarn(t_string msg, int line, char* file, BR2::Exception* e) {
  if (_pint->_bEnabled == false) {
    return;
  }
  SetLoggerColor_Warn();
  _pint->addLineFileToMsg(msg, line, file);
  _pint->log(msg, _pint->createMessageHead(Logger_Internal::LogLevel::Warn), e);
}
void Logger::logWarnCycle(t_string msg, int line, char* file, BR2::Exception* e, int iCycle) {
  //prevents per-frame logging conundrum
  if (Gu::getGraphicsContext() != nullptr) {
    if (Gu::getFpsMeter() != nullptr) {
      if (Gu::getFpsMeter()->frameMod(iCycle)) {
        logWarn(msg, line, file, e);
      }
    }
  }
}
void Logger::logErrorCycle(t_string msg, int line, char* file, BR2::Exception* e, int iCycle) {
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
