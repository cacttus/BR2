/**
*
*    @file LairSpec.h
*    @date April 30, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __LAIRSPEC_1493601854425449768_H__
#define __LAIRSPEC_1493601854425449768_H__

#include "../bottle/BottleHeader.h"

namespace Game {
//MorphTile with gen params for lair
class LairTile : public VirtualMemory {
    friend class ObFile;
    LairSpec* _pLairSpec = nullptr;
    MorphTile* _pMorphTileGroup = nullptr;
    MpFloat _mxPct;     // Percent chance of generation
public:
    LairTile(MorphTile* ps, LairSpec* pLair) : _pMorphTileGroup(ps), _pLairSpec(pLair) { }
    virtual ~LairTile() override { }
    MpFloat& getPct() { return _mxPct; }// Percent chance of generation
    MorphTile* getMorphTile() { return _pMorphTileGroup; }
};
//Walker with gen params for lair
class LairWalker : public VirtualMemory {
    friend class ObFile;
    LairSpec* _pLairSpec = nullptr;
    MpFloat _mxPct;     // Percent chance of generation
    WalkerSpec* _pWalkerSpec = nullptr;
public:
    LairWalker(WalkerSpec* ps, LairSpec* pLair) : _pLairSpec(pLair), _pWalkerSpec(ps) { }
    virtual ~LairWalker() override { }
    MpFloat& getPct() { return _mxPct; }// Percent chance of generation
};
/**
*    @class lairspec
*    @brief This is a "BIOME" well, dungeon etc.  
*/
class LairSpec : public VirtualMemory {
    friend class ObFile;

    t_string _name;
    Hash32 _iNameHashed;
    bool _bDefault = false;
    //MpVec3i _mxPos; //This doesn't matter for default lair
    MpVec3i _mxGlobBounds; //min /max glob bounds size.
    int32_t _nGlobs = 0;
    std::vector<LairTile*> _vecLairTiles;
    std::vector<LairWalker*> _vecLairWalkers;
    uint32_t _iPriority; // Priority of this lair against surrounding lairs.
    float _fSpawnProb = 0;
    float _fDieProb = 0;
    W25NoiseFunc _noiseFunc;

    ClimateSpec* _pClimateParams;
public:
    LairSpec(t_string name);
    virtual ~LairSpec() override;

    ClimateSpec* getClimateParams() { return _pClimateParams; }

    W25NoiseFunc* getNoiseFunc() { return &_noiseFunc; }

    Hash32 getLairId() { return _iNameHashed; }
    int32_t getNumGlobs() { return _nGlobs; }
    MpVec3i& getGlobBounds() { return _mxGlobBounds; }
    //  LairFile* getLairFile() { return _pLairFile; }
    // std::vector<LairSpawnPoint*>* getSpawnPoints() { return &_vecSpawnPoints; }
    uint32_t getPriority() { return _iPriority; }
    std::vector<LairTile*>& getLairTiles() { return _vecLairTiles; }
    std::vector<LairWalker*>& getLairWalkers() { return _vecLairWalkers; }
    t_string getName() { return _name; }
    bool getDefault() { return _bDefault; }
    float getSpawnProb() { return _fSpawnProb; }
    float getDieProb() { return _fDieProb; }

    void createInstance();
};
//Insatnce of lair
class Lair : public VirtualMemory {
    friend class LairMapFile;
    LairSpec* _pLairSpec = nullptr;
    // Walked area.
    MpVec3i        _mxGlobBounds; //min /max glob bounds size.
    int32_t        _nGlobs;

public:
    Lair(LairSpec* ps);
    virtual ~Lair();

    int32_t getNumGlobs() { return _nGlobs; }
    MpVec3i& getGlobBounds() { return _mxGlobBounds; }
    //LairFile* getLairFile() { return _pLairFile; }
    //std::vector<LairSpawnPoint*>* getSpawnPoints() { return &_vecSpawnPoints; }
};



}//ns Game



#endif
