#include "glsl_version.sh"


//Each triangle has 2 edges tri = 3 * 2 quad * 2 edge = 3 * 2 * 2 = 12
layout(triangles) in;
layout(triangle_strip, max_vertices=8) out;

in vec3 _v3Vs[];
in vec3 _n3Vs[];

uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix

uniform uint _ufShowSide;

out float _fGsDist;
smooth out float _fSide;

uniform float _t01;
#define M_PI 3.1415926535897932384626433832795

float affect2(float d) {
   // float dmod = mod(d, 6.0f)/6.0f; /** 2.0f - 1.0f*/;//[-1, 1]
  //  float time = (cos(_t01 * M_PI*2) * dmod + 1.0f) / 2.0f  ;
      float radius = 24;
    float time = (cos(_t01*M_PI*2 + d/radius * M_PI*2) + 1.0f) / 2.0f;

    return time;
}
float affect(vec3 v) {
    return affect2( distance(v, vec3(0,0,0)) );
}
//Note: technically this algorithm is not correct since we're emitting 8 vertexes, we really only need 6 for the 2 legs 2 per corner 
void smooth_line(vec3 base0, vec3 base1, vec3 right, float lineWidth, vec3 inner_n, vec3 my_dir, vec3 up_0, vec3 up_1){
    //inner_n - z fighting, to fix this simply move the lines in towards the triangle, inner_n points into the triangle
    vec3 adjust = inner_n * lineWidth ;
    float d1 = dot(right,inner_n) > 0 ? 1 : 0;//riht o
    
    //Apply a little animation to the grid.
   
    float fr0 = affect(base0);// sine wave modulus of each point.
    float fr1 = affect(base1);// modulu
    
    float fHeightMul = 0.01;// "wave" effect, small, super super small
    
    
    vec3 v[4];
    float f[4];
    v[0] = base0 + right * (lineWidth ) + adjust + (up_0 * (fr0 * lineWidth * fHeightMul));
    v[1] = base0 - right * (lineWidth ) + adjust + (up_0 * (fr0 * lineWidth * fHeightMul));
    v[2] = base1 + right * (lineWidth ) + adjust + (up_1 * (fr1 * lineWidth * fHeightMul));
    v[3] = base1 - right * (lineWidth ) + adjust + (up_1 * (fr1 * lineWidth * fHeightMul));

    //To get the corners right we need to push 1 vertex in.
    vec3 corner_push = my_dir * lineWidth;
    if(d1>0){
        v[0] += corner_push;
        v[2] -= corner_push;
    }
    else{
        v[1] += corner_push;
        v[3] -= corner_push;
    }
    
    //**Flop 0/1 based on which direction we're pointing 2/27/18 
    f[0] = d1;
    f[1] = 1.0f - d1;    
    f[2] = d1;   //
    f[3] = 1.0f - d1;
    
    //Emit 4 verts
    for(int i=0; i<4; ++i){
        gl_Position = _ufProj * _ufView * vec4(v[i],1);
        _fGsDist = gl_Position.w;
        _fSide = f[i];
        EmitVertex();
    }
    
}

void make_lines(){
    float fLineWidth = 0.08f; //** Width of grid lines, make a uniform
    float fGridDist = 0.05f; // **Distance from the triangle plane, this can be a uniform, if we want
    vec3 dir0, dir1, right0, right1, base00, base01, base10, base11;

    //line 0
    base00 = _v3Vs[0] + _n3Vs[0] * fGridDist;    
    base01 = _v3Vs[1] + _n3Vs[1] * fGridDist;  
    dir0 = normalize(base01 - base00);
    right0 = normalize(cross(dir0, _n3Vs[0]));
        
    //line1    
    base10 = _v3Vs[1] + _n3Vs[1] * fGridDist;    
    base11 = _v3Vs[2] + _n3Vs[2] * fGridDist;    
    dir1 = normalize(base11 - base10);
    right1 = normalize(cross(dir1, _n3Vs[1]));
 
    smooth_line(base00, base01, right0, fLineWidth, dir1, dir0, _n3Vs[0], _n3Vs[1]);//dir1 - in towards triangle.// + fLineWidth..don't overlap previous line
    smooth_line(base10, base11, right1, fLineWidth, dir0, dir1, _n3Vs[1], _n3Vs[2]);//in towrards triangel //don't overlap previous line
    
}
//if the triangle indices are in order we can easily create a grid without th eneed for an extra mesh.
//TODO: fix texture problem after reordering indices

void main() {

    if(_ufShowSide==0 ){
        if(dot(_n3Vs[0], vec3(0,1,0)) > 0.5) {
            make_lines();
        }
    }
    else {
        make_lines();
    }
 
}

















