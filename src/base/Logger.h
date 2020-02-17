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

namespace BR2 {
enum class LogLevel {
  Debug,
  Info,
  Warn,
  Error,
};
/**
*   @class Logger
*   @brief Logs information to a text file or console.
*   @note Logger is initially disabled.
*/
class Logger : public OperatingSystemMemory {
public:
  Logger();
  virtual ~Logger() override;

  void init(string_t cacheDir);

  void enableLogToConsole(bool bLogToConsole);
  void enableLogToFile(bool bLogToFile);

  void logError(string_t msg, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarn(string_t msg, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logDebug(string_t msg);
  void logInfo(string_t msg);
  void logError(string_t msg, int line, char* file, BR2::Exception* e = NULL, bool hideStackTrace = false);    // - Log an error with exception contents.
  void logWarn(string_t msg, int line, char* file, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarnCycle(string_t msg, int line, char* file, BR2::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logErrorCycle(string_t msg, int line, char* file, BR2::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logDebug(string_t msg, int line, char* file);
  void logInfo(string_t msg, int line, char* file);
  void logInfo(const char* msg);
  void logDebug(const char* msg);
  void logError(const char* msg, BR2::Exception* e = NULL);
  void logWarn(const char* msg, BR2::Exception* e = NULL);

  string_t getLogPath() { return _logDir; }

private:
  string_t _logDir;
  string_t _logFileName;
  int32_t _nMsg = 0;    // - Profiling variables.  We increment them when we log.
  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogWriteMutex;
  bool _bEnabled = false;
  bool _bLogToFile = false;
  bool _bLogToConsole = false;

  void addLineFileToMsg(string_t msg, int line, char* file);
  string_t createMessageHead(LogLevel level);
  void log(string_t msg, string_t header, BR2::Exception* e);    // - Log an error with exception contents.
  string_t addStackTrace(string_t msg);
};

}//ns BR2


//Macros
#define Br2LogDebug(x) BR2::Gu::getLogger()->logDebug(Stz x,__LINE__,__FILE__ )
#define Br2LogInfo(x) BR2::Gu::getLogger()->logInfo(Stz x,__LINE__,__FILE__ )
#define Br2LogWarn(x) BR2::Gu::getLogger()->logWarn(Stz x,__LINE__,__FILE__, NULL )
#define Br2LogError(x) BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL )
#define Br2LogErrorNoStack(x) BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL, true)

#define Br2LogErrorOnce(x) \
{ \
static bool _logged = false; \
if(_logged == false) { \
_logged = true; \
BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL ); \
} \
} 

#define Br2LogErrorEx(x, aex) BR2::Gu::getLogger()->logError(x,__LINE__,__FILE__, aex )
#define Br2LogWarnCycle(x) BR2::Gu::getLogger()->logWarnCycle(Stz x, __LINE__, __FILE__, NULL, 60)
#define Br2LogErrorCycle(x) BR2::Gu::getLogger()->logErrorCycle(Stz x, __LINE__, __FILE__, NULL, 60)

#define SetLoggerColor_Error() ConsoleColorRed()
#define SetLoggerColor_Info() ConsoleColorGray()
#define SetLoggerColor_Debug() ConsoleColorCyan()
#define SetLoggerColor_Warn() ConsoleColorYellow()


#endif
