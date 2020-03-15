#include "../base/Logger.h"
#include "../math/CubicBezierSpline.h"

#include <algorithm>

namespace BR2 {
size_t seg_id(size_t pt_idx) {
  size_t seg = pt_idx / 3;
  return seg;
}
double seg_t(size_t pt_idx) {
  size_t seg = seg_id(pt_idx);
  double t = ((double)pt_idx - (double)(seg * 3)) / 3;
  return t;
}
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
void CubicBezierSpline::eval(double len, vec3& __out_ out_pt) {
  //Return a given point along the spline.
  //in: length along the spline from 0 to length()
  //out: point at that length
  if (len < 0) {
    len = 0;
  }
  if (len > length()) {
    len = length();
  }
  if (_points.size() == 0) {
    return;
  }
  if (_points.size() == 1) {
    out_pt = _points[0];
    return;
  }

  //Get spline segment
  int64_t pt_id = 0;
  for (int64_t pind = (int64_t)_plens.size() - 1; pind >= 0; --pind) {
    //Must be >= here in order to capture the endpoint segment.
    if (len >= _plens[pind]) {
      pt_id = pind;
      break;
    }
  }

  //Round down to neareset segment, and check for special endpoint case.
  size_t seg_a = pt_id / 3;
  size_t seg_b = pt_id / 3 + 1;
  size_t plen_idx_a = 0;
  size_t plen_idx_b = 0;
  size_t seg_count = (_points.size() / 3);
  if (seg_b > seg_count - 1) {
    plen_idx_b = _plens.size()-1;
  }
  else {
    plen_idx_b = seg_b * 3;
  }
  plen_idx_a = seg_a * 3;

  //Prevent crashing
  if (plen_idx_a >= _plens.size() || plen_idx_b >= _plens.size()) {
    BRLogError("Buffer overflow computing CB spline segment");
    if (_points.size()) {
      out_pt = _points[0];
    }
    Gu::debugBreak();
    return;
  }

  //Compute t --element of-- [0,1].  This is actually *incorrect* as it produces speed artifacts, but for now, it works.
  double len_a = _plens[plen_idx_a];
  double len_b = _plens[plen_idx_b];
  double segt = (len - len_a) / (len_b - len_a);
  eval(segt, seg_a, out_pt);
}
void CubicBezierSpline::eval(double t, size_t segment, vec3& __out_ pt) {
  if (t < 0.0 || t>1.0) {
    static bool bvbb = true;
    if (bvbb) {
      Gu::debugBreak();
    }
  }
  if (segment * 3 + 3 < _points.size()) {
    eval(t,
      _points[segment * 3 + 0],
      _points[segment * 3 + 1],
      _points[segment * 3 + 2],
      _points[segment * 3 + 3], pt);
  }
  else {
    BRLogError("Spline segment out of range.");
    Gu::debugBreak();
  }


}
void CubicBezierSpline::eval(double t, vec3& p0, vec3& p1, vec3& p2, vec3& p3, vec3& __out_ out_pt) {
  // Evaluate a bezier segment, returning the point along the curve as out_pt.
  // https://en.wikipedia.org/wiki/B%C3%A9zier_curve
  //  t e [0, 1]
  //  CB = 
  //  (1-t)^3 * p0 
  //   + 3 * (1-t)^2 * t *p1
  //   + 3 * (1-t) * t^2 * p2
  //   + t^3 * p3.
  double t1 = 1 - t;
  out_pt = (p0 * (double)pow(t1, 3)) + (p1 * (double)(t * pow(t1, 2) * 3)) + (p2 * (double)(pow(t, 2) * 3 * t1)) + (p3 * (double)(pow(t, 3)));
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

    if (m == 0) {
      m = 2;
    }
    else if (m == 2) {
      m = 1;
    }
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

void CubicBezierSpline::iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & vcur, const vec3 & vlast, size_t segmentIndex)> exec, double integral_t) {
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
  if (integral_t < 0.000001) {
    BRLogError("Spline integral step was too small.");
    Gu::debugBreak();
    return;
  }

  //Iterate all points (1/3) of bezier spline.
  size_t debug_nsteps = 0;
  double debug_len = 0;
  if (_points.size() > 0) {
    vec3 vcur;
    eval(seg_t(pointA), seg_id(pointA), vcur); //init
    vec3 vlast = vcur;
    for (size_t pt = pointA; pt < pointB; pt++) {
      size_t seg = seg_id(pt);
      double tPt = seg_t(pt);
      //each a-b is 1/3 of a bezier spline
      double maxt = (1.0 / 3.0);
      for (double t = 0; t <= maxt; t += integral_t) {
        t = std::min(t, maxt);
        eval(tPt + t, seg, vcur);
        exec(vcur, vlast, pt);
        debug_len += (vcur - vlast).length();
        vlast = vcur;
        debug_nsteps++;
      }
    }

    //Evaluate the spline endpoint.
    eval(1, seg_id(pointB) - 1, vcur);
    exec(vcur, vlast, pointB);
    debug_len += (vcur - vlast).length();
    debug_nsteps++;

  }

}


}//ns Game
