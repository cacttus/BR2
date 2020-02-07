/**
*    @file QuadPlane.h
*    @author MetalMario971
*
*    © 2011 
*    @adate 2 / 1 / 2010
*
*/
#pragma once
#ifndef __QUAD_PLANE_H__
#define __QUAD_PLANE_H__

#include "../math/TriPlane.h"
#include "../math/PlaneEx3.h"

namespace Game{


/**
*    @class QuadPlane
*    @brief Derived plane for extra info and stuff.
* DO NOT ADD VIRTUAL METHODS
*/
class QuadPlane : public PlaneEx3 { 
public:
    vec3 p1, p2, p3, p4;
    
    FORCE_INLINE PlanePoint containsPoint( Vector3& point ); // - True if the point falls within the region of the quad plane.
    FORCE_INLINE PlaneHit hitTest( Vector3& p1, Vector3& p2 );
    FORCE_INLINE void construct( Vector3& dp1, Vector3& dp2, Vector3& dp3, Vector3& dp4 );
    FORCE_INLINE void construct();
//QuadPlane& QuadPlane::operator=( const QuadPlane& rhs )
//{
//    *this = rhs;
//    return *this;
//}

    FORCE_INLINE QuadPlane();
    FORCE_INLINE QuadPlane( Vector3&,Vector3&,Vector3&,Vector3& );
    FORCE_INLINE NOT_VIRTUAL ~QuadPlane() override ;
};

FORCE_INLINE PlaneHit 
QuadPlane::hitTest( Vector3& p1, Vector3& p2 )
{
    PlaneHit ret;
    ret._bContained = false;

    float t = intersectLine(p1,p2).t;        // - Plane based
    if(t>=0.0&&t<=1.0)
    {
        Vector3 v=p1+(p2-p1)*t;
        //if(v.y!=0) DebugBreak();
        PlanePoint pp = containsPoint( v );
        ret._bContained = pp.contained;        // - Triangle based
        ret._fHitTime = pp.t;
        return ret;
    }
    return ret;
}
//
//
FORCE_INLINE PlanePoint 
QuadPlane::containsPoint( Vector3& point )
{
    PlanePoint ret;

    // - Here the problem is solved by turning the plane into two triangles.
    // - For CW verts only i think.
    if( (ret.contained=TriPlane::containsPoint(p1,p2,p3,point)) )
        return ret;
    else if( (ret.contained=TriPlane::containsPoint(p2,p3,p4,point)) )
        return ret;
    return ret;
}

/**
*    @fn construct()    
*    @brief Override of the plane construct.  Will calculate the data inside of the QuadPlane
*/
FORCE_INLINE void 
QuadPlane::construct( Vector3& dp1, Vector3& dp2, Vector3& dp3, Vector3& dp4 )
{
    p1=dp1;
    p2=dp2;
    p3=dp3;
    p4=dp4;
    construct();
}
/**
*    @fn construct()    
*    @brief Override of the plane construct.  Will calculate the data inside of the QuadPlane
*/
FORCE_INLINE void 
QuadPlane::construct( )
{
    PlaneEx3::construct(p1,p2,p3);
}
//
//ctor
//
FORCE_INLINE QuadPlane::QuadPlane( Vector3& dp1, Vector3& dp2, Vector3& dp3, Vector3& dp4 )
{
    construct( dp1, dp2, dp3, dp4 );
}


FORCE_INLINE QuadPlane::QuadPlane()
{

}
    
FORCE_INLINE QuadPlane::~QuadPlane()
{

}



}

#endif
