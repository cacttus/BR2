/**
*  @file WorldNode.h
*  @date January 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLDNODE_1515084453509352717_H__
#define __WORLDNODE_1515084453509352717_H__

#include "../model/Model.h"
#include "../bottle/BottleHeader.h"

#include <array>

namespace BR2 {
//s0-3, e0-3, v0-3, c
//s0,s1,s2,s3 e0,e1,e2,e3, v0,v1,v2,v3, c
//proxy class to modify modelNodes loaded.
//it allows us to use generic "modelnode" but also apply grid transforms
class WorldObj : public VirtualMemory {
public:
  WorldObj() {}
  virtual ~WorldObj() override {}

  static std::shared_ptr<WorldObj> WorldObj::create(std::string mobFolder, uint32_t typeID, std::string friendlyName, std::string strBox, std::string strPlace);
  string_t getFriendlyName() { return _strFriendlyName; }
  string_t getMobName() { return _strMobName; }
  uint32_t getTypeId() { return _iTypeId; }
  std::shared_ptr<ModelSpec> getOrLoadModel();

  std::shared_ptr<ModelNode> createInstance(std::shared_ptr<World25> w, vec3& r3Pos);
private:
  vec3 _vBoxFit;
  string_t _strMobName;
  std::shared_ptr<ModelSpec> _pModelSpec = nullptr;
  uint32_t _iTypeId;
  std::string _strFriendlyName;
  std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> _placeOptions;//indexed by side

  static vec3 boxFit(std::shared_ptr<ModelSpec> ms, vec3& vBoxFit);
  static std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> parsePlacementOptions(std::string strPlace);
  static vec3 parseBoxFit(std::string strBox);
  void place(const vec3& r3, vec3& outPos, vec4& outRot);
};



}//ns Game



#endif
