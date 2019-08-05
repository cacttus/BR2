#include "../base/GlobalIncludes.h"
#include "../base/BaseHeader.h"
#include "../base/TypeConv.h"
#include "../base/TStr.h"
#include "../base/StringUtil.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/Fingers.h"
#include "../base/Sequencer.h"
#include "../base/TStr.h"
#include "../base/FrameSync.h"
#include "../base/SoundCache.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/RoomBase.h"
#include "../base/Package.h"

#include "../math/MathAll.h"

#include "../display/RenderUtils.h"
#include "../display/Party.h"
#include "../display/TexCache.h"
#include "../display/ShaderMaker.h"
#include "../display/LightManager.h"
#include "../display/Picker.h"
#include "../display/Gui2d.h"
#include "../display/CameraNode.h"
#include "../display/RenderSettings.h"

#include "../model/VertexFormat.h"
#include "../model/ModelCache.h"

namespace Game {

GLContext::GLContext() {
    //Do not init here.
    _fClearR = 0.02f;
    _fClearG = 0.02f;
    _fClearB = 0.021f;
    _fClearA = 1.0f;
}
void GLContext::createManagers(std::shared_ptr<RoomBase> rb) {
    _pRenderSettings = RenderSettings::create();
    BroLogInfo("GLContext - Building Package");
    _pPackage = std::make_shared<Package>();
    _pPackage->build(FileSystem::getExecutableFullPath());
    BroLogInfo("GLContext - Creating TexCache");
    _pTexCache = std::make_shared<TexCache>(shared_from_this());
    //BroLogInfo("GLContext - Creating MeshCache");
    //_pMeshCache = std::make_shared<MeshCache(this);
  //  BroLogInfo("GLContext - Creating TextBoss");
//    _pTextManager = std::make_shared<TextBoss>(shared_from_this());
    BroLogInfo("GLContext - Creating Party");
    _pParty = std::make_shared<Party>(shared_from_this());
    BroLogInfo("GLContext - Creating Sequencer");
    _pSequencer = std::make_shared<Sequencer>();
    BroLogInfo("GLContext - Creating Fingers");
    _pFingers = std::make_shared<Fingers>();
    _pFingers->init();
    BroLogInfo("GLContext - Creating FpsMeter");
    _pFpsMeter = std::make_shared<FpsMeter>();
    BroLogInfo("GLContext - Creating FrameSync");
    _pFrameSync = std::make_shared<FrameSync>(shared_from_this());
    //This was commented out.  Why? 11/6
    BroLogInfo("GLContext - Creating SoundCache");
    _pSoundCache = std::make_shared<SoundCache>();
    BroLogInfo("GLContext - Creating ShaderMaker & base shaders");
    _pShaderMaker = std::make_shared<ShaderMaker>(shared_from_this());
    _pShaderMaker->initialize(rb);
    BroLogInfo("GLContext -  Lights");
    _pLightManager = std::make_shared<LightManager>(shared_from_this());
    BroLogInfo("GLContext - Model Cache");
    _pModelCache = std::make_shared<ModelCache>(shared_from_this());
    
  //  _CrtCheckMemory();

    BroLogInfo("GLContext - Picker");
    _pPicker = std::make_shared<Picker>(shared_from_this());

   // _CrtCheckMemory();

    BroLogInfo("GLContext - Gui");
    _pGui2d = std::make_shared<Gui2d>();

}
GLContext::~GLContext() {
    //   BroLogInfo("GLContext - Destroying ModelCache");
    //   DEL_MEM(_pModelCache);
    //   //BroLogInfo("GLContext - Destroying MeshCache");
    //   //DEL_MEM(_pMeshCache);
    //   BroLogInfo("GLContext - Destroying Text Man");
    //   DEL_MEM(_pTextManager);
    //   BroLogInfo("GLContext - Destroying Party");
    //   DEL_MEM(_pParty);
    //   BroLogInfo("GLContext - Destroying Sequencer");
    //   DEL_MEM(_pSequencer);
    //   BroLogInfo("GLContext - Destroying Fingers");
    //   DEL_MEM(_pFingers);
    //   BroLogInfo("GLContext - Destroying FrameSync");
    //   DEL_MEM(_pFrameSync);
    //   BroLogInfo("GLContext - Destroying ShaderMaker");
    //   DEL_MEM(_pShaderMaker);
    //   BroLogInfo("GLContext - Destroying SoundCache");
    //   DEL_MEM(_pSoundCache);
    //   BroLogInfo("GLContext - Destroying FpsMeter");
    //   DEL_MEM(_pFpsMeter);
    //   BroLogInfo("GLContext - Destroying Lights");
    //   DEL_MEM(_pLightManager);
    //   BroLogInfo("GLContext - Destroying TexCache");
    //   DEL_MEM(_pTexCache);
    //   BroLogInfo("GLContext - Destroying Picker");
    //   DEL_MEM(_pPicker);
    //   BroLogInfo("GLContext - Destroying Package");
    //   DEL_MEM(_pPackage);
    //   DEL_MEM(_pRenderSettings);
}
bool GLContext::load(std::shared_ptr<RoomBase> rb) {
    _bValid = true;
    setRoom(rb);

    BroLogInfo("GLContext - Loading GL Functions.");
    _bValid = _bValid && loadOpenGLFunctions();

    BroLogInfo("GLContext - Making Vtx Formats.");
    makeVertexFormats();

    BroLogInfo("GLContext - Creating Managers.");
    createManagers(rb);



    return isValid();
}
std::shared_ptr<EngineConfig> GLContext::getConfig() { return Gu::getEngineConfig(); }
void GLContext::update(float delta) {
    if (_pSequencer != nullptr) {
        _pSequencer->update();
    }
    if (_pParty != nullptr) {
        _pParty->update(delta);
    }
    if (_pFpsMeter != nullptr) {
        _pFpsMeter->update();
    }
    if (_pSoundCache != nullptr) {
        _pSoundCache->update();
    }
    if (_pPicker != nullptr) {
        _pPicker->update(_pFingers);
    }
    if (_pGui2d != nullptr) {
        _pGui2d->update(_pFingers);
    }

}

bool GLContext::chkErrRt(bool bDoNotBreak) {
    //Enable runtime errors.
   return OglErr::chkErrRt(shared_from_this(), bDoNotBreak);
}
bool GLContext::chkErrDbg(bool bDoNotBreak) {
    return OglErr::chkErrDbg(shared_from_this(), bDoNotBreak);
}

void GLContext::makeVertexFormats() {

    v_v3c4x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4x2");
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v2c4x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2c4x2");
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3n3x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3n3x2");
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::n3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3x2");
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3n3");
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3");
    v_v3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);

    v_v2x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2x2");
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v2c4::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2c4");
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::e::c4_01);

    //v_v3c3i4i4::_pVertexFormat = new VertexFormat(this, "v_v3c3i4i4");
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_FLOAT, 3, sizeof(vec3), VertexUserType::e::v3_01);
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_FLOAT, 3, sizeof(vec3), VertexUserType::e::c3_01);
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_INT, 4, sizeof(ivec4));
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_INT, 4, sizeof(ivec4));

    v_v3c3x2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c3x2n3");
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::c3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::x2_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3i2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3i2n3");
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::i2_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3c4::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4");
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::e::c4_01);

    v_v3c4x2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4x2n3");
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::x2_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_GuiVert::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_GuiVert");
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_02);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_03);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::u2_01);


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







}//ns Game
