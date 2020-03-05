/**
*  @file Script.h
*  @date 20200304
*  @author MetalMario971
*/
#pragma once
#ifndef __SCRIPT_15833810871154937548_H__
#define __SCRIPT_15833810871154937548_H__

#include "../world/Component.h"

namespace BR2 {
/**
*  @class Script
*  @brief Base script component. This class by itself is the interface for compiled (c++) scripts.
*/
class Script : public Component {
public:
  Script();
  virtual ~Script() override;

  virtual void onDrawShadow(RenderParams& rp) {}
  virtual void onDrawDeferred(RenderParams& rp) {}
  virtual void onDrawForward(RenderParams& rp) {}
  virtual void onDrawDebug(RenderParams& rp) {}
  virtual void onDrawNonDepth(RenderParams& rp) {}
  virtual void onDrawTransparent(RenderParams& rp) {}
  virtual void onDrawUi(RenderParams& rp) {}
  virtual void onIdle(t_timeval ms) {}
  virtual void onScreenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) {}
  virtual void onServerPacketReceived(std::shared_ptr<Packet> pack) {}

};

}//ns Game



#endif
