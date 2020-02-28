#include "glsl_version.sh"


layout(location = 0) in vec3 _v301;
layout(location = 1) in vec3 _c301;
layout(location = 2) in vec2 _x201;
layout(location = 3) in vec3 _n301;

out vec3 _v3Out;
out vec3 _c3Out;
out vec2 _x2Out;
out vec3 _n3Out;

uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix

void main()
{
	_x2Out = _x201;
	_v3Out = _v301;
	_c3Out = _c301;
    _n3Out = _n301;
    
	gl_Position =  _ufProj * _ufView * vec4(_v301,1);	
}