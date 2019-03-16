#include "../base/RoomBase.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/TStr.h"
#include "../base/RoomBase.h"
#include "../base/Img32.h"

#include "../display/CameraNode.h"
#include "../display/RenderPipe.h"

#include "../model/ModelCache.h"
#include "../model/Model.h"
#include "../model/MobFile.h"
#include "../model/MbiFile.h"
#include "../model/Material.h"

namespace Game {
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
ModelCache::ModelCache(std::shared_ptr<GLContext> pc) : _pContext(pc) {
    _pDefaultMaterial = std::make_shared<Material>();
    //default material is the initial blender params.
    

}
ModelCache::~ModelCache() {
    std::map<Hash32, std::shared_ptr<ModelSpec>>::iterator it = _mapModels.begin();
    /*for (; it != _mapModels.end(); it++) {
        std::shared_ptr<ModelSpec> ms = it->second;
        DEL_MEM(ms);
    }*/
    //for (std::pair<Hash32, std::shared_ptr<Armature>> p : _mapArmaturesOrdered) {
    //    DEL_MEM(p.second);
    //}
    //for (std::pair<Hash32, std::shared_ptr<ActionGroup>> p1 : _mapActions) {
    //    DEL_MEM(p1.second);
    //}
}
///////////////////////////////////////////////////////////////////
void ModelCache::addSpec(std::shared_ptr<ModelSpec> ms) {
    Hash32 h = ms->getNameHashed();

    std::map<Hash32, std::shared_ptr<ModelSpec>>::iterator it = _mapModels.find(h);
    if (it == _mapModels.end()) {
        _mapModels.insert(std::make_pair(h, ms));
    }
    else {
        BroLogError("Tried to add duplicate model name '", ms->getName(), "'");
    }
}
std::shared_ptr<ModelSpec> ModelCache::getModelByName(t_string name) {
    Hash32 h = STRHASH(name);
    return getModelByName(h);
}
std::shared_ptr<ModelSpec> ModelCache::getModelByName(Hash32 hash) {
    std::map<Hash32, std::shared_ptr<ModelSpec>>::iterator it = _mapModels.find(hash);
    if (it != _mapModels.end()) {
        return it->second;
    }
    return nullptr;
}

//int32_t ModelCache::addArmature(std::shared_ptr<Armature> a) {
//    if (_mapArmaturesOrdered.find(a->getArmatureId()) == _mapArmaturesOrdered.end()) {
//        _mapArmaturesOrdered.insert(std::make_pair(a->getArmatureId(), a));
//    }
//    else {
//        BroThrowException("Armature '", (int32_t)a, "' already found in cache.  Duplicate loaded.");
//    }
//    return a->getArmatureId();
//}
//std::shared_ptr<Armature> ModelCache::getArmature(int32_t armId) {
//    std::map<int32_t, std::shared_ptr<Armature>>::iterator it = _mapArmaturesOrdered.find(armId);
//    if (it != _mapArmaturesOrdered.end()) {
//        return it->second;
//    }
//    else {
//        return nullptr;
//    }
//}
//Hash32 ModelCache::addAction(std::shared_ptr<ActionGroup> a) {
//    Hash32 ah = a->getNameHash();
//    std::map<Hash32, std::shared_ptr<ActionGroup>>::iterator it_act = _mapActions.find(ah);
//    if (it_act != _mapActions.end()) {
//        BroThrowException("Action '", a->getName(), "'already found in cache. ");
//    }
//    else {
//        _mapActions.insert(std::make_pair(ah, a));
//
//    }
//    return ah;
//}
//std::shared_ptr<ActionGroup> ModelCache::getAction(Hash32 actName) {
//    std::map<Hash32, std::shared_ptr<ActionGroup>>::iterator it = _mapActions.find(actName);
//    if (it == _mapActions.end()) {
//        return nullptr;
//    }
//    return it->second;
//}
t_string ModelCache::getFilePathForMobName(t_string mobName, bool bUseBinary) {
    t_string fileExt;
    t_string modelsDir;
    if (bUseBinary) {
        modelsDir = Gu::getContext()->getRoom()->getModelsBinDir();
        fileExt = ".mbi";
    }
    else {
        modelsDir = Gu::getContext()->getRoom()->getModelsTextDir();
        fileExt = ".mob";
    }

    t_string folderPath = Gu::getContext()->getRoom()->makeAssetPath(modelsDir, mobName);
    t_string filename = FileSystem::combinePath(folderPath, mobName);
    filename += fileExt;

    return filename;
}
std::shared_ptr<ModelSpec> ModelCache::getOrLoadModel(t_string mobName, bool bUseBinary) {

    std::shared_ptr<ModelSpec> ms = getModelByName(STRHASH(mobName));
    if (ms == nullptr) {
        t_string filename = getFilePathForMobName(mobName, bUseBinary);
        if (FileSystem::fileExists(filename) == false) {
            BroLogError("Model File does not exist: '", filename, "'");
            if (false) {
                debugPrintAllModelNames();
            }
            return nullptr;
        }
        else {
            t_timeval t0 = Gu::getMicroSeconds();
            BroLogInfo("Loading model '", mobName, "' from '", filename, "'..");
            {
                if (bUseBinary) {
                    MbiFile mf;
                    if (mf.loadAndParse(filename) == false) {
                        BroLogError("Failed to load model ", mobName, " ");
                    }
                    ms = getModelByName(STRHASH(mobName));
                }
                else {
                    MobFile mf;
                    mf.loadAndParse(filename);
                    ms = getModelByName(STRHASH(mobName));
                }
            }
            BroLogInfo("..Done. ", (uint32_t)(Gu::getMicroSeconds() - t0) / 1000, "ms");

        }
    }
    return ms;
}
t_string ModelCache::debugPrintAllModelNames() {
    t_string strOut = TStr("Loaded:\n");

    for (std::pair<Hash32, std::shared_ptr<ModelSpec>> p : _mapModels) {
        strOut += TStr("   ", p.second->getName(), "\n");
    }
    strOut += TStr("In Dir:\n");
    std::vector<t_string> vecFiles;
    t_string mods = Gu::getContext()->getRoom()->getModelsTextDir();
    mods = Gu::getContext()->getRoom()->makeAssetPath(mods);
    FileSystem::getAllDirs(mods, vecFiles);
    for (t_string file : vecFiles) {
        strOut += TStr("  ", file, "\n");
    }

    mods = Gu::getContext()->getRoom()->getModelsBinDir();
    mods = Gu::getContext()->getRoom()->makeAssetPath(mods);
    FileSystem::getAllDirs(mods, vecFiles);
    for (t_string file : vecFiles) {
        strOut += TStr("  ", file, "\n");
    }

    return strOut;
}
void ModelCache::convertMobToBin(t_string strMobName, bool bOnlyIfNewer, std::string strFriendlyName) {

    t_string filepathText = getFilePathForMobName(strMobName, false);
    t_string filepathBin = getFilePathForMobName(strMobName, true);

    if (FileSystem::fileExists(filepathText) == false) {
        BroLogError("Convert: Could not find mob file ", strMobName);
        return;
    }

    bool bConvert = true;
    if (bOnlyIfNewer) {
        if (FileSystem::fileExists(filepathText) &&
            FileSystem::fileExists(filepathBin)) {
            time_t t0 = FileSystem::getFileModifyTime(filepathText);
            time_t t1 = FileSystem::getFileModifyTime(filepathBin);
            if (t1 > t0) {
                bConvert = false;
               // BroLogInfo("Convet: Mob ",strMobName," is not newer.");
            }
        }
    }

    if (bConvert) {
        unloadModel(strMobName, false);
        BroLogInfo("Convert: Loading MOB ", strMobName);
        MobFile mf;
        mf.loadAndParse(filepathText);
        for (std::shared_ptr<ModelSpec> ms : mf.getModelSpecs()) {
            ms->postMobConversion();
            ms->setFriendlyName(strFriendlyName);
        }

        BroLogInfo("Convert: Saving MOB ", strMobName);
        MbiFile mb;
        for (std::shared_ptr<ModelSpec> ms : mf.getModelSpecs()) {
            mb.getModelSpecs().push_back(ms);
        }
        mb.save(filepathBin);
        unloadModel(strMobName);
    }
}
void ModelCache::unloadModel(t_string strMobName, bool bErrorIfFailed) {
    std::map<Hash32, std::shared_ptr<ModelSpec>>::iterator it = _mapModels.find(STRHASH(strMobName));

    if (it == _mapModels.end()) {
        if(bErrorIfFailed)
        BroLogError("Failed to find model ", strMobName, " to unload.");
    }
    else {
        //std::shared_ptr<ModelSpec> ms = it->second;
       // DEL_MEM(ms);

        _mapModels.erase(it);
    }
}



}//ns Game
