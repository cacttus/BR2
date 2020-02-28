/**
*	Deferred Shader 
*	20150404
*/
#include "glsl_version.sh"


layout(location = 0)in vec3 _v301;
layout(location = 1)in vec2 _x201;	//NOTE!!: color is face normal

uniform mat4 _ufProj;

out vec4 _vertexOut;
out vec2 _tcoordOut;

void main() 
{
	_vertexOut		= _ufProj * vec4(_v301,1);
	_tcoordOut 		= _x201;
	gl_Position = _vertexOut;
}

