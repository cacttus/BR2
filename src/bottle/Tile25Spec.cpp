#include "../base/Hash.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/SpriteBucket.h"

namespace BR2 {
Tile25Spec::Tile25Spec(string_t name, uint8_t iIndex, GridMeshLayer::e em, std::shared_ptr<SpriteSpec> top, std::shared_ptr<SpriteSpec> side,
  std::shared_ptr<SpriteSpec> bot, ClimateSpec& cp, float fRarity) {

  static int32_t iGpuIndex = 0;
  _strName = name;
  _iHashName = STRHASH(_strName);
  _iTile = iIndex;
  _eMatterMode = em;
  _fRarity = fRarity;
  _pClimateSpec = new ClimateSpec();

  *_pClimateSpec = cp;

  _pTop = new Sprite(top);
  _pSide = new Sprite(side);
  _pBot = new Sprite(bot);

  _pTop->setGpuIndex(iGpuIndex++);
  _pSide->setGpuIndex(iGpuIndex++);
  _pBot->setGpuIndex(iGpuIndex++);
}
Tile25Spec::~Tile25Spec() {
  DEL_MEM(_pTop);
  DEL_MEM(_pSide);
  DEL_MEM(_pBot);
  DEL_MEM(_pClimateSpec);
}
MorphTile::MorphTile(string_t strName) {
  _strName = strName;
  _iNameHashed = STRHASH(strName);
}
MorphTile::~MorphTile() {

}








}//ns Game
