/**
*  @file GLContext.h
*  @date November 19, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GLCONTEXT_14795781163972161943_H__
#define __GLCONTEXT_14795781163972161943_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
* @struct GLProfile
* @brief A Graphics profile that can be tested against the Gpu
*/
struct GLProfile : public VirtualMemory {
public:
  int _iDepthBits = 0;
  int _iMinVersion = 0;
  int _iMinSubVersion = 0;
  int _iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; // Set to true to enable the compatibility profile
  bool _bVsync = true; //Automatic on IOS
  void make(int iDepthBits, int iMinVer, int iMinSubver, int iProfile, bool bVsync) {
    _iDepthBits = iDepthBits;
    _iMinSubVersion = iMinSubver;
    _iMinVersion = iMinVer;
    _iProfile = iProfile;
    _bVsync = bVsync;
  }
};

/**
*  @class GLContext
*  @brief OpenGL render context
*  All GL contexts require an SDL window.  To create another context, create a new window.  You can hide the window, and also disable the RenderPipeline if needed.
*/
class GLContext : public VirtualMemoryShared<GLContext> {
public:
  GLContext();
  virtual ~GLContext() override;

  bool create(std::shared_ptr<GraphicsWindow> pMainWindow, GLProfile& profile);
  void updateThisContext();
  static void updateGlobalManagers();

  SDL_GLContext getSDLGLContext() { return _context; }
  int32_t getSupportedDepthSize() { return _iSupportedDepthSize; }

  //virtual void update(float delta);
  bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false);
  bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false);

  void pushCullFace();
  void popCullFace();
  void pushBlend();
  void popBlend();
  void pushDepthTest();
  void popDepthTest();
  void enableCullFace(bool enable);
  void enableBlend(bool enable);
  void enableDepthTest(bool enable);

  static void setWindowAndOpenGLFlags(GLProfile& prof);
  EngineLoopState::e getLoopState() { return _eLoopState; }
  void setLoopState(EngineLoopState::e ee) { _eLoopState = ee; }
  std::shared_ptr<GraphicsWindow> getWindow() { return _pWindow; }
  void setLineWidth(float w);

  std::shared_ptr<RenderPipeline> getRenderPipe() { return _pRenderPipe;}
  std::shared_ptr<Picker> getPicker() {return _pPicker;}
  std::shared_ptr<FpsMeter> getFpsMeter() { return _pFpsMeter; }
  std::shared_ptr<Delta> getDelta() { return _pDelta; }

  static std::shared_ptr<TexCache> getTexCache() { return _pTexCache; }
  static std::shared_ptr<ParticleMaker> getParticleManager() { return _pParticleMaker; }
  static std::shared_ptr<ShaderManager> getShaderManager() { return _pShaderMaker; }

private:
  //Per context managers, since FBOs aren't shared.
  std::shared_ptr<RenderPipeline> _pRenderPipe = nullptr;
  std::shared_ptr<Picker> _pPicker = nullptr;
  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  //Shared context data.
  static std::shared_ptr<TexCache> _pTexCache;
  static std::shared_ptr<ParticleMaker> _pParticleMaker;
  static std::shared_ptr<ShaderManager> _pShaderMaker;

  EngineLoopState::e _eLoopState = EngineLoopState::Update;
  float _fClearR = 1.0f;
  float _fClearG = 1.0f;
  float _fClearB = 1.0f;
  float _fClearA = 1.0f;
  bool _bValid = false;

  //Render Stack.
  std::stack<GLenum> _eLastCullFaceStack;
  std::stack<GLenum> _eLastBlendStack;
  std::stack<GLenum> _eLastDepthTestStack;
  static const int MaxStackSize = 32;

  std::shared_ptr<GraphicsWindow> _pWindow = nullptr; //This may be null, for storage contexts.
  SDL_GLContext _context;
  GLProfile _profile;
  int _iSupportedDepthSize;

  void makeVertexFormats();
  void createManagers();
  bool loadOpenGLFunctions();
  void checkForOpenGlMinimumVersion(int required_version, int required_subversion);
  void getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver);
  void loadCheckProc();
  void printHelpfulDebug();

  bool chkErr(bool bDoNotBreak = false, bool doNotLog = false);

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

}//ns BR2



#endif
