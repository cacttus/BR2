/**
*
*    @file Logger.h
*    @date December 14, 2013
*    @author Derek Page
*
*    © 2013 
*
*
*/
#pragma once
#ifndef __LOGGER_16421047220665463212149_H__
#define __LOGGER_16421047220665463212149_H__

#include "../base/BaseHeader.h"
#include "../base/Gu.h"
//#include "../mem/OperatingSystemMemory.h"
//#include "../base/BaseHeader.h"
//#include "../base/GlobalStorage.h"
//#include "../base/StringWrapper.h"
//Note don't put more #includes here as we include this in globals.h

namespace Game {
/**
*    @class Logger
*    @brief Logs information to a text file.
*
*/
class Logger : public OperatingSystemMemory {
    typedef enum {
        Debug,
        Info,
        Warn,
        Error,
    }LogLevel;
    bool _bWriteDisabledLogsToConsole;
    t_string _logDir;
    t_string _logFileName;
    bool _bEnabled;
    int32_t _nMsg;    // - Profiling variables.  We increment them when we log.
    int32_t _nErr;
    int32_t _nDbg;
    std::atomic_bool _bSuppressLineFileDisplay;
    std::ofstream _f;
    //DECLARE_SYNC(_sync);    //creates sync object

    std::mutex _mtLogWriteMutex;
    
    void addLineFileToMsg(t_string& msg, int line, char* file);
    t_string createMessageHead(LogLevel level);
    void log(t_string& msg, t_string& errPred, Game::Exception* e);    // - Log an error with exception contents.

    // This function is important for when
    // we initialize the logger it can't be enabled so.. we can
    // write to the console in the meantime.
    FORCE_INLINE bool disableCheck(const char* c)
    {
        if(_bEnabled==FALSE)
        {
            //if(_bWriteDisabledLogsToConsole)
               // std::cout<<"logger disabled: "<<c<<std::endl;
            return FALSE;
        }
        return TRUE;
    }
    t_string addStackTrace(t_string msg);
public:
    Logger();
    virtual ~Logger() override;
    void init(std::shared_ptr<RoomBase> rb);

    void disableLogger();
    void enableLogger();
    
    void logError(t_string& msg, Game::Exception* e=NULL);    // - Log an error with exception contents.
    void logWarn(t_string& msg, Game::Exception* e=NULL);    // - Log an error with exception contents.
    void logDebug(t_string& msg);
    void logInfo(t_string& msg);

    void logError(t_string& msg, int line, char* file, Game::Exception* e=NULL);    // - Log an error with exception contents.
    void logWarn(t_string& msg, int line, char* file, Game::Exception* e = NULL);    // - Log an error with exception contents.
    void logWarnCycle(t_string& msg, int line, char* file, Game::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
    void logErrorCycle(t_string& msg, int line, char* file, Game::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
    void logDebug(t_string& msg, int line, char* file );
    void logInfo(t_string& msg, int line, char* file );

    void logInfo(const char* msg);
    void logDebug(const char* msg);
    void logError(const char* msg, Game::Exception* e=NULL);
    void logWarn(const char* msg, Game::Exception* e=NULL);

    t_string getLogPath() { return _logDir; }
};

}//ns game
 // - Log Macros
#define BroLogDebug(...) \
Game::Gu::getLogger()->logDebug( \
Game::TStr(__VA_ARGS__),__LINE__,__FILE__ )
#define BroLogInfo(...) \
Game::Gu::getLogger()->logInfo( \
Game::TStr(__VA_ARGS__),__LINE__,__FILE__ )
#define BroLogWarn(...) \
Game::Gu::getLogger()->logWarn( \
Game::TStr(__VA_ARGS__),__LINE__,__FILE__, NULL )
#define BroLogError(...) \
Game::Gu::getLogger()->logError( \
Game::TStr(__VA_ARGS__),__LINE__,__FILE__, NULL )
#define BroLogErrorEx(astr,aex) \
Game::Gu::getLogger()->logError( \
Game::TStr(astr),__LINE__,__FILE__, aex )
#define BroLogWarnCycle(...)\
Game::Gu::getLogger()->logWarnCycle(\
Game::TStr(__VA_ARGS__), __LINE__, __FILE__, NULL, 60)
#define BroLogErrorCycle(...)\
Game::Gu::getLogger()->logErrorCycle(\
Game::TStr(__VA_ARGS__), __LINE__, __FILE__, NULL, 60)

#define SetLoggerColor_Error() ConsoleColorRed()
#define SetLoggerColor_Info() ConsoleColorGray()
#define SetLoggerColor_Debug() ConsoleColorCyan()
#define SetLoggerColor_Warn() ConsoleColorYellow()


#endif
