/**
*  @file Perf.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __PERF_15665765533990554672_H__
#define __PERF_15665765533990554672_H__

#include "../base/BaseHeader.h"
#include "../base/Stopwatch.h"

namespace BR2 {
class PerfInternal;
/**
*  @class Perf
*  @brief Performance profiling utility class.
*/
class Perf {
public:
  static void beginPerf();
  static void endPerf();
  static void pushPerf();
  static void popPerf();
private:
  static std::shared_ptr<PerfInternal> _internal;
};

}//ns BR2



#endif
