

#include "../base/BaseHeader.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/DebugHelper.h"

namespace Game {
bool OglErr::chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak){
    if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking() == true) {
        return handleErrors(ctx, true, bDoNotBreak);
    }
    return false;
}
bool OglErr::chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak){
    if (Gu::getEngineConfig()->getEnableDebugErrorChecking() == true) {
        return handleErrors(ctx, true, bDoNotBreak);
    }
    return false;
}
void OglErr::checkSDLErr() {
    //Do SDL errors here as well
    const char* c;
    while ((c = SDL_GetError()) != nullptr && *c != 0) {
        BroLogError("SDL: ", c);
        if(Gu::getEngineConfig()->getBreakOnSDLError() == true) { 
            Gu::debugBreak();
        }
        SDL_ClearError();
    }

}
t_string OglErr::glErrToStr(GLenum err) {
    switch(err) { 
		case 0     : return TStr( "GL_NO_ERROR         ");      		
		case 0x0500: return TStr( "GL_INVALID_ENUM     ");  		
		case 0x0501: return TStr( "GL_INVALID_VALUE    ");   	
		case 0x0502: return TStr( "GL_INVALID_OPERATION");       
		case 0x0503: return TStr( "GL_STACK_OVERFLOW   ");    	
		case 0x0504: return TStr( "GL_STACK_UNDERFLOW  ");     	
		case 0x0505: return TStr( "GL_OUT_OF_MEMORY    ");   
    }
    return TStr(" *GL Error code not recognized.");
}
bool OglErr::handleErrors(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak){

    checkSDLErr();

    printAndFlushGpuLog(ctx, true, bDoNotBreak);

    return checkOglErr(ctx, bShowNote, bDoNotBreak);
}
bool OglErr::checkOglErr(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak){
    bool bError = false;

    //GPU Log - 
    // This isn't the Application log it's the hardware log on the card.
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        BroLogError("GL Error: ", glErrToStr(err), " (", (int)err, ")");
        if (Gu::getEngineConfig()->getBreakOnOpenGLError() == true) {
            if (bDoNotBreak == false) {
                Gu::debugBreak();
            }
        }
        bError = true;
    }

    return bError;
}
void OglErr::printAndFlushGpuLog(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak) {

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

            strMsg = TStr("GPU LOG\r\n ID: ", StringUtil::toHex(id, true), "\r\n Msg: ", strMsg);

            GLenum severity = severities[iMsg];
            GLenum type = types[iMsg];
            if (type == GL_DEBUG_TYPE_ERROR)
            {
                t_string _strStackInfo = DebugHelper::getStackTrace();
                BroLogError(strMsg, "\r\n", _strStackInfo);
            }
            else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
            {
                t_string _strStackInfo = DebugHelper::getStackTrace();
                BroLogInfo(strMsg, "\r\n", _strStackInfo);
            }
            else
            {
                t_string _strStackInfo = DebugHelper::getStackTrace();
                BroLogWarn(strMsg, "\r\n", _strStackInfo);
            }

            currPos = currPos + lengths[iMsg];
        }
    } while (numFound > 0);
}
t_string OglErr::glDebugGetErrorSource(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_SOURCE_API: return TStr(" SOURCE API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return TStr(" WINDOW SYSTEM"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return TStr(" SHADER COMPILER"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: return TStr(" THIRD PARTY"); break;
    case GL_DEBUG_SOURCE_APPLICATION: return TStr(" APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER: return TStr(" OTHER"); break;
    }
    return TStr("*No Enum*");
}
t_string OglErr::glDebugGetMessageType(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_TYPE_ERROR: return TStr(" ERROR"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return TStr(" DEPRECATED"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return TStr(" UNDEFINED BEHAVIOR"); break;
    case GL_DEBUG_TYPE_PORTABILITY: return TStr(" PORTABILITY"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: return TStr(" PERFORMANCE"); break;
    case GL_DEBUG_TYPE_MARKER: return TStr(" MARKER"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: return TStr(" PUSH GRP"); break;
    case GL_DEBUG_TYPE_POP_GROUP: return TStr(" POP GRP"); break;
    case GL_DEBUG_TYPE_OTHER: return TStr(" OTHER"); break;
    }
    return TStr("*No Enum*");
}
t_string OglErr::glDebugGetMessageSeverity(int eCode) {
    switch (eCode)
    {
    case GL_DEBUG_SEVERITY_HIGH: return TStr(" HIGH"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: return TStr(" MEDIUM"); break;
    case GL_DEBUG_SEVERITY_LOW: return TStr(" LOW"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: return TStr(" NOTE"); break;
    }
    return TStr("*No Enum*");
}





}//ns Game
