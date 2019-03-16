//DOF shader 2/9/2018 230 am WTF?? go to sleep 
#version 330 core

layout(location = 0)in vec3 _v301;
layout(location = 1)in vec2 _x201;
uniform mat4 _ufProj;
out vec2 _tcoordOut;

void main()
{
	_tcoordOut = _x201;
	gl_Position =  _ufProj * vec4(_v301,1);
}
