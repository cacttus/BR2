/**
*  @file MotionBucket.h
*  @date March 9, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __MOTIONBUCKET_14890899642435083547_H__
#define __MOTIONBUCKET_14890899642435083547_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
#define MOTION25_INVALID_FRAME 0
#define TILE_MASK_BLANK (-1)
#define TILE_MASK_NONE (0)


//holds all motions and compile into a single atlas.
//Only create one of these
class SpriteBucket : public VirtualMemory {
public:
  typedef std::map<Hash32, std::shared_ptr<SpriteSpec>> SpriteSpecMap;
  typedef std::map<uint8_t, Tile25Spec*> TileMap;

public:
  SpriteBucket();
  virtual ~SpriteBucket() override;

  void update(float dt);
  std::shared_ptr<SpriteSpec> getSpecByName(string_t name);
  std::shared_ptr<SpriteSpec> getSpecByName(Hash32 name);
  std::shared_ptr<SpriteSpec> getBlankTile() { return _pBlank; }
  Tile25Spec* getDefaultTile(GridMeshLayer::e mode);
  void getGpuTiles(std::shared_ptr<Atlas> pa, GpuTile __out_(&gpuTiles)[W25_MAX_GPU_SPRITES]);
  int32_t getNumUniqueFrames();
  SpriteSpecMap& getSpecs() { return _mapSpecs; }
  TileMap& getTileSpecs() { return _mapTiles; }
  Tile25Spec* getTileById(uint8_t id);
  Tile25Spec* getTileByName(string_t name);
  void addSpec(std::shared_ptr<SpriteSpec> sp);
  void addToAtlas(std::shared_ptr<Atlas> a);
  void loadMotions(std::shared_ptr<ObFile> obFile);

private:
  SpriteSpecMap _mapSpecs;
  TileMap _mapTiles;
  std::shared_ptr<SpriteSpec> _pBlank = nullptr;

  void setBufferTileMotion(std::shared_ptr<Atlas> pa, Sprite* mot, GpuTile __out_(&gpuTiles)[W25_MAX_GPU_SPRITES]);

  void getUniqueFrameMap(std::map<Hash32, SpriteFrame*>& __out_ map);

};



//This is a frame sequence for an animation.
class SpriteSpec : public VirtualMemory {
  //typedef std::map<Hash32, Motion25Frame*> FrameMap;
public:
  SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, string_t name, std::vector<string_t>& frames,
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
  string_t getName() { return _strName; }
  //void addFrame(string_t imageNameNOPATH);
//  std::vector<Hash32>& getFramesOrdered()  { return _vecFramesOrdered; }
//  std::vector<string_t>& getFrameNames() { return _vecFrameNames; }
  Hash32 getNameHashed() { return _iNameHashed; }
  int32_t numFrames() { return (int32_t)_vecFrames.size(); }

  void addGeneratedFrame(string_t genName, std::shared_ptr<Img32> bi);
  bool getRandomSelect() { return _bRandomSelect; }
  string_t getFirstFrameFileName();

private:
  //std::vector<string_t> _vecFrameNames;
  //std::vector<Hash32> _vecFramesOrdered;
  //FrameMap _mapFrames; //unique frames.
  std::vector<SpriteFrame*> _vecFrames;

  string_t _strName;
  Hash32 _iNameHashed;
  float _fDefaultDuration = 1.0;
  bool _bDefaultLoop = true;
  bool _bRandomSelect = false;
  std::shared_ptr<SpriteBucket> _pBucket = nullptr;
  //computed
  //Motion25Spec* _pMaskSpec = nullptr;

  bool _bIsGenerated = false;

  SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, string_t name, bool bRandomSelect);

};

//Single frame of animation.  Just an image location.
class SpriteFrame : public VirtualMemory {
public:
  SpriteFrame(string_t img);
  SpriteFrame(string_t img, std::shared_ptr<Img32> pGen);
  virtual ~SpriteFrame() override;

  std::shared_ptr<Img32> getGenerated() { return _pGenerated; }

  string_t getImageName() { return _strImage; }
  Hash32 getHash() { return _iHash; }

private:
  string_t _strImage;
  Hash32 _iHash;
  std::shared_ptr<Img32> _pGenerated = nullptr;
};

//Instance of the motion.
class Sprite : public VirtualMemory {
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

private:
  float _fDurationSeconds = 1;
  bool _bLoop = false;
  float _fCurTime = 0;
  std::shared_ptr<SpriteSpec> _pSpec = nullptr;
  int32_t _iCurrentFrameIndex = 0;
  int32_t _iGpuIndex = -1;   //Index of this sprite in the UNIFORM BUFFER on the GPU. Only if the sprite is a base tile or cell.  Generated.  
};


}//ns Game



#endif
