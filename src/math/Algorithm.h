/**
*  @file Algorithm.h
*  @date April 2, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __ALGORITHM_8732141251172380469536_H__
#define __ALGORITHM_8732141251172380469536_H__

//todo: cpp file.
#pragma once
#include "../math/MathHeader.h"
#include "../math/Vec3x.h"
#include "../math/Box3x.h"
#include "../math/BitHacks.h"
#include "../math/Vec3Basis.h"
namespace BR2 {
class Alg {
public:
// legendary 1/sqrt(x), from quake 3 et al
FORCE_INLINE static float rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                        // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}
//todo: functions
// forward modular addition keeping x+n within [0,y] (x+n)%y
// reverse modular addition keeping x-n within [0,y] (yn+x-n)%y

FORCE_INLINE static bool isPow2(int32_t x)
{
    return BitHacks::bitcount(x)==1;
}
FORCE_INLINE static bool isPow2(int64_t x)
{
    return BitHacks::bitcount(x)==1;
}
FORCE_INLINE void linear_interpolate(float a, 
                                     float b, 
                                     float& __out_ avg,
                                     float pct = 0.5f)
{
    avg =  a + ( b -  a)*pct;
}
// cosine interpolate between two values.
FORCE_INLINE static float cerp_1D(float a, float b, float x){
    float ft = x * (float)M_PI;
    float f = (1.0f - cosf(ft)) * 0.5f;
    return  a*(1-f) + b*f;
}
#define cosine_interpolate cerp_1D

// - Linear interpolate between two values
FORCE_INLINE static float lerp_1D(float a, float b, float t)
{
    return a+t*b;
}

//For noise functions see Random.h & Noise.h

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
*   @fn pointOnLine_t
*   @brief Returns the t value of a point projected onto a given line
*   @param a The first point of the line
*   @param b The second point of the line
*   @param p The point to find.
*   @return t - if t<0 then the point is before a, if t>1.0 the point is past b.
*                if t is between 0 and 1 the point is within the boundaries of the line.
*
*    NOTE* You must make sure that A is the start of the ray.
*/
static FORCE_INLINE float pointOnLine_t( const vec3& a, const vec3& b, const vec3& p ) {
    vec3 AP = p - a;    
    vec3 AB = b - a;    
    float ab2 = AB.x*AB.x + AB.y*AB.y + AB.z*AB.z;   
    float ap_ab = AP.x*AB.x + AP.y*AB.y + AP.z*AB.z;   
    float t = ap_ab / ab2;   
    return t;
}

/**
*   @fn pointOnRay_t
*   @brief Returns the closest point on a ray/
*   @remarks Same as PointOnLine_t but we use a ray here.
*   @return t - see pointOnLine_t
*
*    TESTED
*        Note: This had a bug as stupidly I didn't include the z component.
*               works now 1.19.14.
*/
static FORCE_INLINE float pointOnRay_t( const vec3& a, const vec3& p ) {
    vec3 AP = p - a;    
    vec3 AB = a*-1.0f;    
    float ab2 = AB.x*AB.x + AB.y*AB.y + AB.z*AB.z;   
    float ap_ab = AP.x*AB.x + AP.y*AB.y + AP.z*AB.z;   
    float t = ap_ab / ab2;   
    return t;
}
//Static version of above function
//static FORCE_INLINE float pointOnRay_tSTA( Vector3& a, Vector3& p ) {
//    static Vector3 AP = p - a;    
//    static Vector3 AB = a*-1.0f;    
//    static float ab2 = AB.x*AB.x + AB.y*AB.y + AB.z*AB.z;   
//    static float ap_ab = AP.x*AB.x + AP.y*AB.y + AP.z*AB.z;   
//    static float t = ap_ab / ab2;   
//    return t;
//}
/**
*   @fn pointOnRay_SLOW
*   @brief Returns the closest point on a ray.
*
*    TESTED _ WORKS 1.19.14
*/
static Vec3f pointOnLine_SLOW( const vec3& a, const vec3& b, const vec3& p ) {
    
    Vector3 n = b-a;
    n.normalize();
    float pdist = n.dot(p) - n.dot(a);
    return (a+ n*pdist);
}
/**
*  @fn ClosestPointOnAACubeFromPoint
*  @brief This returns the closest C8 point RELATIVE TO THE INPUT POINT for the given cube.
*    I.e. To get the **actual** euclidian point you will have to add 'p' to the return value.
*    this is for optimization.
*
*    ALSO NOTE: 
*
*/
FORCE_INLINE static Vec3f ClosestPointOnAACubeFromPoint( Vec3f& p, Box3f& cc )
{
    Vec3f outPt;
    cc.closestPoint(p,outPt);
    return outPt;
    //This algorithm is incorrect
    //Vec3f pa = cc.min - p, 
    //      pb = cc.max - p, 
    //      pc;
    //
    //if( pa.x < pb.x )
    //    pc.x = pa.x;
    //else 
    //    pc.x = pb.x;
    //
    //if( pa.y < pb.y )
    //    pc.y = pa.y;
    //else 
    //    pc.y = pb.y;
    //
    //if( pa.z < pb.z )
    //    pc.z = pa.z;
    //else 
    //    pc.z = pb.z;
    //
    //return pc;
        
}
/**
*  @fn mint
*  @brief Mersenne integer function 1 dimension

    Tests have showed that all functions have a floating point standard deviation of about 0.6 but remain
    on average around 6.0e-005
    Integer functions have a fairly good distribution from 0-n

    The higher dimension functions distribute well, but have a slight, almost unnoticable bias to stay above zero

*/
static FORCE_INLINE int32_t mint(int32_t seed, int32_t off=0x9d2c5680)
{
     int32_t x;
     
     x = (0x6c078965 * (seed ^ (seed>>30))+(off)) & 0xffffffff;
     x = x ^ (x>>11);
     x = x ^ ((x<<7) & 0x9d2c5680);
     x = x ^ ((x<<15) & 0xefc60000);
     x = x ^ (x>>18);
     return x;
}
/**
*  @fn mint2
*  @brief Mersenne integer function 2 dimensions
*/
static FORCE_INLINE int32_t mint2(int32_t x, int32_t y, int32_t off=0x9d2c5680)
{
    return mint(x,off) ^ mint(y,off);
}
/**
*  @fn mint3
*  @brief Mersenne integer function 3 dimensions
*/
static FORCE_INLINE int32_t mint3(int32_t x, int32_t y, int32_t z, int32_t off=0x9d2c5680)
{
    return mint(x,off) ^ mint(y,off) ^ mint(z,off);
}
/*
*    convert integer to floating point in the range of [-1,1]
*/
static FORCE_INLINE float flotc11(int32_t ix)
{
    //0x007fffff is the fractional portion of a floating point.
    int32_t a = (ix & 0x007fffff) | 0x40000000;
    return( *((float*)&a) - 3.0f );
}
/*
*    convert integer to floating point in the range of [0,1]
*/
static FORCE_INLINE float flotc01(int32_t ix)
{
    //0x007fffff is the fractional portion of a floating point.
    int32_t a = (ix & 0x007fffff) | 0x3f800000;
    return( *((float*)&a) -1.0f );
}
/**
*  @fn 
*  @brief Mersenne float function 1 dimension
*    Return value between [-1,1]
*/
static FORCE_INLINE float meft11(int32_t x, int32_t off=0x9d2c5680)
{
    return flotc11(mint(x,off));
}
static FORCE_INLINE float meft01(int32_t x, int32_t off = 0x9d2c5680)
{
    return flotc01(mint(x, off));
}
/**
*  @fn 
*  @brief Mersenne float function 2 dimension
*    Return value between [-1,1]
*/
// - Mersenne float 2 param
static FORCE_INLINE float meft2(int32_t x, int32_t y, int32_t off=0x9d2c5680)
{
    return flotc11(mint2(x,y,off));
}
/**
*  @fn 
*  @brief Mersenne float function 3 dimension
*    Return value between [-1,1]
*/
static FORCE_INLINE float meft3(int32_t x, int32_t y, int32_t z, int32_t off=0x9d2c5680)
{
    return flotc11(mint3(x,y,z,off));
}
//Mean of a set of numbers.
static FORCE_INLINE double meanf(std::vector<double>& ss)
{
    double sum=0;
    for(size_t i=0; i<ss.size(); ++i)
        sum+=ss[i];
    return (double)(sum/(float)ss.size());
}
//Standard deviation of a set of numbers.
static FORCE_INLINE double sdevf(std::vector<double>& vin)
{
    double mean = meanf(vin);
    double sum2=0;
    double sub;
    for(size_t i=0; i<vin.size(); ++i)
    {
        sub = (vin[i]-mean);
        sum2 += sub*sub; // (a-b)^2
    }
    mean = sum2 / (double)vin.size();
    return (double)MathUtils::broSqrt((float)mean);
}
// - Sampled standard deviation
static FORCE_INLINE double ssdevf(std::vector<double>& vin)
{
    double mean = meanf(vin);
    double sum2=0;
    double sub;
    for(size_t i=0; i<vin.size(); ++i)
    {
        sub = (vin[i]-mean);
        sum2 += sub*sub; // (a-b)^2
    }
    mean = sum2 / ((double)vin.size()-1);
    return (double)MathUtils::broSqrt((float)mean);
}

/**
*  @fn getNormalBasisZ
*  @brief Given a normal vector in 3 dimensions this returns the tangent and binormal basis vectors
*    The vectors point in the relative positive direction in the given LHS or RHS coordinate system.
*/
static FORCE_INLINE void getNormalBasisZ(const Vec3f& __in_ normal, Vec3f& __out_ left, Vec3f& __out_ up)
{
    Vec3f worldUp = Vec3f(0,1,0);
#ifdef COORDINATE_SYSTEM_RHS
    
    if(normal.dot(worldUp)==-1)
        //vectors are oppposing
        worldUp=Vec3f(0,0,1);
    else if(normal.dot(worldUp)==1)
        //vectors are the same
        worldUp=Vec3f(0,0,-1);

    left = worldUp.cross( normal ).normalize();
    up = normal.cross(left).normalize();
#else
    if (normal.dot(worldUp) == -1)
        //vectors are oppposing
        worldUp = Vec3f(0, 0, 1);
    else if (normal.dot(worldUp) == 1)
        //vectors are the same
        worldUp = Vec3f(0, 0, -1);

    left = worldUp.cross(normal).normalize();
    up = normal.cross(left).normalize();
#endif

}
static FORCE_INLINE Vec3Basis getNormalBasisZ(const Vec3f& __in_ normal_z)
{
    Vec3Basis ret;
    ret._z = normal_z;
    getNormalBasisZ(ret._z, ret._x, ret._y);
    return ret;
}
/**
*  @fn getSolidVolumeProjectionBounds
*  @brief Project a solid volume onto a plane returning the minimum and maximum local plane bounding "quad".  In layman's terms
*    this will return a viewport for a camera that fits tightly around the given volume.
*  @remarks This is useful for creating projection volumes from just a point and a plane.
*
*  @param viewNormal - The view normal of the plane to project to
*  @param viewPos - A point on the plane (to complete the plane equation)
*  @param points - A set of geometric points to project.
*  @param nPoints - Number of points in above array.
*  @param out localMinimum - The returned local minimum value.
*  @param out localMaximum - The returned local maximum value.
*/
static FORCE_INLINE void getSolidVolumeProjectionBounds(
    const Vec3f& __in_ viewNormal,
    const Vec3f& __in_ viewPos,
    const Vec3f* __in_ points,
    const int32_t __in_ nPoints,
    float* __out_ hmin,
    float* __out_ hmax,
    float* __out_ vmin,
    float* __out_ vmax,
    Vec3Basis* __out_ v3basis=NULL
    )
{
    float dist, d;
    Vec3f pt, projectedPoint, localH, localV;
    
    d = viewNormal.dot(viewPos)*-1;

    // - Get basis vectors and export them if requested..
    getNormalBasisZ(viewNormal, localH, localV);

    if( v3basis!=NULL )
    {
        v3basis->_x = localH;
        v3basis->_y = localV;
        v3basis->_z = viewNormal;
    }

    float fMaxV = -FLT_MAX;
    int32_t iMaxV = -INT_MAX;
    float fMinV = FLT_MAX;
    int32_t iMinV = INT_MAX;
    float fMaxH = -FLT_MAX;
    int32_t iMaxH = -INT_MAX;
    float fMinH = FLT_MAX;
    int32_t iMinH = INT_MAX;

    Vec3f tmp;

    //Gather the min/max h/v distances 
    for(int ip=0; ip<nPoints; ++ip)
    {
        pt = points[ip];
        dist = viewNormal.dot(pt) + d;
        projectedPoint = (pt) - (viewNormal*dist);
        projectedPoint -= viewPos;

        // H Test
        dist = pointOnRay_t(localH,projectedPoint);
        if(dist < fMinH ) 
        {
            fMinH = dist;
            iMinH = ip;
        }
        if(dist > fMaxH ) 
        {
            fMaxH = dist;
            iMaxH = ip;
        }
        // V Test
        dist = pointOnRay_t(localV,projectedPoint);
        if(dist < fMinV ) 
        {
            fMinV = dist;
            iMinV = ip;
        }
        if(dist > fMaxV ) 
        {
            fMaxV = dist;
            iMaxV = ip;
        }
    }
    //OK so since the basis vectors are normal vectors the results from the
    // projection functions should be ACTUAL DISTANCES.
    *hmin = fMinH,
    *hmax = fMaxH;
    *vmin = fMinV;
    *vmax = fMaxV;
}
//See above comment
static FORCE_INLINE void getSolidVolumeProjectionBounds(
    const Vec3f& __in_ viewNormal,
    const Vec3f& __in_ viewPos,
    const Vec3f* __in_ points,
    const int32_t __in_ nPoints,
    Vec3f* __out_ localMinimum,
    Vec3f* __out_ localMaximum,
    Vec3Basis* __out_ v3basis_in=NULL
    )
{
    float minH, minV, maxH, maxV;
    Vec3f VminH, VminV, VmaxH, VmaxV;
    Vec3Basis v3basis;
    //const Vec3f& __in_ viewNormal,
    //const Vec3f& __in_ viewPos,
    //const Vec3f* __in_ points,
    //const int32_t __in_ nPoints,
    //float* __out_ hmin,
    //float* __out_ hmax,
    //float* __out_ vmin,
    //float* __out_ vmax,
    //Vec3Basis* __out_ v3basis=NULL
    getSolidVolumeProjectionBounds(
        viewNormal,
        viewPos,
        points,
        nPoints,
        &minH,&maxH,&minV,&maxV,
        &v3basis
    );

    VminH = v3basis._x*minH;
    VminV = v3basis._y*minV;
    VmaxH = v3basis._x*maxH;
    VmaxV = v3basis._y*maxV;

    *localMaximum = VmaxH+VmaxV;
    *localMinimum = VminH+VminV;

    if(v3basis_in!=NULL) 
        *v3basis_in = v3basis;
}
/**
*  @fn bezier2v
*  @brief Returns a point along a bezier spline in 2d
*  @param t [0,1] inclusive
*  @param controlPoints - list of 2d control points.
*/
FORCE_INLINE static Vec2f bezier2v( std::vector<Vec2f>& controlPoints, float t )
{
    Vec2f v(0.0f,0.0f);

    if( controlPoints.size() <2 )
        return v;
    
    float omt = 1.0f-t, te1, te2, dte, ni, ii, co;
    int32_t a, b, c, j, n = (int32_t)controlPoints.size()-1;
    //                [ n ]
    // - Sum[n,i=1] [ i ] * (1-t)^(n-i) * t^i * p[i]
    

    // degree = n points -1
    for(int32_t i=0; i<(int32_t)(controlPoints.size()); ++i)
    {
        a=b=c=1;

        for(j=2; j<=n; j++)
            a*=j;
        for(j=2; j<=i; j++)
            b*=j;
        for(j=2; j<=(n-i); j++)
            c*=j;

        //TODO:OPTIMIZE: if needed, we could easily cache a large number of initial bezier coefficients.
        co = (float)a/((float)b*(float)c);    // - Bezier Coefficient n! / (i! * (n-i)!)
        
        te1 = powf( (float)omt, (float)(n-i) );
        te2 = powf( (float)t, (float)i );
        dte = te1*te2;
        ni = n*dte;
        ii = i*dte;
        v.x += co * dte * controlPoints[i].x;
        v.y += co * dte * controlPoints[i].y;
    }
    return v;
}
/**
*  @fn bezier3v
*  @brief Returns a point along a bezier spline in 3d
*  @param t [0,1] inclusive
*  @param controlPoints - list of 2d control points.
*/
FORCE_INLINE static Vec3f bezier3v( std::vector<Vec3f>& controlPoints, float t )
{
    Vec3f v(0.0f,0.0f,0.0f);

    if(controlPoints.size()<2)
        return v;
    
    float omt = 1.0f-t, te1, te2, dte, ni, ii, co;
    int32_t a, b, c, j, n = (int32_t)controlPoints.size()-1;
    //                [ n ]
    // - Sum[n,i=1] [ i ] * (1-t)^(n-i) * t^i * p[i]
    

    // degree = n points -1
    for(int32_t i=0; i<(int32_t)(controlPoints.size()); ++i)
    {
        a=b=c=1;

        //compute factorials n!, i! and (n-i)!
        for(j=2; j<=n; j++)
            a*=j;
        for(j=2; j<=i; j++)
            b*=j;
        for(j=2; j<=(n-i); j++)
            c*=j;

        //TODO:OPTIMIZE: if needed, we could easily cache a large number of initial bezier coefficients.
        co = (float)a/((float)b*(float)c);    // - Bezier Coefficient n! / (i! * (n-i)!)
        
        te1 = powf( (float)omt, (float)(n-i) );
        te2 = powf( (float)t, (float)i );
        
        dte = te1*te2;

        ni = n*dte;
        ii = i*dte;

        v.x += co * dte * controlPoints[i].x;
        v.y += co * dte * controlPoints[i].y;
        v.z += co * dte * controlPoints[i].z;
    }
    return v;
}

FORCE_INLINE STATIC float calcCubeDiagonal(float cubeSide)
{
    //  sqrt( 3(x^2) )
    return MathUtils::broSqrt( (cubeSide*cubeSide)*3 );
}

};

}//ns game



#endif
