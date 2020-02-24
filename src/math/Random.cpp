#include "../math/Random.h"
#include "../base/TypeConv.h"

namespace Game {
int32_t Random::_last = 1283094874;//Arbitrary

t_string Random::randomNumericString(int nDigits) {
  t_string ret;
  for (int i = 0; i < nDigits; ++i) {
    ret += TypeConv::intToStr(nextInt32(0, 9));
  }
  return ret;
}


}//ns game
