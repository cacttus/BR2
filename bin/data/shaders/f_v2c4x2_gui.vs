#include "glsl_version.sh"


layout(location = 0) in vec2 _v201;
layout(location = 1) in vec4 _c401;
layout(location = 2) in vec2 _x201;

out vec2 _x2Out;
out vec4 _c4Out;
out vec2 _v2Out;
void main() {
	_v2Out = _v201;
    _c4Out = _c401;
    _x2Out = _x201;
	gl_Position =  vec4(_v201.x, _v201.y, -1, 1);	
}