/**
*  @file GLContext.h
*  @date November 19, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GLCONTEXT_14795781163972161943_H__
#define __GLCONTEXT_14795781163972161943_H__

#include "../gfx/GraphicsContext.h"
#include "../base/SDLIncludes.h"
#include "../base/SDLGLIncludes.h"
namespace BR2 {
/**
*  @class GLProfile
*  @brief OpenGL pixel format, profile.
*/
struct GLProfile : public VirtualMemory {
public:
  GLProfile(int iDepthBits, int iMinVer, int iMinSubver, int iProfile, bool bVsync, int msaa_buffers, int msaa_samples, bool srgb, bool forward_compatible) {
    _iDepthBits = iDepthBits;
    _iMinSubVersion = iMinSubver;
    _iMinVersion = iMinVer;
    _iProfile = iProfile;
    _bVsync = bVsync;
    _iMSAASamples = msaa_samples;
    _iMSAABuffers = msaa_buffers;
    _bSRGB = srgb;
    _bForwardCompatible = forward_compatible;
  }
  virtual ~GLProfile() override {}
  string_t toString() {
    string_t ret = "";
    string_t profile = "basic";
    if (_iProfile & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) { profile = "compatibility"; }
    else if (_iProfile & SDL_GL_CONTEXT_PROFILE_CORE) { profile = "core"; }
    else if (_iProfile & SDL_GL_CONTEXT_PROFILE_ES) { profile = "ES"; }

    return Stz "version: " + _iMinVersion + "." + _iMinSubVersion + 
      " depth:" + _iDepthBits + " profile:" + profile + " vsync" + _bVsync + " msaa: " + 
      _iMSAABuffers + ", " + _iMSAASamples + " srgb:" + (_bSRGB?"true":"false") + " forward_compatible: " + (_bForwardCompatible? "true" : "false");
  }
public:
  bool _bForwardCompatible = true;
  int _iDepthBits = 32;
  int _iMinVersion = 3;
  int _iMinSubVersion = 3;
  int _iMSAASamples = 0;
  int _iMSAABuffers = 0;
  int _iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; // _CORE, _ES
  bool _bVsync = true; //Automatic on IOS
  bool _bSRGB = false;
};

/**
*  @class GLContext
*  @brief OpenGL rendering context.
*/
class GLContext : public GraphicsContext {
public:
  GLContext(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLProfile> profile, SDL_Window* win);
  virtual ~GLContext() override;

  int32_t getSupportedDepthSize() { return _iSupportedDepthSize; }
  SDL_GLContext getSDLGLContext() { return _context; }

  //virtual void update(float delta) override;
  virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false) override;
  virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false) override;

  virtual void pushCullFace() override;
  virtual void popCullFace() override;
  virtual void pushBlend() override;
  virtual void popBlend() override;
  virtual void pushDepthTest() override;
  virtual void popDepthTest() override;

  //compatibility issues
   void setPolygonMode(PolygonMode p) ;
   void setLineWidth(float w);

  void enableCullFace(bool enable) override;
  void enableBlend(bool enable) override;
  void enableDepthTest(bool enable) override;

  static void setWindowAndOpenGLFlags(std::shared_ptr<GLProfile> prof);


  bool valid() { return _bValid; }
  bool getForwardCompatible() { return _bForwardCompatible; }
private:
  std::shared_ptr<GLProfile> _profile;
  bool _bValid = false;
  bool loadOpenGLFunctions();

  bool _bForwardCompatible = false;
  //Render Stack.
  std::stack<GLenum> _eLastCullFaceStack;
  std::stack<GLenum> _eLastBlendStack;
  std::stack<GLenum> _eLastDepthTestStack;
  static const int MaxStackSize = 32;

  SDL_GLContext _context;

  bool checkOpenGlMinimumVersionInfo(int required_version, int required_subversion);
  void getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver);
  void loadCheckProc();
  void printHelpfulDebug();

  int _iSupportedDepthSize;

public:

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
