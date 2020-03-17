/**
*  @file RenderViewport.h
*  @date September 24, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __VIEWPORT_1483030407219963222425227_H__
#define __VIEWPORT_1483030407219963222425227_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {
/*
Technically there are 3 modes. "Full" wouldn't even be a mode if we wanted to correctly scale the aspect ratio.  But FULL is needed to render shadows, and other 
rendering.
  1. FIT INSIDE the window, maintaining AR or 
  2. FILL the whole window, maintaining AR
  To prevent the FILL from creating a massive buffer, then we just use glScissor to prevent drawing outside the box.

  FIT
  +---------------------+
  |  *            *     |
  |  |            |<game|
  |  *            *     |
  +--------------------+ < window. Game fits inside.  We see the whole game, but have black bars around it.

  FILL
  *--------------*
  |    |     |   |
  |    |window   |
  |    |     |   |
  *--------------* < game. Stretches beyond window to maintain AR, Centers the window.  No black bars, but, we lose some of the game.

*/
enum class ViewportConstraint {
  Adjust,   //Fixes to window height.  Maintains aspect ratio of 1920//
  Full,   //Fixes to window width, and height.
};
/**
*  @class RenderViewport
*  @brief Viewport for rendering to a window.
*/
class RenderViewport_Internal;
class RenderViewport : public VirtualMemoryShared<RenderViewport> {
public:
  RenderViewport(double w, double h, ViewportConstraint constraint);
  virtual ~RenderViewport() override;

  void bind();
  double getWidth();
  double getHeight();
  double getX();
  double getY();
  float getAspectRatio();
  void updateBox(double sx, double sy, double sw, double sh);

private:
  std::unique_ptr<RenderViewport_Internal> _pint = nullptr;
};

}//ns BR2



#endif
