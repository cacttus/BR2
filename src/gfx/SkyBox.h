/**
*  @file HappySky.h
*  @date December 9, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __HAPPYSKY_14813313153843142444_H__
#define __HAPPYSKY_14813313153843142444_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
namespace Game {
/**
*    @class HappySky
*    @brief
*
*/
class SkyBox : public VirtualMemory {
public:
  SkyBox();
  virtual ~SkyBox() override;
  void init(std::shared_ptr<Atlas> pAtlas, float fBoxDiagonalSize2, bool bDeferred);
  void draw(RenderParams& rp);

  float getSize() { return _fSize; }
private:
  std::shared_ptr<MeshNode> _pMesh;
  std::shared_ptr<Atlas> _pAtlas;
  float _fSize = 1;// 1/2 the box diagonal

  void side(std::vector<v_v3n3x2>* verts, std::vector<v_index32>* inds,
    int32_t& iOff, vec3& du, vec3& dv, float siz, vec3& origin, int eMat);

};

}//ns Game



#endif
