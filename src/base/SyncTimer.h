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
    t_timeval _last_tick= 0;
    bool _bRunning = false;
public:
    void start();
    void stop();
    void tick(uint64_t duration, std::function<void()> x); // Call this to update the timer.

    SyncTimer( bool start=true);
    virtual ~SyncTimer() override;
};

}//ns Game



#endif
