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
*  @brief Handles models, caches meshes on the GPU, imports meshes, and converts model data to BR2 format.
*/
class ModelCache : public GLFramework {
public:
  ModelCache(std::shared_ptr<GLContext> ct);
  virtual ~ModelCache() override;
  void addSpec(std::shared_ptr<ModelData> ms);
  std::shared_ptr<ModelData> getModelByName(string_t name);
  std::shared_ptr<ModelData> getModelByName(Hash32 hashed);
  std::shared_ptr<ModelData> getOrLoadModel(string_t mobFolderAndMobName, bool bUseBinary = true);
  string_t debugPrintAllModelNames();
  void convertMobToBin(string_t strMobName, bool bOnlyIfNewer, std::string strFriendlyName);
  void unloadModel(string_t strMobName, bool bErrorIfFailed = true);
  std::shared_ptr<Material> getDefaultMaterial() { return _pDefaultMaterial; }
private:
  std::map<Hash32, std::shared_ptr<ModelData>> _mapModels;
  string_t getFilePathForMobName(string_t mobName, bool bBinary);
  std::shared_ptr<Material> _pDefaultMaterial = nullptr;
};

}//ns BR2



#endif
