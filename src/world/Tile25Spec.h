/**
*  @file Tile25Spec.h
*  @date May 1, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __TILE25SPEC_14936899433271466305_H__
#define __TILE25SPEC_14936899433271466305_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class Tile25Spec
*  @brief
*/
//send tiles to GPU as offsets. Copy this buffer per frame.
class Tile25Spec : public  VirtualMemory {
public:
  Tile25Spec(string_t name, uint8_t iIndex, GridMeshLayer::e em, std::shared_ptr<SpriteSpec> top, std::shared_ptr<SpriteSpec> side, std::shared_ptr<SpriteSpec> bot, ClimateSpec& cp, float fRarity);
  virtual ~Tile25Spec();

  ClimateSpec* getClimateParams() { return _pClimateSpec; }
  Hash32 getHashName() { return _iHashName; }
  GridMeshLayer::e getMatterMode() { return _eMatterMode; }

  string_t getName() { return _strName; }
  W25Tile getTileIndex() { return _iTile; }

  Sprite* getTop() { return _pTop; }
  Sprite* getSide() { return _pSide; }
  Sprite* getBot() { return _pBot; }
private:
  string_t _strName;
  Hash32 _iHashName = 0;
  //The Tile 25's are actually updated per frame, and their contents are copied to the thing.
  Sprite* _pTop = nullptr;
  Sprite* _pSide = nullptr;
  Sprite* _pBot = nullptr;

  uint8_t _iTile = 0;
  GridMeshLayer::e _eMatterMode = GridMeshLayer::e::Invalid;

  ClimateSpec* _pClimateSpec = nullptr;
  float _fRarity = 1.0f;
};
/**
*  @class MorphTile
*  @brief  This is a list of tiles IN ORDEr which morph into one another given heat, presure, moisture, sun
*/
class MorphTile : public VirtualMemory {
  string_t _strName;
  Hash32 _iNameHashed;
  std::set<Tile25Spec*> _setTileSpecs; //We do not own
public:
  std::set<Tile25Spec*>& getTileSpecs() { return _setTileSpecs; }
  Hash32 getNameHashed() { return _iNameHashed; }
  string_t getName() { return _strName; }
  MorphTile(string_t strName);
  virtual ~MorphTile() override;
};
/**
*    @class LairTile
*    @brief  Just a tile
*/
//Tile set that morphs from one to theother based on temperature, etc.


}//ns Game



#endif
