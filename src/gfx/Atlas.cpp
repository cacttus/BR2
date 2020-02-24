#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/Allocator.h"
#include "../base/Img32.h"
#include "../base/Hash.h"


#include "../gfx/Atlas.h"
#include "../gfx/TexCache.h"

namespace BR2 {
Atlas::Atlas(std::shared_ptr<GLContext> ct, string_t na, ivec2& ivGridSize) : _strName(na), Texture2DSpec(ct) {
  _vGridSize = ivGridSize; //_pSpriteMap->getGridDimensions();
}
Atlas::Atlas(std::shared_ptr<GLContext> ct, string_t na, ivec2& viSpriteSize, string_t strImageLoc) : _strName(na), Texture2DSpec(ct), _strPrecompileFileLocation(strImageLoc) {
  _vSpriteSize = viSpriteSize;
  //_vGridSize = ivGridSize; //_pSpriteMap->getGridDimensions();
}
Atlas::~Atlas() {
  _mapImages.clear();
  //for (ImgMap::iterator it = _mapImages.begin(); it != _mapImages.end(); it++) {
  //    std::shared_ptr<AtlasSprite> sp = it->second;
  //    DEL_MEM(sp);
  //}
}
string_t Atlas::getGeneratedFileName() {
  return "generated";
}
std::shared_ptr<Img32> Atlas::tryGetCachedImage() {
  time_t greatestDependencyModifyTime;
  time_t cachedImageModifyTime;
  string_t cachedImageLoc;
  std::shared_ptr<Img32> bi = nullptr;

  cachedImageLoc = getCachedImageFilePath();

  if (FileSystem::fileExists(cachedImageLoc) == false) {
    return nullptr;
  }
  t_timeval t0 = Gu::getMicroSeconds();
  BRLogInfo("Loading cached image '" + getCachedImageFilePath() + "'...");
  {

    greatestDependencyModifyTime = cacheGetGreatestModifyTimeForAllDependencies();
    cachedImageModifyTime = FileSystem::getLastModifyTime(cachedImageLoc);


    if (greatestDependencyModifyTime <= cachedImageModifyTime) {
      bi = Gu::loadImage(cachedImageLoc);
    }
  }
  BRLogInfo("Finished.." + (uint32_t)((Gu::getMicroSeconds() - t0) / 1000) + "ms");

  return bi;
}
time_t Atlas::cacheGetGreatestModifyTimeForAllDependencies() {
  time_t greatestModifyTime;
  time_t currentModifyTime;
  time_t cacheModifyTime;
  string_t cachedImageLoc;

  cachedImageLoc = getCachedImageFilePath();
  if (FileSystem::fileExists(cachedImageLoc) == false) {
    return 0;
  }

  cacheModifyTime = FileSystem::getLastModifyTime(cachedImageLoc);
  greatestModifyTime = TIME_T_MIN;

  // - If any image in the loaded images is greater than x then the cache is invalid.
  ImgMap::iterator ite = _mapImages.begin();
  for (; ite != _mapImages.end(); ite++)//t_vecsize iImage=0; iImage<_vecTexFiles.size(); ++iImage)
  {
    string_t strPath = ite->second->_imgLoc;

    if (ite->second->getIsGenerated() == false) {
      //  spriteFileLoc = getAtlasSpriteFullpath(ite->second);
      if (FileSystem::fileExists(strPath) == false) {
        BRLogError(_strName + " atlas file '" + strPath + "' does not exist.");
        Gu::debugBreak();
        return 0;
      }
      currentModifyTime = FileSystem::getLastModifyTime(strPath);
      greatestModifyTime = MathUtils::brMax(greatestModifyTime, currentModifyTime);
    }
  }

  return greatestModifyTime;
}
void Atlas::compilePrecompiled(bool bMipmaps) {
  //Compiles the atlas as a texture with a set of frames.
  std::shared_ptr<Img32> bi = Gu::loadImage(_strPrecompileFileLocation);

  //Assert that the atlas is evenly spaced.
  if (bi->getWidth() % _vSpriteSize.x != 0) {
    BRLogWarn("Atlas sprites are not evenly spaced along the x axis.");
  }
  if (bi->getHeight() % _vSpriteSize.y != 0) {
    BRLogWarn("Atlas sprites are not evenly spaced along the y axis.");
  }

  _vGridSize.x = bi->getWidth() / _vSpriteSize.x;
  _vGridSize.y = bi->getHeight() / _vSpriteSize.y;

  for (int32_t iy = 0; iy < _vGridSize.y; ++iy) {
    for (int32_t ix = 0; ix < _vGridSize.x; ++ix) {
      addImagePrecompiled(ix, iy);
    }
  }

  finishCompile(bi, bMipmaps);
}
string_t Atlas::constructPrecompiledSpriteName(int32_t ix, int32_t iy) {
  return Stz ix + "_" + iy;
}
void Atlas::addImagePrecompiled(int32_t ix, int32_t iy) {
  string_t strSpriteName;
  Hash32 iHash;

  //Generate a stupid hash in the form x_y
  strSpriteName = constructPrecompiledSpriteName(ix, iy);
  iHash = STRHASH(strSpriteName);

  std::shared_ptr<AtlasSprite> sp = std::make_shared<AtlasSprite>(std::dynamic_pointer_cast<Atlas>(shared_from_this()));

  sp->_iId = iHash;
  sp->_imgLoc = strSpriteName;
  sp->_hash = iHash;
  sp->_viGridPos.x = ix;
  sp->_viGridPos.y = iy;

  AssertOrThrow2(_mapImages.find(iHash) == _mapImages.end());

  _mapImages.insert(std::make_pair(iHash, sp));
}
void Atlas::compileFiles(bool bMipmaps, bool saveAndLoad) {
  AssertOrThrow2(_mapImages.size() > 0);
  std::shared_ptr<Img32> sp = nullptr;

  //OKAY SO THE PROBLEM HERE IS THE SPRITES AND VIGRIDPOS DON"T GET SET!
//   if(saveAndLoad) { 
//       sp = tryGetCachedImage();
//   }

  if (sp == nullptr) {
    t_timeval t0 = Gu::getMicroSeconds();
    BRLogInfo("Composing image '" + getCachedImageFilePath() + "'...");
    {
      sp = composeImage(saveAndLoad);

    }
    BRLogInfo("Finished.." + (uint32_t)((Gu::getMicroSeconds() - t0) / 1000) + "ms");
  }

  printInfoAndErrors(sp);
  finishCompile(sp, bMipmaps);

}
void Atlas::finishCompile(std::shared_ptr<Img32> sp, bool bMipmaps) {
  //Create the texture
  create((unsigned char*)sp->getData()->ptr(), sp->getWidth(), sp->getHeight(), bMipmaps, false, false);

  Gu::getTexCache()->add(getName(), shared_from_this());
  Gu::freeImage(sp);
}
void Atlas::addImage(Hash32 en, string_t loc) {
  std::shared_ptr<AtlasSprite> sp = std::make_shared<AtlasSprite>(getThis<Atlas>());
  sp->_iId = en;
  sp->_imgLoc = loc;
  sp->_hash = en;
  AssertOrThrow2(_mapImages.find(en) == _mapImages.end());
  _mapImages.insert(std::make_pair(en, sp));
}
void Atlas::addImage(Hash32 en, string_t loc, std::shared_ptr<Img32> imgData) {
  std::shared_ptr<AtlasSprite> sp = std::make_shared<AtlasSprite>(getThis<Atlas>());
  sp->_iId = en;
  sp->_imgLoc = Atlas::getGeneratedFileName();
  sp->_hash = en;
  sp->_pGeneratedImage = imgData;
  AssertOrThrow2(_mapImages.find(en) == _mapImages.end());
  _mapImages.insert(std::make_pair(en, sp));
}
void Atlas::removeImage(std::shared_ptr<AtlasSprite> ps) {
  ImgMap::iterator ite = _mapImages.find(ps->_hash);
  if (ite == _mapImages.end()) {
    Gu::debugBreak();
    return;
  }
  _mapImages.erase(ite);
}
std::shared_ptr<Img32> Atlas::composeImage(bool bCache) {
  ivec2 copyLoc;
  std::shared_ptr<Img32> masterImage;
  std::shared_ptr<Img32> pCopy;
  int32_t sizx, sizy;

  AssertOrThrow2(_mapImages.size() > 0);

  //Get the size for all sprites from the first image.
  std::shared_ptr<Img32> ss = Gu::loadImage(_mapImages.begin()->second->_imgLoc);
  sizx = ss->getWidth();//_pSpriteMap->getImageSizePixels().x;
  sizy = ss->getHeight();//_pSpriteMap->getImageSizePixels().y;
  _vSpriteSize = ivec2(sizx, sizy);// _pSpriteMap->getSpriteDimensions();
  Gu::freeImage(ss);

  masterImage = std::make_shared<Img32>(sizx * _vGridSize.x, sizy * _vGridSize.y);

  //All images 32 bit
  copyLoc = ivec2(0, 0);

  int32_t iImage = 0;
  for (ImgMap::iterator ite = _mapImages.begin();
    ite != _mapImages.end(); ite++, iImage++)//t_vecsize iImage=0; iImage<_vecTexFiles.size(); ++iImage)
  {
    std::shared_ptr<AtlasSprite> pSprite = ite->second;
    try {
      //t_string strPath = getAtlasSpriteFullpath(ite->second);
      if (pSprite->getIsGenerated() == false) {
        string_t strPath = ite->second->_imgLoc;
        pCopy = Gu::loadImage(strPath);
      }
      else {
        //teh image was generated
        pCopy = pSprite->_pGeneratedImage;
      }

      //make sure size matches expected sprite size
      //This is set in the atlas.dat file as assw and assh
      AssertOrThrow2(pCopy->getWidth() == _vSpriteSize.x);//, "Sprite width/height does not match the height specified in the atlas file.");
      AssertOrThrow2(pCopy->getHeight() == _vSpriteSize.y);//, "Sprite width/height does not match the height specified in the atlas file.");

      copyLoc.x = (iImage % _vGridSize.x) * _vSpriteSize.x;
      copyLoc.y = (iImage / _vGridSize.y) * _vSpriteSize.y;
    }
    catch (Exception * ex) {
      BRLogError("Failed to Open image.:\r\n" + ex->what());
    }

    try {
      if (pCopy != nullptr) {
        //copy from top left corner.
        masterImage->copySubImageFrom(copyLoc, ivec2(0, 0), ivec2(pCopy->getWidth(), pCopy->getHeight()), pCopy);
        ite->second->_viGridPos.x = (iImage % _vGridSize.x);
        ite->second->_viGridPos.y = (iImage / _vGridSize.y);
      }
    }
    catch (BR2::Exception * ex) {
      BRLogError("Failed to Copy Sub-Image during texture composition.:\r\n" + ex->what());
    }

    pCopy = nullptr;

    Gu::freeImage(pCopy);
  }

  // - Origin must be at bottom left corner
  masterImage->flipV();
  // masterImage->flipH();

  if (bCache) {
    BRLogInfo("Saving atlas '" + getName() + "' to '" + getCachedImageFilePath() + "'.");
    string_t strFileName = getCachedImageFilePath();
    Gu::saveImage(strFileName, masterImage);
  }


  return masterImage;
}
string_t Atlas::getCachedImageFilePath() {
  string_t fn;
  fn = getName() + ".png";
  fn = FileSystem::appendCachePathToFile(fn);
  return fn;
}
void Atlas::getTCoords(Hash32 emat, vec2* __out_ bl, vec2* __out_ br, vec2* __out_ tl, vec2* __out_ tr, bool bHalfPixelPadding) {
  std::shared_ptr<AtlasSprite> sp = getSprite(emat);
  AssertOrThrow2(sp != nullptr);
  getTCoords(&(sp->_viGridPos), bl, br, tl, tr, bHalfPixelPadding);
}
void Atlas::getTCoords(ivec2* viGridPos, vec2* __out_ bl, vec2* __out_ br, vec2* __out_ tl, vec2* __out_ tr, bool bHalfPixelPadding) {

  //Note: Changes to atlas - The atlas was flipped around.
  float sprW = (float)_vSpriteSize.x / (float)getWidth();
  float sprH = (float)_vSpriteSize.y / (float)getHeight();

  float tx = (float)viGridPos->x * sprW;
  float ty = (float)viGridPos->y * sprH;

  bl->x = tx;
  bl->y = ty;

  br->x = tx + sprW;
  br->y = ty;

  tl->x = tx;
  tl->y = ty + sprH;

  tr->x = tx + sprW;
  tr->y = ty + sprH;

}
std::shared_ptr<AtlasSprite> Atlas::getSprite(Hash32 en) {
  ImgMap::iterator it = _mapImages.find(en);
  if (it == _mapImages.end()) {
    return nullptr;
  }
  return it->second;

}
int32_t Atlas::getLinearTileOffset(Hash32 frameId) {
  std::shared_ptr<AtlasSprite> sp = getSprite(frameId);
  AssertOrThrow2(sp != nullptr);
  int32_t ret = sp->_viGridPos.y * _vGridSize.x + sp->_viGridPos.x;


  int32_t dbg_x = ret % _vGridSize.x;
  int32_t dbg_y = ret / _vGridSize.x;

  return ret;
}

void Atlas::printInfoAndErrors(std::shared_ptr<Img32> sp) {

  GLint iMaxTextureSiz;//, maxColorAttachments;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTextureSiz);
  //Some atlas infor.
  BRLogInfo("Atlas size is " + sp->getWidth() + " x " + sp->getHeight()
    + "+ with sprite size " + getSpriteSize().x + " x " + getSpriteSize().y + " and has " + _mapImages.size() + " textures.\r\n"
    + " The Graphics card supports textures up to " + iMaxTextureSiz + " x " + iMaxTextureSiz + " units.\r\n"
    "Maximum Number of Textures for each texture size follows:" +
    "\r\n  " + ((getSpriteSize().x == 8) ? "*" : " ") + "8    x 8    : " + (iMaxTextureSiz / 8) * (iMaxTextureSiz / 8) + " textures."
    "\r\n  " + ((getSpriteSize().x == 16) ? "*" : " ") + "16   x 16   : " + (iMaxTextureSiz / 16) * (iMaxTextureSiz / 16) + " textures."
    "\r\n  " + ((getSpriteSize().x == 32) ? "*" : " ") + "32   x 32   : " + (iMaxTextureSiz / 32) * (iMaxTextureSiz / 32) + " textures."
    "\r\n  " + ((getSpriteSize().x == 64) ? "*" : " ") + "64   x 64   : " + (iMaxTextureSiz / 64) * (iMaxTextureSiz / 64) + " textures."
    "\r\n  " + ((getSpriteSize().x == 128) ? "*" : " ") + "128  x 128  : " + (iMaxTextureSiz / 128) * (iMaxTextureSiz / 128) + " textures."
    "\r\n  " + ((getSpriteSize().x == 256) ? "*" : " ") + "256  x 256  : " + (iMaxTextureSiz / 256) * (iMaxTextureSiz / 256) + " textures."
    "\r\n  " + ((getSpriteSize().x == 512) ? "*" : " ") + "512  x 512  : " + (iMaxTextureSiz / 512) * (iMaxTextureSiz / 512) + " textures."
    "\r\n  " + ((getSpriteSize().x == 1024) ? "*" : " ") + "1024 x 1024 : " + (iMaxTextureSiz / 1024) * (iMaxTextureSiz / 1024) + " textures."
    "\r\n  " + ((getSpriteSize().x == 2048) ? "*" : " ") + "2048 x 2048 : " + (iMaxTextureSiz / 2048) * (iMaxTextureSiz / 2048) + " textures."
    "\r\n  " + ((getSpriteSize().x == 4096) ? "*" : " ") + "4096 x 4096 : " + (iMaxTextureSiz / 4096) * (iMaxTextureSiz / 4096) + " textures."
  );

  if (sp->getWidth() > iMaxTextureSiz) {
    BRThrowException("Atlas: " + getName() + "The generated texture size is " + sp->getWidth() + ". Your graphics card can't handle texture sizes above " + iMaxTextureSiz + ".  TODO: Implement shrinkage.");
  }

}













}//ns Game
