/**
*  @file GpuQuad2.h
*  @date March 10, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __GPUQUAD2_1926533372616313719120_H__
#define __GPUQUAD2_1926533372616313719120_H__

#include "../gfx/GfxHeader.h"
#include "../math/MathHeader.h"
namespace BR2 {
/**
*    @class GpuQuad3 
*    @brief
*/
class GpuQuad3 /*do not inheitie */{
public:
    /*
        Quad Winding:
        CCW
        2
    v0    +-----------+ 1/5   v1
        | T0      / |
        |     /  T1 |
    v2    + /         + 4  v3
        0/3
        indexes: 2,1,0, 2,3,1
    */
    v_v3n3x2* _vertex[4];
    void debugValidate();
    void copyQuadPos(Quad3f* rcRect);
    void copyQuadTcoords(Box2f* rcRect);
    void copyQuadColor(Color4f* color);
    void copyQuadNormal(vec3*);

    GpuQuad3();
    //Note: do not make this dtor virutal. bc if you do we get a vf pointer and this isn't a pure structure
    // so then sadly we cannot use it in the quadbuffermesh
    ~GpuQuad3();
};


}//ns game



#endif
