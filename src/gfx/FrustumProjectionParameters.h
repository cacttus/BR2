/**
*  @file FrustumProjectionParameters.h
*  @date December 5, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __FRUSTUMPROJECTIONPARAMETERS_171293099427757171585801_H__
#define __FRUSTUMPROJECTIONPARAMETERS_171293099427757171585801_H__


#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
/**
*  @class FrustumProjectionParameters
*  @brief Fill this class out for the frustum to create its volume
*/
class FrustumProjectionParameters : public VirtualMemory {
public:
  vec3 _camView; //camera view
  vec3 _camPos; //camera position
  vec3 _upVec; // camera up vec
  Vec3Basis _v3basis;    // basis vectors for projection with Z pointing into the viewport.
};
}//ns game



#endif
