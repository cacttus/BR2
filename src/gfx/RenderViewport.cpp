#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GfxHeader.h"
#include "../gfx/RenderTarget.h"
#include "../math/MathAll.h"

namespace BR2 {
class RenderViewport_Internal {
public:
  std::unique_ptr<RenderViewport_Internal> _pint = nullptr;
  double _x;
  double _y;
  double _w;
  double _h;
  ViewportConstraint _constraint;
};
RenderViewport::RenderViewport(double w, double h, ViewportConstraint constraint) {
  _pint = std::make_unique<RenderViewport_Internal>();

  _pint->_constraint = constraint;
  _pint->_x = 0;
  _pint->_y = 0;
  _pint->_w = w;
  _pint->_h = h;
}
RenderViewport::~RenderViewport() {
  _pint = nullptr;
}
void RenderViewport::bind() {
  GLint ix = (GLint)getX();
  GLint iy = (GLint)getY();
  GLint iw = (GLint)getWidth();
  GLint ih = (GLint)getHeight();

  //Note: in GL x,y is the lower left corner.
  glViewport(ix, iy, iw, ih);

  //Scissor testing
  glEnable(GL_SCISSOR_TEST);
  glScissor(ix, iy, iw, ih);
}
void RenderViewport::updateBox(double sx, double sy, double sw, double sh) {
  if (_pint->_constraint == ViewportConstraint::Adjust) {
    double ar = Gu::getEngineConfig()->windowedAspectRatio();
    double vh = 1;
    double vw = 1;

    if (sw >= sh) {
      vh = sh;
      vw = vh * ar;
      _pint->_x = (sw - vw) * 0.5f;
      _pint->_y = sh - 0; //Note: in GL x,y is the lower left corner, so we subtract sh.
    }
    else {
      vw = sw;
      vh = sh / ar;
      _pint->_x = 0;
      _pint->_y = sh - ((sh - vh) * 0.5f); //Note: in GL x,y is the lower left corner, so we subtract sh.
    }

    _pint->_w = vw;
    _pint->_h = vh;
  }
  else if (_pint->_constraint == ViewportConstraint::Full) {
    _pint->_x = sx;
    _pint->_y = sy;
    _pint->_w = sw;
    _pint->_h = sh;
  }
}
double RenderViewport::getWidth() {
  return _pint->_w;
}
double RenderViewport::getHeight() {
  return _pint->_h;
}
double RenderViewport::getX() {
  return _pint->_x;
}
double RenderViewport::getY() {
  return _pint->_y;
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


}//ns BR2
