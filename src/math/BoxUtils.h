/**
*  @file BoxUtils.h
*  @date April 25, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __BOXUTILS_14616377271394746097_H__
#define __BOXUTILS_14616377271394746097_H__

#include "../math/MathHeader.h"

namespace Game {
//moved defs to mathheader
class BoxUtils : public VirtualMemory {
public:
    static vec3 normalForSide(BoxSide::e side);
    static vec3 normalUpForSide(BoxSide::e side);
    static int32_t cubeSideToGlCubeMapEnum(BoxSide::e side);
};

}//ns Game

#endif
