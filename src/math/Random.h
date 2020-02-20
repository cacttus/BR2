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
/**
*  @class Random.h
*  @brief This supplies a host of random number functions - especially ones used in noise generation.
*  @note -ALSO SEE ALGORITHM.h For better mersenne functions
*/
class Random : public VirtualMemory {
public:
  Random();
  virtual ~Random() override;

  // Random Number Generators (RNGs)
  static int32_t rnd_lh(int32_t val);
  static int32_t static_mersenne(int32_t seed);

  // Noise Functions
  static float static_1f(int32_t x);
  static float static_2f(int32_t x, int32_t y);
  static float static_3f(int32_t x, int32_t y, int32_t z);

  // - Double Prec
  static double static_3d(int32_t x, int32_t y, int32_t z);
  static int32_t nextInt32();
  static uint32_t nextUint32();
  static uint32_t nextUint32(const uint32_t& minXyz, const uint32_t& maxXyz);
  static int32_t nextInt32(const int32_t& min, const int32_t& max);
  // static int32_t nextInt(MpInt& mprange);
   //static int32_t nextInt(MpUint& mprange);
   //static int32_t nextInt(int32_t min, int32_t max);
  static float nextFloat(float min, float max);
  static float rv3();//Same as nextFloat(0,1)
  static bool nextBool();
  static void shuffle(int32_t** xarr, int32_t count, int32_t nIterations = 1);
  static int32_t sequester_i32(int32_t& __inout_ total, int32_t min, int32_t max); //  iteratively chop a whole into randomly sized parts where the parts are within the range min / max.
  static ivec3 nextIVec3(int32_t minXyz, int32_t maxXyz);
  static vec3 nextVec3(float min, float max);

  static ivec3 nextIVec3(const ivec3& minXyz, const ivec3& maxXyz);
  static vec3 nextVec3(const vec3& min, const vec3& max);
  static vec4 nextVec4(const vec4& min, const vec4& max);

  //Shortcut Functions
  static float frand01();//Same as nextFloat(0,1)
  static float frand11();//Same as nextFloat(-1,1)
  static vec3 rv301();//same as nextVec3(0,1)
  static vec3 rv311();//same as nextVec3(-1,1)

  static string_t randomNumericString(int32_t nDigits);    // - Returns a random hash code (not unique)
private:
  static int32_t _last;//Assume this is the seed?

};







}//ns BR2



#endif
