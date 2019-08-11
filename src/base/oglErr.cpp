

#include "../base/BaseHeader.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/DebugHelper.h"
#include "../base/EngineConfig.h"
#include "../base/SDL.h"

namespace Game {
bool OglErr::chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog){
    if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking() == true) {
        return handleErrors(ctx, true, bDoNotBreak, doNotLog);
    }
    return false;
}
bool OglErr::chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog){
    if (Gu::getEngineConfig()->getEnableDebugErrorChecking() == true) {
        return handleErrors(ctx, true, bDoNotBreak, doNotLog);
    }
    return false;
}

t_string OglErr::glErrToStr(GLenum err) {
    switch(err) { 
		case 0     : return  "GL_NO_ERROR         ";      		
		case 0x0500: return  "GL_INVALID_ENUM     ";  		
		case 0x0501: return  "GL_INVALID_VALUE    ";   	
		case 0x0502: return  "GL_INVALID_OPERATION";       
		case 0x0503: return  "GL_STACK_OVERFLOW   ";    	
		case 0x0504: return  "GL_STACK_UNDERFLOW  ";     	
		case 0x0505: return  "GL_OUT_OF_MEMORY    ";   
    }
    return " *GL Error code not recognized.";
}
bool OglErr::handleErrors(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog){

    SDL::checkSDLErr(doNotLog);

    printAndFlushGpuLog(ctx, true, bDoNotBreak, doNotLog);

    return checkOglErr(ctx, bShowNote, bDoNotBreak, doNotLog);
}
bool OglErr::checkOglErr(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog){
    bool bError = false;

    //GPU Log - 
    // This isn't the Application log it's the hardware log on the card.
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        if (doNotLog==false) {
            BroLogError("GL Error: " + glErrToStr(err) + " (" + (int)err + ")");
        }

        if (Gu::getEngineConfig()->getBreakOnOpenGLError() == true) {
            if (bDoNotBreak == false) {
                Gu::debugBreak();
            }
        }
        bError = true;
    }

    return bError;
}
void OglErr::printAndFlushGpuLog(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog) {

    //Enable this in engine.cpp
 //   glEnable(GL_DEBUG_OUTPUT);
    if(ctx == nullptr){
        BroLogWarn("Context not initialized (context isseu");
        return;
    }
    if (!ctx->glGetDebugMessageLog)
    {
        BroLogWarn("Opengl log not initialized (context isseu");
        return;
    }

    GLuint numMsgs = 64;
    GLuint numFound;

    static GLint maxMsgLen = -1;
    if(maxMsgLen == -1){
        //avoid doing this every call.
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);
    }

    if(maxMsgLen <= 0) {
        BroLogError("GL_MAX_DEBUG_MESSAGE_LENGTH returned 0.");
        maxMsgLen = -2;
        return ;
    }

    do
    {
        std::vector<GLchar> msgData(numMsgs * maxMsgLen);
        std::vector<GLenum> sources(numMsgs);
        std::vector<GLenum> types(numMsgs);
        std::vector<GLenum> severities(numMsgs);
        std::vector<GLuint> ids(numMsgs);
        std::vector<GLsizei> lengths(numMsgs);

        numFound = ctx->glGetDebugMessageLog(
            numMsgs
            , (GLsizei)msgData.size()
            , &sources[0]
            , &types[0]
            , &ids[0]
            , &severities[0]
            , &lengths[0]
            , &msgData[0]
        );
        if (numFound == 0) {
            return ;
        }


        sources.resize(numFound);
        types.resize(numFound);
        severities.resize(numFound);
        ids.resize(numFound);
        lengths.resize(numFound);

        std::vector<t_string> messages;
        messages.reserve(numFound);

        std::vector<GLchar>::iterator currPos = msgData.begin();
        for (size_t iMsg = 0; iMsg < lengths.size(); ++iMsg)
        {
            int id = ids[iMsg];
            //GTX670 Driver
            //NVidia - invalid messages / infos
            if (id == 0x00020071){ return ; } // memory usage
            else if (id == 0x00020084) { return; } // Texture state usage warning: Texture 0 is base level inconsistent. Check texture size.
            else if (id == 0x00020061) { return; } // Framebuffer detailed info: The driver allocated storage for renderbuffer 1.
            else if (id == 0x00020004) { return; } // Usage warning: Generic vertex attribute array ... uses a pointer with a small value (...). Is this intended to be used as an offset into a buffer object?
            else if (id == 0x00020072) { return; } // Buffer performance warning: Buffer object ... (bound to ..., usage hint is GL_STATIC_DRAW) is being copied/moved from VIDEO memory to HOST memory.
            else if (id == 0x00020074) { return; } // Buffer usage warning: Analysis of buffer object ... (bound to ...) usage indicates that the GPU is the primary producer and consumer of data for this buffer object.  The usage hint s upplied with this buffer object, GL_STATIC_DRAW, is inconsistent with this usage pattern.  Try using GL_STREAM_COPY_ARB, GL_STATIC_COPY_ARB, or GL_DYNAMIC_COPY_ARB instead.
            else if (id == 0x00020070) { return; } // Total VBO Usage in the system... (Useful information)
            else if (id == 0x00020043) { return; } // A non-Fullscreen clear caused a fallback from CSAA to MSAA; - probolem in clearing cube shadow buffers
            //Other (mom's house) driver
            else if (id == 0x07){ return ; }// glLineWidth Deprecated (other driver)


            t_string strMsg = std::string(currPos, currPos + lengths[iMsg] - 1);
            t_string strSrc = glDebugGetErrorSource(sources[iMsg]);
            t_string strType = glDebugGetMessageType(types[iMsg]);
            t_string strSev = glDebugGetMessageSeverity(severities[iMsg]);

            if (doNotLog == false) {
                strMsg = "GPU LOG\r\n ID: "+ StringUtil::toHex(id, true)+ "\r\n Msg: "+ strMsg;

                GLenum severity = severities[iMsg];
                GLenum type = types[iMsg];
                if (type == GL_DEBUG_TYPE_ERROR)
                {
                    t_string _strStackInfo = DebugHelper::getStackTrace();
                    BroLogError(strMsg+ "\r\n" + _strStackInfo);
                }
                else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
                {
                    t_string _strStackInfo = DebugHelper::getStackTrace();
                    BroLogInfo(strMsg + "\r\n" + _strStackInfo);
                }
                else
                {
                    t_string _strStackInfo = DebugHelper::getStackTrace();
                    BroLogWarn(strMsg + "\r\n" + _strStackInfo);
                }
            }

            currPos = currPos + lengths[iMsg];
        }
    } while (numFound > 0);
}
t_string OglErr::glDebugGetErrorSource(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_SOURCE_API: return " SOURCE API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return (" WINDOW SYSTEM"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return (" SHADER COMPILER"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: return (" THIRD PARTY"); break;
    case GL_DEBUG_SOURCE_APPLICATION: return (" APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER: return (" OTHER"); break;
    }
    return ("*No Enum*");
}
t_string OglErr::glDebugGetMessageType(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_TYPE_ERROR: return (" ERROR"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return (" DEPRECATED"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return (" UNDEFINED BEHAVIOR"); break;
    case GL_DEBUG_TYPE_PORTABILITY: return (" PORTABILITY"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: return (" PERFORMANCE"); break;
    case GL_DEBUG_TYPE_MARKER: return (" MARKER"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: return (" PUSH GRP"); break;
    case GL_DEBUG_TYPE_POP_GROUP: return (" POP GRP"); break;
    case GL_DEBUG_TYPE_OTHER: return (" OTHER"); break;
    }
    return ("*No Enum*");
}
t_string OglErr::glDebugGetMessageSeverity(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_SEVERITY_HIGH: return (" HIGH"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: return (" MEDIUM"); break;
    case GL_DEBUG_SEVERITY_LOW: return (" LOW"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: return (" NOTE"); break;
    }
    return ("*No Enum*");
}





}//ns Game
