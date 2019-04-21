#include "glsl_version.sh"



layout(location = 0) in vec3 _v301;
layout(location = 1) in vec2 _x201;

out vec3 _v3Out;
out vec2 _x2Out;

uniform mat4 _ufProj;	//projection matrix


void main()
{
	_x2Out = _x201;	
	gl_Position =  _ufProj * vec4(_v301.xy,0,1);
}