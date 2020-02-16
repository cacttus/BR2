//cube map shadows
//MetalMario971
//20160426
#version 430
#include "df_skin.sh"

layout(location = 0)in vec3 _v301;//_vertex;

uniform mat4 _ufView;
uniform mat4 _ufProj;
uniform mat4 _ufModel;

out vec3 _v3Out;
void main()
{
    vec3 v3 = _v301;
    vec3 n3 = vec3(0,1,0);
    if(_ufWeightCount>0) {
        doSkin(v3, n3);
    }
    
	_v3Out = vec3(_ufModel * vec4(v3,1));
	gl_Position =  _ufProj * _ufView * _ufModel * vec4(v3,1);
}
