#include "../base/GlobalIncludes.h"
#include "../base/BaseHeader.h"
#include "../base/TypeConv.h"

#include "../base/StringUtil.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/Fingers.h"
#include "../base/Sequencer.h"

#include "../base/FrameSync.h"
#include "../base/SoundCache.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/AppBase.h"
#include "../base/Package.h"

#include "../math/MathAll.h"

#include "../gfx/RenderUtils.h"
#include "../gfx/Party.h"
#include "../gfx/TexCache.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/LightManager.h"
#include "../gfx/Picker.h"
#include "../gfx/Gui2d.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderSettings.h"

#include "../model/VertexFormat.h"
#include "../model/ModelCache.h"

namespace Game {

GLContext::GLContext() {
    //Do not init here.

}

GLContext::~GLContext() {

}
bool GLContext::load(std::shared_ptr<AppBase> rb) {
    bool valid = true;
    valid = valid && GraphicsContext::load(rb);
    if (!valid) {
        return false;
    }
    valid = valid && loadOpenGLFunctions();

    return valid;
}

bool GLContext::chkErrRt(bool bDoNotBreak, bool doNotLog) {
    //Enable runtime errors.
    return OglErr::chkErrRt(shared_from_this(), bDoNotBreak, doNotLog);
}
bool GLContext::chkErrDbg(bool bDoNotBreak, bool doNotLog) {
    return OglErr::chkErrDbg(shared_from_this(), bDoNotBreak, doNotLog);
}
bool GLContext::loadOpenGLFunctions() {
    bool bValid = true;

#define SDLGLP(x, y, z) do{ \
if( !(x = (y)SDL_GL_GetProcAddress(z))) \
{ \
BroLogError("GL Method "+ z+" not supported by your GPU, or there was another error somewhere.."); \
bValid = false; \
Gu::debugBreak(); \
} \
} while(0)

#define SDLGLP2(x, y) do{ \
if( !(y = (x)SDL_GL_GetProcAddress(#y))) \
{ \
BroLogError("GL Method "+ #y+ " not supported by your GPU, or there was another error somewhere.."); \
bValid = false; \
Gu::debugBreak(); \
} \
} while(0)

    SDLGLP(glUseProgram, PFNGLUSEPROGRAMPROC, "glUseProgram");
    SDLGLP(glBindBuffer, PFNGLBINDBUFFERPROC, "glBindBuffer");
    SDLGLP(glGenBuffers, PFNGLGENBUFFERSPROC, "glGenBuffers");
    SDLGLP(glBufferData, PFNGLBUFFERDATAPROC, "glBufferData");
    SDLGLP(glBufferSubData, PFNGLBUFFERSUBDATAPROC, "glBufferSubData");
    SDLGLP(glShaderSource, PFNGLSHADERSOURCEPROC, "glShaderSource");
    SDLGLP(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC, "glGetShaderInfoLog");
    SDLGLP(glCreateProgram, PFNGLCREATEPROGRAMPROC, "glCreateProgram");
    SDLGLP(glAttachShader, PFNGLATTACHSHADERPROC, "glAttachShader");
    SDLGLP(glLinkProgram, PFNGLLINKPROGRAMPROC, "glLinkProgram");
    SDLGLP(glDetachShader, PFNGLDETACHSHADERPROC, "glDetachShader");
    SDLGLP(glDeleteShader, PFNGLDELETESHADERPROC, "glDeleteShader");
    SDLGLP(glCompileShader, PFNGLCOMPILESHADERPROC, "glCompileShader");
    SDLGLP(glCreateShader, PFNGLCREATESHADERPROC, "glCreateShader");
    SDLGLP(glDeleteProgram, PFNGLDELETEPROGRAMPROC, "glDeleteProgram");

    SDLGLP(glGetProgramiv, PFNGLGETPROGRAMIVPROC, "glGetProgramiv");
    SDLGLP(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC, "glGetProgramInfoLog");
    SDLGLP(glGetShaderiv, PFNGLGETSHADERIVPROC, "glGetShaderiv");

    SDLGLP(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC, "glEnableVertexAttribArray");
    SDLGLP(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC, "glVertexAttribPointer");
    SDLGLP(glVertexAttribIPointer, PFNGLVERTEXATTRIBIPOINTERPROC, "glVertexAttribIPointer");
    SDLGLP(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC, "glDisableVertexAttribArray");

    SDLGLP(glActiveTexture, PFNGLACTIVETEXTUREPROC, "glActiveTexture");

    SDLGLP(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC, "glGetUniformLocation");
    SDLGLP(glGetDebugMessageLog, PFNGLGETDEBUGMESSAGELOGPROC, "glGetDebugMessageLog");

    SDLGLP(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC, "glGenVertexArrays");
    SDLGLP(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC, "glBindVertexArray");
    SDLGLP(glDeleteBuffers, PFNGLDELETEBUFFERSPROC, "glDeleteBuffers");
    SDLGLP(glIsBuffer, PFNGLISBUFFERPROC, "glIsBuffer");

    SDLGLP(glMapBuffer, PFNGLMAPBUFFERPROC, "glMapBuffer");
    SDLGLP(glUnmapBuffer, PFNGLUNMAPBUFFERPROC, "glUnmapBuffer");
    SDLGLP(glGetBufferParameteriv, PFNGLGETBUFFERPARAMETERIVPROC, "glGetBufferParameteriv");
    SDLGLP(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC, "glDeleteVertexArrays");

    //** Uniforms
    SDLGLP(glUniformMatrix2fv, PFNGLUNIFORMMATRIX2FVPROC, "glUniformMatrix2fv");
    SDLGLP(glUniformMatrix3fv, PFNGLUNIFORMMATRIX3FVPROC, "glUniformMatrix3fv");
    SDLGLP(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC, "glUniformMatrix4fv");

    SDLGLP(glUniform1i, PFNGLUNIFORM1IPROC, "glUniform1i");


    SDLGLP(glUniform1iv, PFNGLUNIFORM1IVPROC, "glUniform1iv");
    SDLGLP(glUniform2iv, PFNGLUNIFORM2IVPROC, "glUniform2iv");
    SDLGLP(glUniform3iv, PFNGLUNIFORM3IVPROC, "glUniform3iv");
    SDLGLP(glUniform4iv, PFNGLUNIFORM4IVPROC, "glUniform4iv");

    SDLGLP(glUniform1fv, PFNGLUNIFORM1FVPROC, "glUniform1fv");
    SDLGLP(glUniform2fv, PFNGLUNIFORM2FVPROC, "glUniform2fv");
    SDLGLP(glUniform3fv, PFNGLUNIFORM3FVPROC, "glUniform3fv");
    SDLGLP(glUniform4fv, PFNGLUNIFORM4FVPROC, "glUniform4fv");

    SDLGLP(glUniform1uiv, PFNGLUNIFORM1UIVPROC, "glUniform1uiv");
    SDLGLP(glUniform2uiv, PFNGLUNIFORM2UIVPROC, "glUniform2uiv");
    SDLGLP(glUniform3uiv, PFNGLUNIFORM3UIVPROC, "glUniform3uiv");
    SDLGLP(glUniform4uiv, PFNGLUNIFORM4UIVPROC, "glUniform4uiv");


    //Framebuffers
    SDLGLP(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC, "glBindFramebuffer");
    SDLGLP(glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC, "glBindRenderbuffer");
    SDLGLP(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC, "glGenFramebuffers");
    SDLGLP(glFramebufferParameteri, PFNGLFRAMEBUFFERPARAMETERIPROC, "glFramebufferParameteri");
    SDLGLP(glDrawBuffers, PFNGLDRAWBUFFERSPROC, "glDrawBuffers");
    SDLGLP(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC, "glFramebufferTexture2D");
    SDLGLP(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC, "glCheckFramebufferStatus");
    SDLGLP(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC, "glDeleteFramebuffers");

    SDLGLP(glTexStorage2D, PFNGLTEXSTORAGE2DPROC, "glTexStorage2D");
    SDLGLP(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC, "glGenerateMipmap");

    SDLGLP(glGenRenderbuffers, PFNGLGENRENDERBUFFERSPROC, "glGenRenderbuffers");
    SDLGLP(glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC, "glRenderbufferStorage");
    SDLGLP(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC, "glFramebufferRenderbuffer");


    SDLGLP(glGetUniformBlockIndex, PFNGLGETUNIFORMBLOCKINDEXPROC, "glGetUniformBlockIndex");// = nullptr;
    SDLGLP(glUniformBlockBinding, PFNGLUNIFORMBLOCKBINDINGPROC, "glUniformBlockBinding");//= nullptr;
    SDLGLP(glBindBufferBase, PFNGLBINDBUFFERBASEPROC, "glBindBufferBase");//        = nullptr;
    SDLGLP(glGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC, "glGetActiveUniform");


    SDLGLP(glGetVertexAttribIuiv, PFNGLGETVERTEXATTRIBIUIVPROC, "glGetVertexAttribIuiv");
    SDLGLP(glGetVertexAttribfv, PFNGLGETVERTEXATTRIBFVPROC, "glGetVertexAttribfv");
    SDLGLP(glGetVertexAttribiv, PFNGLGETVERTEXATTRIBIVPROC, "glGetVertexAttribiv");
    SDLGLP(glGetVertexAttribIiv, PFNGLGETVERTEXATTRIBIIVPROC, "glGetVertexAttribIiv");
    SDLGLP(glGetFramebufferAttachmentParameteriv, PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, "glGetFramebufferAttachmentParameteriv");
    SDLGLP(glGetActiveUniformBlockiv, PFNGLGETACTIVEUNIFORMBLOCKIVPROC, "glGetActiveUniformBlockiv");

    SDLGLP(glTexImage2DMultisample, PFNGLTEXIMAGE2DMULTISAMPLEPROC, "glTexImage2DMultisample");
    SDLGLP(glBlitFramebuffer, PFNGLBLITFRAMEBUFFERPROC, "glBlitFramebuffer");


    SDLGLP(glGetProgramBinary, PFNGLGETPROGRAMBINARYPROC, "glGetProgramBinary");// = nullptr;

    SDLGLP(glIsProgram, PFNGLISPROGRAMPROC, "glIsProgram");// = nullptr;
    SDLGLP(glProgramBinary, PFNGLPROGRAMBINARYPROC, "glProgramBinary");// = nullptr;
    SDLGLP(glValidateProgram, PFNGLVALIDATEPROGRAMPROC, "glValidateProgram");

    SDLGLP(glGetActiveAttrib, PFNGLGETACTIVEATTRIBPROC, "glGetActiveAttrib");
    SDLGLP(glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC, "glGetAttribLocation");
    SDLGLP(glMemoryBarrier, PFNGLMEMORYBARRIERPROC, "glMemoryBarrier");
    SDLGLP(glMapBufferRange, PFNGLMAPBUFFERRANGEPROC, "glMapBufferRange");
    SDLGLP(glGetActiveUniformBlockName, PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC, "glGetActiveUniformBlockName");

    SDLGLP(glFenceSync, PFNGLFENCESYNCPROC, "glFenceSync");
    SDLGLP(glIsSync, PFNGLISSYNCPROC, "glIsSync");
    SDLGLP(glGetSynciv, PFNGLGETSYNCIVPROC, "glGetSynciv");
    SDLGLP(glDeleteSync, PFNGLDELETESYNCPROC, "glDeleteSync");
    SDLGLP(glDispatchCompute, PFNGLDISPATCHCOMPUTEPROC, "glDispatchCompute");
    SDLGLP(glGetIntegeri_v, PFNGLGETINTEGERI_VPROC, "glGetIntegeri_v");
    SDLGLP(glShaderStorageBlockBinding, PFNGLSHADERSTORAGEBLOCKBINDINGPROC, "glShaderStorageBlockBinding");
    SDLGLP(glGetProgramResourceIndex, PFNGLGETPROGRAMRESOURCEINDEXPROC, "glGetProgramResourceIndex");

    // SDLGLP2(PFNGLFENCESYNCPROC, glFenceSync);
    // SDLGLP2(PFNGLISSYNCPROC, glIsSync);
    // SDLGLP2(PFNGLGETSYNCIVPROC, glGetSynciv);
    // SDLGLP2(PFNGLDELETESYNCPROC, glDeleteSync);
    // SDLGLP2(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute);
    // SDLGLP2(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v);
    // SDLGLP2(PFNGLSHADERSTORAGEBLOCKBINDINGPROC, glShaderStorageBlockBinding);
    // SDLGLP2(PFNGLGETPROGRAMRESOURCEINDEXPROC, glGetProgramResourceIndex);


    SDLGLP2(PFNGLCOPYIMAGESUBDATAPROC, glCopyImageSubData);
    SDLGLP2(PFNGLDELETETEXTURESEXTPROC, glDeleteTextures);

    return bValid;
}
void GLContext::setLineWidth(float w) {
#ifdef COMPATIBILITY_PROFILE_ENABLED
    glLineWidth(w);
#else
    BroLogErrorOnce("glLineWidth not supported");
#endif
}

void GLContext::pushCullFace() {
    GLint cull;
    glGetIntegerv(GL_CULL_FACE, &cull);

    if (_eLastCullFaceStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastCullFaceStack.push(cull);
    }
}
void GLContext::popCullFace() {
    GLint cull = _eLastCullFaceStack.top();

    if (_eLastCullFaceStack.size() > 0) {
        _eLastCullFaceStack.pop();
    }
    if (_eLastCullFaceStack.size() == 0) {
        _eLastCullFaceStack.push(1);
    }

    if (cull) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}
void GLContext::pushBlend() {
    GLint cull;
    glGetIntegerv(GL_BLEND, &cull);

    if (_eLastBlendStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastBlendStack.push(cull);
    }
}
void GLContext::popBlend() {
    GLint cull = _eLastBlendStack.top();

    if (_eLastBlendStack.size() > 0) {
        _eLastBlendStack.pop();
    }
    if (_eLastBlendStack.size() == 0) {
        _eLastBlendStack.push(1);
    }

    if (cull) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }
}
void GLContext::pushDepthTest() {
    GLint cull;
    glGetIntegerv(GL_DEPTH_TEST, &cull);

    if (_eLastDepthTestStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastDepthTestStack.push(cull);
    }
}
void GLContext::popDepthTest() {
    GLint cull = _eLastDepthTestStack.top();

    if (_eLastDepthTestStack.size() > 0) {
        _eLastDepthTestStack.pop();
    }
    if (_eLastDepthTestStack.size() == 0) {
        _eLastDepthTestStack.push(1);
    }

    if (cull) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}
//
//void GLContext::beginWin32InlineDebug() {
//#ifdef _WIN32
//#ifdef _DEBUG
//    //**INLINE MODE -- REMOVE FOR RELEASE
//    Gu::getShaderMaker()->shaderBound(nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//    chkErrDbg();
//
//    mat4 mProj = Gu::getCamera()->getProj();
//    // mProj.transpose();
//    mat4 mView = Gu::getCamera()->getView();
//    // mView.transpose();
//    glMatrixMode(GL_PROJECTION);
//    glLoadMatrixf((GLfloat*)& mProj);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadMatrixf((GLfloat*)& mView);
//#endif
//#endif
//}

void GLContext::enableCullFace(bool enable) {
    if (enable)glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}
void GLContext::enableBlend(bool enable) {
    if (enable)glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}

void GLContext::enableDepthTest(bool enable) {
    if (enable)glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}


}//ns Game
