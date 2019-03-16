/**
*
*    @file oglErr.h
*    @date November 22, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __OGLERR_1479868789525442042_H__
#define __OGLERR_1479868789525442042_H__

#include "../base/BaseHeader.h"

namespace Game {
/**
*    @class oglErr
*    @brief
*
*/
class OglErr  {
    static t_string glDebugGetErrorSource(int eCode);
    static t_string glDebugGetMessageType(int eCode);
    static t_string glDebugGetMessageSeverity(int eCode);
    static void printAndFlushGpuLog(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak);
    static t_string glErrToStr(GLenum err);
    static bool handleErrors(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak);
    static bool checkOglErr(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak);

public:
     static bool chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false);
     static bool chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false);
     static void checkSDLErr();
};

}//ns Game



#endif
