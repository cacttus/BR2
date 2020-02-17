/**
*
*  @file BoxUtils.h
*  @date April 25, 2016
*  @author MetalMario971
*
*    � 2016
*
*
*/
#pragma once
#ifndef __BOXUTILS_14616377271394746097_H__
#define __BOXUTILS_14616377271394746097_H__

#include "../math/MathHeader.h"

namespace BR2 {
//moved defs to mathheader
class BoxUtils : public VirtualMemory {
public:
  STATIC vec3 normalForSide(BoxSide::e side);
  STATIC vec3 normalUpForSide(BoxSide::e side);
  STATIC int32_t cubeSideToGlCubeMapEnum(BoxSide::e side);
};

}//ns BR2

#endif
