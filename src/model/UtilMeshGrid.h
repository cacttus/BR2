/**
*  @file UtilMeshGrid.h
*  @date November 21, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __UTILMESHGRID_14540990129246418912863_H__
#define __UTILMESHGRID_14540990129246418912863_H__

#include "../model/UtilMesh.h"

namespace BR2 {
/**
*  @class UtilMeshGrid
*  @brief Draws a grid.
*/
class UtilMeshGrid : public UtilMesh {
public:
  UtilMeshGrid(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> pc, float r = 1.0f, float g = 1.0f, float b = 1.0f,
    int32_t nSlices = 60,
    float fSliceWidth = 20.0f,
    Vector3& center = Vector3(0, 0, 0));
  virtual ~UtilMeshGrid() override;

  virtual void generate();
  virtual void preDraw();
  virtual void postDraw();

private:
  Color4f _color;
  int32_t _nSlices;
  float _fSliceWidth;
  Vec3f _center;
  float _lineWidth;
};

}//ns BR2



#endif
