/**
*  @file Spline.h
*  @date 20200313
*  @author MetalMario971
*/
#pragma once
#ifndef __SPLINE_1584159113247377914_H__
#define __SPLINE_1584159113247377914_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
namespace BR2 {
/**
*  @class Spline
*  @brief Base class for splines.  Currently we support Cubic Bezier and Linear.
*/
class Spline : public VirtualMemoryShared<Spline> {
public:
  Spline();
  virtual ~Spline() override;
  
  const std::vector<vec3>& points() { return _points; }
  double length();
  double length(size_t pointA, size_t pointB, double integral_t = 1.0 / 100.0);
  void calculate();

  virtual void setPoints(std::vector<vec3>& v, bool tangentialize, bool normalize) = 0;
  virtual void iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & vcur, const vec3 & vlast)> exec, double integral_t) = 0;
  virtual void eval(double t, vec3& __out_ pt) = 0;

protected:
  double _length = 1;
  std::vector<vec3> _points;
};


//class LinearSpline : public VirtualMemoryShared<Spline> {
//public:
//  LinearSpline(std::vector<vec3> points);
//  virtual ~LinearSpline() override;
//
//  virtual void setPoints(std::vector<vec3>& v, bool tangentialize = true, bool normalize = false) = 0;
//  virtual double length(size_t pointA, size_t pointB, double integral_t = 1.0 / 100.0) = 0;
//  virtual void iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & vcur, const vec3 & vlast)> exec, double integral_t) = 0;
//};


}//ns Game



#endif
