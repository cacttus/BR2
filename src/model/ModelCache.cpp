#include "../app/AppBase.h"
#include "../base/Base.h"
#include "../app/AppBase.h"
#include "../base/Img32.h"

#include "../gfx/CameraNode.h"
#include "../gfx/RenderPipeline.h"

#include "../model/ModelCache.h"
#include "../model/Model.h"
#include "../model/MobFile.h"
#include "../model/MbiFile.h"
#include "../model/Material.h"

namespace Game {
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
ModelCache::ModelCache() {
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
        BroLogError("Tried to add duplicate model name '" + ms->getName() + "'");
    }
}
std::shared_ptr<ModelSpec> ModelCache::getModelByName(string_t name) {
    Hash32 h = STRHASH(name);
    return getModelByName(h);
}
std::shared_ptr<ModelSpec> ModelCache::getModelByName(Hash32 hash) {
    auto it = _mapModels.find(hash);
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
string_t ModelCache::getFilePathForMobName(string_t mobName, bool bUseBinary) {
    string_t fileExt;
    string_t modelsDir;
    if (bUseBinary) {
        modelsDir = Gu::getApp()->getModelsBinDir();
        fileExt = ".mbi";
    }
    else {
        modelsDir = Gu::getApp()->getModelsTextDir();
        fileExt = ".mob";
    }

    string_t folderPath = Gu::getApp()->makeAssetPath(modelsDir, mobName);
    string_t filename = FileSystem::combinePath(folderPath, mobName);
    filename += fileExt;

    return filename;
}
std::shared_ptr<ModelSpec> ModelCache::getOrLoadModel(string_t mobName, bool bUseBinary) {

    std::shared_ptr<ModelSpec> ms = getModelByName(STRHASH(mobName));
    if (ms == nullptr) {
        string_t filename = getFilePathForMobName(mobName, bUseBinary);
        if (FileSystem::fileExists(filename) == false) {
            BroLogError("Model File does not exist: '" + filename + "'");
            if (false) {
                debugPrintAllModelNames();
            }
            return nullptr;
        }
        else {
            t_timeval t0 = Gu::getMicroSeconds();
            BroLogInfo("Loading model '" + mobName + "' from '" + filename + "'..");
            {
                if (bUseBinary) {
                    MbiFile mf;
                    if (mf.loadAndParse(filename) == false) {
                        BroLogError("Failed to load model " + mobName + " ");
                    }
                    ms = getModelByName(STRHASH(mobName));
                }
                else {
                    MobFile mf;
                    mf.loadAndParse(filename);
                    ms = getModelByName(STRHASH(mobName));
                }
            }
            BroLogInfo("..Done. " + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");

        }
    }
    return ms;
}
string_t ModelCache::debugPrintAllModelNames() {
    string_t strOut = ("Loaded:\n");

    for (std::pair<Hash32, std::shared_ptr<ModelSpec>> p : _mapModels) {
        strOut += Stz "   " + p.second->getName()+ "\n";
    }
    strOut += ("In Dir:\n");
    std::vector<string_t> vecFiles;
    string_t mods = Gu::getApp()->getModelsTextDir();
    mods = Gu::getApp()->makeAssetPath(mods);
    FileSystem::getAllDirs(mods, vecFiles);
    for (string_t file : vecFiles) {
        strOut += Stz "  " +file+ "\n";
    }

    mods = Gu::getApp()->getModelsBinDir();
    mods = Gu::getApp()->makeAssetPath(mods);
    FileSystem::getAllDirs(mods, vecFiles);
    for (string_t file : vecFiles) {
        strOut += Stz "  " +file +"\n";
    }

    return strOut;
}
void ModelCache::convertMobToBin(string_t strMobName, bool bOnlyIfNewer, std::string strFriendlyName) {

    string_t filepathText = getFilePathForMobName(strMobName, false);
    string_t filepathBin = getFilePathForMobName(strMobName, true);

    if (FileSystem::fileExists(filepathText) == false) {
        BroLogError("Convert: Could not find mob file " + strMobName);
        return;
    }

    bool bConvert = true;
    if (bOnlyIfNewer) {
        if (FileSystem::fileExists(filepathText) &&
            FileSystem::fileExists(filepathBin)) {
            time_t t0 = FileSystem::getLastModifyTime(filepathText);
            time_t t1 = FileSystem::getLastModifyTime(filepathBin);
            if (t1 > t0) {
                bConvert = false;
               // BroLogInfo("Convet: Mob ",strMobName," is not newer.");
            }
        }
    }

    if (bConvert) {
        unloadModel(strMobName, false);
        BroLogInfo("Convert: Loading MOB " + strMobName);
        MobFile mf;
        mf.loadAndParse(filepathText);
        for (std::shared_ptr<ModelSpec> ms : mf.getModelSpecs()) {
            ms->postMobConversion();
            ms->setFriendlyName(strFriendlyName);
        }

        BroLogInfo("Convert: Saving MOB " + strMobName);
        MbiFile mb;
        for (std::shared_ptr<ModelSpec> ms : mf.getModelSpecs()) {
            mb.getModelSpecs().push_back(ms);
        }
        mb.save(filepathBin);
        unloadModel(strMobName);
    }
}
void ModelCache::unloadModel(string_t strMobName, bool bErrorIfFailed) {
    std::map<Hash32, std::shared_ptr<ModelSpec>>::iterator it = _mapModels.find(STRHASH(strMobName));

    if (it == _mapModels.end()) {
        if(bErrorIfFailed)
        BroLogError("Failed to find model " + strMobName + " to unload.");
    }
    else {
        //std::shared_ptr<ModelSpec> ms = it->second;
       // DEL_MEM(ms);

        _mapModels.erase(it);
    }
}



}//ns Game
