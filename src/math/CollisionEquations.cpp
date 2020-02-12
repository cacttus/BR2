
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../math/MathAll.h"
#include "../math/CollisionEquations.h"
//#include "../dev/DebugHelper.h"
//#include "../math/MathIncludes.h"
//#include "../physics/PhysicsIncludes.h"
//#include "../physics/PhysicsVolume.h"
//#include "../physics/SphericalVolume.h"
//#include "../physics/TriangleMeshVolume.h"

namespace BR2 {

CollisionResult::e CollisionEquations::sat_axis_box_t_fast(
    const float& v0, const float& c0, const float& r0,
    const float& v1, const float& c1, const float& r1,
    float& __out_ t_out, float& __out_ t0_out, float& __out_ t1_out) {

    if(v0-v1==0.0){
        float ax1, ax2, bx1, bx2;
        ax1 = c0 - r0;
        ax2 = c0 + r0;
        bx1 = c1 - r1;
        bx2 = c1 + r1;
        sat_box_int_swap_t(ax1, ax2);
        sat_box_int_swap_t(bx1, bx2);
        if (int_intersect_t(ax1, ax2, bx1, bx2) == false) {
            //We are not already intersecting - so no collision
            t0_out = t1_out = FLT_MAX;
            return CollisionResult::e::Avoided;
        }
        //We are already intersecting - so collision at t=0
        t0_out = t1_out = 1.0;
        return CollisionResult::e::Stuck;
    }

    //Original thing
    t0_out = (c1 - c0 + r0 + r1) / (v0 - v1);
    t1_out = (c1 - c0 - r0 - r1) / (v0 - v1);

    if (t1_out < t0_out) {
        float tmp = t1_out;
        t1_out = t0_out;
        t0_out = tmp;
    }
    //We do not perform an 01 check here.
    //This is because for any axis you can already be colliding, or collide a long time later.
    // if the t0 <0 && t1 <0 then no collision
    // if t0 >1 && t1 > 1 then no collision
    // otherwise yes
    if(t0_out <0 && t1_out <0){
        return CollisionResult::e::Avoided;
    }
    if(t0_out >1 && t1_out >1) {
        return CollisionResult::e::Avoided;
    }
    return CollisionResult::e::Collided;
}
bool CollisionEquations::in_range_t(float t0, float t1, float x) {
    bool b = (x >= t0 && x <= t1);
    return b;
}
bool CollisionEquations::in_01(float x) {
    bool b = in_range_t(0, 1, x);
    b |= in_range_t(0, x, 1);
    b |= in_range_t(x, 1, 0);
    return b;
}
bool CollisionEquations::int_intersect_t(float ta0, float ta1, float tb0, float tb1) {
    //Return true if the two intervals intersect
    return CollisionEquations::in_range_t(ta0, ta1, tb0) || 
        CollisionEquations::in_range_t(ta0, ta1, tb1) || 
        CollisionEquations::in_range_t(tb0, tb1, ta0) || 
        CollisionEquations::in_range_t(tb0, tb1, ta1);
}
void CollisionEquations::sat_box_int_swap_t(float& a, float& b) {
    if (a > b) {
        float tmp = a;
        a = b;
        b = tmp;
    }
}
bool CollisionEquations::sat_box_axis_t(float c1, float c2, float r1, float r2, float v1, float v2, 
    float& out_t0, float& out_t1, bool& countXYZ, bool& bStuck) {
#ifdef _DEBUG
    //We don't want this to fail at runtime, 
    AssertOrThrow2(r1 > 0.0f);
    AssertOrThrow2(r2 > 0.0f);
#else 
    if(r1 < 0.0f || r2 < 0.0f) {
        BroLogWarn("Error 922508- box radius was zero for object.");
    }
#endif
    countXYZ = true;
    bStuck = false;


    float tt;
   // float tb0, tb1;

    CollisionResult::e cr;

    //These return the same results for some reason.
   cr = CollisionEquations::sat_axis_box_t_fast(v1, c1, r1, v2, c2, r2, tt, out_t0, out_t1);
   if(cr == CollisionResult::e::Collided) {
      return true;
   }
   else if(cr == CollisionResult::e::Stuck){
       countXYZ = false;
       bStuck = true;
       return true;
   }

   return false;

}

bool CollisionEquations::sat_box3_box3_t(const vec3& c1, const vec3& c2, const vec3& r1, const vec3& r2,
    const vec3& v1, const vec3& v2, float& out_t0, float& out_t1, int& ax_t0, int& ax_t1, bool& bStuck, 
    bool bCheckingIfStuck) {
    //SAT algorithm for an axis aligned box.
    //c1,c2 - center of box.  r1,r2 - 1/2 width of box, v1,v2 velocity of box, out_t0, t1, - hit and leave times along v
    //ax_t0 = 0,1,2 = x,y,z axis we collided on
    vec3 t0, t1;
    bool countX, countY, countZ;
    bool stuckX, stuckY, stuckZ;

    stuckX = stuckY = stuckZ = true;

    if (sat_box_axis_t(c1.x, c2.x, r1.x, r2.x, v1.x, v2.x, t0.x, t1.x, countX, stuckX) == false) {
        return false; 
    }
    if (sat_box_axis_t(c1.y, c2.y, r1.y, r2.y, v1.y, v2.y, t0.y, t1.y, countY, stuckY) == false) {
        return false; 
    }
    if (sat_box_axis_t(c1.z, c2.z, r1.z, r2.z, v1.z, v2.z, t0.z, t1.z, countZ, stuckZ) == false) {
        return false; 
    }

    bStuck = stuckX && stuckY && stuckZ;

    //if(countX && ((t0.x < 0 && t1.x <0) || (t0.x>1 && t1.x>1))){
    //    return false;
    //}
    //if (countY && ((t0.y < 0 && t1.y <0) || (t0.y>1 && t1.y>1))) {
    //    return false;
    //}
    //if (countZ && ((t0.z < 0 && t1.z <0) || (t0.z>1 && t1.z>1))) {
    //    return false;
    //}

    float ct0 = FLT_MAX;

    if (countX && (t0.x >= 0.0f) && (t0.x < ct0)) { ct0 = t0.x; ax_t0 = 0; }
    if (countY && (t0.y >= 0.0f) && (t0.y < ct0)) { ct0 = t0.y; ax_t0 = 1; }
    if (countZ && (t0.z >= 0.0f) && (t0.z < ct0)) { ct0 = t0.z; ax_t0 = 2; }

    if (countX && (t1.x >= 0.0f) && (t1.x < ct0)) { ct0 = t1.x; ax_t0 = 0; }
    if (countY && (t1.y >= 0.0f) && (t1.y < ct0)) { ct0 = t1.y; ax_t0 = 1; }
    if (countZ && (t1.z >= 0.0f) && (t1.z < ct0)) { ct0 = t1.z; ax_t0 = 2; }

    float ct1 = -FLT_MAX;

    if (countX && (t0.x <= 1.0f) && (t0.x > ct1)) { ct1 = t0.x; ax_t1 = 0; }
    if (countY && (t0.y <= 1.0f) && (t0.y > ct1)) { ct1 = t0.y; ax_t1 = 1; }
    if (countZ && (t0.z <= 1.0f) && (t0.z > ct1)) { ct1 = t0.z; ax_t1 = 2; }

    if (countX && (t1.x <= 1.0f) && (t1.x > ct1)) { ct1 = t1.x; ax_t1 = 0; }
    if (countY && (t1.y <= 1.0f) && (t1.y > ct1)) { ct1 = t1.y; ax_t1 = 1; }
    if (countZ && (t1.z <= 1.0f) && (t1.z > ct1)) { ct1 = t1.z; ax_t1 = 2; }

    out_t0 = ct0;
    out_t1 = ct1;

    if(in_range_t(0.0f, 1.0f, out_t0) || in_range_t(0.0f,1.0f, out_t1)) {
        return true;
    }
    else {
        return false;
    }
}

bool same_side_box_ax_n(float p0min, float p0max, float n, float n_pt){
    bool ret;
    //we're checking signed distance to an "axis plane" here.
    // we're checking BA (p0) is on the side of BB.
    //from the plane equation signed distance = n dot p - d, d = n dot pointOnPlane
    //Return true if this axis is on the given plane side.
    float dx00 = (n)*p0min - (n)*n_pt;
    float dx01 = (n)*p0max - (n)*n_pt;

    ret = (dx00 >= 0) && (dx01 >= 0);
    return ret;
}
void CollisionEquations::aa_box_contact_plane_3(const Box3f* ba, const Box3f* bb, vec3& out_n, const vec3& in_v) {
    //The one that works -- 
    //in_v = velocity
    //out_n - slide plane normal
    //ba - box that is moving
    //bb - box to slide against
    
    //These will collide with an axis.  They're basically the same as checking if the boxes intersect on each axis.
    bool s0 = same_side_box_ax_n(ba->_min.x, ba->_max.x, -1, bb->_min.x);
    bool s1 = same_side_box_ax_n(ba->_min.x, ba->_max.x,  1, bb->_max.x);
    bool s2 = same_side_box_ax_n(ba->_min.y, ba->_max.y, -1, bb->_min.y);
    bool s3 = same_side_box_ax_n(ba->_min.y, ba->_max.y,  1, bb->_max.y);
    bool s4 = same_side_box_ax_n(ba->_min.z, ba->_max.z, -1, bb->_min.z);
    bool s5 = same_side_box_ax_n(ba->_min.z, ba->_max.z,  1, bb->_max.z);

    float cv = 0.5773502691896259f; // 1/ sqrt( 1^2 + 1^2 + 1^2 )
    float ev = 0.7071067811865475f; // 1/ sqrt( 1^2 + 1^2 )

    //Construct the slide plane.  If we didn't format it we'd have the correct collision
    //plane for normals, points, and edges.
    float fx=0, fy=0, fz=0;
    if(s0) fx = -1;
    else if(s1) fx = 1;
    else fx = 0;

    if (s2) fy = -1;
    else if (s3) fy = 1;
    else fy = 0;

    if (s4) fz = -1;
    else if (s5) fz = 1;
    else fz = 0;

    //Filter components so that we have only one normal
    //So here - we favor a side axis via the velocity vector.  If we didn't we'd be using the
    // edges and points as slide planes, which cause the character to "pause" when colliding between seamless
    // cubes since we will collide with edges very often.
    float vx = fabsf(fx * in_v.x), vy = fabsf(fy * in_v.y), vz = fabsf(fz* in_v.z);
    if(fx !=0 && fy !=0) {
        if(vx > vy) {
            fx = 0;
        }
        else {
            fy = 0;
        }
    }
    if (fx != 0 && fz != 0) {
        if (vx > vz) {
            fx = 0;
        }
        else {
            fz = 0;
        }
    }
    if (fy != 0 && fz != 0) {
        if (vy > vz) {
            fy = 0;
        }
        else {
            fz = 0;
        }
    }

    out_n.construct(fx,fy,fz);
    out_n.normalize();

}
void CollisionEquations::aa_point_contact_plane(vec3& pt, vec3& bc, vec3& out_n) {
    //Return the closest normal to the box.
    //bc = center of box
    //pt - point that rests on the side of the box.
    vec3 diff = (pt - bc).normalize();

    vec3 n[6];

    n[0].construct( 1,  0,  0);
    n[1].construct(-1,  0,  0);
    n[2].construct( 0,  1,  0);
    n[3].construct( 0, -1,  0);
    n[4].construct( 0,  0,  1);
    n[5].construct( 0,  0, -1);

    float max = -FLT_MAX;
    for(int i=0; i<6; ++i){
        float f = diff.dot(n[i]);
        if(f > max){
            max = f;
            out_n = n[i];
        }
    }
}


//Collide a dynamic moving hull with a static one.  Return t0, t1 at collision.
//This is more optimal than d hull vs d hull because the collision equation reduces to line/plane (no sqrt).
bool CollisionEquations::sat_hull_hull_t(
    const ConvexHull& a_hull, const vec3& a_hull_v, 
    const ConvexHull& b_hull, const vec3& b_hull_v, //b hull does not move
    float& out_t0, float& out_t1)
{
    bool bRet = false;

    for(GpuFace gp : a_hull._faces) {
        float a_te0, a_te1, b_te0, b_te1; // t values for extents.
        int32_t a_ie0, a_ie1, b_ie0, b_ie1; //point indexes of extents

        //Project all points along the GPUFACE normal
        sat_hull_proj_ext(a_hull, gp, a_te0, a_te1, a_ie0, a_ie1);
        sat_hull_proj_ext(b_hull, gp, b_te0, b_te1, b_ie0, b_ie1);
 
        //Preliminary checkes before we collide
        //We must have positive indexes.
        if(a_ie0 >= 0 && a_ie1 >= 0 && b_ie0 >= 0 && b_ie1 >= 0) {
          //  CollisionResult::e ve0, ve1;
           // float vt0, vt1;

            //Check for stuck this is faster than the one below
            if ((a_te0 > 0) && (a_te1 < 0)  ||
                (a_te1 > 0) && (a_te0 < 0)  ||
                (b_te0 > 0) && (b_te1 < 0)  ||
                (b_te1 > 0) && (b_te0 < 0) ) {

            }
            else {
                float a_vel_t = Alg::pointOnRay_t(gp._n, a_hull_v);
                float b_vel_t = Alg::pointOnRay_t(gp._n, b_hull_v);

                vec3 a_vel_proj_t = gp._n * a_vel_t;//projected velocity for hull A
                vec3 b_vel_proj_t = gp._n * b_vel_t;//projected velocity for hull A

                vec3 a_c = a_hull._points[a_ie0] + (a_hull._points[a_ie1] - a_hull._points[a_ie0]) * 0.5;
                vec3 b_c = b_hull._points[b_ie0] + (b_hull._points[b_ie1] - b_hull._points[b_ie0]) * 0.5;


            }


        }
    }
    for (GpuFace gp : b_hull._faces) {
        //same
    }



    return bRet;
}
void CollisionEquations::sat_hull_proj_ext(
    const ConvexHull& hull, const GpuFace& face,
    float& __out_ t_e0, float& __out_ t_e1,
    int32_t& __out_ ind_e0, int32_t& __out_ ind_e1) {
    //Project the SAT hull's extents along this plane normal.
    t_e0 = -FLT_MAX;
    t_e1 = FLT_MAX;
    ind_e0 = ind_e1 = -1;
    float t;
    for (size_t ipt=0; ipt < hull._points.size(); ipt++) { //vec3 pt : hull._points) {
        t = Alg::pointOnRay_t(face._n, hull._points[ipt]);
        if( t < t_e1 ) {
            t_e1 = t;
            ind_e1 = (int32_t)ipt;
        }
        if (t > t_e0) {
            t_e0 = t;
            ind_e0 = (int32_t)ipt;
        }
    }
}
 //float CollisionEquations::hull_ext_pt_t_stat(
 //    const vec3& p0, const vec3& v0, 
 //    const vec3& p1, const vec3& v1){

 //    if(v0 - v1 == 0){
 //        //Fix this.
 //       Gu::debugBreak();
 //    }
 //    else {
 //        return (p1 - p0) / (v0 - v1);
 //    }
 //}

 CollisionResult::e CollisionEquations::line_plane_collision_linear(
     const vec3& p1, // point 1
     const vec3& p2, // point 2
     const Plane3f& plane, //plane
     float& out_t //[out] t along line. v = p1 + (p2-p1)*t
 )
 {
     return line_plane_collision_linear(p1, p2, plane.n, plane.d, out_t);
 }
 CollisionResult::e CollisionEquations::line_plane_collision_linear(
     const vec3& p1, // point 1
     const vec3& p2, // point 2
     const vec3& plane_n,
     const float& plane_d,
     float& out_t //[out] t along line. v = p1 + (p2-p1)*t
 )
 {
     vec3 ray = p2 - p1;
     float nv = plane_n.dot(ray);

     if (nv == 0.0f) {//or zero - if nv is zero avoid infinity dividde
         return CollisionResult::Avoided; //no velocity or screwy7 plane normal
     }

    //Solve this -
    // n . (p0+t(p1-p0)) + d = 0
     float a1 = -plane_n.dot(p1) - plane_d;
     out_t = a1 / nv;
     
     if ((out_t >= 0.0f) && (out_t <= 1.0f)){
         return CollisionResult::Collided;
     }

     return CollisionResult::Avoided;
 }
 CollisionResult::e CollisionEquations::sphere_collide_sphere(
     const vec3& v1, const vec3& p1, const float r1
     , const vec3& v2, const vec3& p2, const float r2
     , float& t, float& root_1, float& root_2
)
{
    // - Check first to see if we collide at all ( kindof an optimization )
    vec3 dp = p2 - p1;
    vec3 dv = v2 - v1;

    float r = r1 + r2;
    float pp = dp.squaredLength();
    float rr = r*r;
    float c = pp - rr;

    //spheres intersect eachother
    if (pp <= rr) {
        t = dp.length();    //Distance - for stuck
        return CollisionResult::e::Stuck;
    }

    float pv = dp.dot(dv);

    //spheres move away from eachother
    //these do not apply tou our system.  they would work theoretically if the sphere vecotrs
    // were not intersecting anything at presenta nd the spheres had a clear path, but they
    //do not work if you have say a sphere that is not going to move along its vector because
    //it already collides
    //if(pv>=0) 
    //    return CollisionResult::Avoided;
    //
    float vv = dv.dot(dv);

    //if ( (pv + vv) <= 0 && (vv + 2 * pv + c) >= 0 )
    //    return CollisionResult::Avoided;

    // - parameters for the quadratic formula
    float a = vv;
    float b = 2 * pv;

    // b^2 -4ac  the quadratic formula discriminant ( we are solving for t )
    float d = b*b - 4.0f*a*c;

    if (d < 0.0f) {
        //imaginary root
        return CollisionResult::e::Avoided;
    }

    if (a == 0.0f)
    {
        if (pv == 0.0f)
        {
            //divisor is zero.  Return that we avoided the collision
            //to avoid blowing up.
            t = 1.0f;
            return CollisionResult::e::Avoided;
        }
        //when a is zero we have a linear equation and can't do the quadratic equation.
        // v^2t^2 + 2pv*t + p^2 - r^2 = 0
        t = (rr - pp) / (2 * pv);
        if (t >= 0.0f && t <= 1.0f) {
            return CollisionResult::e::Collided;
        }
        else {
            return CollisionResult::e::Avoided;
        }
    }

    float sqrtD = MathUtils::broSqrt(d);

    root_1 = (-b + sqrtD) / (2 * a);
    root_2 = (-b - sqrtD) / (2 * a);

    //make root1 smaller than root 2
    if (root_1 > root_2)
    {
        float temp = root_2;
        root_2 = root_1;
        root_1 = temp;
    }

    if (root_1 >= 0.0f && root_1 <= 1.0f)
    {
        t = root_1;
        return CollisionResult::e::Collided;
    }
    if (root_2 >= 0.0f && root_2 <= 1.0f)
    {
        t = root_2;
        return CollisionResult::e::Collided;
    }

    return CollisionResult::e::Avoided;
}
bool Ceq::triangleAabbTest(const vec3 *vertices, const vec3 &aabbMin, const vec3 &aabbMax)
{
    //
    // @fn Copied from bullet.
    // @brief QUICK Test a triangle against an AABB.  Returns true if it intersects the box, false if not
    //
    const vec3& p1 = vertices[0];
    const vec3& p2 = vertices[1];
    const vec3& p3 = vertices[2];

    if (MathUtils::broMin(MathUtils::broMin(p1.x, p2.x), p3.x) > aabbMax.x) return false;
    if (MathUtils::broMax(MathUtils::broMax(p1.x, p2.x), p3.x) < aabbMin.x) return false;

    if (MathUtils::broMin(MathUtils::broMin(p1.z, p2.z), p3.z) > aabbMax.z) return false;
    if (MathUtils::broMax(MathUtils::broMax(p1.z, p2.z), p3.z) < aabbMin.z) return false;

    if (MathUtils::broMin(MathUtils::broMin(p1.y, p2.y), p3.y) > aabbMax.y) return false;
    if (MathUtils::broMax(MathUtils::broMax(p1.y, p2.y), p3.y) < aabbMin.y) return false;

    return true;
}


}//ns game
