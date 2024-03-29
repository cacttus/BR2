/**
*  @file TexCache.h
*  @date November 26, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __TEXCACHE_1480188610676155930_H__
#define __TEXCACHE_1480188610676155930_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {
class TexFile {
public:
  string_t _name;
  string_t _loc;
  TexFile(string_t loc);
  TexFile(string_t name, string_t loc);
};
/**
*  @class TexCache
*  @brief Caches loaded textures.
*/
class TexCache : public VirtualMemory {
public:
  ///Initialize in CPP file
  static const std::string WorldGrass;
  static const std::string WorldDirt;
  static const std::string WorldPlank;
  static const std::string WorldBrick;
  static const std::string Selector;
  static const std::string Brick64Des;
  static const std::string SelectorRed;
  static const std::string WalWd;
  static const std::string RockWd;
  static const std::string GrassWd;
  static const std::string RopeWd;
  static const std::string LadderWd;
  static const std::string MdWoodItem;

  TexCache(std::shared_ptr<GLContext> ctx);
  virtual ~TexCache() override;
  std::shared_ptr<Texture2DSpec> getOrLoad(TexFile tc, bool bIsGenerated = false, bool bRepeatU = false, bool bRepeatV = false);
  bool add(string_t name, std::shared_ptr<Texture2DSpec> ss, bool bErrorIfFound = true);
  std::shared_ptr<Texture2DSpec> addAsGeneratedImage(string_t name, const std::shared_ptr<Img32> ss);
  GLuint getDummy1x1TextureCube() { return _i1x1DummyCubeTexture; }
  GLuint getDummy1x1Texture2D() { return _i1x2Dummy2DTexture; }
  GLuint getDummy1x1NormalTexture2D() { return _i1x1DummyBump2DTexture; }
  //std::shared_ptr<Texture2DSpec> getDummy1x1Tex();

private:
  typedef std::map<Hash32, std::shared_ptr<Texture2DSpec>> TexMap;
  TexMap _cache;
  std::shared_ptr<Texture2DSpec> _pBound = nullptr; //NOT reliable
  std::shared_ptr<GLContext> _pContext;

  GLuint _i1x1DummyCubeTexture = 0;
  GLuint _i1x2Dummy2DTexture = 0;
  GLuint _i1x1DummyBump2DTexture = 0;
};

}//ns Game



#endif
