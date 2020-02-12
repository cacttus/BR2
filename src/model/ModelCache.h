/**
*
*    @file ModelCache.h
*    @date December 6, 2017
*    @author MetalMario971
*
*    © 2017
*
*
*/
#pragma once
#ifndef __MODELCACHE_1512579301228390014_H__
#define __MODELCACHE_1512579301228390014_H__

#include "../base/BaseHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*  @class ModelCache
*  @brief Handles meshes, and caches meshes on the GPU.  Also imports meshes, and converts them to proprietary mesh format.
*/
class ModelCache : public VirtualMemory {
    std::map<Hash32, std::shared_ptr<ModelSpec>> _mapModels;
    //std::map<int32_t, std::shared_ptr<Armature>> _mapArmaturesOrdered;
    //std::map<Hash32, std::shared_ptr<ActionGroup>> _mapActions;
    string_t getFilePathForMobName(string_t mobName, bool bBinary);
    std::shared_ptr<Material> _pDefaultMaterial = nullptr;
public:
    ModelCache();
    virtual ~ModelCache() override;
    void addSpec(std::shared_ptr<ModelSpec> ms);
    std::shared_ptr<ModelSpec> getModelByName(string_t name);
    std::shared_ptr<ModelSpec> getModelByName(Hash32 hashed);
    //int32_t addArmature(std::shared_ptr<Armature> a);
    //std::shared_ptr<Armature> getArmature(int32_t iArmId);
    //Hash32 addAction(std::shared_ptr<ActionGroup> a);
    //std::shared_ptr<ActionGroup> getAction(Hash32 actName);
    std::shared_ptr<ModelSpec> getOrLoadModel(string_t mobFolderAndMobName, bool bUseBinary = true);
    string_t debugPrintAllModelNames();
    void convertMobToBin(string_t strMobName, bool bOnlyIfNewer, std::string strFriendlyName);
    void unloadModel(string_t strMobName, bool bErrorIfFailed = true);
    std::shared_ptr<Material> getDefaultMaterial() { return _pDefaultMaterial; }
};

}//ns Game



#endif
