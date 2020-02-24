/**
*  @file Stopwatch.h
*  @date November 8, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __STOPWATCH_11036904868224987022_H__
#define __STOPWATCH_11036904868224987022_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class Stopwatch
*  @brief Records milliseconds between instances of code.  Similar to .NET Stopwatch class.
*/
class Stopwatch : public VirtualMemory {
public:
  Stopwatch(t_string name = "", bool bStart = false);
  virtual ~Stopwatch() override;

  void start(t_string name = "");
  virtual t_string stop(bool bLogMillis = false, bool bFancy = false);
  bool pulse(t_timeval isGreaterThanMs);

  t_timeval getStart() { return _tA; }
  void setStart(t_timeval a) { _tA = a; }
  void print(t_string& st);//print to stdout
  t_timeval deltaMicroseconds();
  t_timeval deltaMicrosecondsRemainder();
  t_timeval deltaMilliseconds();
  t_string toString(bool bFancy = true);
  t_string pulse(bool bLog = false, bool bFancy = false);    // convert current delta to string and restart the counter.
private:
  t_timeval _tA, _tB; //DWORD
  t_string _strName;
  t_string _strCache;
};

}//ns game



#endif
