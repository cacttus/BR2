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
*  @brief A pixel-perfect pick buffer.
*/
class Picker : public VirtualMemory {
public:
  Picker(std::shared_ptr<RenderPipeline> pc);
  virtual ~Picker() override;
  uint32_t getSelectedPixelId() { return _uiLastSelectedPixelId; }
  void update(std::shared_ptr<InputManager> pInput);
  uint32_t genPickId();

private:
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;
  uint32_t _iid = 0;
  uint32_t _uiLastSelectedPixelId;//Note: This is relative to the last UserSelectionSet - the Id here is not fixed.
  void updatePickedPixel(int32_t x, int32_t y);
  void samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId);
};

}//ns Game



#endif
