#include "../math/Random.h"
#include "../base/TypeConv.h"

namespace BR2 {
int32_t Random::_last = 1283094874;//Arbitrary

string_t Random::randomNumericString(int nDigits) {
  string_t ret;
  for (int i = 0; i < nDigits; ++i) {
    ret += TypeConv::intToStr(nextInt32(0, 9));
  }
  return ret;
}


}//ns BR2
