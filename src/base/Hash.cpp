#include "../base/Hash.h"

#include "../math/Algorithm.h"
#include "../base/Gu.h"

namespace Game {
;

//Prime Numbers
//Gneeratoer: https://asecuritysite.com/encryption/random3
const uint32_t Hash::Fnv_Basis_32[FNV_MAX_ALGORITHMS_32] = {
     2166136261
   , 50331653
   , 3331805713
   , 2324833853
};
const uint32_t Hash::Fnv_Prime_32[FNV_MAX_ALGORITHMS_32] = {
     16777619
   , 805306457
   , 124675729
   , 417013901
};
const uint64_t Hash::Fnv_Basis_64[FNV_MAX_ALGORITHMS_64] = {
      16540412399126526479
    , 1604091723691372229
    , 1729532795367666019
    , 15711276102652330523
};
const uint64_t Hash::Fnv_Prime_64[FNV_MAX_ALGORITHMS_64] = {
      17577211953059058149
    , 4401858765275463419
    , 8868681736133192851
    , 6500018318822434283
};

Hash::Hash() {
}
Hash::~Hash() {
}

Hash32 Hash::computeStringHash32bit(const t_string& str, uint32_t iAlgorithmIndex)
{
    AssertOrThrow2(iAlgorithmIndex < FNV_MAX_ALGORITHMS_32);

    uint32_t n = Fnv_Basis_32[iAlgorithmIndex];
    for (size_t i = 0; i < str.size(); ++i)
    {
        n *= Fnv_Prime_32[iAlgorithmIndex];
        n ^= (uint32_t)str[i];
    }
    return (Hash32)n;
}
uint64_t Hash::computeStringHash64bit(const t_string& str, uint64_t iAlgorithmIndex)
{
    AssertOrThrow2(iAlgorithmIndex < FNV_MAX_ALGORITHMS_64);

    uint64_t n = Fnv_Basis_64[iAlgorithmIndex];;
    for (size_t i = 0; i < str.size(); ++i)
    {
        n *= Fnv_Prime_64[iAlgorithmIndex];
        n ^= (uint64_t)str[i];
    }
    return n;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

EasyNoise::EasyNoise(int iSeed) : _iSeed(iSeed) {
    //Modified the permutation table from the article to use a custom table based on a random seed.
    for (int x = 0; x < 512; x++) {
        p[x] = Alg::mint(_iSeed + x) % 256;
    }
}
float EasyNoise::perlin(float x, float y, float z) {
    int xi = (int)x & 255;                              // Calculate the "unit cube" that the point asked will be located in
    int yi = (int)y & 255;                              // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = (int)z & 255;                              // plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
    float xf = x - (int)x;
    float yf = y - (int)y;
    float zf = z - (int)z;
    float u = Gu::fade(fabsf(xf));
    float v = Gu::fade(fabsf(yf));
    float w = Gu::fade(fabsf(zf));

#define inc(x) ((x)+1)

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = p[p[p[xi] + yi] + zi];
    aba = p[p[p[xi] + inc(yi)] + zi];
    aab = p[p[p[xi] + yi] + inc(zi)];
    abb = p[p[p[xi] + inc(yi)] + inc(zi)];
    baa = p[p[p[inc(xi)] + yi] + zi];
    bba = p[p[p[inc(xi)] + inc(yi)] + zi];
    bab = p[p[p[inc(xi)] + yi] + inc(zi)];
    bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

    float x1, x2, y1, y2;
    x1 = lerp(grad(aaa, xf, yf, zf),           // The gradient function calculates the dot product between a pseudorandom
        grad(baa, xf - 1, yf, zf),             // gradient vector and the vector from the input coordinate to the 8
        u);                                     // surrounding points in its unit cube.
    x2 = lerp(grad(aba, xf, yf - 1, zf),           // This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
        grad(bba, xf - 1, yf - 1, zf),             // values we made earlier.
        u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1),
        grad(bab, xf - 1, yf, zf - 1),
        u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
        grad(bbb, xf - 1, yf - 1, zf - 1),
        u);
    y2 = lerp(x1, x2, v);

    return (lerp(y1, y2, w) + 1) / 2;
}
float EasyNoise::grad(int hash, float x, float y, float z)
{
    switch (hash & 0xF)
    {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
    }
}
float EasyNoise::terrain(float x, float y, float z, int octaves, float persistence, float scale) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    float sx = x * scale, sy = y * scale, sz = z * scale;

    for (int i = 0; i < octaves; i++) {
        total += perlin(sx * frequency, sy * frequency, sz * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}







}//ns game
