#include "../gfx/RenderViewport.h"
#include "../gfx/GfxHeader.h"
#include "../base/GraphicsWindow.h"

namespace BR2 {
RenderViewport::RenderViewport(int32_t w, int32_t h, ViewportConstraint constraint) {
  _constraint = constraint;

  _rect._max.x = w;
  _rect._max.y = h;
  _rect._min.x = 0;
  _rect._min.y = 0;

  setWidth(w); // - Constrains the window to the aspect ratio.
  setHeight(h);

  _lastRect = 0;
}
RenderViewport::~RenderViewport() {
}
void RenderViewport::bind(std::shared_ptr<RenderTarget> target) {
  updateBox(target);

  int32_t ix = getX();
  int32_t iy = getY();
  int32_t iw = getWidth();
  int32_t ih = getHeight();

  glViewport(ix, iy, iw, ih);

  //Scissor testing
  glScissor(ix, iy, iw, ih);

}
void RenderViewport::updateBox(std::shared_ptr<RenderTarget> target) {
  if (_constraint == ViewportConstraint::Fixed) {
  }
  else if (_constraint == ViewportConstraint::AdjustHeight) {
  }
  else if (_constraint == ViewportConstraint::Fullscreen) {
    _rect._min.x = 0;
    _rect._min.y = 0;
    _rect._max.x = target->getWidth();
    _rect._max.y = target->getHeight();

  }
  _lastRect = _rect;
}
void RenderViewport::setHeight(int32_t h) {
  _rect._max.y = h;
}
void RenderViewport::setWidth(int32_t w) {
  _rect._max.x = w;
}
int32_t RenderViewport::getWidth() {
  return _rect._max.x;
}
int32_t RenderViewport::getHeight() {
  return _rect._max.y;
}
int32_t RenderViewport::getX() {
  return _rect._min.x;
}
int32_t RenderViewport::getY() {
  return _rect._min.y;
}
void RenderViewport::setX(int32_t x) {
  _rect._min.x = x;
}
void RenderViewport::setY(int32_t y) {
  _rect._min.y = y;
}
float RenderViewport::getAspectRatio() {
  float ar = 0;
  if (getHeight() == 0) {
    ar = 1.0f;
  }
  else {
    ar = (float)getWidth() / (float)getHeight();
  }
  return ar;
}
bool RenderViewport::containsPointRelativeToWindow(vec2& mp) {
  if (mp.x < 0.0f) {
    return false;
  }
  if (mp.x > getWidth() - 1) {
    return false;
  }
  if (mp.y < 0.0f) {
    return false;
  }
  if (mp.y > getHeight() - 1) {
    return false;
  }
  return true;
}



}//ns game
