/**
*  @file Hash.h
*  @date February 13, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __HASH_170808485161582608310807_H__
#define __HASH_170808485161582608310807_H__

#include "../base/BaseHeader.h"
#include "../math/MathAll.h"

namespace BR2 {

/**
*  @class Hash
*  @brief
*
*/
class Hash : public VirtualMemory {
public:

    // - FNV Algorithm powers and basis
    #define FNV_PRIME32 (16777619)
    #define FNV_PRIME64 (1099511628211)
    #define FNV_BASIS32 (2166136261)
    #define FNV_BASIS64 (14695981039346656037U)
    #define FNV_BASIS128 (144066263297769815596495629667062367629U)
    #define FNV_PRIME128 (309485009821345068724781371)

    #define FNV_MAX_ALGORITHMS_32 (4)
    #define FNV_MAX_ALGORITHMS_64 (4)

    Hash();
    virtual ~Hash() override ;

    STATIC const uint32_t Fnv_Basis_32[FNV_MAX_ALGORITHMS_32];
    STATIC const uint32_t Fnv_Prime_32[FNV_MAX_ALGORITHMS_32];
    STATIC const uint64_t Fnv_Basis_64[FNV_MAX_ALGORITHMS_64];
    STATIC const uint64_t Fnv_Prime_64[FNV_MAX_ALGORITHMS_64];

    //Fnv Algorithm and an awesome website
    //http://isthe.com/chongo/tech/comp/fnv/#FNV-1a
    STATIC Hash32 computeStringHash32bit(const string_t& str, uint32_t iAlgorithmIndex=0);
    STATIC uint64_t computeStringHash64bit(const string_t& str, uint64_t iAlgorithmIndex=0);

    
};

#define STRHASH(x) Hash::computeStringHash32bit(x , 0)

//http://flafla2.github.io/2014/08/09/perlinnoise.html
class EasyNoise : public VirtualMemory {

    int _iSeed = 0;
    int p[512];

    float grad(int hash, float x, float y, float z);
    float lerp(float a, float b, float x) { return a + x * (b - a); }
public:
    float perlin(float x, float y, float z);
    float terrain(float x, float y, float z, int octaves, float persistence, float scale);
    EasyNoise(int iSeed);
    virtual ~EasyNoise() { 
    
    }
};






}//ns BR2



#endif
