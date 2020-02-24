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
/**
*  @class Logger
*  @brief Logs information to a text file or console.
*  @note Logger is initially disabled.
*/
class Logger_Internal;
class Logger : public OperatingSystemMemory {
public:
  Logger();
  virtual ~Logger() override;

  void init(std::shared_ptr<AppBase> rb);

  void enableLogToConsole(bool bLogToConsole);
  void enableLogToFile(bool bLogToFile);

  void logError(t_string msg, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarn(t_string msg, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logDebug(t_string msg);
  void logInfo(t_string msg);

  void logError(t_string msg, int line, char* file, BR2::Exception* e = NULL, bool hideStackTrace = false);    // - Log an error with exception contents.
  void logWarn(t_string msg, int line, char* file, BR2::Exception* e = NULL);    // - Log an error with exception contents.
  void logWarnCycle(t_string msg, int line, char* file, BR2::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logErrorCycle(t_string msg, int line, char* file, BR2::Exception* e = NULL, int iCycle = 60);    // - Log an error with exception contents.
  void logDebug(t_string msg, int line, char* file);
  void logInfo(t_string msg, int line, char* file);

  void logInfo(const char* msg);
  void logDebug(const char* msg);
  void logError(const char* msg, BR2::Exception* e = NULL);
  void logWarn(const char* msg, BR2::Exception* e = NULL);

  t_string getLogPath();
private:
  std::unique_ptr<Logger_Internal> _pint = nullptr;
};

}//ns game


//Macros
#define BRLogDebug(x) BR2::Gu::getLogger()->logDebug(Stz x,__LINE__,__FILE__ )
#define BRLogInfo(x) BR2::Gu::getLogger()->logInfo(Stz x,__LINE__,__FILE__ )
#define BRLogWarn(x) BR2::Gu::getLogger()->logWarn(Stz x,__LINE__,__FILE__, NULL )
#define BRLogError(x) BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL )
#define BRLogErrorNoStack(x) BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL, true)

#define BRLogErrorOnce(x) \
{ \
static bool _logged = false; \
if(_logged == false) { \
_logged = true; \
BR2::Gu::getLogger()->logError(Stz x,__LINE__,__FILE__, NULL ); \
} \
} 

#define BRLogErrorEx(x, aex) \
BR2::Gu::getLogger()->logError(x,__LINE__,__FILE__, aex )
#define BRLogWarnCycle(x) BR2::Gu::getLogger()->logWarnCycle(Stz x, __LINE__, __FILE__, NULL, 60)
#define BRLogErrorCycle(x) BR2::Gu::getLogger()->logErrorCycle(Stz x, __LINE__, __FILE__, NULL, 60)

#define SetLoggerColor_Error() ConsoleColorRed()
#define SetLoggerColor_Info() ConsoleColorGray()
#define SetLoggerColor_Debug() ConsoleColorCyan()
#define SetLoggerColor_Warn() ConsoleColorYellow()


#endif
