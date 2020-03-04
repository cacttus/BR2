/**
*  @file ObFile.h
*  @date March 30, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __OBFILE_1490906948419075780_H__
#define __OBFILE_1490906948419075780_H__

#include "../base/PoundFile.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @file ObFile
*  @brief The main file for game settings.
*/
class ObFile : public PoundFile {
  typedef std::map<Hash32, std::shared_ptr<SpriteSpec>> MotionSpecMap;
public:
  ObFile(std::shared_ptr<Scene> ps);
  virtual ~ObFile() override;

  std::vector<WalkerSpec*>& getWalkerSpecs() { return _vecWalkers; }
  std::vector<LairSpec*>& getLairSpecs() { return _vecLairs; }
  std::vector<std::shared_ptr<WorldObjectSpec>>& getMobSpecs() { return _vecWorldObjs; }

  std::shared_ptr<W25Config> getW25Config() { return _pW25Config; }
  //std::vector<WorldObjectSpec*>& getPixObjSpecs() { return _vecPixObjSpecs; }
  std::vector<std::shared_ptr<SpriteSpec>>& getMotions() { return _vecMotionSpecs; }
  std::vector<Tile25Spec*>& getTiles() { return _vecTileSpecs; }
  //std::vector<ObModel*>& getModels() { return _vecModelSpecs; }
  std::shared_ptr<SpriteBucket> getBucket() { return _pBucket; }

protected:
  std::shared_ptr<Scene> _pScene=nullptr;
  std::shared_ptr<SpriteBucket> _pBucket = nullptr;
  std::vector<std::shared_ptr<SpriteSpec>> _vecMotionSpecs;
  std::vector<std::shared_ptr<WorldObjectSpec>> _vecWorldObjs;
  std::vector<Tile25Spec*> _vecTileSpecs;
  std::vector<WalkerSpec*> _vecWalkers;
  std::vector<LairSpec*> _vecLairs;
  std::vector<MorphTile*> _vecMorphTiles;
  MorphTile* _pCurMorphTile = nullptr;
  std::shared_ptr<WorldObjectSpec> _pCurObjSpec = nullptr;
  bool _bCurSpecValid = true;
  LairSpec* _pCurLairSpec = nullptr;
  WalkerSpec* _pCurWalkerSpec = nullptr;
  std::shared_ptr<W25Config> _pW25Config = nullptr;

  virtual void pkp(std::vector<string_t>& tokens);
  virtual void preLoad();
  virtual void postLoad();

  void parseConfig(std::vector<string_t>& tokens);
  void parseVer(std::vector<string_t>& tokens);
  void parseSprites(std::vector<string_t>& tokens);
  void parseMobs(std::vector<string_t>& tokens);
  void parseTiles(std::vector<string_t>& tokens);
  void parseMorphTiles(std::vector<string_t>& tokens);
  void parseObjects(std::vector<string_t>& tokens);
  void parseWalkers(std::vector<string_t>& tokens);
  void parseLairs(std::vector<string_t>& tokens);

  std::shared_ptr<SpriteSpec> getMot(string_t& name);
  void parseGetSetMotion(string_t& tok, string_t& msName, std::shared_ptr<SpriteSpec>& ms);
  void processMotion(string_t name, std::vector<string_t>& files, float fDuration, bool bLoop, bool bRandomSel);
  void processTile(string_t name, int32_t index, GridMeshLayer::e eMatterMode,
    std::shared_ptr<SpriteSpec> pTop, std::shared_ptr<SpriteSpec> pSide, std::shared_ptr<SpriteSpec> pBot, ClimateSpec& cp, float rarity);
  //void removeMotion(Motion25Spec* ps);
  void processShiftMotionImage(std::shared_ptr<SpriteSpec> ps, bool shiftH, bool shiftV);
  Tile25Spec* getTileSpecByName(string_t name);
  LairGenLocale::e parseGenLocale(string_t str);
  LairTile* parseLairTile(std::vector<string_t>& tokens, string_t specName, int& iind);
  MpVec3i parse_mxv3(std::vector<string_t>& tokens, string_t specName, int& iind);
  MpUint parse_mxui2(std::vector<string_t>& tokens, string_t specName, int& iind);
  MpFloat parse_mxf2(std::vector<string_t>& tokens, string_t specName, int& iind);
  int32_t parseNbr_int(std::vector<string_t>& tokens, int& iind);
  uint32_t parseNbr_uint(std::vector<string_t>& tokens, int& iind);
  float parseNbr_float(std::vector<string_t>& tokens, int& iind);
  MorphTile* getMorphTileGroupByName(string_t);
  ClimateSpec parseClimate(std::vector<string_t>& tokens, string_t specName, int& iind);
  WalkerSpec* getWalkerSpecByName(string_t n);
  std::map<PlaceIndex, PlaceMode::e> parsePlacementOptions(std::string strPlace);
  ivec3 parseBoxFit(std::string boxFit);

};

}//ns Game



#endif
