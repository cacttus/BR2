/**
*
*    @file Perf.h
*    @date August 23, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __PERF_15665765533990554672_H__
#define __PERF_15665765533990554672_H__

#include "../base/BaseHeader.h"
#include "../base/Stopwatch.h"

namespace Game {
/**
*    @class Perf
*    @brief
*
*/
class Perf {
    static t_string _strCachedProf;
    static std::stack<Stopwatch> _stopw;
    static void pulsePerf();
public:
    static void beginPerf();
    static void endPerf();
    static void pushPerf();
    static void popPerf();

};

}//ns Game



#endif
