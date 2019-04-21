#include "glsl_version.sh"


layout(triangles) in;
layout(line_strip, max_vertices=6) out;

uniform float _ufNormalLength;
uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix
uniform mat4 _ufModel;

uniform vec3 _ufCamPos;

in vec3 _v3VS[];
in vec3 _n3VS[];

out float _fColorDevGS;

void main() {

    mat4 pvm = _ufProj * _ufView;
    
    for(int i=0; i<gl_in.length(); i++) {
        vec3 v3 = _v3VS[i];
        vec3 n3 = _n3VS[i];
    
        vec3 pos = (_ufModel * vec4(v3,1)).xyz;
        vec3 norm = normalize((_ufModel * vec4(v3 + n3,1)).xyz - pos);//*Note: this must match the computation in d_v3n3_diffuse &c.

        vec4 vv = pvm * vec4(pos, 1.0);
        vec4 vn = pvm * vec4(pos + norm * _ufNormalLength, 1.0);
        
        //Deviance from the camera [0,1]
        _fColorDevGS = (dot(normalize(_ufCamPos - pos), norm) + 1.0) * 0.5;
        
        gl_Position = vv;
        gl_Position = vv;
        EmitVertex();

        gl_Position = vn;
        EmitVertex();

        EndPrimitive();
    }

}









