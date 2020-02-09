#include "../gfx/TexCache.h"
#include "../base/Hash.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../gfx/Texture2DSpec.h"

namespace Game {

const std::string TexCache::WorldGrass = "./data/tx32-grass.png";
const std::string TexCache::WorldDirt = "./data/tx64_dirt.png";
const std::string TexCache::WorldPlank = "./data/tx64_plank.png";
const std::string TexCache::WorldBrick = "./data/tx64_brick.png";

const std::string TexCache::MdWoodItem = "./data/tx-itm-wood.png";

const std::string TexCache::Selector = "./data/tx-sel.png";
const std::string TexCache::SelectorRed = "./data/tx-sel-rd.png";

const std::string TexCache::Brick64Des = "./data/tx64-brick-des.png";
const std::string TexCache::WalWd = "./data/tx-w-wd.png";
const std::string TexCache::RockWd = "./data/tx-rock2.png";
const std::string TexCache::GrassWd = "./data/tx-gs.png";
const std::string TexCache::RopeWd = "./data/tx64-rope.png";
const std::string TexCache::LadderWd = "./data/tx64-ladder.png";

///////////////////////////////////////////////////////////////////
TexCache::TexCache(std::shared_ptr<GLContext> ct) : _pContext(ct) {
  int iWidthHeight = 1;
  vec4 v(1, 1, 1, 1);

  glGenTextures(1, &_i1x1DummyCubeTexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _i1x1DummyCubeTexture);
  for (int i = 0; i < 6; ++i) {
    //Note values here must match ShadowBox due to glCopyTexSubImage
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT,
      iWidthHeight, iWidthHeight, 0, SHADOW_CUBE_MAP_TEX_FORMAT, SHADOW_CUBE_MAP_TEX_TYPE, (void*)&v);
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glGenTextures(1, &_i1x2Dummy2DTexture);
  glBindTexture(GL_TEXTURE_2D, _i1x2Dummy2DTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, iWidthHeight, iWidthHeight, 0, GL_RGBA, GL_FLOAT, (void*)&v);
  glBindTexture(GL_TEXTURE_2D, 0);

  v.construct(0, 0, 1, 1);//XZY Bump texture up
  glGenTextures(1, &_i1x1DummyBump2DTexture);
  glBindTexture(GL_TEXTURE_2D, _i1x1DummyBump2DTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, iWidthHeight, iWidthHeight, 0, GL_RGBA, GL_FLOAT, (void*)&v);
  glBindTexture(GL_TEXTURE_2D, 0);
}
TexCache::~TexCache() {
  TexMap::iterator ite = _cache.begin();
  //for (; ite != _cache.end(); ite++) {
  //    std::shared_ptr<Texture2DSpec> sp = ite->second;
  //    DEL_MEM(sp);
  //}
  _cache.clear();
}
bool TexCache::add(t_string name, std::shared_ptr<Texture2DSpec> ss, bool bErrorIfFound) {
  Hash32 ih = Hash::computeStringHash32bit(name, 0);
  TexMap::iterator it = _cache.find(ih);
  if (it != _cache.end()) {
    if (bErrorIfFound) {
      BroLogError("Texture cache had duplicate texure: " + name);
      Gu::debugBreak();
    }
    return false;
  }
  _cache.insert(std::make_pair(ih, ss));
  return true;
}
///////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<Texture2DSpec>> TexCache::getOrLoad(std::vector<std::string> texName, bool bIsGenerated, bool bRepeatU, bool bRepeatV) {
  std::vector<std::shared_ptr<Texture2DSpec>> out;
  for (std::string str : texName) {
    std::shared_ptr<Texture2DSpec> tx = getOrLoad(str, bIsGenerated, bRepeatU, bRepeatV);
    out.push_back(tx);
  }
  return out;
}
std::shared_ptr<Texture2DSpec> TexCache::getOrLoad(std::string texName, bool bIsGenerated, bool bRepeatU, bool bRepeatV) {
  std::shared_ptr<Texture2DSpec> ret = nullptr;

  int32_t ih = Hash::computeStringHash32bit(texName, 0);
  TexMap::iterator ite = _cache.find(ih);

  if (ite != _cache.end()) {
    ret = ite->second;
  }
  else if (bIsGenerated == false) {
    ret = std::make_shared<Texture2DSpec>(texName, _pContext, bRepeatU, bRepeatV);
    _cache.insert(std::make_pair(ih, ret));
  }

  return ret;
}
std::shared_ptr<Texture2DSpec> TexCache::addAsGeneratedImage(t_string name, const std::shared_ptr<Img32> ss) {
  std::shared_ptr<Texture2DSpec> pRet = std::make_shared<Texture2DSpec>(ss, _pContext);
  add(name, pRet);

  return pRet;
}
//
//void TexCache::bindIfDifferent(std::shared_ptr<Texture2DSpec> tex) {
//    //Efficiently bind only the textures we need to.
//    //**No longer are we going to use this because it's causing some
//   //synch issues - this is an optimization that we can enable in the future
//    //in the framebuffer swap we're binding a new textuer so we have to account for everywhere we call glBindTexture
//    if (tex == nullptr) {
//        glBindTexture(GL_TEXTURE_2D, 0);
//    }
//    else{// if (_pBound != tex) {
//        tex->bind();
//    }
//    _pBound = tex;
//}



}//ns Game
