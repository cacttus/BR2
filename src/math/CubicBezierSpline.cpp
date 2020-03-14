#include "../base/Logger.h"
#include "../math/CubicBezierSpline.h"


namespace BR2 {
CubicBezierSpline::CubicBezierSpline(std::vector<vec3> points, bool tangentialize, bool normalize) {
  setPoints(points, tangentialize, normalize);
  if (tangentialize) {
    tangentializeControlPoints(points);
  }
  if (normalize) {
    normalizeControlPoints(points);
  }
}
CubicBezierSpline::~CubicBezierSpline() {

}
void CubicBezierSpline::eval(double t, vec3& __out_ pt) {
  //t is from 0, 1

  //**TODO**
  //double len  = length() * t;

}
void CubicBezierSpline::setPoints(std::vector<vec3>& v_in, bool tangentialize, bool normalize) {
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
void CubicBezierSpline::tangentializeControlPoints(std::vector<vec3>& __inout_ points) {
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
void CubicBezierSpline::normalizeControlPoints(std::vector<vec3>& __inout_ points, float length) {
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
size_t CubicBezierSpline::numSegments() {
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
void CubicBezierSpline::iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & vcur, const vec3 & vlast)> exec, double integral_t) {
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
void CubicBezierSpline::eval(double t, size_t segment, vec3& __out_ pt) {
  eval(t,
    _points[segment * 3 + 0],
    _points[segment * 3 + 1],
    _points[segment * 3 + 2],
    _points[segment * 3 + 3], pt);
}
void CubicBezierSpline::eval(double t, vec3& p0, vec3& p1, vec3& p2, vec3& p3, vec3& __out_ out_pt) {
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

}//ns Game
