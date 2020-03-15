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
*  @brief The node-based implementation of motion along a fixed geometric path, supporting speed, accelleration and easing.
*/
class Path : public SceneNode {
public:
  Path(string_t name, std::shared_ptr<Spline> spline, float speed = 3, float accel = 1, PathEasing pe = PathEasing::None);
  virtual ~Path() override;

  void update(float dt);

  virtual void drawForwardDebug(RenderParams& rp) override;

  void start();
  void restart();
  void pause();
  vec3 tangent();
  vec3 location() { return _curPoint; }
  void setSpline(std::shared_ptr<Spline> sp);

private:
  std::shared_ptr<Spline> _pSpline = nullptr;

  float _curLen = 0;
  float _curSpeed = 5.0f;
  vec3 _curPoint= vec3(0,0,0);

  //int _iCurSegment = 0;//Current bezier segment
  float _curInterpolation = 0.f;
  float _maxSpeed = 1.0f;
  float _accelleration = 1.0f;
  PathEasing _easing = PathEasing::EaseInAndOut;

  bool _running = false;
  float _easeInPercent = 0.05;
  float _easeOutPercent = 0.05;

  std::shared_ptr<UtilMeshInline> _pDrawSpline = nullptr;
  std::shared_ptr<UtilMeshInline> _pDrawPoints = nullptr;
  std::shared_ptr<UtilMeshInline> _pDrawHandles = nullptr;
  std::shared_ptr<UtilMeshBox> _pDrawBox = nullptr;
  Box3f _box;
  void updateDebugDraw();
};

}//ns Game



#endif
