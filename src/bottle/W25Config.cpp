#include "../bottle/W25Config.h"


namespace BR2 {
W25Config::W25Config() {

}
W25Config::~W25Config() {

}
int32_t W25Config::getSeed() {
  if (getDebugSeed() != 0) {
    return getDebugSeed();
  }
  else {
    if (_iGameSeed == 0) {
      _iGameSeed = Random::nextInt32();
    }
    return _iGameSeed;
  }
}
bool W25Config::hideTransparentSide(W25Geom opaque, W25Geom transparent, int iSide) {
  //if the same bits are set on botht he transparent and opaque side, then the config is the same
  //for that side isn't it?  so we hide the transparent tris on that side
#define side_bits(a,b,c,d) (W25Geom)(1 << a) | (W25Geom)(1 << b) | (W25Geom)(1 << c) | (W25Geom)(1 << d)
  int side[6];
  side[0] = side_bits(0, 2, 4, 6);
  side[1] = side_bits(1, 3, 5, 7);
  side[2] = side_bits(0, 1, 4, 5);
  side[3] = side_bits(2, 3, 6, 7);
  side[4] = side_bits(0, 1, 2, 3);
  side[5] = side_bits(4, 5, 6, 7);

  W25Geom op = opaque & side[iSide];
  W25Geom tp = transparent & side[iSide];
  //Assuming L R B T A F
  return op == tp;

}






}//ns Game
