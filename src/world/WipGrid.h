/**
*  @file WipGrid.h
*  @date April 3, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __W25NOISEFIELD_14912799923863802711_H__
#define __W25NOISEFIELD_14912799923863802711_H__

#include "../base/Hash.h"
#include "../world/WorldHeader.h"

namespace BR2 {
class GData {
public:
  W25Geom _geom[GridMeshLayer::e::MaxMatters];
  W25Tile _tile[GridMeshLayer::e::MaxMatters];
  //Tile25Spec* _pTile[MatterMode::e::MaxMatters];
};
//class NoiseData {
//public:
//    float _fValue[MatterMode::e::MaxMatters];
//    W25Tile _tile[MatterMode::e::MaxMatters];
//   // Tile25Spec* _pTile[MatterMode::e::MaxMatters];
//};
/**
*  @class W25NoiseField
*  @brief createa  a random noise field for the "game world" triangle mesh
*/
class WipGrid : EasyNoise {
public:
  WipGrid(ivec3& viPos, LairSpec* pLair, int iSeed);
  virtual ~WipGrid();

  void generate();
  void makeCell(int ix, int iy, int iz, WorldCell* __in_ pc);
  //void makeNoiseForRegion(ivec3&);
 // float getValue(ivec3& iPos);
  void clear(Tile25Spec* pt);

public:
  //size_t _iNoiseArrSize;
  size_t _iGDataSize;
  GData* _pGData = nullptr;
  //NoiseData* _pNoiseValues = nullptr;
  LairSpec* _pLair = nullptr;
  ivec3 _viPos;

  //  void setDv(int ix, int iy, int iz, float v, MatterMode::e eMatter);
  //  float getDv(int ix, int iy, int iz, MatterMode::e eMatter);
  //  void makeLandAndSea();
  //  void walkers();
  //  float makeNoisePointSolid(int xx, int yy, int zz);
  //  float makeNoisePointLiquid(int xx, int yy, int zz);
  //float makeNoisePointGas(int xx, int yy, int zz);
//  float genPointLocal(int xx, int yy, int zz, MatterMode::e eMatterMode);
//  float getOrGenPoint(int g_ix, int g_iy, int g_iz, int c_ix, int c_iy, int c_iz, MatterMode::e eMatterMode);
//  W25Geom genGeom(int c_ix, int c_iy, int c_iz, MatterMode::e eMatterMode);
//  void compile();


};

}//ns Game



#endif
