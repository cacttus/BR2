#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec4 _rectVS[];
in vec4 _clipVS[];
in vec4 _texVS[];
in vec2 _texsizVS[];
flat in uvec2 _pick_colorVS[];

out vec2 _vert;
out vec2 _tex;

flat out vec4 _clip;
flat out vec2 _texSiz;//We can do compute this in the GS and output better data before the PS
flat out uvec2 _pick_color;
flat out vec2 _texPos;//don't interpolate


float p0x(vec4 f) { return f.x; }
float p0y(vec4 f) { return f.y; }
float p1x(vec4 f) { return f.z; }
float p1y(vec4 f) { return f.w; }

void setGS(){
//Uniform primitie values
    _clip        = _clipVS[0];
    _texSiz      = _texsizVS[0];
    _pick_color  = _pick_colorVS[0];
    _texPos      = vec2(_texVS[0].x, _texVS[0].y);//not sure why xw.  x,y is already in opengl's BL oeiinf
}

void main() {
    /*
    0x0y                   1x0y
        0------------ ----2
        |          /      |
        |  /              |
        1 ----------------3
    0x1y                   1x1y
    */
    
    setGS();
    _tex            = vec2(p0x(_texVS [0]), p0y(_texVS [0]));
    _vert           = vec2(p0x(_rectVS[0]), p0y(_rectVS[0]));
    gl_Position     = vec4(p0x(_rectVS[0]), p0y(_rectVS[0]), -1, 1);
    EmitVertex();
    
    setGS();
    _tex            = vec2(p0x(_texVS [0]), p1y(_texVS [0]));
    _vert           = vec2(p0x(_rectVS[0]), p1y(_rectVS[0]));
    gl_Position     = vec4(p0x(_rectVS[0]), p1y(_rectVS[0]), -1, 1);
    EmitVertex();
    
    setGS();
    _tex            = vec2(p1x(_texVS [0]), p0y(_texVS [0]));
    _vert           = vec2(p1x(_rectVS[0]), p0y(_rectVS[0]));
    gl_Position     = vec4(p1x(_rectVS[0]), p0y(_rectVS[0]), -1, 1);
    EmitVertex();
    
    setGS();
    _tex            = vec2(p1x(_texVS [0]), p1y(_texVS [0]));
    _vert           = vec2(p1x(_rectVS[0]), p1y(_rectVS[0]));
    gl_Position     = vec4(p1x(_rectVS[0]), p1y(_rectVS[0]), -1, 1);
    EmitVertex();
    
    EndPrimitive();

    
    
}





