#version 330 core
#include "df_skin.sh"
layout(location = 0) in vec3 _v301;
layout(location = 1) in vec3 _n301;

out vec3 _v3Out;
out vec3 _n3Out;

uniform mat4 _ufView;
uniform mat4 _ufProj;	
uniform mat4 _ufModel;

void main()
{
    vec3 v3 = _v301;
    vec3 n3 = _n301;
    if(_ufWeightCount>0) {
        doSkin(v3, n3);
    }
    
    //*Make sure normal computation matches f_v3n3_normals.vs / gs
    _v3Out = vec3(_ufModel * vec4(v3,1));
    _n3Out = normalize((_ufModel * vec4(v3 + n3, 1)).xyz - _v3Out);

	
	gl_Position =  _ufProj * _ufView * _ufModel * vec4(v3,1);	
}