/**
*  @file ModelCache.h
*  @date December 6, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __MODELCACHE_1512579301228390014_H__
#define __MODELCACHE_1512579301228390014_H__

#include "../base/BaseHeader.h"
#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class ModelCache
*  @brief simple mod cahce
*/
class ModelCache : public VirtualMemory {
public:
  ModelCache(std::shared_ptr<GLContext> pc);
  virtual ~ModelCache() override;
  void addSpec(std::shared_ptr<ModelSpec> ms);
  std::shared_ptr<ModelSpec> getModelByName(t_string name);
  std::shared_ptr<ModelSpec> getModelByName(Hash32 hashed);
  //int32_t addArmature(std::shared_ptr<Armature> a);
  //std::shared_ptr<Armature> getArmature(int32_t iArmId);
  //Hash32 addAction(std::shared_ptr<ActionGroup> a);
  //std::shared_ptr<ActionGroup> getAction(Hash32 actName);
  std::shared_ptr<ModelSpec> getOrLoadModel(t_string mobFolderAndMobName, bool bUseBinary = true);
  t_string debugPrintAllModelNames();
  void convertMobToBin(t_string strMobName, bool bOnlyIfNewer, std::string strFriendlyName);
  void unloadModel(t_string strMobName, bool bErrorIfFailed = true);
  std::shared_ptr<Material> getDefaultMaterial() { return _pDefaultMaterial; }
private:
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::map<Hash32, std::shared_ptr<ModelSpec>> _mapModels;
  //std::map<int32_t, std::shared_ptr<Armature>> _mapArmaturesOrdered;
  //std::map<Hash32, std::shared_ptr<ActionGroup>> _mapActions;
  t_string getFilePathForMobName(t_string mobName, bool bBinary);
  std::shared_ptr<Material> _pDefaultMaterial = nullptr;


};

}//ns Game



#endif
