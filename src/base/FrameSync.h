/**
*  @file FrameSync.h
*  @date November 27, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __FRAMESYNC_27052177462681769929099_H__
#define __FRAMESYNC_27052177462681769929099_H__

#include "../gfx/GfxHeader.h"
//#include "../scene/SceneHeader.h"
namespace BR2 {
/**
*  @class FrameSync
*  @brief Synchronize the number of updates or "frames" per second in a running application (i.e. in a while loop).
*/
class FrameSync : public VirtualMemory {
public:
  FrameSync(std::shared_ptr<GraphicsWindow> win);
  virtual ~FrameSync() override;
  void disable();
  void enable();
  bool isEnabled() { return !_bVsyncDisabled; }

  void syncBegin();    // Call before you do your frame update
  int syncEnd();        // Call after you do your frame update.

  float getCurrentFrameCap();

private:
  t_timeval _syncStart;
  t_timeval _syncEnd;
  bool _bVsyncDisabled = false;
  float _nFramesMax = 60.0f;//Max FPS - the cap if we want to cap it.  
  std::shared_ptr<GraphicsWindow> _pGraphicsWindow = nullptr;
  int64_t calculateWaitMicroseconds();
  bool shouldWait(int64_t& waitUs);
};

}//ns game



#endif
