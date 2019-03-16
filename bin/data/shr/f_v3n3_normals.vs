#version 330 core

#include "df_skin.sh"

layout(location = 0) in vec3 _v301;
layout(location = 1) in vec3 _n301;

out vec3 _v3VS;
out vec3 _n3VS;

void main()
{
   vec3 v3 = _v301;
   vec3 n3 = _n301;

    if(_ufWeightCount>0) {
        doSkin(v3, n3);
    }

    _v3VS = v3;
    _n3VS = n3;

	gl_Position =  vec4(_v301,1); //
}