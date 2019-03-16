/**
*
*    @file FrameSync.h
*    @date November 27, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __FRAMESYNC_27052177462681769929099_H__
#define __FRAMESYNC_27052177462681769929099_H__

#include "../display/DisplayHeader.h"
//#include "../scene/SceneHeader.h"
namespace Game {;
/**
*    @class FrameSync
*    @brief Synchronize the number of updates or "frames" per second in a running application (i.e. in a while loop).
*/
class FrameSync : public VirtualMemory {
private:
    std::shared_ptr<GLContext> _pContext = nullptr;
    t_timeval _syncStart;
    t_timeval _syncEnd;
    bool _bVsyncDisabled = false;
    float _nFramesMax = 60.0f;//Max FPS - the cap if we want to cap it.  
    int64_t calculateWaitMicroseconds();
    bool shouldWait(int64_t& waitUs);
public:
    void disable() ;
    void enable()  ;
    FORCE_INLINE bool isEnabled() { return !_bVsyncDisabled; }

    void syncBegin();    // Call before you do your frame update
    int syncEnd();        // Call after you do your frame update.

    float getCurrentFrameCap();

    //+-- CTOR/DTOR --+
    FrameSync(std::shared_ptr<GLContext> ct);
    virtual ~FrameSync() override ;
};

}//ns game



#endif
