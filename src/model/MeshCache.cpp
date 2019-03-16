//#include "../base/Hash.h"
//#include "../base/ObjectFile.h"
//#include "../base/Exception.h"
//#include "../base/TStr.h"
//#include "../base/Hash.h"
//#include "../base/Logger.h"
//#include "../model/MeshCache.h"
//#include "../model/MeshSpec.h"
//
//namespace Game {
/////////////////////////////////////////////////////////////////////
//MeshCache::MeshCache(std::shared_ptr<GLContext> ctx) : _pContext(ctx)
//{
//
//}
/////////////////////////////////////////////////////////////////////
//MeshCache::~MeshCache()
//{
//    CacheMap::iterator ite = _cache.begin();
//    for (; ite != _cache.end(); ite++) {
//        std::shared_ptr<MeshSpec> sp = ite->second;
//        DEL_MEM(sp);
//    }
//    _cache.clear();
//}
/////////////////////////////////////////////////////////////////////
//
////**OK there's nothing wrong with this but since we're moving to MOB files i'm removing this code to load OBJ files.
////std::shared_ptr<MeshSpec> MeshCache::getOrLoad(std::string objFileName) {
////    std::shared_ptr<ObjectFile> off = nullptr;
////    std::shared_ptr<MeshSpec> ms_ret = nullptr;
////    int32_t ih = Hash::computeStringHash32bit(meshName, 0);
////    CacheMap::iterator ite = _cache.find(ih);
////
////    if (ite != _cache.end()) {
////        ms_ret = ite->second;
////    }
////    else {
////        off = new ObjectFile(_pContext);
////        off->load(meshName, true);
////        
////        for(std::shared_ptr<MeshSpec> ms : off->getMeshSpecs()){
////            addMesh(ms);
////
////            Hash32 mhh = STRHASH(ms->getName());
////            if(mhh = ih){
////                ms_ret = ms;
////            }
////        }
////
////        DEL_MEM(off);
////    }
////
////    if(ms_ret == nullptr){
////        BroLogError("Failed to get requestsed mesh '",meshName,"'from file...");
////    }
////    return ms_ret;
////}
//Hash32 MeshCache::addMesh(std::shared_ptr<MeshSpec> ms) {
//    Hash32 mhh = STRHASH(ms->getName());
//    if (_cache.find(mhh) == _cache.end()) {
//        _cache.insert(std::make_pair(mhh, ms));
//    }
//    else {
//        BroThrowException(TStr("Tried to add duplicate mesh '", ms->getName(), "' to mesh cache."));
//    }
//
//    return mhh;
//}
//std::shared_ptr<MeshSpec> MeshCache::getMeshByName(Hash32 hash){
//   // Hash32 mhh = STRHASH(ms->getName());
//    CacheMap::iterator ite = _cache.find(hash);
//    if (ite != _cache.end()) {
//        return ite->second;
//    }
//    else {
//        BroLogError("Could not find mesh in cache.");
//    }
//
//    return nullptr;
//}
////void TexCache::bindIfDifferent(std::shared_ptr<Texture2DSpec> tex) {
////    //Efficiently bind only the textures we need to.
////    if (tex == nullptr) {
////        _pBound = tex;
////    }
////    else if (_pBound != tex) {
////        tex->bind();
////        _pBound = tex;
////    }
////}
//
//
//
//
//
//}//ns Game
