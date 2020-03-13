/**
*  @file Path.h
*  @date 20200312
*  @author MetalMario971
*/
#pragma once
#ifndef __PATH_15840713463338944896_H__
#define __PATH_15840713463338944896_H__

#include "../world/WorldHeader.h"
#include "../model/SceneNode.h"

#include <vector>
namespace BR2 {
/**
*  @class Path
*  @brief Basic path class allowing for bezier, or linear interpolations.  Paths are manually updated, you are responsible for calling update().  From what I've read cubic bezier curves
*   are generally far better for eveyrthing than  quadratic ones. https://stackoverflow.com/questions/18814022/what-is-the-difference-between-cubic-bezier-and-quadratic-bezier-and-their-use-c
*   Monotonic Curvature.
*  https://en.wikipedia.org/wiki/B%C3%A9zier_curve
*
*  # of points must be 3n + 1, where 3n is the number of control segments.
*        control points
*      v     v        |      |
*      *     *        |      |
*   *            *    v      v  *
*   ^            ^    *      *  ^
*   |            |              |
*           endpoints
*
*/
class Path : public SceneNode {
public:
  Path(string_t name, std::vector<vec3> pts, bool tangentializeControlPoints, bool normalizeControlPoints, float speedMetersPerSecond = 3, float accelerationMetersPerSecondSquared = 1,
    SplineType pm = SplineType::CubicBezier, PathEasing pe = PathEasing::EaseInAndOut);
  virtual ~Path() override;


  const std::vector<vec3>& points() { return _points; }
  void setPoints(std::vector<vec3>& v, bool tangentialize = true, bool normalize = false);
  void update(float dt);

  virtual void drawForwardDebug(RenderParams& rp) override;

  void start();
  void restart();
  void pause();
  vec3 tangent();
  vec3 location() { return _location; }
  size_t numSegments();
private:
  float _curT = 0;
  double _length = 1;
  float _t = 0;
  float _curSpeed = 0.0f;
  vec3 _location;

  int _iCurSegment = 0;//Current bezier segment
  //int _curPointIndex = 0;
  float _curInterpolation = 0.f;
  //  vec3 _tangent;
  float _maxSpeed = 1.0f;
  float _accelleration = 1.0f;
  SplineType _movement = SplineType::CubicBezier;
  PathEasing _easing = PathEasing::EaseInAndOut;

  bool _running = false;

  float _easeInPercent = 0.05;
  float _easeOutPercent = 0.05;

  std::vector<vec3> _points;
  std::shared_ptr<UtilMeshInline> _pDrawSpline = nullptr;
  std::shared_ptr<UtilMeshInline> _pDrawPoints = nullptr;
  std::shared_ptr<UtilMeshInline> _pDrawHandles = nullptr;

  void calculate();
  void eval(double t, size_t segment, vec3& __out_ pt);
  void eval(double t, vec3& p0, vec3& p1, vec3& p2, vec3& p3, vec3& __out_ pt);
  double length(size_t pointA, size_t pointB, double integral_t = 1.0 / 100.0);
  void iterate(size_t pointA, size_t pointB, std::function<void(const vec3 & cur, const vec3 & last)>, double integral_t = 1.0 / 100.0);
  void tangentializeControlPoints(std::vector<vec3>& __inout_ points);
  void normalizeControlPoints(std::vector<vec3>& __inout_ points, float length = 1);

};

}//ns Game



#endif
