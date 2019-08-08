#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/TStr.h"
#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../base/AppBase.h"
#include "../base/GLContext.h"

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
t_string Logger::createMessageHead(LogLevel level) {
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
void Logger::logDebug(const char* msg) {
    if (_bEnabled == false) {
        return;
    }

    logDebug(t_string(msg));
}
void Logger::logInfo(const char* msg) {
    if (_bEnabled == false) {
        return;
    }

    logInfo(t_string(msg));
}
void Logger::logError(const char* msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }

    logError(t_string(msg), e);
}
void Logger::logWarn(const char* msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }

    logWarn(t_string(msg), e);
}
void Logger::logDebug(t_string msg) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Debug();
    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logDebug(t_string msg, int line, char* file) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Debug();
    addLineFileToMsg(msg, line, file);

    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logError(t_string msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Error();

    msg = addStackTrace(msg);

    log(msg, createMessageHead(LogLevel::Error), e);
}
void Logger::logError(t_string msg, int line, char* file, Game::Exception* e, bool hideStackTrace) {
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
void Logger::logInfo(t_string msg) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Info();
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logInfo(t_string msg, int line, char* file) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Info();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logWarn(t_string msg, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Warn();
    log(msg, createMessageHead(LogLevel::Warn), e);
}
void Logger::logWarn(t_string msg, int line, char* file, Game::Exception* e) {
    if (_bEnabled == false) {
        return;
    }
    SetLoggerColor_Warn();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Warn), e);
}
void Logger::logWarnCycle(t_string msg, int line, char* file, Game::Exception* e, int iCycle) {
    //prevents per-frame logging conundrum
    if (Gu::getContext() != nullptr) {
        if (Gu::getContext()->getFpsMeter() != nullptr) {
            if (Gu::getContext()->getFpsMeter()->frameMod(iCycle)) {
                logWarn(msg, line, file, e);
            }
        }
    }
}
void Logger::logErrorCycle(t_string msg, int line, char* file, Game::Exception* e, int iCycle) {
    //prevents per-frame logging conundrum
    if (Gu::getContext() != nullptr) {
        if (Gu::getContext()->getFpsMeter() != nullptr) {
            if (Gu::getContext()->getFpsMeter()->frameMod(iCycle)) {
                logError(msg, line, file, e);
            }
        }
    }
}
void Logger::log(t_string msg, t_string header, Game::Exception* e) {
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
t_string Logger::addStackTrace(t_string msg) {
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
void Logger::addLineFileToMsg(t_string msg, int line, char* file) {
    if (_bSuppressLineFileDisplay == false) {
        msg = msg + "  (" + FileSystem::getFileNameFromPath(file) + " : " + line + ")";
    }
}

}//ns game
