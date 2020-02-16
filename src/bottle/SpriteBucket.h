/**
*
*    @file MotionBucket.h
*    @date March 9, 2017
*    @author MetalMario971  
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __MOTIONBUCKET_14890899642435083547_H__
#define __MOTIONBUCKET_14890899642435083547_H__

#include "../base/PoundFile.h"
#include "../bottle/BottleHeader.h"

namespace Game {
#define MOTION25_INVALID_FRAME 0
#define TILE_MASK_BLANK (-1)
#define TILE_MASK_NONE (0)


//holds all motions and compile into a single atlas.
//Only create one of these
class SpriteBucket : public VirtualMemory {
public:
    typedef std::map<Hash32, std::shared_ptr<SpriteSpec>> SpriteSpecMap;
    typedef std::map<uint8_t, Tile25Spec*> TileMap;
private:
    SpriteSpecMap _mapSpecs;
    TileMap _mapTiles;
    std::shared_ptr<SpriteSpec> _pBlank = nullptr;

    void setBufferTileMotion(std::shared_ptr<Atlas> pa, Sprite* mot, GpuTile __out_(&gpuTiles)[W25_MAX_GPU_SPRITES]);

    void getUniqueFrameMap(std::map<Hash32, SpriteFrame*>& __out_ map);
public:
    SpriteBucket();
    virtual ~SpriteBucket() override;

    void update(float dt);
    std::shared_ptr<SpriteSpec> getSpecByName(t_string name);
    std::shared_ptr<SpriteSpec> getSpecByName(Hash32 name);
    std::shared_ptr<SpriteSpec> getBlankTile() { return _pBlank; }
    Tile25Spec* getDefaultTile(GridMeshLayer::e mode);
    void getGpuTiles(std::shared_ptr<Atlas> pa, GpuTile __out_(&gpuTiles)[W25_MAX_GPU_SPRITES]);
    int32_t getNumUniqueFrames();
    SpriteSpecMap& getSpecs() { return _mapSpecs; }
    TileMap& getTileSpecs() { return _mapTiles; }
    Tile25Spec* getTileById(uint8_t id);
    Tile25Spec* getTileByName(t_string name);
    void addSpec(std::shared_ptr<SpriteSpec> sp);
    void addToAtlas(std::shared_ptr<Atlas> a);
    void loadMotions(std::shared_ptr<ObFile> obFile);
};



//This is a frame sequence for an animation.
class SpriteSpec : public VirtualMemory {
    //typedef std::map<Hash32, Motion25Frame*> FrameMap;
private:
    //std::vector<t_string> _vecFrameNames;
    //std::vector<Hash32> _vecFramesOrdered;
    //FrameMap _mapFrames; //unique frames.
    std::vector<SpriteFrame*> _vecFrames;

    t_string _strName;
    Hash32 _iNameHashed;
    float _fDefaultDuration = 1.0;
    bool _bDefaultLoop = true;
    bool _bRandomSelect = false;
    std::shared_ptr<SpriteBucket> _pBucket = nullptr;
    //computed
    //Motion25Spec* _pMaskSpec = nullptr;

    bool _bIsGenerated = false;

    SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, t_string name, bool bRandomSelect);
public:
    SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, t_string name, std::vector<t_string>& frames, 
        float fDefaultDuration, bool bDefaultLoop,
        bool bRandomSelect);
    virtual ~SpriteSpec() override;

    bool getIsGenerated() { return _bIsGenerated; }
    void setIsGenerated(bool b) { _bIsGenerated = b; }

    std::vector<SpriteFrame*>& getFrames() { return _vecFrames; }
    //void setMask(Motion25Spec* mask) { _pMaskSpec = mask; }
    //Motion25Spec* getMaskSpec() { return _pMaskSpec; }

    bool getDefaultLoop() { return _bDefaultLoop; }
    float getDefaultDuration() { return _fDefaultDuration; }
    t_string getName() { return _strName; }
    //void addFrame(t_string imageNameNOPATH);
  //  std::vector<Hash32>& getFramesOrdered()  { return _vecFramesOrdered; }
  //  std::vector<t_string>& getFrameNames() { return _vecFrameNames; }
    Hash32 getNameHashed() { return _iNameHashed; }
    int32_t numFrames() { return (int32_t)_vecFrames.size(); }

    void addGeneratedFrame(t_string genName, std::shared_ptr<Img32> bi);

    bool getRandomSelect() { return _bRandomSelect; }


    t_string getFirstFrameFileName();
};

//Single frame of animation.  Just an image location.
class SpriteFrame : public VirtualMemory {
private:
    t_string _strImage;
    Hash32 _iHash;
    std::shared_ptr<Img32> _pGenerated = nullptr; 
public:
    SpriteFrame(t_string img);
    SpriteFrame(t_string img, std::shared_ptr<Img32> pGen);
    virtual ~SpriteFrame() override;

    std::shared_ptr<Img32> getGenerated() { return _pGenerated; }

    t_string getImageName() { return _strImage; }
    Hash32 getHash() { return _iHash; }
};

//Instance of the motion.
class Sprite : public VirtualMemory {
    float _fDurationSeconds = 1;
    bool _bLoop = false;
    float _fCurTime = 0;
    std::shared_ptr<SpriteSpec> _pSpec = nullptr;
    int32_t _iCurrentFrameIndex = 0;
    int32_t _iGpuIndex = -1;   //Index of this sprite in the UNIFORM BUFFER on the GPU. Only if the sprite is a base tile or cell.  Generated.  

public:
    Sprite(std::shared_ptr<SpriteSpec> bu);
    Sprite(std::shared_ptr<SpriteSpec> bu, float durationSeconds, bool bLoop);
    virtual ~Sprite() override;
    std::shared_ptr<SpriteSpec> getSpec() { return _pSpec; }
    void update(float delta);
    Hash32 getFrameAtlasIndex(); //This allows you to index directly into the texture atlas.
    
    int32_t getGpuIndex() { return _iGpuIndex; }
    void setGpuIndex(int32_t idx) {
        AssertOrThrow2(idx >= 0 && idx <= W25_MAX_GPU_SPRITES);
        _iGpuIndex = idx;
    }
};


}//ns Game



#endif
