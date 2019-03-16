/**
*
*    @file Vec3Basis.h
*    @date December 4, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __VEC3BASIS_16067298231458192238031_H__
#define __VEC3BASIS_16067298231458192238031_H__

#include "../math/Vec3x.h"

namespace Game {;


/*
    Defines a set of basis vectors.
    This is used for viewport calcs
    Z points into the viewport, i.e. we look down the Z axis.
*/
class Vec3Basis : public VirtualMemory {
public:

    Vec3f _x, _y, _z;
};

}//ns game



#endif
