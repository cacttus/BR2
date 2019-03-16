//20151122
#version 330 core

layout(location = 0)in vec2 _v201;// _v3f_01
layout(location = 1)in vec2 _x201; // _x2f_01

uniform mat4 _ufView;
uniform mat4 _ufProj;
uniform mat4 _ufModel;

out vec4 _vertexOut;
out vec2 _tcoordOut;

void main()
{
	_tcoordOut = _x201;
	_vertexOut = _ufModel * vec4(_v201,0,1);
	
	gl_Position =  _ufProj * _ufView * _ufModel * vec4(_v201,0,1);
}
