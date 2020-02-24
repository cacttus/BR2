#include "../base/Stopwatch.h"
#include "../base/Logger.h"
#include "../base/DateTime.h"
#include "../base/Gu.h"


namespace BR2 {

Stopwatch::Stopwatch(t_string strName, bool bStart) :
  _tA(0)
  , _tB(0) {
  _strName = strName;
  if (bStart) {
    start();
  }
}
Stopwatch::~Stopwatch() {
}
void Stopwatch::start(t_string strName) {
  _strName = strName;
  _tA = Gu::getMicroSeconds();
}
t_string Stopwatch::stop(bool bLog, bool bFancy) {
  _tB = Gu::getMicroSeconds();

  if (bLog == true) {
    BRLogDebug(toString(bFancy));
  }

  return toString(bFancy);
}
t_timeval Stopwatch::deltaMilliseconds() {
  return (_tB - _tA) / 1000;
}
t_timeval Stopwatch::deltaMicroseconds() {
  return (_tB - _tA);
}
t_timeval Stopwatch::deltaMicrosecondsRemainder() {
  return (_tB - _tA) % 1000;
}
t_string Stopwatch::toString(bool bFancy) {
  if (bFancy) {
    return DateTime::timeToStr(DateTime::getTime(deltaMilliseconds()));
  }
  else {

    return Stz "" + _strName + " " + StringUtil::format("%.2f", (float)deltaMilliseconds() + ((float)deltaMicrosecondsRemainder() / 1000.0f)) + "ms";
  }
}
void Stopwatch::print(t_string& st) {
  t_string str = st + toString() + "ms";
  Gu::print(str);
}
t_string Stopwatch::pulse(bool bLog, bool bFancy) {
  t_string ret = stop(bLog, bFancy);
  start();
  return ret;
}

bool Stopwatch::pulse(t_timeval isGreaterThanMs) {
  bool bRet;
  stop();
  {
    if (deltaMilliseconds() >= isGreaterThanMs)
      bRet = true;
    else
      bRet = false;
  }
  start();

  return bRet;
}

}//ns game
