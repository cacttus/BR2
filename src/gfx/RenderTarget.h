/**
*  @file RenderTarget.h
*  @date 20200225
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERTARGET_158266575555650870_H__
#define __RENDERTARGET_158266575555650870_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class RenderTarget
*  @brief Interface for rendering targets: GL buffer, or window surface.
*/
class RenderTarget : public GLFramework {
public:
  RenderTarget(std::shared_ptr<GLContext> ct);
  virtual ~RenderTarget() override;

  virtual int32_t getWidth() = 0;
  virtual int32_t getHeight() = 0;
};

}//ns Game



#endif
