/**
*  @file oglErr.h
*  @date November 22, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __OGLERR_1479868789525442042_H__
#define __OGLERR_1479868789525442042_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class oglErr
*  @brief Handles OpenGL Errors.
*/
class OglErr {
public:
  static bool chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false, bool doNotLog = true);
  static bool chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false, bool doNotLog = true);
};

}//ns Game



#endif
