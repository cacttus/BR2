/*----------------------------------------
    View frustum class.
    Calculates the view frustum each
    frame.
    Programmer: Derek Page
    Sept 15, 2007
    Planes:
    n,f,l,r,t,b
    For detail see documentation.
    1/31/2010 - modified
    2/1/2010 - Fixed major bug in width of frustum near calculation that was messing things up before.
            also added enumerations of planes / points.
    20141203 mods for shadow maps
----------------------------------------*/
#pragma once
#ifndef __FRUSTUM_CLASS_H__
#define __FRUSTUM_CLASS_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
#include "../gfx/DisplayHeader.h"
#include "../model/BaseNode.h"
namespace Game {
/**
*    @class Frustum
*    @details Frustum class.  
*    The normals of the frustum planes point inward
*s
*    TODO: update znear and zfar from the camera!!! and aspect_ratio from the viewport!!! and tan_fov_2!!
*/
class FrustumBase : public VirtualMemory {
public:
    static const int NumPlanes = 6;
    static const int NumPoints = 8;
protected:
    QuadPlane Planes[NumPlanes];
    Vector3 Points[NumPoints];    //nbl fbl fbr nbr ntl ftl ftr ntr
    float tan_fov_2;
    float z_near = 1.0f;
    float z_far = 1000.0f;
    ProjectionMode::e _eProjectionMode;
    std::shared_ptr<Viewport> _pViewportRef;    // - Reference to camera viewport.
    //Line3f* _satLine;
    Box3f* _minimax;
//
    //void makeSatLine(Vec3f& nearCenter, Vec3f& farCenter);
    void constructPointsAndPlanes(Vec3f& farCenter, Vec3f& nearCenter, Vec3f& upVec, Vec3f& rightVec,
                                  float w_near_2, float w_far_2,
                                  float h_near_2, float h_far_2
                                  );
    void projectScreenPointToWorldPoint(const vec2& screenPt, vec3& __out_ worldPt, 
        FrustumPoint tl, FrustumPoint bl, FrustumPoint tr, float fPushMultiplier, bool bTest=false);
    void setupOrthographic( FrustumProjectionParameters* params);
    void setupPerspective( FrustumProjectionParameters* params, bool bProjectBox = false);
    void setupBox( FrustumProjectionParameters* params);
    //void updateBoundBox();//We don't do this in the generic system for frustums.
public:
    FrustumBase(std::shared_ptr<Viewport> pv, float fov);
    virtual ~FrustumBase() override ;

    mat4 getProjectionMatrix();
    // - Get / SET
    Box3f* getBoundBox() {return _minimax;}
    //Line3f* getSatLine() { return _satLine; }
    Line3f getCenterAxis();    // - Returns the view vector of this frustum.
    Vector3    getViewVector();    // - Returns the view vector of this frustum.
    Vec3f getNearPlaneCenterPoint();
    //Matrix4x4 getProjMat(){ return proj_mat; }
    QuadPlane PlaneAt(int i){ return Planes[i]; }
    Vector3 PointAt(FrustumPoint i){ return Points[i]; }
    const QuadPlane* getPlane(int32_t i){  AssertOrThrow2((i>=0)&&(i<NumPlanes)); return &(Planes[i]); }
    const Vec3f* getPoint(int32_t i){  AssertOrThrow2((i>=0)&&(i<NumPoints)); return &(Points[i]); }
    Vec3f* getPoints(){ return Points; }

    float getZFar(){ return z_far; }
    float getZNear() { return z_near; }
    void setZFar(float f) { z_far = f; }
    void setZNear(float f) { z_near = f; }


    // - Intersection algorithms
   // bool intersectLine2D( Vec2f lp1, Vec2f lp2 );
    bool hasPointXZ( Vec2f& dp );
    bool hasPoint( Vector3& p );        // - Checks for a point inside the frustum
    bool hasQuad(Vector3& p1, Vector3& p2, Vector3& p3, Vector3& p4);
    bool hasTri(Vector3& p1, Vector3& p2, Vector3& p3 );
    bool hasBox(const Box3f* pCube);
    bool hasFrustum(std::shared_ptr<FrustumBase> pf);
    float getTanFov2() { return tan_fov_2; }
    void update( const Vector3& NormalizedView, const Vector3& CamPos,  const Vector3& upVec,ProjectionMode::e fpt);

    void projectFrom(std::shared_ptr<FrustumBase> pOther, Vec3f& viewNormal, Vec3f& viewPos, float nearDist, float farDist, bool bOrthographic=true); // - Creates a projection encompassing another frustum.
    void setFov(float fov);
    void screenPointToWorldPointNear(const vec2& screenPt, vec3& __out_ worldPt, float fPushMultiplier=0.0f, bool bTest=false);
    void screenPointToWorldPointFar(const vec2& screenPt, vec3& __out_ worldPt, float fPushMultiplier=0.0f, bool bTest=false);
    void screenQuadToWorldQuad(const Box2f* __in_ pScreenQuad, Quad3f* __out_ pWorldQuad, float fPushMultiplier = 0.001f, bool bTest=false);
};

}

#endif