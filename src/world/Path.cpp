#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/InputManager.h"
#include "../base/BaseHeader.h"
#include "../model/UtilMeshInline.h"
#include "../world/Path.h"

#include <algorithm>

namespace BR2 {
Path::Path(string_t name, std::vector<vec3> pts, bool tangentializeControlPoints, bool normalizeControlPoints, float speedMetersPerSecond, float accelerationMetersPerSecondSquared, SplineType pm, PathEasing pe) : SceneNode(name, nullptr) {
  init();

  _movement = pm;
  _easing = pe;

  _maxSpeed = speedMetersPerSecond;
  _accelleration = accelerationMetersPerSecondSquared;

  //Calculates the bezier curve
  setPoints(pts, tangentializeControlPoints, normalizeControlPoints);
  start();
}
Path::~Path() {

}
void Path::setPoints(std::vector<vec3>& v_in, bool tangentialize, bool normalize) {
  //copy points
  _points.clear();
  _points.swap(v_in);

  //Truncate points, removing any extras
  size_t m = _points.size() % 3;
  if (m != 1) {
    BRLogWarn("Curve point count was not 3n+1, Truncating to 3n+1");

    if (_points.size() < 4) {
      BRLogWarn("Bezier segment did not have at least 4 points.");
    }

    if (m == 0) { m = 2; } //1 point missing, so remove 3
    if (m == 1) {} //ok, satifies 3n + 1
    if (m == 2) { m = 1; } // 3 points missing, remove 2
    for (size_t n = 1; n <= m; n++) {
      _points.erase(_points.begin() + _points.size() - n);
    }
  }

  if (tangentialize) {
    tangentializeControlPoints(_points);
  }
  if (normalize) {
    normalizeControlPoints(_points);
  }
  calculate();
}
void Path::tangentializeControlPoints(std::vector<vec3>& __inout_ points) {
  //Tangentialize all control points, preserving the control point length
  // We can do this multiple ways.  We take an approach of using the first control point, in reference to the previosu one, here.
  for (size_t ip = 0; ip < points.size(); ip += 3) {
    if ((int64_t)ip - 1 >= 0 && ip + 1 < points.size()) {
      float cp_length = (points[ip - 1] - points[ip]).length();
      vec3 n = (points[ip] - points[ip - 1]).normalize();
      points[ip + 1] = points[ip] + (n * cp_length);
    }
  }
}
void Path::normalizeControlPoints(std::vector<vec3>& __inout_ points, float length) {
  //Normalize all control points to the given length
  for (size_t ip = 0; ip < points.size(); ip += 3) {
    if (ip + 1 < points.size()) {
      points[ip + 1] = points[ip] + (points[ip + 1] - points[ip]).normalize() * length;
    }

    if (ip + 3 < points.size()) {
      points[ip + 2] = points[ip + 3] + (points[ip + 2] - points[ip + 3]).normalize() * length;
    }
  }
}
size_t Path::numSegments() {
  return _points.size() / 3;
}
void seg_t(size_t pt_idx, size_t& __out_ seg, float& __out_ t) {
  //Returns the given segment number, and t [0,1] value of the given point.
  seg = pt_idx / 3;
  if (pt_idx > 0 && pt_idx % 3 == 0) {
    //Keep segment endpoints within the previous segment
    seg -= 1;
  }
  t = ((float)pt_idx - (float)(seg * 3)) / 3;
}
void Path::iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & vcur, const vec3 & vlast)> exec, double integral_t) {
  //Iterates over the spline integral, giving the lambda the cur, and last points.
  if (pointA >= _points.size()) {
    BRLogError("Point '" + pointA + "' outside range '" + (_points.size() == 0 ? 0 : (_points.size() - 1)) + "'");
    Gu::debugBreak();
    return;
  }
  if (pointB >= _points.size()) {
    BRLogError("Point '" + pointB + "' outside range '" + (_points.size() == 0 ? 0 : (_points.size() - 1)) + "'");
    Gu::debugBreak();
    return;
  }
  if (integral_t < 0.00000001) {
    BRLogError("Spline integral step was too small.");
    Gu::debugBreak();
    return;
  }

  size_t segA, segB;
  float tA, tB;
  seg_t(pointA, segA, tA);
  seg_t(pointB, segB, tB);
  size_t debug_nsteps = 0;
  if (_points.size() > 0) {
    vec3 vcur;
    vec3 vlast;
    eval(tA, segA, vcur); //init
    vlast = vcur;
    for (size_t iseg = segA; iseg <= segB; ++iseg) {
      for (double t = 0; t <= 1; t += integral_t) {
        eval(t, iseg, vcur);
        exec(vcur, vlast);
        //_length += (vcur - vlast).lengthd();
        vlast = vcur;
        debug_nsteps++;
      }
    }
  }

}
double Path::length(size_t pointA, size_t pointB, double integral_t) {
  double len = 0;
  iterate(pointA, pointB,
    [&len](const vec3& cur, const vec3& last) {
      len += (cur - last).lengthd();
    }, integral_t);
  return len;
}
void Path::calculate() {
  //Calculate length
  _length = length(0, _points.size() - 1);
  _pDrawSpline = nullptr;

  float draw_step = 1.0 / 100.0;

  vec4 color;

  //
  color = Random::nextVec4(vec4(.2f, .2f, .2f, 1), vec4(1, 1, 1, 1));
  _pDrawSpline = std::make_shared<UtilMeshInline>(Gu::getCoreContext());
  _pDrawSpline->begin(GL_LINES);
  iterate(0, _points.size() - 1,
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
  for (size_t i = 0; i < _points.size(); i += 3) {
    _pDrawPoints->vt1(_points[i + 0], color);
    if (i + 2 < _points.size()) {
      _pDrawPoints->vt1(_points[i + 1], handleColor);
      _pDrawPoints->vt1(_points[i + 2], handleColor);
    }
  }
  _pDrawPoints->end();

  //
  color = vec4(.03f, 0.21f, 0.899f, 1);
  _pDrawHandles = std::make_shared<UtilMeshInline>(Gu::getCoreContext());
  _pDrawHandles->begin(GL_LINES);
  vec3 h0, h1;
  for (size_t i = 0; i < _points.size(); i += 3) {
    if (i + 1 < _points.size()) {
      h0 = _points[i + 0];
      h1 = _points[i + 1];
      _pDrawHandles->vt2(h0, h1, &color);
    }

    if (i + 3 < _points.size()) {
      h0 = _points[i + 2];
      h1 = _points[i + 3];
      _pDrawHandles->vt2(h0, h1, &color);
    }
  }
  _pDrawHandles->end();

}
void Path::eval(double t, size_t segment, vec3& __out_ pt) {
  eval(t,
    _points[segment * 3 + 0],
    _points[segment * 3 + 1],
    _points[segment * 3 + 2],
    _points[segment * 3 + 3], pt);
}
void Path::eval(double t, vec3& p0, vec3& p1, vec3& p2, vec3& p3, vec3& __out_ out_pt) {
  // https://en.wikipedia.org/wiki/B%C3%A9zier_curve
  // Honestly a dvec3 would provide greater precision.
  //  t e [0, 1]
  //  CB = 
  //  (1-t)^3 * p0 
  //   + 3 * (1-t)^2 * t *p1
  //   + 3 * (1-t) * t^2 * p2
  //   + t^3 * p3.
  double t1 = 1 - t;
  out_pt = (p0 * (float)pow(t1, 3)) + (p1 * (float)(t * pow(t1, 2) * 3)) + (p2 * (float)(pow(t, 2) * 3 * t1)) + (p3 * (float)(pow(t, 3)));
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
  if (_running) {

    if (_easing == PathEasing::EaseIn || _easing == PathEasing::EaseInAndOut) {
      _curSpeed += _accelleration * dt;
    }
    _curT += dt;

    //update cur segment
    _curSpeed += _accelleration * dt;

    if (_easing == PathEasing::EaseIn) {
      float easeInLength = _easeInPercent * _length;
      //Let's say, we are going to ease in the first 10% of the path.  
      //_accelleration
    }
    float easeOutLength = _easeOutPercent * _length;


    //We will just use a naieve implementation without incremental timesteps (in case of huge lags)
    eval(_curT, _iCurSegment, _location);
  }
}
vec3 Path::tangent() {
  //B'(t) = 3 * (1-t)^2 * (p1-p0) + 6*(1-t) * t * (p2-p1) + 3 * t^2 * (p3-p2)
  vec3 vRet;// = 3 * (1 - )
  return vRet;
}
void Path::drawForwardDebug(RenderParams& rp) {
  glLineWidth(1.5);
  if (_pDrawSpline) {
    _pDrawSpline->setModelMatrix(getWorld());
    _pDrawSpline->draw(rp.getCamera());
  }
  if (Gu::getGlobalInput()->keyPressOrDown(SDL_SCANCODE_F3)) {

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
