//GUI Shader
#include "glsl_version.sh"


layout(location = 0) in vec2 _v201;
layout(location = 1) in vec2 _x201;


out vec2 _x2Out;
out vec3 _v3Out;
void main()
{
    //this is the stuff from the GUI shader.  It should work.  Note we don't even need a matrix
	_x2Out = _x201;
    _v3Out = vec3(_v201.x, _v201.y, -1);
	gl_Position =  vec4(_v201.x, _v201.y, -1, 1);
}