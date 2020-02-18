/**
*  @file ProjectedRay.h
*  @date February 7, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __PROJECTEDRAY_225281493524253882519177_H__
#define __PROJECTEDRAY_225281493524253882519177_H__

#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
namespace BR2 {


class ProjectedRay : public VirtualMemory {
public:
  
  ProjectedRay() : _isOpt(false), Length(10000.0f) {}
  virtual ~ProjectedRay() override {}

  vec3 getOrigin() const { return Origin; }
  vec3 getBegin() const { return getOrigin(); }
  vec3 getDir() const { return Dir; }
  vec3 getEnd() const { return getOrigin() + getDir(); }

  vec3 getHitPoint(PlaneHit& ph) const;

  FORCE_INLINE void opt() {
    //**New - optimization
    //http://people.csail.mit.edu/amy/papers/box-jgt.pdf
    //Don't set to zero. We need infinity (or large value) here.
    //if(Dir.x != 0.0f)    
    InvDir.x = 1.0f / Dir.x;
    //else
//        InvDir.x = 0.0f;
//    if(Dir.y != 0.0f)    
    InvDir.y = 1.0f / Dir.y;
    //    else
    //        InvDir.y = 0.0f;
    //    if(Dir.z != 0.0f)    
    InvDir.z = 1.0f / Dir.z;
    //    else
    //        InvDir.z = 0.0f;

    Sign[0] = InvDir.x < 0;
    Sign[1] = InvDir.y < 0;
    Sign[2] = InvDir.z < 0;

    _isOpt = true;
  }
public:
  vec3 Origin;
  vec3 Dir;

  // Found the following two cool optimizations on WIlliams et. al (U. Utah)
  vec3 InvDir;
  int32_t Sign[3];

  bool _isOpt;    // - return true if  we optimized this

  float Length;// Max length

};

class Ray_t : public ProjectedRay {
public:
  Ray_t() {
    _t = FLT_MAX;
    _vNormal.construct(0, 0, 0);
  }
  virtual ~Ray_t() override {
  }

  const vec3& getNormal() { return _vNormal; }
  void setNormal(vec3& v) { _vNormal = v; }
  float getTime() const {
    return _t;
  }
  void setTime(float f) {
    _t = f;
  }
  bool isHit()  const {
    return _t >= 0.0f && _t <= 1.0f;
  }
  vec3 getHitPoint() const {
    vec3 ret = getBegin() + (getEnd() - getBegin()) * getTime();
    return ret;
  }

private:
  float _t;
  vec3 _vNormal;
};

}//ns BR2



#endif
