/**
*  @file Line3.h
*  @date December 5, 2014
*  @author MetalMario971
*
*  @file Line3
*  @date 3 20 2010
*    20141205 Major updates for SAT
*/
#pragma once
#ifndef __LINE3_1391916606309252141724654_H__
#define __LINE3_1391916606309252141724654_H__

#include "../math/Vec3x.h"

namespace BR2 {

/**
*  @class Line3f
*  @breif Line class.  This is going to be used for SAT
*/
class Line3f : public VirtualMemory {
public:
    vec3 _p[2];
    FORCE_INLINE vec3& p0(){ return _p[0]; }
    FORCE_INLINE vec3& p1(){ return _p[1]; }

    FORCE_INLINE Line3f();
    FORCE_INLINE Line3f(vec3& v1, vec3&  v2);
    FORCE_INLINE virtual ~Line3f() override;
    FORCE_INLINE void construct(vec3& dp0, vec3& dp1);

    FORCE_INLINE vec3 pointOnLine(vec3& pt);//Returns closest point on this line.
    FORCE_INLINE float dist2(vec3& pt);//Squared distance from point to this line
};
FORCE_INLINE Line3f::Line3f()
{ 
}
FORCE_INLINE Line3f::Line3f(vec3& v1, vec3&  v2)
{ 
    construct(v1,v2);
}
FORCE_INLINE Line3f::~Line3f()
{
}
FORCE_INLINE void Line3f::construct(vec3& dp0, vec3& dp1) 
{
    p0()=dp0; 
    p1()=dp1;
}
//Returns closest point on this line.
FORCE_INLINE vec3 Line3f::pointOnLine(vec3& pt)
{
    vec3 ret;
    vec3 dP = pt - p0();
    vec3 dL = p1() - p0();
    float dPdL = dP.dot(dL);
    float dLdL = dL.dot(dL);
    float t = dPdL / dLdL;
            
    ret = dL*t;
}
//Squared distance from point to this line
FORCE_INLINE float Line3f::dist2(vec3& pt)
{
    return (pt-pointOnLine(pt)).length2();
}

}//ns game



#endif
