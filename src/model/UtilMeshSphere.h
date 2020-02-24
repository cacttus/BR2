/**
*  @file UtilMeshSphere.h
*  @date December 12, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __UTILMESHSPHERE_1453030652222211530917323_H__
#define __UTILMESHSPHERE_1453030652222211530917323_H__

#include "../model/UtilMesh.h"

namespace BR2 {
/**
*  @class UtilMeshSphere
*  @brief
*/
class UtilMeshSphere : public UtilMesh {
public:
  UtilMeshSphere(std::shared_ptr<GLContext> ctx, float radius, vec3& vOffset, vec4& vColor, int32_t nSlices, int32_t nStacks);
  virtual ~UtilMeshSphere() override;

  virtual void generate() override;
  virtual void preDraw() override;
  virtual void postDraw() override;

private:
    float _fRadius;
    vec3 _vOffset;
    vec4 _vColor;
    bool _blnWireFrame;
    std::shared_ptr<MeshSpec> _pSpec = nullptr; 
    int32_t _nSlices,_nStacks;
};

}//ns game



#endif
