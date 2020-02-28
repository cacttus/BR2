#include "glsl_version.sh"


layout(location = 0) in vec3 _v301;
layout(location = 1) in vec3 _n301;

out vec3 _v3Vs;
out vec3 _n3Vs;

void main() {
    _n3Vs = _n301;
    _v3Vs = _v301;
}