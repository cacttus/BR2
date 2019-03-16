#version 330 core

//Each triangle has 2 edges tri = 3 * 2 quad * 2 edge = 3 * 2 * 2 = 12
layout(points) in;
layout(triangle_strip, max_vertices=18) out;

in vec3 _v3Vs[];
in vec3 _n3Vs[];

uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix
uniform float _t01;
#define M_PI 3.1415926535897932384626433832795

out float _fGsDist;

float affect2(float d) {
    float radius = 24;
    float time = (cos(_t01*M_PI*2 + d/radius * M_PI*2) + 1.0f) / 2.0f;

    return time;
}
float affect(vec3 v) {
    return affect2( distance(v, vec3(0,0,0)) );
}
//if the triangle indices are in order we can easily create a grid without th eneed for an extra mesh.
//TODO: fix texture problem after reordering indices
void main() {
    float fBot = 2.0f; 
    float fTop = 1.0f;
    float fWidth = 0.5f;
    vec3 n = _n3Vs[0];
    
    vec3 vx;
    if(n.x != 1.0){
        vx = vec3(1,0,0);
    }
    else {
        vx = vec3(0,1,0);
    }
    

    vec3 t = normalize(cross(vx, n));
    vec3 b = normalize(cross(n, t));
    
    vec3 vbot = _v3Vs[0];
    vec3 vc0 = vbot + n * fBot + b * fWidth;
    vec3 vc1 = vbot + n * fBot + t * fWidth;
    vec3 vc2 = vbot + n * fBot - b * fWidth;
    vec3 vc3 = vbot + n * fBot - t * fWidth;
    vec3 vtop = vbot + n * (fBot + fTop);
   
    mat4 m = _ufProj * _ufView;
   
   //testing
    gl_Position = m * vec4(vc0,1);
    _fGsDist = gl_Position.w;
    EmitVertex();    
    gl_Position = m * vec4(vbot,1);
    _fGsDist = gl_Position.w;
    EmitVertex();
    gl_Position = m * vec4(vc1,1);
    _fGsDist = gl_Position.w;
    EmitVertex();     
    
    gl_Position = m * vec4(vc1,1);
    _fGsDist = gl_Position.w;
    EmitVertex();   
    gl_Position = m * vec4(vbot,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    gl_Position = m * vec4(vc2,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    
   gl_Position = m * vec4(vc2,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
   gl_Position = m * vec4(vbot,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    gl_Position = m * vec4(vc3,1);
    _fGsDist = gl_Position.w;
    EmitVertex();      
    
 //bot
    
    gl_Position = m * vec4(vc1,1);
    _fGsDist = gl_Position.w;
    EmitVertex();    
    gl_Position = m * vec4(vtop,1);
    _fGsDist = gl_Position.w;
    EmitVertex();
    gl_Position = m * vec4(vc0,1);
    _fGsDist = gl_Position.w;
    EmitVertex();     
    
    gl_Position = m * vec4(vc2,1);
    _fGsDist = gl_Position.w;
    EmitVertex();   
    gl_Position = m * vec4(vtop,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    gl_Position = m * vec4(vc1,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    
   gl_Position = m * vec4(vc3,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
   gl_Position = m * vec4(vtop,1);
    _fGsDist = gl_Position.w;
    EmitVertex();  
    gl_Position = m * vec4(vc2,1);
    _fGsDist = gl_Position.w;
    EmitVertex();      
    
    
}

















