/**
*  @file CollisionEquations.h
*  @date November 16, 2011
*  @author MetalMario971
*/
#pragma once
#pragma once
#ifndef __COLLISIONEQUATIONS_3050108728632172712251_H__
#define __COLLISIONEQUATIONS_3050108728632172712251_H__

//equations for collision detection
#include "../math/Box3x.h"
#include "../math/ConvexAlgorithms.h"

namespace BR2 {
#define i_in(a) ((unsigned int&) a)
#define i_min(a,b) (((a)<(b))?(a):(b))
#define i_max(a,b) (((a)>(b))?(a):(b))

class GpuFace {
public:
  vec3 _n;
  float _d;
};
class ConvexHull {
public:
  std::vector<GpuFace> _faces;
  std::vector<vec3> _points;
};

/**
*  @brief static methods for collision handling
*/
class CollisionEquations {
  static bool sat_box_int_1d_t(const float& c0, const float& c1,
    const float& r0, const float& r1, const float& v0, const float& v1,
    float& __out_ t_out, float& __out_ t0_out, float& __out_ t1_out);
  static bool in_range_t(float t0, float t1, float x);
  static bool int_intersect_t(float ta0, float ta1, float tb0, float tb1);
  static void sat_box_int_swap_t(float& a, float& b);
  static bool in_01(float x);
  static CollisionResult::e sat_axis_box_t_fast(
    const float& v0, const float& c0, const float& r0,
    const float& v1, const float& c1, const float& r1,
    float& __out_ t_out, float& __out_ t0_out, float& __out_ t1_out);
  static bool sat_box_axis_t(float c1, float c2, float r1, float r2, float v1, float v2,
    float& out_t0, float& out_t1, bool& countXYZ, bool& bStuck);
  static void sat_hull_proj_ext(const ConvexHull& hull, const GpuFace& face,
    float& __out_ t_e0, float& __out_ t_e1,
    int32_t& __out_ ind_e0, int32_t& __out_ ind_e1);
  static float hull_ext_pt_t(const vec3& p0, const vec3& v0,
    const vec3& p1, const vec3& v1);
public:
  static bool sat_box3_box3_t(const vec3& c1, const vec3& c2, const vec3& r1, const vec3& r2,
    const vec3& v1, const vec3& v2, float& out_t0, float& out_t1, int& ax_t0, int& ax_t1,
    bool& bStuck, bool bCheckingIfStuck);
  static void aa_point_contact_plane(vec3& pt, vec3& bc, vec3& out_n);
  static void aa_box_contact_plane_3(const Box3f* ba, const Box3f* bb, vec3& out_n, const vec3& in_v);
  static CollisionResult::e line_plane_collision_linear(
    const vec3& p1, // position of sphere
    const vec3& p2, // velocity of sphere
    const Plane3f& plane,
    float& out_t //[out] collision time
  );
  static CollisionResult::e CollisionEquations::line_plane_collision_linear(
    const vec3& p1, // point 1
    const vec3& p2, // point 2
    const vec3& plane_n,
    const float& plane_d,
    float& out_t //[out] t along line. v = p1 + (p2-p1)*t
  );

  static bool triangleAabbTest(const vec3* vertices, const vec3& aabbMin, const vec3& aabbMax);

  //dynamic vs static hull
  static bool sat_hull_hull_t(
    const ConvexHull& a_hull, const vec3& a_hull_v,
    const ConvexHull& b_hull, const vec3& b_hull_v,
    float& out_t0, float& out_t1);
  static CollisionResult::e CollisionEquations::sphere_collide_sphere(
    const vec3& v1, const vec3& p1, const float r1
    , const vec3& v2, const vec3& p2, const float r2
    , float& t, float& root_1, float& root_2
  );
};


//save finger cramps
typedef CollisionEquations Ceq;



}//ns game




#endif
