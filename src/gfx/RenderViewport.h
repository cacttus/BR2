/**
*  @file Viewport.h
*  @date September 24, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __VIEWPORT_1483030407219963222425227_H__
#define __VIEWPORT_1483030407219963222425227_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
#include "../gfx/GfxHeader.h"
namespace BR2 {
enum class ViewportConstraint {
  Fixed,  //Does not scale with window.
  AdjustHeight,   //Fixes to window height, with width maintained aspect ratio.
  Fullscreen,   //Fixes to window width, and height.
};
/**
*  @class WindowViewport
*  @brief Viewport for rendering to a window.
*/
class RenderViewport : public VirtualMemoryShared<RenderViewport> {
public:
  RenderViewport(
    int32_t w
    , int32_t h
    , ViewportConstraint constraint = ViewportConstraint::AdjustHeight
  );
  virtual ~RenderViewport() override;

  void bind(std::shared_ptr<RenderTarget> target);
  void setHeight(int32_t h);
  void setWidth(int32_t w);
  void setX(int32_t x);
  void setY(int32_t y);
  int32_t getWidth();
  int32_t getHeight();
  int32_t getX();
  int32_t getY();
  float getAspectRatio();
  bool containsPointRelativeToWindow(vec2& mp);
  void updateBox(std::shared_ptr<RenderTarget> target);

private:
  Box3i _rect;
  Box3i _lastRect;
  ViewportConstraint _constraint;
};

}//ns BR2



#endif
