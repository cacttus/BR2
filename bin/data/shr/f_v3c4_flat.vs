
#version 430
layout(location = 0)in vec3 _v301; //_vertex;
layout(location = 1)in vec4 _c401; //_color;


out vec4 _colorOut;

uniform mat4 _ufModel;
uniform mat4 _ufView;
uniform mat4 _ufProj;

void main() {
	_colorOut = _c401;

	// peanut vendor man
	gl_Position = (_ufProj * _ufView * _ufModel) * vec4(_v301,1);
}
