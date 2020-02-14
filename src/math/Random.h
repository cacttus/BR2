/**
*  @file Random.h
*  @date July 15, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __RANDOM_311671063623511130142920_H__
#define __RANDOM_311671063623511130142920_H__

#include "../base/BaseHeader.h"
#include "../base/machineTypes.h"
#include "../base/StringUtil.h"
#include "../math/Minimax.h"
#include "../math/Algorithm.h"

namespace BR2 {

//#define _USE_C_RAND

/**
*  @class Random.h
*  @brief This supplies a host of random number functions - especially ones used in noise generation.
*'    
    @note -ALSO SEE ALGORITHM.h For better mersenne functions
*/
class Random : public VirtualMemory {
    STATIC int32_t _last;//Assume this is the seed?
public:

    // Random Number Generators (RNGs)
    FORCE_INLINE STATIC int32_t rnd_lh(int32_t val);
    FORCE_INLINE STATIC int32_t static_mersenne(int32_t seed);

    // Noise Functions
    FORCE_INLINE STATIC float static_1f( int32_t x );
    FORCE_INLINE STATIC float static_2f( int32_t x, int32_t y );
    FORCE_INLINE STATIC float static_3f( int32_t x, int32_t y, int32_t z );
    
    // - Double Prec
    FORCE_INLINE STATIC double static_3d( int32_t x, int32_t y, int32_t z );
    FORCE_INLINE STATIC int32_t nextInt32();
    FORCE_INLINE STATIC uint32_t nextUint32();
    FORCE_INLINE STATIC uint32_t nextUint32(const uint32_t& minXyz, const uint32_t& maxXyz);
    FORCE_INLINE STATIC int32_t nextInt32(const int32_t& min, const int32_t& max);
   // FORCE_INLINE STATIC int32_t nextInt(MpInt& mprange);
    //FORCE_INLINE STATIC int32_t nextInt(MpUint& mprange);
    //FORCE_INLINE STATIC int32_t nextInt(int32_t min, int32_t max);
    FORCE_INLINE STATIC float nextFloat(float min, float max);
    FORCE_INLINE STATIC float rv3();//Same as nextFloat(0,1)
    FORCE_INLINE STATIC bool nextBool();
    FORCE_INLINE STATIC void shuffle(int32_t** xarr, int32_t count, int32_t nIterations = 1);
    FORCE_INLINE STATIC int32_t sequester_i32(int32_t& __inout_ total, int32_t min, int32_t max); //  iteratively chop a whole into randomly sized parts where the parts are within the range min / max.
    FORCE_INLINE STATIC ivec3 nextIVec3(int32_t minXyz, int32_t maxXyz);
    FORCE_INLINE STATIC vec3 nextVec3(float min, float max);

    FORCE_INLINE STATIC ivec3 nextIVec3(const ivec3& minXyz, const ivec3& maxXyz);
    FORCE_INLINE STATIC vec3 nextVec3(const vec3& min, const vec3& max);
    FORCE_INLINE STATIC vec4 nextVec4(const vec4& min, const vec4& max);


    //Shortcut Functions
    FORCE_INLINE STATIC float frand01();//Same as nextFloat(0,1)
    FORCE_INLINE STATIC float frand11();//Same as nextFloat(-1,1)
    FORCE_INLINE STATIC vec3 rv301();//same as nextVec3(0,1)
    FORCE_INLINE STATIC vec3 rv311();//same as nextVec3(-1,1)

    STATIC string_t randomNumericString(int32_t nDigits);    // - Returns a random hash code (not unique)

    FORCE_INLINE Random();
    FORCE_INLINE virtual ~Random() override ;
};
FORCE_INLINE Random::Random()
{
#ifdef _USE_C_RAND
    srand((unsigned int)time(NULL));
#else
    _last = (int32_t)time(NULL);
#endif
}
FORCE_INLINE Random::~Random()
{
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/**
*  @fn
*  @brief chop a number up into random parts where each part is within the range [min,max]
*    and also subtract from that number.
*/
FORCE_INLINE int32_t Random::sequester_i32(int32_t& __inout_ total, int32_t min, int32_t max)
{
    int32_t val;
    if(total >= 2*max)
    {
        val = Random::nextInt32(min,max);
    }
    else 
    {
        int32_t newMax = total - min;
        if( newMax <= 0 )
        {
            val = min; 
        }
        else
        {
            val = Random::nextInt32(min,newMax);
        }
    }

    total -= val;
    return val;
}

FORCE_INLINE int32_t Random::rnd_lh(int32_t val)
{
    int32_t ret;
    ret =((uint64_t)14*val) % 15;
    //last=val;
    return ret;
}

/**
*  @fn static_mersenne
*  @brief This mersenne function is based on a mersenne function
*    with an index of 1.  The code source is from Wikipedia.
*/
FORCE_INLINE int32_t Random::static_mersenne(int32_t seed)
{
     int32_t i=1,y;
     
     y = (0x6c078965 * (seed ^ (seed>>30))+i) & 0xffffffff;
     y = y ^ (y>>11);
     y = y ^ ((y<<7) & 0x9d2c5680);
     y = y ^ ((y<<15) & 0xefc60000);
     y = y ^ (y>>18);

    return y;
}
FORCE_INLINE float Random::static_1f( int32_t x ) 
{
    //This is a neato way to get the fractional part of a float.
    int32_t a = (static_mersenne(x)&0x007fffff) | 0x40000000;
    return ( *((float*)&a) - 3.0f );
}
FORCE_INLINE float Random::static_2f( int32_t x, int32_t y ) 
{
    //This is a neato way to get the fractional part of a float.
    int32_t a = ((static_mersenne(x)+static_mersenne(y))&0x007fffff) | 0x40000000;
    return( *((float*)&a) - 3.0f );
}
FORCE_INLINE float Random::static_3f( int32_t x, int32_t y, int32_t z ) 
{
    //float f = (static_1(x) + static_1(y) + static_1(z)) / 3.0f;
    //return f;
    int32_t x1 = static_mersenne(x);
    int32_t y1 = static_mersenne(y);
    int32_t z1 = static_mersenne(z);

    int32_t aa = static_mersenne(x1^z1^y1);

    // - Returns the floating point part of the sum of 3 RNGs
    int32_t a = ( (aa) &0x007fffff) | 0x40000000;

    return( *((float*)&a) - 3.0f );
   // unsigned int a = ((static_mersenne(x)+static_mersenne(y)+static_mersenne(z))&0x007fffff) | 0x40000000;
   // return( *((float*)&a) - 3.0f );
}
FORCE_INLINE double Random::static_3d( int32_t x, int32_t y, int32_t z ) 
{
    //float f = (static_1(x) + static_1(y) + static_1(z)) / 3.0f;
    //return f;
    int32_t x1 = static_mersenne(x);
    int32_t y1 = static_mersenne(y);
    int32_t z1 = static_mersenne(z);

    int32_t aa = (x1+z1+y1);

    // - Returns the floating point part of the sum of 3 RNGs
    int32_t a = ( (aa) &0x007fffff) | 0x40000000;

    return( *((double*)&a) - 3.0f );
   // unsigned int a = ((static_mersenne(x)+static_mersenne(y)+static_mersenne(z))&0x007fffff) | 0x40000000;
   // return( *((float*)&a) - 3.0f );
}

FORCE_INLINE bool Random::nextBool()
{
    return (nextInt32()%2==0);//even/odd=bool
}
FORCE_INLINE int32_t Random::nextInt32()
{
#ifdef _USE_C_RAND
    return rand();
#else
    return (_last = Alg::mint(_last));
#endif
}
//FORCE_INLINE int32_t Random::nextInt(MpInt& range)
//{
//    return nextInt(range._min, range._max);
//}
//FORCE_INLINE int32_t Random::nextInt(MpUint& range)
//{
//    return nextInt((int32_t)range._min, (int32_t)range._max);
//}
//FORCE_INLINE int32_t Random::nextInt(int32_t min, int32_t max)
//{
//    return nextInt32(min, max);
//}
FORCE_INLINE float Random::nextFloat(float min, float max){
#ifdef _USE_C_RAND
    float r = (float)nextInt() / RAND_MAX;//[0,1]
    return (r * (max-min) + min);
#else
    float ret = Alg::meft01(nextInt32());
    ret *= (max-min);
    ret += min;
    return ret;
#endif
}
FORCE_INLINE float Random::frand01() {
    return nextFloat(0.0f, 1.0f);
}
FORCE_INLINE float Random::frand11() {
    return nextFloat(-1.0f, 1.0f);
}
// Randomly shuffle pointers
// This allows us to simply iterate over the exhastive list of possible movements
// until we find one that works.             
FORCE_INLINE void Random::shuffle(int32_t** xarr, int32_t count, int32_t nIterations)
{
    int32_t shval;
    for(int niter=0; niter<nIterations; ++niter)
    {
        for(int ni=0; ni<count; ++ni)
        {
            shval = Random::nextInt32(0, count-1);
            int32_t* tmp = xarr[shval];
            xarr[shval] = xarr[ni];
            xarr[ni] = tmp;
        }
    }
}

FORCE_INLINE ivec3 Random::nextIVec3(int32_t minXyz, int32_t maxXyz)
{
    return ivec3(
        Random::nextInt32(minXyz,maxXyz),
        Random::nextInt32(minXyz,maxXyz),
        Random::nextInt32(minXyz,maxXyz)
        );
}
FORCE_INLINE vec3 Random::nextVec3(float  minXyz, float  maxXyz)
{
    return vec3(
        Random::nextFloat(minXyz, maxXyz),
        Random::nextFloat(minXyz, maxXyz),
        Random::nextFloat(minXyz, maxXyz)
    );
}
FORCE_INLINE vec3 Random::rv301()
{
    return vec3(
        Random::nextFloat(0.0f, 1.0f),
        Random::nextFloat(0.0f, 1.0f),
        Random::nextFloat(0.0f, 1.0f)
    );
}

FORCE_INLINE vec3 Random::rv311()
{
    return vec3(
        Random::nextFloat(-1.0f, 1.0f),
        Random::nextFloat(-1.0f, 1.0f),
        Random::nextFloat(-1.0f, 1.0f)
    );
}
FORCE_INLINE ivec3 Random::nextIVec3(const ivec3& minXyz, const ivec3& maxXyz) {
    return ivec3(
        Random::nextInt32(minXyz.x, maxXyz.x),
        Random::nextInt32(minXyz.y, maxXyz.y),
        Random::nextInt32(minXyz.z, maxXyz.z)
    );
}
FORCE_INLINE vec3 Random::nextVec3(const vec3& minXyz, const vec3& maxXyz) {
    return vec3(
        Random::nextFloat(minXyz.x, maxXyz.x),
        Random::nextFloat(minXyz.y, maxXyz.y),
        Random::nextFloat(minXyz.z, maxXyz.z)
    );
}
FORCE_INLINE vec4 Random::nextVec4(const vec4& minXyz, const vec4& maxXyz) {
    return vec4(
        Random::nextFloat(minXyz.x, maxXyz.x),
        Random::nextFloat(minXyz.y, maxXyz.y),
        Random::nextFloat(minXyz.z, maxXyz.z),
        Random::nextFloat(minXyz.w, maxXyz.w)
    );
}
FORCE_INLINE uint32_t Random::nextUint32(const uint32_t& min, const uint32_t& max) {
    AssertOrThrow2(max >= min);
    return min + (((uint32_t)Random::nextInt32()) % (max - min + 1));
}
FORCE_INLINE int32_t Random::nextInt32(const int32_t& min, const int32_t& max) {
    AssertOrThrow2(max >= min);
    return min + (Random::nextInt32() % (max - min + 1));
}









}//ns game



#endif
