///**
//*
//*  @file MeshCache.h
//*  @date November 30, 2016
//*  @author MetalMario971
//*/
//#pragma once
//#ifndef __MESHCACHE_14804868003449107072_H__
//#define __MESHCACHE_14804868003449107072_H__
//
//#include "../model/ModelHeader.h"
//
//namespace Game {
///**
//*  @class MeshCache
//*  @brief
//*
//*/
//class MeshCache : public VirtualMemory {
//    typedef std::map<Hash32, std::shared_ptr<MeshSpec>> CacheMap;
//    CacheMap _cache;
//    std::shared_ptr<GLContext> _pContext;
//public:
//    MeshCache(std::shared_ptr<GLContext> ctx);
//    virtual ~MeshCache() override;
//    
//    //std::shared_ptr<MeshSpec> getOrLoad(std::string objFileName);
//    Hash32 addMesh(std::shared_ptr<MeshSpec>);
//    std::shared_ptr<MeshSpec> getMeshByName(Hash32 hash);
//};
//
//}//ns Game
//
//
//
//#endif
