#version 430
//#version 330 core

layout(location = 0) in vec2 _v201;
layout(location = 1) in vec4 _c401;
layout(location = 2) in vec2 _x201;

out vec2 _x2Out;
out vec4 _c4Out;

void main() {
	_x2Out = _x201;
    _c4Out = _c401;
	gl_Position =  vec4(_v201.x, _v201.y, -1, 1);	
}