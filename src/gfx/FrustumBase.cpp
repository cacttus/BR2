
#include "../base/Logger.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/FrustumProjectionParameters.h"
#include "../gfx/RenderViewport.h"

namespace BR2 {
FrustumBase::FrustumBase(std::shared_ptr<RenderViewport> pv, float fov) : _pViewportRef(pv) {
  _minimax = new Box3f();

  setFov((fov));//fov is absolutely required for all frustum operattions.
}
FrustumBase::~FrustumBase() {
}
void FrustumBase::update(const Vector3& NormalizedView, const Vector3& CamPos, const Vector3& upVec, ProjectionMode::e fpt) {
  FrustumProjectionParameters params;
  params._camPos = CamPos;
  params._camView = NormalizedView;
  params._upVec = upVec;

  params._v3basis = Alg::getNormalBasisZ(NormalizedView);

  _eProjectionMode = fpt;

  //NOTE: basis.X will be screwy in LHS coords see note below.

  if (fpt == ProjectionMode::e::Perspective) {
    setupPerspective(&params);
  }
  //else if(fpt==FrustumProjectionType::FPT_BOX) {
  //    setupBox(&params);
  //}
  else if (fpt == ProjectionMode::e::Orthographic) {
    setupOrthographic(&params);
  }
  else {
    BrThrowNotImplementedException();
  }

  //    updateBoundBox();
}
// - Returns the center of this frustum's near plane.
Vec3f FrustumBase::getNearPlaneCenterPoint() {
  return PointAt(BR2::fpt_nbl) + (PointAt(BR2::fpt_ntr) - PointAt(BR2::fpt_nbl)) / 2.0f;
}
/**
*  @fn hasBox()
*  @return true if the frustum collides with an axis-aligned bound-box
*  @brief Collides cube with convex hull
*/
bool FrustumBase::hasBox(const Box3f* pCube) {
  bool    ret = false; // Inside the frustum
  Vector3 min, max;
  float d1, d2;
  if (pCube->_max < pCube->_min) {
    AssertOrThrow2(pCube->_max >= pCube->_min);
  }

  for (int i = 0; i < 6; ++i) {
    min = pCube->_min, max = pCube->_max;

    //  - Calculate the negative and positive vertex
    if (Planes[i].n.x < 0) {
      min.x = pCube->_max.x;
      max.x = pCube->_min.x;
    }

    if (Planes[i].n.y < 0) {
      min.y = pCube->_max.y;
      max.y = pCube->_min.y;
    }

    if (Planes[i].n.z < 0) {
      min.z = pCube->_max.z;
      max.z = pCube->_min.z;
    }


    d1 = Planes[i].dist(max);
    d2 = Planes[i].dist(min);

    //20170518 - Swapped the <> to be >.
    //20171208 - what the fuck was I on here?  INWARD is where normals point
    //Normals point INWARD from teh frutum.  If the box is outside the d values should both be NEGATIVE.
    //Changed back to <
    if (d1 < 0.0f && d2 < 0.0f) {
      return false;
    }
    //if(d2< 0.0f) 
        //ret = true; // Currently we intersect the frustum.  Keep checking the rest of the planes to see if we're outside.
  }
  return true;
}

/**
*  @fn getCenterAxis
*  @brief Returns the center axis line that runs through the length of the frustum.
*/
Line3f FrustumBase::getCenterAxis() {
  Vector3 center_far = (Points[fpt_fbl] - Points[fpt_ftr]) * 0.5f;
  Vector3 center_near = (Points[fpt_nbl] - Points[fpt_ntr]) * 0.5f;
  return Line3f(center_near, center_far);
}
/**
*  @fn getViewVector
*  @brief returns the normalized view vector that would be projecting out of the face of the camera.
*/
Vector3 FrustumBase::getViewVector() {
  // /
  Vector3 center_far = (Points[fpt_fbl] - Points[fpt_ftr]) * 0.5f;
  Vector3 center_near = (Points[fpt_nbl] - Points[fpt_ntr]) * 0.5f;
  return (center_far - center_near).normalize();
}

/**
*  @fn setupOrthographic
*  @brief Sets up an orthographic projection given the parameters
*/
void FrustumBase::setupOrthographic(FrustumProjectionParameters* params) {
  Vec3f nc, fc;

  nc = params->_camPos + (params->_camView * z_near);
  fc = params->_camPos + (params->_camView * z_far);

  float vpWidth_2 = _pViewportRef->getWidth() * 0.5f;
  float vpHeight_2 = _pViewportRef->getHeight() * 0.5f;

  // Will this work?? IDK! ha
  constructPointsAndPlanes(fc, nc,
    params->_v3basis._y, params->_v3basis._x,
    vpWidth_2, vpHeight_2,
    vpWidth_2, vpHeight_2);

}
void FrustumBase::setupPerspective(FrustumProjectionParameters* params, bool bProjectBox) {
  //If box is true we setup the frustum to be that of a shadowmapped box frustum for point lights.
  float ar = ((float)_pViewportRef->getHeight() / (float)_pViewportRef->getWidth());

  // - Need to be in rads
  float w_near_2 = tan_fov_2 * z_near;
  float h_near_2 = w_near_2 * ar;

  float w_far_2;
  if (bProjectBox)
    w_far_2 = tan_fov_2 * (z_far - w_near_2);
  else
    w_far_2 = tan_fov_2 * z_far;
  float h_far_2 = w_far_2 * ar;

  //fixed X product
  Vector3 right = params->_v3basis._x;//params->_camView.cross(params->_upVec);//_upVec.cross(NormalizedView);

  Vector3 nearCenter;
  Vector3 farCenter;

  nearCenter = params->_camPos;
  farCenter = params->_camPos;

  if (bProjectBox == true) {
    nearCenter += params->_camView * w_near_2; // "push" the frustum out a bit so that the distance from the camera center (light center) is half the viewport width.
    farCenter += params->_camView * (z_far + w_near_2);
  }
  else {
    nearCenter += params->_camView * z_near;
    farCenter += params->_camView * z_far;
  }

  constructPointsAndPlanes(
    farCenter
    , nearCenter
    , params->_v3basis._y
    , params->_v3basis._x
    , w_near_2
    , w_far_2
    , h_near_2
    , h_far_2
  );
}
void FrustumBase::setupBox(FrustumProjectionParameters* params) {
  setupPerspective(params, true);
}
void FrustumBase::constructPointsAndPlanes(Vec3f& farCenter, Vec3f& nearCenter,
  Vec3f& upVec, Vec3f& rightVec,
  float w_near_2, float w_far_2,
  float h_near_2, float h_far_2
) {

  // - Points are right
  Points[fpt_nbl] = (nearCenter - (upVec * h_near_2) - (rightVec * w_near_2));
  Points[fpt_fbl] = (farCenter - (upVec * h_far_2) - (rightVec * w_far_2));

  Points[fpt_nbr] = (nearCenter - (upVec * h_near_2) + (rightVec * w_near_2));
  Points[fpt_fbr] = (farCenter - (upVec * h_far_2) + (rightVec * w_far_2));

  Points[fpt_ntl] = (nearCenter + (upVec * h_near_2) - (rightVec * w_near_2));
  Points[fpt_ftl] = (farCenter + (upVec * h_far_2) - (rightVec * w_far_2));

  Points[fpt_ntr] = (nearCenter + (upVec * h_near_2) + (rightVec * w_near_2));
  Points[fpt_ftr] = (farCenter + (upVec * h_far_2) + (rightVec * w_far_2));

  // - Construct AA bound box
  _minimax->_min = Vec3f::VEC3X_MAX();
  _minimax->_max = Vec3f::VEC3X_MIN();

  for (int i = 0; i < 8; ++i) {
    _minimax->_min = Vec3f::minv(_minimax->_min, Points[i]);
    _minimax->_max = Vec3f::maxv(_minimax->_max, Points[i]);
  }
  //TODO: Optimize:
  //        1) we don't use the fourth value of the QuadPlane4 at all
  //        2) QuadPLane4 calculates a TBN basis.  We don't need that.
  //  1   2
  //
  //  3   4
  //
  // - Construct so that the normals are facing into the frustum  - Checked all is good
  Planes[fp_near].construct(Points[fpt_ntl], Points[fpt_ntr], Points[fpt_nbl], Points[fpt_nbr]);
  Planes[fp_far].construct(Points[fpt_ftr], Points[fpt_ftl], Points[fpt_fbr], Points[fpt_fbl]);

  Planes[fp_left].construct(Points[fpt_ftl], Points[fpt_ntl], Points[fpt_fbl], Points[fpt_nbl]);
  Planes[fp_right].construct(Points[fpt_ntr], Points[fpt_ftr], Points[fpt_nbr], Points[fpt_fbr]);

  Planes[fp_top].construct(Points[fpt_ntr], Points[fpt_ntl], Points[fpt_ftr], Points[fpt_ftl]);
  Planes[fp_bottom].construct(Points[fpt_fbr], Points[fpt_fbl], Points[fpt_nbr], Points[fpt_nbl]);

  // makeSatLine(nearCenter,farCenter);
}
//void FrustumBase::makeSatLine(Vec3f& nearCenter, Vec3f& farCenter)
//{
//    _satLine->p0() = nearCenter;
//    _satLine->p1() = farCenter;
//}
//bool FrustumBase::intersectLine2D( Vec2f lp1, Vec2f lp2 )
//{
//    if( hasPointXZ(lp1) )
//        return true;
//    if( hasPointXZ(lp2) )
//        return true;
//
//    if( Ceq::lineCol2D( 
//        Vec2f( Points[fpt_fbr].x, Points[fpt_fbr].z ),
//        Vec2f( Points[fpt_fbl].x, Points[fpt_fbl].z ),
//        lp1,lp2)
//        )
//        return true;
//    if( Ceq::lineCol2D( 
//        Vec2f( Points[fpt_fbl].x, Points[fpt_fbl].z ),
//        Vec2f( Points[fpt_nbl].x, Points[fpt_nbl].z ),
//        lp1,lp2)
//        )
//        return true;
//    if( Ceq::lineCol2D( 
//        Vec2f( Points[fpt_nbl].x, Points[fpt_nbl].z ),
//        Vec2f( Points[fpt_nbr].x, Points[fpt_nbr].z ),
//        lp1,lp2)
//        )
//        return true;
//    if( Ceq::lineCol2D( 
//        Vec2f( Points[fpt_nbr].x, Points[fpt_nbr].z ),
//        Vec2f( Points[fpt_fbr].x, Points[fpt_fbr].z ),
//        lp1,lp2)
//        )
//        return true;
//    return false;//??? put here to disable warning
//}
bool FrustumBase::hasPointXZ(Vec2f& dp) {
  Vector3 p = Vector3(dp.x, 0, dp.y);
  if (Planes[fp_near].dist(p) < 0.0f)        return false;
  if (Planes[fp_far].dist(p) < 0.0f)        return false;
  if (Planes[fp_left].dist(p) < 0.0f)        return false;
  if (Planes[fp_right].dist(p) < 0.0f)    return false;

  return true;
}
/*--------------------------------------------
        Point inside frustum routine.
        Returns true if a point is in
        the frustum, false otherwise.
--------------------------------------------*/
bool FrustumBase::hasPoint(Vector3& p) {
  if (Planes[fp_near].dist(p) < 0.0f)        return false;
  if (Planes[fp_far].dist(p) < 0.0f)        return false;
  if (Planes[fp_left].dist(p) < 0.0f)        return false;
  if (Planes[fp_top].dist(p) < 0.0f)        return false;
  if (Planes[fp_right].dist(p) < 0.0f)    return false;
  if (Planes[fp_bottom].dist(p) < 0.0f)    return false;

  return true;
}
/**
*  @fn hasTri
*  @brief Returns true if the triangle intersects the frustum, or is inside at all.
*
*/
bool FrustumBase::hasTri(Vector3& p1, Vector3& p2, Vector3& p3) {
  //-- 20160528 This algorithm is WAY incorrect.
  //--  this isn't needed at all however >:)

  //if( hasPoint(p1) || hasPoint(p2) || hasPoint(p3) )
  //    return true;

  //for( int i=0 ;i<6; ++i ) 
  //{
  //    if( Planes[i].hitTest(p1,p2).point.contained )return true;
  //    else if( Planes[i].hitTest(p2,p3).point.contained )return true;
  //    else if( Planes[i].hitTest(p3,p1).point.contained )return true;
  //}

  //// - Cull problem has nothing to do with this
  //TriPlane p(p1,p2,p3);
  //// Projected rays
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_ftl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbr],Points[fpt_fbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbl],Points[fpt_fbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntr],Points[fpt_ftr]).point.contained ) return true;
  //// Far Quad
  //if(p.hitTest(Points[fpt_ftl],Points[fpt_ftr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_fbl],Points[fpt_fbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ftl],Points[fpt_fbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ftr],Points[fpt_fbr]).point.contained ) return true;
  //// Near Quad
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_ntr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbl],Points[fpt_nbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_nbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntr],Points[fpt_nbr]).point.contained ) return true;

  BrThrowNotImplementedException();
  return false;
}
/**
*  @fn hasQuad()
*  @brief Returns true if the given quad intersects the frustum.
*  @return true if the quad intersects the frustum.
*
*    Don't know if this works.  I think i've tested it before, but should probably make sure.
*/
bool FrustumBase::hasQuad(Vector3& p1, Vector3& p2, Vector3& p3, Vector3& p4) {
  //-- 20160528 This algorithm is WAY incorrect.
  //--  this isn't needed at all however >:(

  //if( hasPoint(p1) || hasPoint(p2) || hasPoint(p3) || hasPoint(p4) )
  //    return true;
  ////TriPlane tp;

  //for( int i=0 ;i<6; ++i ) 
  //{
  //    if( Planes[i].hitTest(p1,p2).point.contained )return true;
  //    else if( Planes[i].hitTest(p2,p3).point.contained )return true;
  //    else if( Planes[i].hitTest(p3,p4).point.contained )return true;
  //    else if( Planes[i].hitTest(p4,p1).point.contained )return true;
  //}

  //QuadPlane p(p1,p2,p3,p4);
  //// Projected rays
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_ftl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbr],Points[fpt_fbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbl],Points[fpt_fbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntr],Points[fpt_ftr]).point.contained ) return true;
  //// Far Quad
  //if(p.hitTest(Points[fpt_ftl],Points[fpt_ftr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_fbl],Points[fpt_fbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ftl],Points[fpt_fbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ftr],Points[fpt_fbr]).point.contained ) return true;
  //// Near Quad
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_ntr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_nbl],Points[fpt_nbr]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntl],Points[fpt_nbl]).point.contained ) return true;
  //if(p.hitTest(Points[fpt_ntr],Points[fpt_nbr]).point.contained ) return true;

  BrThrowNotImplementedException();
  return false;
}
/**
*  @fn projectFrom()
*  @brief This is for creating light frustums.
*        We project a set of points onto the light's plane surface.
*        We then calculate the orthographic viewport size based on the minimum and maximum points.
*/
void FrustumBase::projectFrom(std::shared_ptr<FrustumBase> pOther, Vec3f& viewNormal, Vec3f& viewPos, float nearDist, float farDist, bool bOrthographic) {
  FrustumProjectionParameters params;
  float vmin, vmax, hmin, hmax;

  if (!bOrthographic)
    throw new NotImplementedException();

  Alg::getSolidVolumeProjectionBounds(
    viewNormal,
    viewPos,
    pOther->getPoints(),
    8,
    &hmin, &hmax, &vmin, &vmax,
    (Vec3Basis*)&(params._v3basis)
  );

  // - Adjust orthographic position based on the new total width of the projected view volume
  float vpWidth = (hmax - hmin);
  float vpHeight = (vmax - vmin);
  float vpHeight_2 = vpHeight * 0.5f;
  float vpWidth_2 = vpWidth * 0.5f;

  Vec3f bottom_right_corner_of_viewport =
    viewPos
    + params._v3basis._x * hmin
    + params._v3basis._y * vmin;

  Vec3f newCenter =
    bottom_right_corner_of_viewport
    + params._v3basis._x * (vpWidth_2) // x * width / 2
    +params._v3basis._y * (vpHeight_2) //y * height/2
    ;

  // - We're going to have to adjust cam pos based on w/h
  params._camPos = newCenter;

  params._camView = viewNormal;

  setupOrthographic(&params);
}
/**
*  @fn projectScreenPointToWorldPoint
*  @brief Projects a point onto the screen Near or Far plane given 3 points.
*/
void FrustumBase::projectScreenPointToWorldPoint(
  const vec2& screenPoint,
  vec3& __out_ worldPt,
  FrustumPoint tl,
  FrustumPoint bl,
  FrustumPoint tr,
  float fPushMultiplier,
  bool bTest
) {
  AssertOrThrow2(_pViewportRef != NULL);

  float wratio = screenPoint.x * (1 / _pViewportRef->getWidth()); // x / width = % of x
  float hratio = screenPoint.y * (1 / _pViewportRef->getHeight()); // y / height
  vec3 pTl, pTr, pBl;
  if (!bTest) {
    pTl = PointAt(tl);
    pTr = PointAt(tr);
    pBl = PointAt(bl);
  }
  else {
    //Test the Frustum UI
    pTl = vec3(-100, 100, 0);
    pTr = vec3(100, 100, 0);
    pBl = vec3(-100, 20, 0);
  }

  vec3 dp1 = pTr - pTl;
  vec3 dp2 = pBl - pTl;

  dp1 *= wratio;
  dp2 *= hratio;

  worldPt = pTl + dp1 + dp2;

  vec3 pushNormal = PlaneAt(frustum_plane_index::fp_near).n;
  worldPt += pushNormal * fPushMultiplier;
}
void FrustumBase::screenPointToWorldPointNear(const vec2& screenPoint, vec3& __out_ worldPt, float fPushMultiplier, bool bTest) {
  projectScreenPointToWorldPoint(screenPoint, worldPt, fpt_ntl, fpt_nbl, fpt_ntr, fPushMultiplier, bTest);
}
void FrustumBase::screenPointToWorldPointFar(const vec2& screenPoint, vec3& __out_ worldPt, float fPushMultiplier, bool bTest) {
  projectScreenPointToWorldPoint(screenPoint, worldPt, fpt_ftl, fpt_fbl, fpt_ftr, fPushMultiplier, bTest);
}
void FrustumBase::screenQuadToWorldQuad(const Box2f* __in_ pScreenQuad, Quad3f* __out_ pWorldQuad, float fPushMultiplier, bool bTest) {
  AssertOrThrow2(pScreenQuad != NULL);
  AssertOrThrow2(pWorldQuad != NULL);

  /*
  Screen Quad
  0
         1
  World Quad
  3      2
  1      0
  For Index Order see: QuadBufferMesh::assignIndexes
  */
  screenPointToWorldPointNear(vec2(pScreenQuad->p1().x, pScreenQuad->p1().y), pWorldQuad->p0(), fPushMultiplier, bTest);
  screenPointToWorldPointNear(vec2(pScreenQuad->p0().x, pScreenQuad->p1().y), pWorldQuad->p1(), fPushMultiplier, bTest);
  screenPointToWorldPointNear(vec2(pScreenQuad->p1().x, pScreenQuad->p0().y), pWorldQuad->p2(), fPushMultiplier, bTest);
  screenPointToWorldPointNear(vec2(pScreenQuad->p0().x, pScreenQuad->p0().y), pWorldQuad->p3(), fPushMultiplier, bTest);
}
bool FrustumBase::hasFrustum(std::shared_ptr<FrustumBase> pf) {
  return hasBox(pf->getBoundBox());

}
mat4 FrustumBase::getProjectionMatrix() {
  if (_eProjectionMode == ProjectionMode::e::Perspective) {
    //calc proj matrix
    float vpWidth_2 = tan_fov_2 * z_near;
    float arat_1 = 1.0f/_pViewportRef->getAspectRatio();
    float vw = vpWidth_2;
    float vh = vpWidth_2 * arat_1;

    return mat4::getProjection(
      z_near, z_far,
      vw, -vw,
      vh, -vh
    );


  }
  else if (_eProjectionMode == ProjectionMode::e::Orthographic) {
    float fNear = getZNear();
    float fFar = getZFar();

    //*This mode seems to get the coordinate
    int iOrthoWidth = _pViewportRef->getWidth() - 1;
    int iOrthoHeight = _pViewportRef->getHeight() - 1;
    //_mProj = mat4::getLHOrtho(0, (float)iOrthoWidth, 0, (float)iOrthoHeight, -1.0f, 1.0f);
    Matrix4x4 mm;
    float left = 0;
    float right = (float)iOrthoWidth;
    float top = 0;
    float bottom = (float)iOrthoHeight;
    float neard = -1.0f;
    float fard = 1.0f;

    float a1 = 2.0f / (left - right);
    float a2 = 2.0f / (bottom - top);  //IDK WY
    float a3 = -2.0f / (fard - neard); //IDK WY
    float t1 = (right + left) / (right - left) * -1.0f;
    float t2 = (top + bottom) / (top - bottom) * -1.0f;
    float t3 = (fard + neard) / (fard - neard) * -1.0f;


    //Row major order version
    //mm._m11 =a1, mm._m12 = 0, mm._m13 = 0, mm._m14 =t1,
    //mm._m21 = 0, mm._m22 =a2, mm._m23 = 0, mm._m24 =t2,
    //mm._m31 = 0, mm._m32 = 0, mm._m33 =a3, mm._m34 =t3,
    //mm._m41 = 0, mm._m42 = 0, mm._m43 = 0, mm._m44 = 1;

    // ** OpenGL version - the transpose of the former.
    mm._m11 = a1, mm._m12 = 0, mm._m13 = 0, mm._m14 = 0,
      mm._m21 = 0, mm._m22 = a2, mm._m23 = 0, mm._m24 = 0,
      mm._m31 = 0, mm._m32 = 0, mm._m33 = a3, mm._m34 = 0,
      mm._m41 = t1, mm._m42 = t2, mm._m43 = t3, mm._m44 = 1;

    //mat4::getOrtho()
    return mm;
  }
  else {
    BrThrowNotImplementedException();
  }

}
void FrustumBase::setFov(float fov) {
  if (fov > 179 || fov < 1) {
    Br2LogWarn("Frustum FOV " + fov + " was invalid, setting to valid number");
    fov = 45;
  }
  tan_fov_2 = tanf(MathUtils::degToRad(fov / 2.0f));
}
//void FrustumBase::updateBoundBox()
//{
//    getBoundBoxObject()->genResetLimits();
//    
//    for(int i=0; i<8; ++i)  
//        getBoundBoxObject()->genExpandByPoint(Points[i]);
//}


}//nsgame