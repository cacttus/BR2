/**
*
*    @file GLContext.h
*    @date November 19, 2016
*    @author Derek Page
*
*    � 2016
*
*
*/
#pragma once
#ifndef __GLCONTEXT_14795781163972161943_H__
#define __GLCONTEXT_14795781163972161943_H__

#include "../gfx/GraphicsContext.h"

namespace Game {

/**
*    @class GLContext
*    @brief OpenGL render context
*/
class GLContext : public GraphicsContext {
private:
    bool _bValid = false;
    bool loadOpenGLFunctions();

    //Render Stack.
    static std::stack<GLenum> _eLastCullFaceStack;
    static std::stack<GLenum> _eLastBlendStack;
    static std::stack<GLenum> _eLastDepthTestStack;
    static const int MaxStackSize = 32;

public:
    GLContext();
    virtual ~GLContext() override;

    virtual bool load(std::shared_ptr<AppBase> br) override;
    virtual void update(float delta) override;
    virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false) override;
    virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false) override;

    virtual void pushCullFace() override;
    virtual void popCullFace() override;
    virtual void pushBlend() override;
    virtual void popBlend() override;
    virtual void pushDepthTest() override;
    virtual void popDepthTest() override;

    void setLineWidth(float w);
   // void beginWin32InlineDebug();

    PFNGLUSEPROGRAMPROC         glUseProgram = nullptr;
    PFNGLBINDBUFFERARBPROC      glBindBuffer = nullptr;
    PFNGLGENBUFFERSPROC         glGenBuffers = nullptr;
    PFNGLBUFFERDATAPROC         glBufferData = nullptr;
    PFNGLBUFFERSUBDATAPROC      glBufferSubData = nullptr;
    PFNGLSHADERSOURCEPROC       glShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC      glCompileShader = nullptr;
    PFNGLGETSHADERIVPROC        glGetShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC   glGetShaderInfoLog = nullptr;
    PFNGLCREATEPROGRAMPROC      glCreateProgram = nullptr;
    PFNGLATTACHSHADERPROC       glAttachShader = nullptr;
    PFNGLLINKPROGRAMPROC        glLinkProgram = nullptr;
    PFNGLDETACHSHADERPROC       glDetachShader = nullptr;
    PFNGLDELETESHADERPROC       glDeleteShader = nullptr;
    PFNGLCREATESHADERPROC       glCreateShader = nullptr;
    PFNGLDELETEPROGRAMPROC      glDeleteProgram = nullptr;
    PFNGLGETPROGRAMIVPROC       glGetProgramiv = nullptr;
    PFNGLGETPROGRAMINFOLOGPROC  glGetProgramInfoLog = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
    PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = nullptr;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
    PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
    PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog = nullptr;
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
    PFNGLISBUFFERPROC glIsBuffer = nullptr;
    PFNGLMAPBUFFERPROC glMapBuffer = nullptr;
    PFNGLUNMAPBUFFERPROC glUnmapBuffer = nullptr;
    PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;

    PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
    PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
    PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri = nullptr;
    PFNGLDRAWBUFFERSPROC glDrawBuffers = nullptr;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
    PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;

    PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = nullptr;

    PFNGLTEXSTORAGE2DPROC glTexStorage2D = nullptr;
    PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;

    //Render Buffers
    PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr;
    PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr;
    PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = nullptr;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;

    PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = nullptr;
    PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = nullptr;
    PFNGLBINDBUFFERBASEPROC glBindBufferBase = nullptr;

    PFNGLUNIFORM1IPROC        glUniform1i = nullptr;

    PFNGLUNIFORM1IVPROC       glUniform1iv = nullptr;
    PFNGLUNIFORM2IVPROC       glUniform2iv = nullptr;
    PFNGLUNIFORM3IVPROC       glUniform3iv = nullptr;
    PFNGLUNIFORM4IVPROC       glUniform4iv = nullptr;

    PFNGLUNIFORM1FVPROC       glUniform1fv = nullptr;
    PFNGLUNIFORM2FVPROC       glUniform2fv = nullptr;
    PFNGLUNIFORM3FVPROC       glUniform3fv = nullptr;
    PFNGLUNIFORM4FVPROC       glUniform4fv = nullptr;

    PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = nullptr;
    PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = nullptr;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;

    PFNGLUNIFORM1UIVPROC      glUniform1uiv = nullptr;
    PFNGLUNIFORM2UIVPROC      glUniform2uiv = nullptr;
    PFNGLUNIFORM3UIVPROC      glUniform3uiv = nullptr;
    PFNGLUNIFORM4UIVPROC      glUniform4uiv = nullptr;

    PFNGLGETVERTEXATTRIBIUIVPROC  glGetVertexAttribIuiv = nullptr;
    PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = nullptr;
    PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = nullptr;
    PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv = nullptr;
    PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = nullptr;
    PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = nullptr;

    PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = nullptr;
    PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;

    PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = nullptr;
    PFNGLISPROGRAMPROC glIsProgram = nullptr;
    PFNGLPROGRAMBINARYPROC glProgramBinary = nullptr;
    PFNGLVALIDATEPROGRAMPROC glValidateProgram = nullptr;

    PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = nullptr;
    PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
    PFNGLMEMORYBARRIERPROC glMemoryBarrier = nullptr;
    PFNGLMAPBUFFERRANGEPROC glMapBufferRange = nullptr;

    PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = nullptr;


    PFNGLFENCESYNCPROC  glFenceSync = nullptr;
    PFNGLISSYNCPROC     glIsSync = nullptr;
    PFNGLGETSYNCIVPROC  glGetSynciv = nullptr;
    PFNGLDELETESYNCPROC glDeleteSync = nullptr;
    PFNGLDISPATCHCOMPUTEPROC glDispatchCompute = nullptr;
    PFNGLGETINTEGERI_VPROC   glGetIntegeri_v = nullptr;
    PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding = nullptr;
    PFNGLGETPROGRAMRESOURCEINDEXPROC   glGetProgramResourceIndex = nullptr;

    PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData = nullptr;
    PFNGLDELETETEXTURESEXTPROC glDeleteTextures = nullptr;
};

}//ns Game



#endif
