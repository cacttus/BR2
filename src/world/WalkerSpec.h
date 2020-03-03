/**
*  @file WalkerSpec.h
*  @date May 1, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __WALKERSPEC_14936915721119879286_H__
#define __WALKERSPEC_14936915721119879286_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class WalkerSpec
*  @brief
*/
//BtLairWalkerSpec -> LairWalkerSpec
class WalkerSpec : public VirtualMemory {
  friend class ObFile;
public:
  WalkerSpec(string_t strName);
  virtual ~WalkerSpec() override;

  std::vector<LairTile*>& getLairTiles() { return _vecLairTiles; }
  string_t getName() { return _strName; }
  Hash32 getNameHashed() { return _iNameHashed; }
  Walker* createInstance();

private:
  string_t _strName;
  Hash32 _iNameHashed;
  std::vector<LairTile*> _vecLairTiles;
  WalkerType::e _eWalkerType;    // type
  LairGenLocale::e _eLocality; // glob/lair
  Tile25Spec* _pTileSpec = nullptr; // NULL if we're not a material walker. But this can really be set for any walker.
//  MpUint _mxCount;            // min/max count of wwalkers
  MpUint _mxBranches;            // lobe probability
  MpInt _mxHopCount;
  WalkerVolumeType::e _eVolumeType;
  MpVec3 _mxVolumeRadiusMin;
  MpVec3 _mxVolumeRadiusMax;
  //bool _bIsNew;    // if the walker is new i.e. not a frozen walker.
  //int32_t _iRngState;
  //ivec3 _viSuppliedPosition;
  //ivec3 _viReferencePosition;
  //std::vector<ivec3> _vecReferencePositions;
};

class Walker : public VirtualMemory {
  friend class WalkerSpec;
public:
  Walker(WalkerSpec* ps) : _pSpec(ps) {}
  virtual ~Walker() override {}
private:
  WalkerSpec* _pSpec = nullptr;
  uint32_t _uiCount;
  uint32_t _uiBranches;
};

}//ns Game



#endif
