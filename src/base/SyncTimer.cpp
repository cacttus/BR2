#include "../base/SyncTimer.h"
#include "../base/Gu.h"


namespace BR2 {
SyncTimer::SyncTimer(bool start) {
  _bRunning = start;
}
SyncTimer::~SyncTimer() {
}
void SyncTimer::start() {
  _bRunning = true;
}
void SyncTimer::stop() {
  _bRunning = false;
}
void SyncTimer::tick(uint64_t duration, std::function<void()> func) {
  //IMO makes more sense to put the tick lambda in the update routine.
  //It allows you to dynamically update the time or lambda if you want. Also putting the lambda where the code
  //is actually run is easier to understand.  Further, you don't need to store the duration or the lambda in this structure.
  if (_bRunning) {
    t_timeval cur = Gu::getMilliSeconds();
    if (cur - _last_tick >= duration) {
      func();
      _last_tick = cur;
    }
  }
}








}//ns Game
