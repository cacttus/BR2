/**
*
*    @file ObFile.h
*    @date March 30, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __OBFILE_1490906948419075780_H__
#define __OBFILE_1490906948419075780_H__

#include "../base/PoundFile.h"
#include "../bottle/BottleHeader.h"

namespace Game {

class ObFile : public PoundFile {
    typedef std::map<Hash32, std::shared_ptr<SpriteSpec>> MotionSpecMap;

protected:
    virtual void pkp(std::vector<t_string>& tokens);
    virtual void preLoad();
    virtual void postLoad();
    std::shared_ptr<RoomBase> _pRoom;
    std::shared_ptr<SpriteBucket> _pBucket = nullptr;
    std::vector<std::shared_ptr<SpriteSpec>> _vecMotionSpecs;
    //std::vector<WorldObjectSpec*> _vecPixObjSpecs;
    std::vector<std::shared_ptr<WorldObj>> _vecWorldObjs;
   // std::vector<ObModel*> _vecModelSpecs;
    std::vector<Tile25Spec*> _vecTileSpecs;
    std::vector<WalkerSpec*> _vecWalkers;
    std::vector<LairSpec*> _vecLairs;
    std::vector<MorphTile*> _vecMorphTiles;
    MorphTile* _pCurMorphTile = nullptr;
    std::shared_ptr<WorldObj> _pCurObjSpec = nullptr;
    bool _bCurSpecValid = true;
    LairSpec* _pCurLairSpec = nullptr;
    WalkerSpec* _pCurWalkerSpec = nullptr;
    std::shared_ptr<W25Config> _pW25Config = nullptr;

    void parseConfig(std::vector<t_string>& tokens);
    void parseVer(std::vector<t_string>& tokens);
    void parseSprites(std::vector<t_string>& tokens);
    void parseMobs(std::vector<t_string>& tokens);
    void parseTiles(std::vector<t_string>& tokens);
    void parseMorphTiles(std::vector<t_string>& tokens);
    void parseObjects(std::vector<t_string>& tokens);
    void parseWalkers(std::vector<t_string>& tokens);
    void parseLairs(std::vector<t_string>& tokens);

    std::shared_ptr<SpriteSpec> getMot(t_string& name);
    void parseGetSetMotion(t_string& tok, t_string& msName, std::shared_ptr<SpriteSpec>& ms);
    void processMotion(t_string name, std::vector<t_string>& files, float fDuration, bool bLoop, bool bRandomSel);
    void processTile(t_string name, int32_t index, GridMeshLayer::e eMatterMode,
        std::shared_ptr<SpriteSpec> pTop, std::shared_ptr<SpriteSpec> pSide, std::shared_ptr<SpriteSpec> pBot, ClimateSpec& cp, float rarity);
    //void removeMotion(Motion25Spec* ps);
    void processShiftMotionImage(std::shared_ptr<SpriteSpec> ps, bool shiftH, bool shiftV);
    Tile25Spec* getTileSpecByName(t_string name);
    LairGenLocale::e parseGenLocale(t_string str);
    LairTile* parseLairTile(std::vector<t_string>& tokens, t_string specName, int& iind);
    MpVec3i parse_mxv3(std::vector<t_string>& tokens, t_string specName, int& iind);
    MpUint parse_mxui2(std::vector<t_string>& tokens, t_string specName, int& iind);
    MpFloat parse_mxf2(std::vector<t_string>& tokens, t_string specName, int& iind);
    int32_t parseNbr_int(std::vector<t_string>& tokens, int& iind);
    uint32_t parseNbr_uint(std::vector<t_string>& tokens, int& iind);
    float parseNbr_float(std::vector<t_string>& tokens, int& iind);
    MorphTile* getMorphTileGroupByName(t_string);
    ClimateSpec parseClimate(std::vector<t_string>& tokens, t_string specName, int& iind);
    WalkerSpec* getWalkerSpecByName(t_string n);
    std::map<PlaceIndex, PlaceMode::e> parsePlacementOptions(std::string strPlace);
    ivec3 parseBoxFit(std::string boxFit);

public:
    ObFile(std::shared_ptr<RoomBase> pRoom);
    virtual ~ObFile() override;

    std::vector<WalkerSpec*>& getWalkerSpecs() { return _vecWalkers; }
    std::vector<LairSpec*>& getLairSpecs() { return _vecLairs; }
    std::vector<std::shared_ptr<WorldObj>>& getMobSpecs() { return _vecWorldObjs; }

    std::shared_ptr<W25Config> getW25Config() { return _pW25Config; }
    //std::vector<WorldObjectSpec*>& getPixObjSpecs() { return _vecPixObjSpecs; }
    std::vector<std::shared_ptr<SpriteSpec>>& getMotions() { return _vecMotionSpecs; }
    std::vector<Tile25Spec*>& getTiles() { return _vecTileSpecs; }
    //std::vector<ObModel*>& getModels() { return _vecModelSpecs; }
    std::shared_ptr<SpriteBucket> getBucket() { return _pBucket; }
};

}//ns Game



#endif
