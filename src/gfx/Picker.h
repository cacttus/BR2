/**
*  @file Picker.h
*  @date December 27, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __PICKER_15143971382984040801_H__
#define __PICKER_15143971382984040801_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class Picker
*  @brief Manages pixel-perfect picking.
*/
class Picker : public GLFramework {
public:
  //**Note do not set this to be anything but full alpha, if blending is enabled teh blend will screw up this value.
  static const uint32_t c_iInvalidPickId = 0;//0xFFFFFFFF;
public:
  Picker(std::shared_ptr<GLContext> pc, std::shared_ptr<RenderPipe> rp);
  virtual ~Picker() override;
  uint32_t getSelectedPixelId() { return _uiLastSelectedPixelId; }
  void update(std::shared_ptr<InputManager> pFingers);
  uint32_t genPickId();

private:
  uint32_t _iid = 0;
  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  uint32_t _uiLastSelectedPixelId = 0;//Note: This is relative to the last UserSelectionSet - the Id here is not fixed.

  void updatePickedPixel(int32_t x, int32_t y);
  void samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId);
};

}//ns Game



#endif
