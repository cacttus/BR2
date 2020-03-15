/**
*  @file CubicBezierSpline.h
*  @date 20200313
*  @author MetalMario971
*/
#pragma once
#ifndef __CUBICBEZIERSPLINE_15841591132937611233_H__
#define __CUBICBEZIERSPLINE_15841591132937611233_H__

#include "../math/Spline.h"

namespace BR2 {
/**
*  @class CubicBezierSpline
*  @brief Cubic Bezier spline.  For the scenegraph implementaion see @cPath.
*  https://en.wikipedia.org/wiki/B%C3%A9zier_curve
*
*  Number of points must be 3n + 1, where 3n is the number of control segments.
* 
*           Control points
*      v     v              
*      *     *              
*   *            *    v      v  *
*   ^            ^    *      *  ^
*             Points            Endpoint (special)
*/
class CubicBezierSpline : public Spline {
public:
  CubicBezierSpline(std::vector<vec3> points, bool tangentialize = true, bool normalize = false);
  virtual ~CubicBezierSpline() override;

  virtual void iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & cur, const vec3 & last, size_t segmentIndex)>, double integral_t = 1.0 / 100.0) override;
  virtual void setPoints(std::vector<vec3>& v, bool tangentialize = true, bool normalize = false) override;
  virtual void eval(double len, vec3& __out_ pt) override;
  size_t numSegments();
private:
  std::map<size_t, double> _segLens;
  void eval(double t, size_t segment, vec3& __out_ pt);
  void eval(double t, vec3& p0, vec3& p1, vec3& p2, vec3& p3, vec3& __out_ pt);
  void tangentializeControlPoints(std::vector<vec3>& __inout_ points);
  void normalizeControlPoints(std::vector<vec3>& __inout_ points, float length = 1);
};

}//ns Game



#endif
