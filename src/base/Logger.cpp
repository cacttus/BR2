#include "../base/Logger.h"
#include "../base/FileSystem.h"

#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../app/AppBase.h"
#include "../base/GLContext.h"
#include "../base/FpsMeter.h"

namespace Game {
Logger::Logger() {
}
Logger::~Logger() {
}
void Logger::init(std::shared_ptr<AppBase> rb) {
    _logFileName = "Log.txt";
    _logDir = rb->makeAssetPath(rb->getCacheDir(), _logFileName);

     //*Note: do not call the #define shortcuts here.
    logInfo(Stz  "Logger Initializing " + DateTime::dateTimeToStr(DateTime::getDateTime()));
}
string_t Logger::createMessageHead(LogLevel level) {
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
void Logger::logDebug(const char* msg) {
    if (_bEnabled == false) {
        return;
    }

    logDebug(string_t(msg));
}
void Logger::logInfo(const char* msg) {
    if (_bEnabled == false) {
        return;
    }

    logInfo(string_t(msg));
}
void Logger::logError(const char* msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }

    logError(string_t(msg), e);
}
void Logger::logWarn(const char* msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }

    logWarn(string_t(msg), e);
}
void Logger::logDebug(string_t msg) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Debug();
    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logDebug(string_t msg, int line, char* file) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Debug();
    addLineFileToMsg(msg, line, file);

    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logError(string_t msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Error();

    msg = addStackTrace(msg);

    log(msg, createMessageHead(LogLevel::Error), e);
}
void Logger::logError(string_t msg, int line, char* file, Game::Exception* e, bool hideStackTrace) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Error();
    addLineFileToMsg(msg, line, file);

    if (hideStackTrace == false) {
        msg = addStackTrace(msg);
    }

    log(msg, createMessageHead(LogLevel::Error), e);
}
void Logger::logInfo(string_t msg) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Info();
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logInfo(string_t msg, int line, char* file) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Info();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logWarn(string_t msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Warn();
    log(msg, createMessageHead(LogLevel::Warn), e);
}
void Logger::logWarn(string_t msg, int line, char* file, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Warn();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Warn), e);
}
void Logger::logWarnCycle(string_t msg, int line, char* file, Game::Exception* e, int iCycle) {
    //prevents per-frame logging conundrum
  /*if (Gu::getGraphicsContext() != nullptr) {
    if (Gu::getFpsMeter() != nullptr) {
      if (Gu::getFpsMeter()->frameMod(iCycle)) {*/
        logWarn(msg, line, file, e);
  //    }
  //  }
  //}
}
void Logger::logErrorCycle(string_t msg, int line, char* file, Game::Exception* e, int iCycle) {
    //prevents per-frame logging conundrum
    //if (Gu::getGraphicsContext() != nullptr) {
    //    if (Gu::getFpsMeter() != nullptr) {
    //        if (Gu::getFpsMeter()->frameMod(iCycle)) {
                logError(msg, line, file, e);
    //        }
    //    }
    //}
}
void Logger::log(string_t msg, string_t header, Game::Exception* e) {
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
string_t Logger::addStackTrace(string_t msg) {
    msg += "\r\n";
    msg += DebugHelper::getStackTrace();
    return msg;
}
void Logger::enableLogToConsole(bool bLogToConsole) {
    _bLogToConsole = bLogToConsole;
    _bEnabled = _bLogToConsole || _bLogToFile;
}
void Logger::enableLogToFile(bool bLogToFile) {
    _bLogToFile = bLogToFile;
    _bEnabled = _bLogToConsole || _bLogToFile;
}
void Logger::addLineFileToMsg(string_t msg, int line, char* file) {
    if (_bSuppressLineFileDisplay == false) {
        msg = msg + "  (" + FileSystem::getFileNameFromPath(file) + " : " + line + ")";
    }
}

}//ns game
