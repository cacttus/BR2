#version 430

layout(location = 0) in vec3 _v301;
layout(location = 1) in vec3 _n301;
layout(location = 2) in vec2 _x201;

out vec3 _v3Out;
out vec2 _x2Out;
out vec3 _n3Out;

uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix
uniform mat4 _ufModel; // model space
uniform mat4 _ufNormal;

void main()
{
    _n3Out = normalize(mat3(_ufNormal) * _n301);
	//_n3Out = normalize(vec3(_ufNormal * vec4(_n301,1)));
	_x2Out = _x201;
	_v3Out = vec3(_ufModel * vec4(_v301,1));
	
	gl_Position =  _ufProj * _ufView * _ufModel * vec4(_v301,1);	
}