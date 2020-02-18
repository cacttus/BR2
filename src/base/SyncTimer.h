/**
*  @file SyncTimer.h
*  @date August 7, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __SYNCTIMER_1565162047291700213_H__
#define __SYNCTIMER_1565162047291700213_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class SyncTimer
*  @brief  A timer that runs synchronous with the game clock.  Updates to the lambda get called on the engine's update() routine.
*/
class SyncTimer : public VirtualMemory {
public:
  SyncTimer(bool start = true);
  virtual ~SyncTimer() override;

  void start();
  void stop();
  void tick(uint64_t duration, std::function<void()> x); // Call this to update the timer.
private:
  t_timeval _last_tick = 0;
  bool _bRunning = false;

};

}//ns BR2



#endif
