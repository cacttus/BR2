#version 330 core

layout(location = 0) in vec3 _v301;
layout(location = 1) in ivec2 _i201;
layout(location = 2) in vec3 _n301;

out vec3 _v3Out;
out vec2 _x2Out;
out vec3 _n3Out;

uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix

uniform int _ufTilesW;
uniform int _ufTilesH;
uniform float _ufSprW; // THis is the Sprite width / texture width
uniform float _ufSprH;

// #define DEBUG_SHOW_TEX 1

layout(std140) uniform GpuSprites {
    int _ufGpuSprites[128];
};

void main() {
    int iCoord = _i201.x;// bl = 0, br = 1, tl = 2 tr = 3
    int iBufferIndex = _i201.y;
   
    //This is the linear sprite index in the grid.
    int iSpriteOff = _ufGpuSprites[iBufferIndex];
    
    int iTile_x = iSpriteOff % _ufTilesW;
    int iTile_y = iSpriteOff / _ufTilesH;

    iTile_y = _ufTilesH - iTile_y - 1; // Flip the texture so that we reference from Top Left -1 because just it's needed
    
    _x2Out.x = float(iTile_x) * _ufSprW;// = (itile * spritePxW) / texturePxW =  tile * spritePxW/texturePxW
    _x2Out.y = float(iTile_y) * _ufSprH;


    if(iCoord == 1 || iCoord == 3){
        _x2Out.x += _ufSprW;
    }
    if(iCoord == 2 || iCoord == 3){
        _x2Out.y += _ufSprH;
    }

	_v3Out = _v301;

    _n3Out = _n301;
    
    #ifdef DEBUG_SHOW_TEX
    //Debug show whole texture
    _x2Out.x = _x2Out.y = 0.0f;
    if(iCoord == 1 || iCoord == 3){
     // //Add W
      _x2Out.x += 1.0f;
    }
    if(iCoord == 2 || iCoord == 3){
     // //add H
      _x2Out.y += 1.0f;
    }

    float fuck = float(_ufTilesW) + float(_ufTilesH) + _ufSprW + _ufSprH + float(_ufGpuSprites[0]) +  _n301.x;
    fuck = clamp(fuck, 0, 0.00000001f);

    //Debug
    #endif
    
	gl_Position =  _ufProj * _ufView * vec4(_v301,1);	
}