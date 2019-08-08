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

#include "../base/BaseAll.h"
namespace Game {
class Viewport;
class CameraNode;
//class MeshCache;
//class TextBoss;
class Party;
class Sequencer;
class AppBase;
class Fingers;
class FpsMeter;
class FrameSync;
class SoundCache;
class Logger;
class ShaderMaker;
class EngineConfig;
class LightManager;
class ModelCache;
class Picker;
class Package;
class PhysicsWorld;
class Gui2d;
class RenderSettings;

/**
*    @class GLContext
*    @brief OpenGL render context
*
*/
class GLContext : public VirtualMemoryShared<GLContext> {
private:
    std::shared_ptr<TexCache> _pTexCache = nullptr;
    std::shared_ptr<CameraNode> _pCamera = nullptr;
    //  std::shared_ptr<TextBoss> _pTextManager = nullptr;
    std::shared_ptr<Party> _pParty = nullptr;
    std::shared_ptr<Sequencer> _pSequencer = nullptr;
    std::shared_ptr<AppBase> _pApp = nullptr;
    std::shared_ptr<Fingers> _pFingers = nullptr;
    std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
    std::shared_ptr<FrameSync> _pFrameSync = nullptr;
    std::shared_ptr<SoundCache> _pSoundCache = nullptr;
    std::shared_ptr<ShaderMaker> _pShaderMaker = nullptr;
    std::shared_ptr<LightManager> _pLightManager = nullptr;
    std::shared_ptr<ModelCache> _pModelCache = nullptr;
    std::shared_ptr<Picker> _pPicker = nullptr;
    std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
    std::shared_ptr<Package> _pPackage = nullptr;
    std::shared_ptr<RenderSettings> _pRenderSettings = nullptr;
    std::shared_ptr<Gui2d> _pGui2d = nullptr;
    std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
    EngineLoopState::e _eLoopState = EngineLoopState::Update;
    float _fClearR = 1.0f, _fClearG = 1.0f, _fClearB = 1.0f, _fClearA = 1.0f;
    // bool _bEnableRtErrors = false;
     //bool _bUseMapBuffer = false;
    bool _bValid = false;

    bool loadOpenGLFunctions();
    void createManagers(std::shared_ptr<AppBase> rb);

    void makeVertexFormats();
public:
    GLContext();
    virtual ~GLContext() override;

    bool load(std::shared_ptr<AppBase> br);
    void update(float delta);
    EngineLoopState::e getLoopState() { return _eLoopState; }
    void setLoopState(EngineLoopState::e ee) { _eLoopState = ee; }
    //Required Globals (cannot be NULL for the game impl.)
    std::shared_ptr<RenderSettings> getRenderSettings() { AssertOrThrow2(_pRenderSettings != nullptr);  return _pRenderSettings; }
    std::shared_ptr<Package> getPackage() { AssertOrThrow2(_pPackage != nullptr);  return _pPackage; }
    std::shared_ptr<ModelCache> getModelCache() { AssertOrThrow2(_pModelCache != nullptr);  return _pModelCache; }
    //  std::shared_ptr<TextBoss> getTextBoss() { AssertOrThrow2(_pTextManager != nullptr);return _pTextManager;}
    std::shared_ptr<Sequencer> getSequencer() { AssertOrThrow2(_pSequencer != nullptr); return _pSequencer; }
    std::shared_ptr<Fingers> getFingers() { AssertOrThrow2(_pFingers != nullptr); return _pFingers; }
    std::shared_ptr<FpsMeter> getFpsMeter() { AssertOrThrow2(_pFpsMeter != nullptr); return _pFpsMeter; }
    std::shared_ptr<FrameSync> getFrameSync() { AssertOrThrow2(_pFrameSync != nullptr); return _pFrameSync; }
    std::shared_ptr<SoundCache> getSoundCache() { AssertOrThrow2(_pSoundCache != nullptr); return _pSoundCache; }
    std::shared_ptr<ShaderMaker> getShaderMaker() { AssertOrThrow2(_pShaderMaker != nullptr);  return _pShaderMaker; }
    std::shared_ptr<AppBase> getRoom() { AssertOrThrow2(_pApp != nullptr); return _pApp; }
    std::shared_ptr<TexCache> getTexCache() { AssertOrThrow2(_pTexCache != nullptr); return _pTexCache; }
    std::shared_ptr<LightManager> getLightManager() { AssertOrThrow2(_pLightManager != nullptr);  return _pLightManager; }
    std::shared_ptr<Picker> getPicker() { AssertOrThrow2(_pPicker != nullptr); return _pPicker; }
    std::shared_ptr<Gui2d> getGui() { AssertOrThrow2(_pGui2d != nullptr);  return _pGui2d; }
    std::shared_ptr<PhysicsWorld> getPhysicsWorld() { AssertOrThrow2(_pPhysicsWorld != nullptr); return _pPhysicsWorld; }
    std::shared_ptr<Party> getParty() { AssertOrThrow2(_pParty != nullptr); return _pParty; }
    std::shared_ptr<CameraNode> getCamera() { AssertOrThrow2(_pCamera != nullptr); return _pCamera; }
    std::shared_ptr<EngineConfig> getConfig();
    std::shared_ptr<RenderPipe> getRenderPipe() { return _pRenderPipe; }
    void setRenderPipe(std::shared_ptr<RenderPipe> r) { _pRenderPipe = r; }
    void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { AssertOrThrow2(_pPhysicsWorld == nullptr); _pPhysicsWorld = p; }
    void setLineWidth(float w);

    void setCamera(std::shared_ptr<CameraNode> pc) { _pCamera = pc; }
    void setRoom(std::shared_ptr<AppBase> b) { _pApp = b; }
    // void setRtErrors(bool b);

    float& getClearR() { return _fClearR; }
    float& getClearG() { return _fClearG; }
    float& getClearB() { return _fClearB; }
    float& getClearA() { return _fClearA; }

    bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false); //check for errrors
    bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false); //check for errrors
    bool isValid() { return _bValid; }

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
