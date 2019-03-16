#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/TStr.h"
#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../base/RoomBase.h"
#include "../base/GLContext.h"

#define writeDisabledToConsoe

namespace Game {
;
Logger::Logger() :
    _nMsg(0)
    , _nErr(0)
    , _bSuppressLineFileDisplay(false)
    , _bWriteDisabledLogsToConsole(false)
    , _bEnabled(FALSE) // ** Default enabled to false so that we don't recur forever when constructing
{
}
Logger::~Logger()
{
}
//////////////////////////////////////////////////////////////////////////
void Logger::init(std::shared_ptr<RoomBase> rb) {

    _logFileName = TStr("CongaLog.txt");

    _logDir = rb->makeAssetPath(rb->getCacheDir(), _logFileName);

    #ifdef _DEBUG
    _bSuppressLineFileDisplay = false;
    #else
    _bSuppressLineFileDisplay = false;
    #endif

    _bEnabled = true;

    //*Note: do not call the #define shortcuts here.
    logInfo(TStr(""));
    logInfo(TStr("=========================================================================================="));
    logInfo(TStr("============ Logger Init ", DateTime::dateTimeToStr(DateTime::getDateTime()), "============"));
    logInfo(TStr(""));
    logInfo(TStr("Operating System: ", Gu::getOperatingSystemName()));
}
void Logger::logDebug(const char* msg)
{
    if (!disableCheck(msg)) {
        return;
    }

    logDebug(t_string(msg));
}
void Logger::logInfo(const char* msg)
{
    if (!disableCheck(msg)) {
        return;
    }

    logInfo(t_string(msg));
}
void Logger::logError(const char* msg, Game::Exception* e)
{
    if (!disableCheck(msg)) {
        return;
    }

    logError(t_string(msg), e);
}
void Logger::logWarn(const char* msg, Game::Exception* e)
{
    if (!disableCheck(msg)) {
        return;
    }

    logWarn(t_string(msg), e);
}
void Logger::disableLogger()
{
    _bEnabled = false;
}
void Logger::enableLogger()
{
    _bEnabled = true;
}
void Logger::addLineFileToMsg(t_string& msg, int line, char* file)
{
    if (_bSuppressLineFileDisplay == false)
        msg = TStr(msg, "  (", FileSystem::getFileNameFromPath(TStr(file)), " : ", line, ")");
}
t_string Logger::createMessageHead(LogLevel level)
{
    t_string str;
    t_string szLine;
    if (level == LogLevel::Debug)
        str = "DBG";
    else if (level == LogLevel::Info)
        str = "INF";
    else if (level == LogLevel::Warn)
        str = "WRN";
    else if (level == LogLevel::Error) {
        str = "ERR";
    }


    szLine = StringUtil::getPaddedNumber(_nMsg, 6, '.');
    return TStr("[", szLine, "][", DateTime::timeToStr(DateTime::getTime()), "][", str, "]");
}
void Logger::logDebug(t_string& msg)
{
    if (!disableCheck(msg.c_str())) return;
    SetLoggerColor_Debug();
    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logDebug(t_string& msg, int line, char* file)
{
    if (!disableCheck(msg.c_str())) return;
    SetLoggerColor_Debug();
    addLineFileToMsg(msg, line, file);

    log(msg, createMessageHead(LogLevel::Debug), NULL);
}
void Logger::logError(t_string& msg, Game::Exception* e)
{
    if (!disableCheck(msg.c_str())) return;
    SetLoggerColor_Error();

    msg = addStackTrace(msg);

    log(msg, createMessageHead(LogLevel::Error), e);
}
void Logger::logError(t_string& msg, int line, char* file, Game::Exception* e)
{
    if (!disableCheck(msg.c_str())) return;
    SetLoggerColor_Error();
    addLineFileToMsg(msg, line, file);

    msg = addStackTrace(msg);

    log(msg, createMessageHead(LogLevel::Error), e);
}

void Logger::logInfo(t_string& msg)
{
    if (!disableCheck(msg.c_str()))
        return;
    SetLoggerColor_Info();
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logInfo(t_string& msg, int line, char* file)
{
    if (!disableCheck(msg.c_str()))
        return;
    SetLoggerColor_Info();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Info), NULL);
}
void Logger::logWarn(t_string& msg, Game::Exception* e)
{
    if (!disableCheck(msg.c_str())) return;
    SetLoggerColor_Warn();
    log(msg, createMessageHead(LogLevel::Warn), e);
}

void Logger::logWarnCycle(t_string& msg, int line, char* file, Game::Exception* e, int iCycle)
{
    //Logs ina cycle in the game loop (prevents per-frame logging conundrum)
    if(Gu::getContext()!=nullptr){
        if(Gu::getContext()->getFpsMeter() != nullptr){
            if(Gu::getContext()->getFpsMeter()->frameMod(iCycle)){
                logWarn(msg,line,file,e);
            }
        }
    }
}
void Logger::logErrorCycle(t_string& msg, int line, char* file, Game::Exception* e, int iCycle)
{
    //Logs ina cycle in the game loop (prevents per-frame logging conundrum)
    if (Gu::getContext() != nullptr) {
        if (Gu::getContext()->getFpsMeter() != nullptr) {
            if (Gu::getContext()->getFpsMeter()->frameMod(iCycle)) {
                logError(msg, line, file, e);
            }
        }
    }
}
void Logger::logWarn(t_string& msg, int line, char* file, Game::Exception* e)
{
    if (!disableCheck(msg.c_str())) {
        return;
    }
    SetLoggerColor_Warn();
    addLineFileToMsg(msg, line, file);
    log(msg, createMessageHead(LogLevel::Warn), e);
}
// - Output an error
void Logger::log(t_string &msg, t_string& errpred, Game::Exception* e)
{
    std::lock_guard<std::mutex> guard(_mtLogWriteMutex);

    if (!disableCheck(msg.c_str())) {
        return;
    }

    t_string m;
    m += errpred;
    m += t_string("  ") + msg;
    if (e) {
        m += t_string(", Exception: ") + t_string(e->what());
    }
    m += t_string("\n");

    Gu::print(m);

    if (!FileSystem::fileExists(_logDir)) {
        FileSystem::createDirectoryRecursive(FileSystem::getPathFromPath(_logDir));
        FileSystem::createFile(_logDir, false, false);
    }

    //  OperatingSystem::suppressError(183,"Suppressing windows dumb 'append' error 183",false);

    _f.open(_logDir.c_str(), std::ofstream::app);
    if (_f.fail() == false) {
        _f.write(m.c_str(), (std::streamsize)m.length());
        _f.close();
    }
    else {
        //Ignore log writes.  Not app critical
        //Debug only . This is not necessarily an error.
        //We could wait for the file to become available by write-checking
        //it a couple of times
        Gu::debugBreak();

        //// - Throw fatal exception to exit application
        //_mtLogWriteMutex.unlock();
        //throw new Game::Exception(
        //    Game::GR_FATAL
        //    , "Failed to open log file.  This could be due to filesystem access privileges."
        //    , __LINE__
        //    , __FILE__
        //    , false
        //    );    
    }

    //   OperatingSystem::suppressError(183,"Suppressing windows dumb 'append' error 183",false);


    _nMsg++;
    _nErr++;
    SetLoggerColor_Info();
}
t_string Logger::addStackTrace(t_string msg) {
    msg += "\r\n";
    msg += DebugHelper::getStackTrace();
    return msg;
}

}//ns game
