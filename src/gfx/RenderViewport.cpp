#include "../base/Logger.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GfxHeader.h"
#include "../gfx/RenderTarget.h"
#include "../math/MathAll.h"

namespace BR2 {
class RenderViewport_Internal {
public:
  std::unique_ptr<RenderViewport_Internal> _pint = nullptr;
  Box3i _rect;
  Box3i _lastRect;
  ViewportConstraint _constraint;
};

RenderViewport::RenderViewport(int32_t w, int32_t h, ViewportConstraint constraint) {
  _pint = std::make_unique<RenderViewport_Internal>();

  _pint->_constraint = constraint;

  _pint->_rect._max.x = w;
  _pint->_rect._max.y = h;
  _pint->_rect._min.x = 0;
  _pint->_rect._min.y = 0;

  setWidth(w); // - Constrains the window to the aspect ratio.
  setHeight(h);

  _pint->_lastRect = 0;
}
RenderViewport::~RenderViewport() {
  _pint = nullptr;
}
void RenderViewport::bind(std::shared_ptr<RenderTarget> target) {
  if (target != nullptr) {
    updateBox(target);
  }

  int32_t ix = getX();
  int32_t iy = getY();
  int32_t iw = getWidth();
  int32_t ih = getHeight();

  glViewport(ix, iy, iw, ih);

  //Scissor testing
  glScissor(ix, iy, iw, ih);
}
void RenderViewport::updateBox(std::shared_ptr<RenderTarget> target) {
  if (_pint->_constraint == ViewportConstraint::Fixed) {
    //Note: we should place viewport in the center of the screen.
  }
  else if (_pint->_constraint == ViewportConstraint::AdjustHeight) {
  }
  else if (_pint->_constraint == ViewportConstraint::Fullscreen) {
    if (target) {
      _pint->_rect._min.x = 0;
      _pint->_rect._min.y = 0;
      _pint->_rect._max.x = target->getWidth();
      _pint->_rect._max.y = target->getHeight();
    }
    else {
      BRThrowException("Failed to find fullscreen target for updateBox");
    }
  }
  _pint->_lastRect = _pint->_rect;
}
void RenderViewport::setX(int32_t x) {
  _pint->_rect._min.x = x;
}
void RenderViewport::setY(int32_t y) {
  _pint->_rect._min.y = y;
}
void RenderViewport::setHeight(int32_t h) {
  _pint->_rect._max.y = h;
}
void RenderViewport::setWidth(int32_t w) {
  _pint->_rect._max.x = w;
}
int32_t RenderViewport::getWidth() {
  return _pint->_rect._max.x;
}
int32_t RenderViewport::getHeight() {
  return _pint->_rect._max.y;
}
int32_t RenderViewport::getX() {
  return _pint->_rect._min.x;
}
int32_t RenderViewport::getY() {
  return _pint->_rect._min.y;
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



}//ns BR2
