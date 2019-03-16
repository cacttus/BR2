/**
*
*    @file ConvexAlgorithms.h
*    @date December 5, 2014
*    @author Derek Page
*
*    � 2014 
*
*
*/
#pragma once
#ifndef __CONVEXALGORITHMS_186081759026531609525247_H__
#define __CONVEXALGORITHMS_186081759026531609525247_H__

#include "../math/MathHeader.h"

namespace Game {;

class ConvexUtils {
public:

STATIC Vec3f closestPointOnHull(Vec3f& point, Vec3f* hullPoints, int32_t nPoints);

//FWDDCL - Function FWDDCL
STATIC
void convex_proj_t_3f(
               Vec3f* norm,
               Vec3f* pt,
               int32_t n_pt,
               float* __out_ t_min_out,
               float* __out_ t_max_out,
               int32_t* __out_ pt_i_min_out,
               int32_t* __out_ pt_i_max_out
               );
STATIC
int32_t convex_n_proj_box(
              Plane3f* pf,        //proj line
              Vec3f* pt1,    //pts
              //int32_t npt1,
              Vec3f* pt2,        //pts
              //int32_t npt2, 
              Vec3f* v_h1,    //hulls
              Vec3f* v_h2,
              Vec3f* v_n_h1,    //vels
              Vec3f* v_n_h2,
              float* __out_ out_t    // time along Velocity vectors that we collide.
              );
STATIC
int32_t boxBoxConvex(
                 Vec3f* pt_h1,        // points hull 1
                 Plane3f* pf_h1,    // planes hull 1
                 Vec3f* v_h1,        // velocity
                 Vec3f* v_n_h1,        // normalized vl

                 Vec3f* pt_h2,        // points hull 2
                 Plane3f* pf_h2,    // planes hull 2
                 Vec3f* v_h2,        // velocity
                 Vec3f* v_n_h2,

                 float* t_out,    // time till collision
                 
                 int32_t* p_h1_i_out, // Point indexes where the hulls collided
                 int32_t* p_h2_i_out
                 );
STATIC
int32_t convexConvexTest(
                 int32_t n_pt_h1,        // num points hull 1
                 int32_t n_pf_h1,        // num planes hull 1
                 Vec3f* pt_h1,        // points hull 1
                 Plane3f* pf_h1,    // planes hull 1
                 Vec3f* v_h1,        // velocity
                 Vec3f* v_n_h1,        // normalized velocity 1
                 
                 int32_t n_pt_h2,        // num points hull 2
                 int32_t n_pf_ht,        // num planes hull 2
                 Vec3f* pt_h2,        // points hull 2
                 Plane3f* pf_h2,    // planes hull 2
                 Vec3f* v_h2,        // velocity
                 Vec3f* v_n_h2,
                
                 float* t_out,    // time till collision
                 
                 int32_t* p_h1_i_out, // Point indexes where the hulls collided
                 int32_t* p_h2_i_out,
                 bool is_box        // true if we are box so we can simplify equations.
                 );



};


}//ns game



#endif
