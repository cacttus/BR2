#include "../math/Random.h"
#include "../base/TypeConv.h"

#include <time.h>

namespace BR2 {
int32_t Random::_last = 1283094874;//Arbitrary
Random::Random() {
#ifdef _USE_C_RAND
  srand((unsigned int)time(NULL));
#else
  _last = (int32_t)time(NULL);
#endif
}
Random::~Random() {
}
t_string Random::randomNumericString(int nDigits) {
  t_string ret;
  for (int i = 0; i < nDigits; ++i) {
    ret += TypeConv::intToStr(nextInt32(0, 9));
  }
  return ret;
}


}//ns game
