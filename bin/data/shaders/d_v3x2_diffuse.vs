#include "glsl_version.sh"

#include "df_skin.sh"
layout(location = 0) in vec3 _v301;
layout(location = 1) in vec2 _x201;

out vec3 _v3Out;
out vec2 _x2Out;

uniform mat4 _ufView;
uniform mat4 _ufProj;	
uniform mat4 _ufModel; 

void main()
{
    vec3 v3 = _v301;
    vec3 n3 = vec3(0,1,0);
    if(_ufWeightCount>0) {
        doSkin(v3, n3);
    }

	_x2Out = _x201;
	_v3Out = vec3(_ufModel * vec4(v3,1));
	
	gl_Position =  _ufProj * _ufView * _ufModel * vec4(v3,1);	
}