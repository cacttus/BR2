/**
*  @file Cube3.h
*  @date January 1, 208
*  @author MetalMario971
*/
#pragma once
#ifndef __CUBE3_49452698133252756717211_H__
#define __CUBE3_49452698133252756717211_H__

#include "../base/BaseHeader.h"

#include "../math/Vec3x.h"
//#include "../math/Vector3.h"
//#include "../math/MathHeader.h"
#include "../math/ProjectedRay.h"
//#include "../math/Sphere3.h"
#include "../math/ConvexAlgorithms.h"
#include "../math/Plane3.h"
#include "../math/RaycastHit.h"

namespace BR2 {
/**
*  @class Box3x
*  @brief Axis Aligned Bounding Box.
*/
template < class Tx >
class Box3x : public VirtualMemory {
public:
  typedef Vec3x<Tx> Vec3_t;

  //DO NOT ALTER THIS DATA 
  Vec3x<Tx> _min;
  Vec3x<Tx> _max;
  //DO NOT ALTER THIS DATA 

  FORCE_INLINE Box3x(const Tx& minx, const Tx& miny, const Tx& minz, const Tx& maxx, const Tx& maxy, const Tx& maxz);
  FORCE_INLINE Box3x(Vec3x<Tx>& vmin, Vec3x<Tx>& vmax);
  FORCE_INLINE Box3x();
  FORCE_INLINE ~Box3x();

  void construct(Box3x<Tx>& rhs);

  FORCE_INLINE Vec3x<Tx>& getMin();
  FORCE_INLINE Vec3x<Tx>& getMax();
  FORCE_INLINE const Vec3x<Tx>& getMin() const;
  FORCE_INLINE const Vec3x<Tx>& getMax() const;

  FORCE_INLINE STATIC void snapPointToGridCubeVertex(Vec3x<Tx>* pt, Box3f* cube, float gridCubeSize = 1.0f);
  FORCE_INLINE STATIC void snapPointToGridCube(Vec3x<Tx>* pt, Box3f* cube, float gridCubeSize = 1.0f);
  FORCE_INLINE STATIC void getCubeIntersectionVolume(Box3f* a, Box3f* b, Box3f* __out_ out);
  FORCE_INLINE STATIC void getPoints(const Vec3x<Tx>& vmin, const Vec3x<Tx>& vmax, Vec3x<Tx>* points);
  FORCE_INLINE STATIC void getPlanesAx(Vec3x<Tx>* points, Plane3f* planes);
  FORCE_INLINE STATIC void getPlanesOriented(Vec3x<Tx>* points, Plane3f* planes);

  FORCE_INLINE void copyFrom(const Box3f* other);
  FORCE_INLINE bool containsInclusive(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool containsExclusive(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool containsBottomLeftInclusive(const Vec3x<Tx>& v) const;
  FORCE_INLINE bool cubeIntersect_EasyOut_Inclusive(Box3f* cc);
  FORCE_INLINE bool cubeIntersect_EasyOut_Exclusive(Box3f* cc);
  FORCE_INLINE bool RayIntersect(const ProjectedRay* ray, RaycastHit* bh = NULL) const;
  FORCE_INLINE bool sphereIntersect(Sphere3f* s);
  FORCE_INLINE bool sphereIntersect(Vec3x<Tx>* center, float r);
  FORCE_INLINE void genResetLimits();
  FORCE_INLINE void genExpandByPoint(const Vec3x<Tx>& pt);
  FORCE_INLINE void genExpandByBox(Box3f* pc);
  FORCE_INLINE bool getHasVolume(float epsilon = FLT_MIN);
  FORCE_INLINE void getSatExtents(Vec3x<Tx>& normal, float& minf, float& maxf);
  FORCE_INLINE void getPoints(Vec3x<Tx>* __out_ points, int32_t arrayElementCount) const;
  FORCE_INLINE bool fullyContains(Box3f* cc);
  FORCE_INLINE bool RayIntersect_EasyOut(Ray_t* ray) const;
  FORCE_INLINE bool LineOrRayIntersectInclusive_EasyOut(const ProjectedRay* ray, RaycastHit* __opt_ __out_ bh = NULL) const;
  FORCE_INLINE bool LineOrRayIntersectInclusive_FullData(ProjectedRay* ray, RaycastHit* bh) const;
  FORCE_INLINE bool RayIntersect_Slow(ProjectedRay* ray, RaycastHit* bh);
  FORCE_INLINE const Vec3x<Tx>& bounds(int32_t in__) const;
  FORCE_INLINE Vec3x<Tx> center();
  FORCE_INLINE Vec3x<Tx> diagonal();
  FORCE_INLINE void closestPoint(const Vec3x<Tx>& point, Vec3f& __out_ closestPt) const;
  FORCE_INLINE float distance2(const Vec3x<Tx>& point) const;  // Accurate distance to a point.
  FORCE_INLINE float getVolumeArbitrary();
  FORCE_INLINE float getVolumePositiveOnly();
  //FORCE_INLINE float distance2(Box3x* cube);
  FORCE_INLINE bool validateBoundBox();
  FORCE_INLINE bool volumeIsNotTooBig();
  FORCE_INLINE bool validateInterval() const;
  FORCE_INLINE float getWidth() { return (_max.x - _min.x); }
  FORCE_INLINE float getHeight() { return (_max.y - _min.y); }
  FORCE_INLINE float getDepth() { return (_max.z - _min.z); }
  FORCE_INLINE float outerRadius(); // Computes a spherical radius that contains this box
  FORCE_INLINE float innerRadius(); // Computes a spherical radius that is contained within this box
  FORCE_INLINE string_t toString();
  FORCE_INLINE void operator=(const Tx& rhs);
  FORCE_INLINE bool operator!=(const Box3x<Tx>& rhs) const;
  FORCE_INLINE bool limitSizeForEachAxis(const vec3& vBasePos, const float& fSize);
};

template < class Tx >
FORCE_INLINE Box3x<Tx>::Box3x(const Tx& minx, const Tx& miny, const Tx& minz,
  const Tx& maxx, const Tx& maxy, const Tx& maxz) {
  _min = Vec3x<Tx>(minx, miny, minz);
  _max = Vec3x<Tx>(maxx, maxy, maxz);
}
template < class Tx >
FORCE_INLINE Box3x<Tx>::Box3x(Vec3x<Tx>& vmin, Vec3x<Tx>& vmax) {
  _min = vmin;
  _max = vmax;
}
template < class Tx >
FORCE_INLINE Box3x<Tx>::Box3x() {
}
template < class Tx >
FORCE_INLINE Box3x<Tx>::~Box3x() {

}
template < class Tx >
void Box3x<Tx>::construct(Box3x<Tx>& rhs) {
  _min = rhs._min;
  _max = rhs._max;
}

template < class Tx >
FORCE_INLINE Vec3x<Tx>& Box3x<Tx>::getMin() { return _min; }
template < class Tx >
FORCE_INLINE Vec3x<Tx>& Box3x<Tx>::getMax() { return _max; }
template < class Tx >
FORCE_INLINE const Vec3x<Tx>& Box3x<Tx>::getMin() const { return _min; }
template < class Tx >
FORCE_INLINE const Vec3x<Tx>& Box3x<Tx>::getMax() const { return _max; }

template < class Tx >
FORCE_INLINE void Box3x<Tx>::copyFrom(const Box3f* other) {
  _min = other->getMin();
  _max = other->getMax();
}

// - returns true if this contains the given point , including whether the point is on the bounds.
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::containsInclusive(const Vec3x<Tx>& v) const {
  return (
    (v.x >= _min.x) && (v.x <= _max.x) &&
    (v.y >= _min.y) && (v.y <= _max.y) &&
    (v.z >= _min.z) && (v.z <= _max.z)
    );
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::containsExclusive(const Vec3x<Tx>& v) const {
  return (
    (v.x > _min.x) && (v.x < _max.x) &&
    (v.y > _min.y) && (v.y < _max.y) &&
    (v.z > _min.z) && (v.z < _max.z)
    );
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::containsBottomLeftInclusive(const Vec3x<Tx>& v) const {
  return (
    (v.x >= _min.x) && (v.x < _max.x) &&
    (v.y >= _min.y) && (v.y < _max.y) &&
    (v.z >= _min.z) && (v.z < _max.z)
    );
}
/**
*    @fn cubeIntersect_EasyOut
*    @brief Intersects two AABB cubes - easy out version.
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::cubeIntersect_EasyOut_Inclusive(Box3f* cc) {
  //Testing on MSVC this simple test beats out all 5 other tests by +- 100ms for 10m tests
  return cc->_min.x <= _max.x && cc->_min.y <= _max.y && cc->_min.z <= _max.z &&
    _min.x <= cc->_max.x && _min.y <= cc->_max.y && _min.z <= cc->_max.z;
}
/**
*    @fn cubeIntersect_EasyOut
*    @brief Intersects two AABB cubes - easy out version.
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::cubeIntersect_EasyOut_Exclusive(Box3f* cc) {
  bool b = cc->_min.x < _max.x && cc->_min.y < _max.y && cc->_min.z < _max.z &&
    _min.x < cc->_max.x && _min.y < cc->_max.y && _min.z < cc->_max.z;
  return b;
}
/**
*    @fn RayIntersect
*    @brief Returns true if the given ray intersects this Axis aligned
*    cube volume.
*    @param bh - Reference to a BoxHit structure.
*    @prarm ray - The ray to test against the box.
*    @return true if ray intersects, false otherwise.
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::RayIntersect(const ProjectedRay* ray, RaycastHit* bh) const {
  AssertOrThrow2(ray->_isOpt == true);

  float txmin, txmax, tymin, tymax, tzmin, tzmax;
  bool bHit;

  txmin = (bounds(ray->Sign[0]).x - ray->Origin.x) * ray->InvDir.x;
  txmax = (bounds(1 - ray->Sign[0]).x - ray->Origin.x) * ray->InvDir.x;

  tymin = (bounds(ray->Sign[1]).y - ray->Origin.y) * ray->InvDir.y;
  tymax = (bounds(1 - ray->Sign[1]).y - ray->Origin.y) * ray->InvDir.y;

  if ((txmin > tymax) || (tymin > txmax)) {
    if (bh != NULL)
      bh->_bHit = false;
    return false;
  }
  if (tymin > txmin) {
    txmin = tymin;
  }
  if (tymax < txmax) {
    txmax = tymax;
  }

  tzmin = (bounds(ray->Sign[2]).z - ray->Origin.z) * ray->InvDir.z;
  tzmax = (bounds(1 - ray->Sign[2]).z - ray->Origin.z) * ray->InvDir.z;

  if ((txmin > tzmax) || (tzmin > txmax)) {
    if (bh != NULL) {
      bh->_bHit = false;
    }
    return false;
  }
  if (tzmin > txmin) {
    txmin = tzmin;
  }
  if (tzmax < txmax) {
    txmax = tzmax;
  }
  bHit = ((txmin >= 0.0f) && (txmax <= ray->Length));

  //**Note changed 20151105 - this is not [0,1] this is the lenth along the line in which 
  // the ray enters and exits the cube, so any value less than the maximum is valid
  //bh->_bHit = ( (txmin <= 1.0f) && (txmax >= 0.0f) );
  if (bh != NULL) {
    bh->_bHit = bHit;
    bh->_t = txmin;
  }

  return bHit;
}
/**
*    http://stackoverflow.com/questions/4578967/cube-sphere-intersection-test
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::sphereIntersect(Vec3x<Tx>* center, float r) {
  float dist_squared = r * r;

  if (center->x < _min.x) {
    dist_squared -= MathUtils::sqrf(center->x - _min.x);
  }
  else if (center->x > _max.x) {
    dist_squared -= MathUtils::sqrf(center->x - _max.x);
  }
  if (center->y < _min.y) {
    dist_squared -= MathUtils::sqrf(center->y - _min.y);
  }
  else if (center->y > _max.y) {
    dist_squared -= MathUtils::sqrf(center->y - _max.y);
  }
  if (center->z < _min.z) {
    dist_squared -= MathUtils::sqrf(center->z - _min.z);
  }
  else if (center->z > _max.z) {
    dist_squared -= MathUtils::sqrf(center->z - _max.z);
  }

  return dist_squared > 0;
}
// - Given a point convert it to the a cube at the corner of the grid cube.
// IN: pt - vector to convert to cube
// OUT cube - cube to store converted information
// IN: gridCubeSzie - size of a cube in the grid space.
template < class Tx >
FORCE_INLINE void Box3x<Tx>::snapPointToGridCubeVertex(Vec3x<Tx>* pt, Box3f* cube, float gridCubeSize) {
  float dfx, dfy, dfz;
  float adx, ady, adz;
  float gcs, gcs_2;

  AssertOrThrow2(cube != NULL);

  gcs = gridCubeSize;
  gcs_2 = gcs * 0.5f;

  // get the remainder of the point on the defined grid.
  dfx = (fmodf((pt->x), gcs));
  dfy = (fmodf((pt->y), gcs));
  dfz = (fmodf((pt->z), gcs));

  //when you go negative you snap in reverse.
  if (pt->x < 0.0f)
    dfx = gcs + dfx;
  if (pt->y < 0.0f)
    dfy = gcs + dfy;
  if (pt->z < 0.0f)
    dfz = gcs + dfz;

  // - add or sub based on quadrant (vertex) the point lies in. 
  // if we are greater snap to the greater edge, lesser snap to the lesser
  adx = ((dfx) >= gcs_2) ? (gcs_2) : (-gcs_2);
  ady = ((dfy) >= gcs_2) ? (gcs_2) : (-gcs_2);
  adz = ((dfz) >= gcs_2) ? (gcs_2) : (-gcs_2);

  cube->_min.x = pt->x - dfx + adx;
  cube->_min.y = pt->y - dfy + ady;
  cube->_min.z = pt->z - dfz + adz;

  cube->_max = cube->_min + gcs;
}
// - Given a point convert it to a cube.
// IN: pt - vector to convert to cube
// OUT cube - cube to store converted information
// IN: gridCubeSzie - size of a cube in the grid space.
template < class Tx >
FORCE_INLINE void Box3x<Tx>::snapPointToGridCube(Vec3x<Tx>* pt, Box3f* cube, float gridCubeSize) {
  float r_gcs = 1.0f / gridCubeSize;

  AssertOrThrow2(cube != NULL);
  // get the remainder of the point on the defined grid.

  cube->_min.x = floorf(pt->x * r_gcs) * gridCubeSize;
  cube->_min.y = floorf(pt->y * r_gcs) * gridCubeSize;
  cube->_min.z = floorf(pt->z * r_gcs) * gridCubeSize;

  cube->_max = cube->_min + gridCubeSize;
}
template < class Tx >
FORCE_INLINE void Box3x<Tx>::genResetLimits() {
  _min = VEC3_MAX;
  _max = VEC3_MIN;
}
template < class Tx >
FORCE_INLINE void Box3x<Tx>::genExpandByPoint(const Vec3x<Tx>& pt) {
  _min = Vec3x<Tx>::minv(_min, pt);
  _max = Vec3x<Tx>::maxv(_max, pt);
}
template < class Tx >
FORCE_INLINE void Box3x<Tx>::genExpandByBox(Box3f* pc) {
  genExpandByPoint(pc->_min);
  genExpandByPoint(pc->_max);
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::getHasVolume(float epsilon) {
  if (getVolumePositiveOnly() == 0.0) {
    return false;
  }
  return true;
}
template < class Tx >
FORCE_INLINE float Box3x<Tx>::getVolumePositiveOnly() {
  float ax = (_max.x - _min.x);
  float ay = (_max.y - _min.y);
  float az = (_max.z - _min.z);
  if (ax < 0.0f) ax = 0.0f;
  if (ay < 0.0f) ay = 0.0f;
  if (az < 0.0f) az = 0.0f;
  return ax * ay * az;
}
template < class Tx >
FORCE_INLINE float Box3x<Tx>::getVolumeArbitrary() {
  return (_max.x - _min.x) * (_max.y - _min.y) * (_max.z - _min.z);
}

/**
*    @fn getSatExtents()
*    i.e. supports
*    @brief Returns the SAT extents (supports) for the given cube.  Look at the SAT algorithm for more information.
*    @note Basically we just crush all the cube points along a ray (normal) and return the min and max distances along that ray from the point such that P+dist*Ray = Point
*    @param normal: the normal to crush points along

*/
template < class Tx >
FORCE_INLINE void Box3x<Tx>::getSatExtents(Vec3x<Tx>& normal, float& minf, float& maxf) {

  float minT, maxT;
  int32_t minP, maxP;

  //Get points. 
  Vec3x<Tx> pt[8];
  getPoints((Vec3x<Tx>*)pt, 8);

  ConvexUtils::convex_proj_t_3f(&normal,
    (Vec3x<Tx>*)pt,
    8,
    &minT, &maxT, &minP, &maxP
  );
  minf = minT;
  maxf = maxT;
}
/**
*    @fn getCubeIntersectionVolume
*    @brief Returns the intersection area of two cubes. This is the cube that would exist as " A n B " between the two if they intersected.
*
*    @note if the two cubes do not in fact collide this returns undefined results.
*/
template < class Tx >
FORCE_INLINE void Box3x<Tx>::getCubeIntersectionVolume(Box3f* a, Box3f* b, Box3f* __out_ out) {
  AssertOrThrow2(out != NULL);
  AssertOrThrow2(b != NULL);
  AssertOrThrow2(a != NULL);

#ifdef _DEBUG
  if (!a->cubeIntersect_EasyOut_Exclusive(b))
    BRThrowException("Debug, cube a did not intersect b");
#endif

  out->_max = Vec3x<Tx>::minv(a->_max, b->_max);
  out->_min = Vec3x<Tx>::maxv(a->_min, b->_min);
}
template < class Tx >
FORCE_INLINE void Box3x<Tx>::getPoints(/*out*/ Vec3x<Tx>* __out_ points, int32_t arrayElementCount) const {
  AssertOrThrow2(arrayElementCount == 8);
  Box3f::getPoints(_min, _max, points);
}

template < class Tx >
FORCE_INLINE void Box3x<Tx>::getPoints(const Vec3x<Tx>& vmin, const Vec3x<Tx>& vmax, Vec3x<Tx>* points) {
  //LHS coords
  // This is the marching cubes index winding.
  points[0] = Vec3x<Tx>(vmin.x, vmin.y, vmin.z);
  points[1] = Vec3x<Tx>(vmax.x, vmin.y, vmin.z);
  points[2] = Vec3x<Tx>(vmin.x, vmax.y, vmin.z);
  points[3] = Vec3x<Tx>(vmax.x, vmax.y, vmin.z);

  points[4] = Vec3x<Tx>(vmin.x, vmin.y, vmax.z);
  points[5] = Vec3x<Tx>(vmax.x, vmin.y, vmax.z);
  points[6] = Vec3x<Tx>(vmin.x, vmax.y, vmax.z);
  points[7] = Vec3x<Tx>(vmax.x, vmax.y, vmax.z);
}
// Get planes for a box AXIS ALIGNED
template < class Tx >
FORCE_INLINE void Box3x<Tx>::getPlanesAx(Vec3x<Tx>* points, Plane3f* planes) {
  // - Outward facing normals
  // Index is the indexes from DMC.
  // refer to "side indexes.png"
  planes[0].n = Vec3x<Tx>(-1, 0, 0);
  planes[0].d = planes[0].n.dot(points[1]);    //Point1 is "min"
  planes[2].n = Vec3x<Tx>(0, -1, 0);
  planes[2].d = planes[2].n.dot(points[1]);
  planes[4].n = Vec3x<Tx>(0, 0, -1);
  planes[4].d = planes[4].n.dot(points[1]);

  planes[1].n = Vec3x<Tx>(1, 0, 0);
  planes[1].d = planes[1].n.dot(points[7]);    // point 7 is "max"
  planes[3].n = Vec3x<Tx>(0, 1, 0);
  planes[3].d = planes[3].n.dot(points[7]);
  planes[5].n = Vec3x<Tx>(0, 0, 1);
  planes[5].d = planes[5].n.dot(points[7]);
}
//Slower than get planes
// we have to normalize the normals
template < class Tx >
FORCE_INLINE void Box3x<Tx>::getPlanesOriented(Vec3x<Tx>* points, Plane3f* planes) {
  // - Outward facing normals
  // Index is the indexes from DMC.
  // refer to "side indexes.png"

  // Normal vertex references in no particular special order except CCW to face normal
  Plane3f::getNormalCcw(points[2], points[1], points[4], planes[0].n);
  Plane3f::getNormalCcw(points[4], points[0], points[1], planes[2].n);
  Plane3f::getNormalCcw(points[0], points[1], points[2], planes[4].n);

  planes[0].d = planes[0].n.dot(points[1]);    //Point1 is "min"
  planes[2].d = planes[2].n.dot(points[1]);
  planes[4].d = planes[4].n.dot(points[1]);

  Plane3f::getNormalCcw(points[0], points[3], points[4], planes[1].n);
  Plane3f::getNormalCcw(points[3], points[2], points[7], planes[3].n);
  Plane3f::getNormalCcw(points[5], points[4], points[7], planes[5].n);

  planes[1].d = planes[1].n.dot(points[7]);    // point 7 is "max"
  planes[3].d = planes[3].n.dot(points[7]);
  planes[5].d = planes[5].n.dot(points[7]);
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::fullyContains(Box3f* cc) {
  return (containsExclusive(cc->_min) && containsExclusive(cc->_max));
}

template < class Tx >
FORCE_INLINE bool Box3x<Tx>::RayIntersect_EasyOut(Ray_t* ray) const {
  RaycastHit rh;

  LineOrRayIntersectInclusive_EasyOut(ray, &rh);
  ray->setTime(rh._t);

  return ray->isHit();
}
/**
*    @fn RayIntersectOrPointContainedInclusive
*    @brief This is a more verbose test for either a line or a ray inside a
*    cube volume.
*
*    @return True if the ray penetrates the cube, or the line points are contained in the cube.
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::LineOrRayIntersectInclusive_EasyOut(const ProjectedRay* ray, RaycastHit* __opt_ __out_ bh) const {
  AssertOrThrow2(ray != NULL);

  if (RayIntersect(ray, bh))
    return TRUE;
  // - otherwise check for points contained.
  if (containsInclusive(ray->getOrigin())) {
    if (bh != NULL) {
      bh->_p1Contained = true;
      bh->_bHit = true;
    }
    return TRUE;
  }

  if (containsInclusive(ray->getOrigin() + ray->getDir())) {
    if (bh != NULL) {
      bh->_p2Contained = true;
      bh->_bHit = true;
    }
    return TRUE;
  }

  return FALSE;
}



/**
*    @fn
*    @brief Same as above function but this one does not return early and includes all data.
*/
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::LineOrRayIntersectInclusive_FullData(ProjectedRay* ray, RaycastHit* bh) const {
  AssertOrThrow2(bh != NULL);
  AssertOrThrow2(ray != NULL);

  RayIntersect(ray, bh);

  // - otherwise check for points contained.
  if (containsInclusive(ray->Origin)) {
    bh->_p1Contained = TRUE;
    bh->_bHit = TRUE;
  }
  if (containsInclusive(ray->Origin + ray->Dir)) {
    bh->_p2Contained = TRUE;
    bh->_bHit = TRUE;
  }

  return bh->_bHit;
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::RayIntersect_Slow(ProjectedRay* ray, RaycastHit* bh) {
  AssertOrThrow2(bh != NULL);
  AssertOrThrow2(ray != NULL);
  float tmin, tmax, tymin, tymax, tzmin, tzmax;

  if (ray->Dir.x >= 0) {
    tmin = (bounds(0).x - ray->Origin.x) / ray->Dir.x;
    tmax = (bounds(1).x - ray->Origin.x) / ray->Dir.x;
  }
  else {
    tmin = (bounds(1).x - ray->Origin.x) / ray->Dir.x;
    tmax = (bounds(0).x - ray->Origin.x) / ray->Dir.x;
  }

  if (ray->Dir.y >= 0) {
    tymin = (bounds(0).y - ray->Origin.y) / ray->Dir.y;
    tymax = (bounds(1).y - ray->Origin.y) / ray->Dir.y;
  }
  else {
    tymin = (bounds(1).y - ray->Origin.y) / ray->Dir.y;
    tymax = (bounds(0).y - ray->Origin.y) / ray->Dir.y;
  }
  if ((tmin > tymax) || (tymin > tmax)) {
    bh->_bHit = FALSE;
    return FALSE;
  }
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  if (ray->Dir.z >= 0) {
    tzmin = (bounds(0).z - ray->Origin.z) / ray->Dir.z;
    tzmax = (bounds(1).z - ray->Origin.z) / ray->Dir.z;

  }
  else {
    tzmin = (bounds(1).z - ray->Origin.z) / ray->Dir.z;
    tzmax = (bounds(0).z - ray->Origin.z) / ray->Dir.z;
  }
  if ((tmin > tzmax) || (tzmin > tmax)) {
    bh->_bHit = FALSE;
    return FALSE;
  }
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  bh->_bHit = ((tmin <= 1.0f) && (tmax >= 0.0f));
  bh->_t = tmin;
  return bh->_bHit;
}
//Used in ray intersect
template < class Tx >
FORCE_INLINE const Vec3x<Tx>& Box3x<Tx>::bounds(int32_t in__) const {
  if (in__ == 0) return _min;
  return _max;
}
template < class Tx >
FORCE_INLINE Vec3x<Tx> Box3x<Tx>::center() {
  return _min + (_max - _min) * 0.5f;
}
template < class Tx >
FORCE_INLINE Vec3x<Tx> Box3x<Tx>::diagonal() {
  return (_max - _min);
}
/**
*    @fn closestPoint
*    @brief Finds the closest point on the surface of the cube to another point in space.
*          if the closest point is a face or edge (i.e. face or edge verts are equal distances)
*             we return the interpolated closest point from the projection of the point on the plane .
*/
template < class Tx >
FORCE_INLINE void Box3x<Tx>::closestPoint(const Vec3x<Tx>& point, Vec3f& __out_ closestPt) const {
#define CubeVertCount  8

  Vec3x<Tx> cp[CubeVertCount];
  int32_t closest[CubeVertCount];    // if we have similars we will have to interpolate the centers.
  int32_t nClosest;
  float lastMax;
  float dist;

  getPoints(cp, CubeVertCount);

  dist = cp[0].distance2(point);
  lastMax = dist;
  closest[0] = 0;// index of the closest point
  nClosest = 1;

  for (int n = 1; n < CubeVertCount; ++n) //start n at 1 asssuming the first vertex is the closest
  {
    dist = point.distance2(cp[n]);
    if (dist < lastMax) {
      lastMax = dist;
      //Reset closest array
      closest[0] = n;
      nClosest = 1;
    }
    else if (dist == closest[0])// if the distance is the same, add the point since now an edge, or face is closest
    {
      closest[nClosest] = n;
      nClosest++;
    }
  }

  // return the closest cube point.
  // if multiple points have equal distances from the gien point, interpolate those points.
  if (nClosest == 1) {
    //There was a single closest point
    AssertOrThrow2(closest[0] >= 0 && closest[0] < CubeVertCount);
    closestPt = cp[closest[0]];
  }
  else if (nClosest == 2) {
    AssertOrThrow2(closest[0] >= 0 && closest[0] < CubeVertCount);
    AssertOrThrow2(closest[1] >= 0 && closest[1] < CubeVertCount);

    // 2 points were equally close to the given point. (cube edge)
    Vec3f v1 = cp[closest[0]] + (cp[closest[1]] - cp[closest[0]]) * 0.5f;
    closestPt = v1.distance2(point);
  }
  else if (nClosest == 3) {
    AssertOrThrow2(closest[0] >= 0 && closest[0] < CubeVertCount);
    AssertOrThrow2(closest[1] >= 0 && closest[1] < CubeVertCount);
    AssertOrThrow2(closest[2] >= 0 && closest[2] < CubeVertCount);

    //3 points were equally closest to the given point
    Vec3f avg;
    vec3::bilinear_interpolate(cp[closest[0]],
      cp[closest[1]],
      cp[closest[0]],
      cp[closest[2]],
      avg,
      0.5f
    );

    closestPt = avg.distance2(point);
  }
  else if (nClosest == 4) {
    AssertOrThrow2(closest[0] >= 0 && closest[0] < CubeVertCount);
    AssertOrThrow2(closest[1] >= 0 && closest[1] < CubeVertCount);
    AssertOrThrow2(closest[2] >= 0 && closest[2] < CubeVertCount);
    AssertOrThrow2(closest[3] >= 0 && closest[3] < CubeVertCount);


    //4 points were equally closest (cube face)
    //Bilinear interpolate the shit
    Vec3f avg;
    vec3::bilinear_interpolate(cp[closest[0]],
      cp[closest[1]],
      cp[closest[2]],
      cp[closest[3]],
      avg,
      0.5f
    );

    closestPt = avg.distance2(point);
  }
  else {
    // BroThrowException("There were more than 4 points in the cube distance equation.");
  }
}
/**
*    @fn
*    @brief Returns ACCURATE squared distance to a point.
*/
template < class Tx >
FORCE_INLINE float Box3x<Tx>::distance2(const Vec3x<Tx>& point) const {
  Vec3f retPt;
  closestPoint(point, retPt);
  return point.distance2(retPt);
}
// make sure bound box is correct
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::validateBoundBox() {
  bool b1 = validateInterval();
  bool b2 = getHasVolume();
  bool b3 = volumeIsNotTooBig();
  return  b1 && b2 && b3;
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::volumeIsNotTooBig() {
  float f = getVolumePositiveOnly();
  bool b = f < (double)PHY_MAX_BOUND_BOX_VOLUME;
  return b;
}
//Make sure the box's interval is valid
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::validateInterval() const {
  return (_max.x >= _min.x) && (_max.y >= _min.y) && (_max.z >= _min.z);
}
template < class Tx >
FORCE_INLINE void Box3x<Tx>::operator=(const Tx& rhs) {
  _min = rhs;
  _max = rhs;
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::operator!=(const Box3x<Tx>& rhs) const {
  return (this->_min != rhs._min) || (this->_max != rhs._max);
}
template < class Tx >
FORCE_INLINE float Box3x<Tx>::outerRadius() {
  Vec3x<Tx> c = center();
  Vec3x<Tx> dMin = _min - c;
  Vec3x<Tx> dMax = _max - c;
  Vec3x<Tx> vMax = Vec3x<Tx>::maxv(dMin.abs(), dMax.abs());
  Tx fMax = MathUtils::brMax(vMax.x, MathUtils::brMax(vMax.y, vMax.z));
  return fMax;
}
template < class Tx >
FORCE_INLINE float Box3x<Tx>::innerRadius() {
  Vec3x<Tx> c = center();
  Vec3x<Tx> dMin = _min - c;
  Vec3x<Tx> dMax = _max - c;
  Vec3x<Tx> vMin = Vec3x<Tx>::minv(dMin.abs(), dMax.abs());
  Tx fMin = MathUtils::brMin(vMin.x, MathUtils::brMin(vMin.y, vMin.z));
  return fMin;
}
template < class Tx >
FORCE_INLINE string_t Box3x<Tx>::toString() {
  return Stz "{"+ _min.toString()+ "},{"+ _max.toString()+ "}";
}
template < class Tx >
FORCE_INLINE bool Box3x<Tx>::limitSizeForEachAxis(const vec3& vBasePos, const float& fSize) {
  bool b = false;
  if (this->validateBoundBox() == true) {
    float fw = getWidth();
    float fh = getHeight();
    float fd = getDepth();

    if (fw > fSize) {
      _min.x = vBasePos.x - fSize * 0.5f;
      _max.x = vBasePos.x + fSize * 0.5f;
      b = true;
    }
    if (fh > fSize) {
      _min.y = vBasePos.y - fSize * 0.5f;
      _max.y = vBasePos.y + fSize * 0.5f;
      b = true;
    }
    if (fd > fSize) {
      _min.z = vBasePos.z - fSize * 0.5f;
      _max.z = vBasePos.z + fSize * 0.5f;
      b = true;
    }
  }
  return b;
}














}//ns game



#endif
