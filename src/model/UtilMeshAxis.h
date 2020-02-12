/**
*
*    @file UtilMeshAxis.h
*    @date November 21, 2015
*    @author MetalMario971
*
*    © 2015
*
*
*/
#pragma once
#ifndef __UTILMESHAXIS_1863520755176052563980_H__
#define __UTILMESHAXIS_1863520755176052563980_H__

#include "../model/UtilMesh.h"

namespace BR2 {
/**
*  @class UtilMeshAxis
*  @brief draws an axis centered in the world.
*/
class UtilMeshAxis : public UtilMesh {
public:
  virtual void generate() override;
  virtual void preDraw() override;
  virtual void postDraw() override;

  UtilMeshAxis(std::shared_ptr<GLContext> ctx,
    float scale = 10.0f, float lineWidth = 1.0f, mat4& transform = mat4::identity());

  virtual ~UtilMeshAxis() override;
private:
  float _scale, _lineWidth;
  mat4 _mTransform;

};

}//ns game



#endif
