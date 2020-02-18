/**
*  @file RaycastHit.h
*  @date May 26, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __RAYCASTHIT_14642406831305391755_H__
#define __RAYCASTHIT_14642406831305391755_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"

namespace BR2 {
/**
*  @struct RaycastHit
*  @brief Returns hit test data from a projected ray.
*/
class RaycastHit : public VirtualMemory {
public:
  RaycastHit() {
    reset();
  }
  bool trySetClosestHit(float& __inout_ closest_t) {
    //Easy way of iterating a closest hit.
    if (_bHit && (_t < closest_t)) {
      closest_t = _t;
      return true;
    }
    return false;
  }
  void* getPickData() {
    return _pPickData;
  }
  void reset() {
    _bHit = false;
    _p1Contained = false;
    _p2Contained = false;
    _t = FLT_MAX;
    _pPickData = NULL;
  }
  void copyFrom(RaycastHit* bh) {
    this->_bHit = bh->_bHit;
    this->_p1Contained = bh->_p1Contained;
    this->_p2Contained = bh->_p2Contained;
    this->_t = bh->_t;
    this->_pPickData = bh->_pPickData;
  }
public:
  bool _bHit;    // Whether the ray intersected the box.
  bool _p1Contained;
  bool _p2Contained;
  float _t; // - Time to hit [0,1]
  void* _pPickData; // picked object (BvhObject3*)
  vec3 _vNormal; //The normal of the plane the raycast hit.
  //Do not include ray data for optimization.


};

}//ns BR2



#endif
