/**
*  @file Logger.h
*  @date December 14, 2013
*  @author MetalMario971
*/
#pragma once
#ifndef __LOGGER_16421047220665463212149_H__
#define __LOGGER_16421047220665463212149_H__

#include "../base/BaseHeader.h"
#include "../base/Gu.h"

namespace Game {
/**
*  @class Logger
*  @brief Logs information to a text file or console.
*  @note Logger is initially disabled.
*/
class Logger : public OperatingSystemMemory {

public:
  Logger();
  virtual ~Logger() override;

  void init(std::shared_ptr<AppBase> rb);

  void enableLogToConsole(bool bLogToConsole);
  void enableLogToFile(bool bLogToFile);

  void logError(t_string msg, Game::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarn(t_string msg, Game::Exception* e = NULL);    // - Log an error with exception contents.
  void logDebug(t_string msg);
  void logInfo(t_string msg);

  void logError(t_string msg, int line, char* file, Game::Exception* e = NULL, bool hideStackTrace = false);    // - Log an error with exception contents.
  void logWarn(t_string msg, int line, char* file, Game::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarnCycle(t_string msg, int line, char* file, Game::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logErrorCycle(t_string msg, int line, char* file, Game::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logDebug(t_string msg, int line, char* file);
  void logInfo(t_string msg, int line, char* file);

  void logInfo(const char* msg);
  void logDebug(const char* msg);
  void logError(const char* msg, Game::Exception* e = NULL);
  void logWarn(const char* msg, Game::Exception* e = NULL);

  t_string getLogPath() { return _logDir; }
private:
  typedef enum { Debug, Info, Warn, Error, }LogLevel;
private:
  t_string _logDir;
  t_string _logFileName;
  int32_t _nMsg = 0;    // - Profiling variables.  We increment them when we log.

  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogWriteMutex;

  bool _bEnabled = false;
  bool _bLogToFile = false;
  bool _bLogToConsole = false;

  void addLineFileToMsg(t_string msg, int line, char* file);
  t_string createMessageHead(LogLevel level);
  void log(t_string msg, t_string header, Game::Exception* e);    // - Log an error with exception contents.
  t_string addStackTrace(t_string msg);

};

}//ns game


//Macros
#define BRLogDebug(x) Game::Gu::getLogger()->logDebug(Stz x,__LINE__,__FILE__ )
#define BRLogInfo(x) Game::Gu::getLogger()->logInfo(Stz x,__LINE__,__FILE__ )
#define BRLogWarn(x) Game::Gu::getLogger()->logWarn(Stz x,__LINE__,__FILE__, NULL )
#define BRLogError(x) Game::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL )
#define BRLogErrorNoStack(x) Game::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL, true)

#define BRLogErrorOnce(x) \
{ \
static bool _logged = false; \
if(_logged == false) { \
_logged = true; \
Game::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL ); \
} \
} 

#define BRLogErrorEx(x, aex) \
Game::Gu::getLogger()->logError(x,__LINE__,__FILE__, aex )
#define BRLogWarnCycle(x) Game::Gu::getLogger()->logWarnCycle(Stz x, __LINE__, __FILE__, NULL, 60)
#define BRLogErrorCycle(x) Game::Gu::getLogger()->logErrorCycle(Stz x, __LINE__, __FILE__, NULL, 60)

#define SetLoggerColor_Error() ConsoleColorRed()
#define SetLoggerColor_Info() ConsoleColorGray()
#define SetLoggerColor_Debug() ConsoleColorCyan()
#define SetLoggerColor_Warn() ConsoleColorYellow()


#endif
