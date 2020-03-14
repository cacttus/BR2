#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/InputManager.h"
#include "../model/UtilMeshInline.h"
#include "../base/BaseHeader.h"
#include "../world/Path.h"
#include "../math/CubicBezierSpline.h"

#include <algorithm>

namespace BR2 {
Path::Path(string_t name, std::shared_ptr<Spline> spline, float speed, float accel, PathEasing pe) : SceneNode(name, nullptr) {
  init();

  _easing = pe;
  _maxSpeed = speed;
  _accelleration = accel;

  setSpline(spline);
  start();
}
Path::~Path() {
}
void Path::setSpline(std::shared_ptr<Spline> sp) {
  _pSpline = sp;
  updateDebugDraw();
}
void Path::start() {
  _iCurSegment = 0;
  _curInterpolation = 0;
  if (_easing == PathEasing::EaseIn || _easing == PathEasing::EaseInAndOut) {
    _curSpeed = 0;
  }
  else {
    _curSpeed = _maxSpeed;
  }
  _running = true;
}
void Path::pause() {
  _running = false;
}
void Path::restart() {
  start();
}
void Path::update(float dt) {
  if (_pSpline) {

    if (_running) {

      //if (_easing == PathEasing::EaseIn || _easing == PathEasing::EaseInAndOut) {
      //  _curSpeed += _accelleration * dt;
      //}
      //_curT += dt;

      ////update cur segment
      //_curSpeed += _accelleration * dt;

      //if (_easing == PathEasing::EaseIn) {
      //  float easeInLength = _easeInPercent * _length;
      //  //Let's say, we are going to ease in the first 10% of the path.  
      //  //_accelleration
      //}
      //float easeOutLength = _easeOutPercent * _length;


      ////We will just use a naieve implementation without incremental timesteps (in case of huge lags)
      
      //**TDOO: figure htis out.  T may be part of the length, or the location?  Probably the lenght
      _pSpline->eval(_curT, _location);
    }
  }

}
vec3 Path::tangent() {
  //B'(t) = 3 * (1-t)^2 * (p1-p0) + 6*(1-t) * t * (p2-p1) + 3 * t^2 * (p3-p2)
  vec3 vRet;// = 3 * (1 - )
  return vRet;
}
void Path::updateDebugDraw() {
  _pDrawSpline = nullptr;
  float draw_step = 1.0 / 100.0;
  vec4 color;
  //
  color = Random::nextVec4(vec4(.2f, .2f, .2f, 1), vec4(1, 1, 1, 1));
  _pDrawSpline = std::make_shared<UtilMeshInline>(Gu::getCoreContext());
  _pDrawSpline->begin(GL_LINES);
  _pSpline->iterate(0, _pSpline->points().size() - 1,
    [&](const vec3& cur, const vec3& last) {
      if ((cur - last).length2() > 0.00001f) {
        _pDrawSpline->vt2(last, cur, &color);
      }
    }, draw_step);
  _pDrawSpline->end();

  //
  color = vec4(0.989f, 0.02f, 0.341f, 1);
  vec4 handleColor = vec4(0.989f, 0.92f, 0.041f, 1);
  _pDrawPoints = std::make_shared<UtilMeshInline>(Gu::getCoreContext());
  _pDrawPoints->begin(GL_POINTS);
  for (size_t i = 0; i < _pSpline->points().size(); i += 3) {
    _pDrawPoints->vt1(_pSpline->points()[i + 0], color);
    if (i + 2 < _pSpline->points().size()) {
      _pDrawPoints->vt1(_pSpline->points()[i + 1], handleColor);
      _pDrawPoints->vt1(_pSpline->points()[i + 2], handleColor);
    }
  }
  _pDrawPoints->end();

  //
  color = vec4(.03f, 0.21f, 0.899f, 1);
  _pDrawHandles = std::make_shared<UtilMeshInline>(Gu::getCoreContext());
  _pDrawHandles->begin(GL_LINES);
  vec3 h0, h1;
  for (size_t i = 0; i < _pSpline->points().size(); i += 3) {
    if (i + 1 < _pSpline->points().size()) {
      h0 = _pSpline->points()[i + 0];
      h1 = _pSpline->points()[i + 1];
      _pDrawHandles->vt2(h0, h1, &color);
    }

    if (i + 3 < _pSpline->points().size()) {
      h0 = _pSpline->points()[i + 2];
      h1 = _pSpline->points()[i + 3];
      _pDrawHandles->vt2(h0, h1, &color);
    }
  }
  _pDrawHandles->end();
}
void Path::drawForwardDebug(RenderParams& rp) {
  glLineWidth(1.5);
  if (_pDrawSpline) {
    _pDrawSpline->setModelMatrix(getWorld());
    _pDrawSpline->draw(rp.getCamera());
  }
  if (Gu::getGlobalInput()->keyPressOrDown(SDL_SCANCODE_F7)) {
    if (_pDrawPoints) {
      _pDrawPoints->setModelMatrix(getWorld());
      glPointSize(4.0);
      _pDrawPoints->draw(rp.getCamera());
    }
    if (_pDrawHandles) {
      _pDrawHandles->setModelMatrix(getWorld());
      _pDrawHandles->draw(rp.getCamera());
    }
  }

}



}//ns Game
