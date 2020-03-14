#include "../math/Spline.h"


namespace BR2 {

Spline::Spline() {
}
Spline::~Spline() {
}
void Spline::calculate() {
  //Calculate length
  _length = length(0, _points.size() - 1);
}
double Spline::length() {
  return _length;
}
double Spline::length(size_t pointA, size_t pointB, double integral_t) {
  double len = 0;
  iterate(pointA, pointB,
    [&len](const vec3& cur, const vec3& last) {
      len += (cur - last).lengthd();
    }, integral_t);
  return len;
}

}//ns Game
