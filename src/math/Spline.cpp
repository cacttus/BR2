#include "../math/Spline.h"
#include "../base/Logger.h"

namespace BR2 {

Spline::Spline() {
}
Spline::~Spline() {
}
void Spline::calculate() {
  const double integral_t = 1.0f / 100.0f;

  //Calculate point lengths
  _plens.clear();
  double len = 0;
  int64_t ind = -1;
  iterate(0, _points.size() - 1,
    [&len, this, &ind](const vec3& cur, const vec3& last, size_t pointIndex) {
      double dlen = (cur - last).lengthd();
      len += dlen;
      if (pointIndex != ind) {
        _plens.push_back(len);
        ind = pointIndex;
      }
    }, integral_t);

  _length = _plens[_plens.size() - 1];

  LogAssert(_points.size() == _plens.size());
}

}//ns Game
