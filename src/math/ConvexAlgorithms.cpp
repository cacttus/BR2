//#include "../math/MathIncludes.h"
//#include "../math/Algorithm.h"

#include "../math/MathAll.h"
namespace Game {

/**
*    @fn convex_axis_t
*    @brief Smashes points of a convex hull along the line of a plane normal
*    This is a SAT function. (seaparate axis teheorom)
*    @return returns the minimum and maximum "t" along the normal wehre (Normal * t)=extent
*/
    void ConvexUtils::convex_proj_t_3f(
                   Vec3f* norm,
                   Vec3f* pt,
                   int32_t n_pt,
                   float* __out_ t_min_out,
                   float* __out_ t_max_out,
                   int32_t* __out_ pt_i_min_out,
                   int32_t* __out_ pt_i_max_out
                   )
{
//    int32_t ipt;
//    float proj_t;
//    *t_min_out = FLT_MAX;
//    *t_max_out = -FLT_MAX;
//    
///*
//    There has to be a way to optimize an aabb projection.
//    Not thinking of this right now.. but we should return
//    to this in the future.
//*/
//
//    // - Loop through all points.
//    for(ipt=0; ipt<n_pt; ++ipt)
//    {
//        // - Project the "t" onto the line.
//        proj_t = pointOnRay_t(*norm,pt[ipt]);
//
//        // - Test for min/max in the T.
//        if(proj_t<*t_min_out)
//        {
//            *t_min_out = proj_t;
//            *pt_i_min_out = ipt;
//        }
//        if(proj_t>*t_max_out)
//        {
//            *t_max_out = proj_t;
//            *pt_i_max_out = ipt;
//        }
//    }

}

/**
*    @fn convex_n_proj_box
*    @brief Project a box solid onto a line.  This is a SAT function.
*
    THID SIS MESSED UP
    THID SIS MESSED UP
    THID SIS MESSED UP
    THID SIS MESSED UP
    THID SIS MESSED UP
    THID SIS MESSED UP
    THID SIS MESSED UP
*/
int32_t ConvexUtils::convex_n_proj_box( 
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
              )
{
    float 
        min_t_h1 = FLT_MAX, 
        max_t_h1 = -FLT_MAX, 
        min_t_h2 = FLT_MAX, 
        max_t_h2 = -FLT_MAX
        ;    // saved "t" along the line for the min/max
    int32_t 
        min_p_h1, 
        max_p_h1, 
        min_p_h2, 
        max_p_h2
        ;        // point indexes

    // THis must have something to do with the componentized velocity.
    //Vec3f 
    //    comp_v_h1 = v_n_h1.dot(pf->n)*v_h1,
    //    comp_v_h2 = v_n_h2.dot(pf->n)*v_h2
    //    ;    // componentized velocities.
    //        // the amount of the velocity vector along this line (positive or negative)

    convex_proj_t_3f(&(pf->n),pt1,8,&min_t_h1,&max_t_h1,&min_p_h1,&max_p_h1);

    convex_proj_t_3f(&(pf->n),pt2,8,&min_t_h2,&max_t_h2,&min_p_h2,&max_p_h2);

    // * Now that we have the 4 axis minimax "line-boxes" do a basic collision test.

    //**Following won't work for predetermined collisions.
    // - minimax axis test
    if(max_t_h1 < min_t_h2)
        return 0;
    if(max_t_h2 < min_t_h1)
        return 0;
    return 1;


}
// - boc 
int32_t ConvexUtils::boxBoxConvex(     
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
                 )
{
    int32_t ret;
    int32_t ipf;//loop var

    for(ipf=0; ipf<6; ++ipf)
    {
        ret = convex_n_proj_box(
            &(pf_h1[ipf]),    

            pt_h1,
            pt_h2,
            v_h1,
            v_h2, 
            v_n_h1,
            v_n_h2,
            t_out
            );

        if(ret==0)
            return 0;
    }
    for(ipf=0; ipf<6; ++ipf)
    {
        ret = convex_n_proj_box(
            &(pf_h2[ipf]),    

            pt_h1,
            pt_h2,
            v_h1,
            v_h2, 
            v_n_h1,
            v_n_h2,
            t_out
            );
        if(ret==0)
            return 0;

    }

    return 1;
}
/**
*    @fn 
*    @brief Convex hull collision
*    Return true/false if collision and the time until the convex hulls collide, 
    and the point index of the hull collisions
*/
int32_t ConvexUtils::convexConvexTest(
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
                 t_bool is_box        // true if we are box so we can simplify equations.
                 )
{
    if(is_box)
    {
        return boxBoxConvex(pt_h1,pf_h1,v_h1,v_n_h1,pt_h2,pf_h2,v_h2,v_n_h2,t_out,p_h1_i_out,p_h2_i_out);
    }
    else throw new NotImplementedException();

    return 0;
}
 Vec3f ConvexUtils::closestPointOnHull(Vec3f& point, Vec3f* hullPoints, int32_t nPoints)
{
    BroThrowNotImplementedException();
    return Vec3f(0,0,0);
    // Abandoned
    // we need either normals or indexes in order to verify topology -- just points alone doesn't work.
    /*
    Vec3f cp[nPoints];
    int32_t closest[nPoints];    // if we have similars we will have to interpolate the centers.
    int32_t nClosest;
    float lastMax;
    float dist;

    dist        = cp[0].distance2(point);
    lastMax        = dist;
    closest[0]    = lastMax;
    nClosest    = 1;

    for(int n=1; n<nPoints; ++n)
    {
        dist = point.distance2(cp[n]);
        if(dist < lastMax)
        {
            lastMax = dist;
            //Reset closest array
            nClosest = 1;
            closest[0] = n;
        }
        else if(dist==closest[0])
        {
            nClosest++;
            closest[nClosest] = n;
        }
    }
    
    if(nClosest==1)
    {
        //DO NOTHIGN
        closestPt = cp[closest[0]];
    }
    else if(nClosest==2)
    {
        Vec3f v1 = cp[closest[0]] + (cp[closest[1]] - cp[closest[0]]) * 0.5f;
        closestPt = v1.distance2(point);
    }
    else if(nClosest==3)
    {
        Vec3f avg;
        bilinear_interpolate(    cp[closest[0]], 
                                cp[closest[1]], 
                                cp[closest[0]], 
                                cp[closest[2]],
                                avg,
                                0.5f
                                );

        closestPt = avg.distance2(point);
    }
    else if(nClosest==4)
    {
        //Bilinear interpolate the shit
        Vec3f avg;
        bilinear_interpolate(    cp[closest[0]], 
                                cp[closest[1]], 
                                cp[closest[2]], 
                                cp[closest[3]],
                                avg,
                                0.5f
                                );

        closestPt = avg.distance2(point);
    }
    else
        BroThrowException("There were more than 4 points in the cube distance equation.  This is impossible.  Feel free to slap me.");
*/

}


























}//ns game
