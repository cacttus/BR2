
#version 330 core
layout(location = 0)in vec2 _v201; //_vertex;
layout(location = 1)in vec4 _c401; //_color;
out vec4 _colorOut;

void main() {
	_colorOut = _c401;

	// peanut vendor man
	gl_Position = vec4(_v201.x, _v201.y, -1, 1);
}
