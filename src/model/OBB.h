/**
*  @file OBB.h
*  @date January 16, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __OBB_1516130353270368388_H__
#define __OBB_1516130353270368388_H__

#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class OBB
*  @brief Oriented bounding box.
*/
class OBB : public VirtualMemory {
public:
  OBB();
  virtual ~OBB() override;
  void setInvalid() { _bInvalid = true; }
  bool getInvalid() { return  _bInvalid; }
  void calc(mat4& mat, const Box3f* base);
  // void drawForward(UtilMeshInline& mi);
  vec3* getVerts() { return _verts; }
private:
  vec3 _verts[8];
  bool _bInvalid = false;
};

}//ns BR2



#endif
