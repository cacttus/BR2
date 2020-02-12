#include "../base/Base.h"
#include "../base/EngineConfig.h"
#include "../base/DateTime.h"
#include "../base/Img32.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/RenderPipeline.h"
#include "../gfx/DeferredFramebuffer.h"
#include "../gfx/ForwardFramebuffer.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/BufferRenderTarget.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/ShadowFrustum.h"
#include "../gfx/TexCache.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/UiControls.h"
#include "../model/MeshNode.h"
#include "../model/MeshUtils.h"
#include "../model/VertexFormat.h"
#include "../world/Scene.h"

namespace Game {
RenderPipeline::RenderPipeline(std::shared_ptr<GraphicsWindow> w) {
  _vClear = vec4(0, 0, 0, 1);
  _pWindow = w;
}
RenderPipeline::~RenderPipeline() {
  releaseFbosAndMesh();
}
const vec4& RenderPipeline::getClear() {
  return _vClear;
}
void RenderPipeline::setClear(vec4& v) {
  _vClear = v;;
  _pBlittedForward->setClear(_vClear);
  _pBlittedDeferred->setClear(_vClear);
}
void RenderPipeline::init(int32_t iWidth, int32_t iHeight, string_t strEnvTexturePath) {
  BroLogInfo("[RenderPipe] Initializing.");
  if (iWidth <= 0 || iHeight <= 0) {
    BroLogError("[RenderPipe] Got framebuffer of width or height < 0" + iWidth + "," + iHeight);
  }

  //Enable some stuff.
#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
#endif
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  releaseFbosAndMesh();

  // - Setup Framebuffers.
  _bMsaaEnabled = Gu::getEngineConfig()->getEnableMsaa();
  _nMsaaSamples = Gu::getEngineConfig()->getMsaaSamples();
  _iLastWidth = iWidth;
  _iLastHeight = iHeight;

  BroLogInfo("[RenderPipe] Checking Caps");
  checkDeviceCaps(iWidth, iHeight);

  if (_bMsaaEnabled) {
    glEnable(GL_MULTISAMPLE);
    getGraphicsContext()->chkErrRt();
  }

  //Mesh
  BroLogInfo("[RenderPipe] Creating Quad Mesh");
  createQuadMesh(iWidth, iHeight);

  //Shaders
  if (_pDeferredShader == nullptr) {
    _pDeferredShader = getGraphicsContext()->getShaderMaker()->makeShader(std::vector<string_t>{
      "d_v3x2_lighting.vs", "d_v3x2_lighting.ps" });
  }
  if (_pForwardShader == nullptr) {
    _pForwardShader = getGraphicsContext()->getShaderMaker()->makeShader(std::vector<string_t>{
      "f_v3x2_fbo.vs", "f_v3x2_fbo.ps"});
  }
  getGraphicsContext()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  getGraphicsContext()->chkErrDbg();
  getGraphicsContext()->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  getGraphicsContext()->chkErrDbg();
  getGraphicsContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  getGraphicsContext()->chkErrDbg();

  //Guess what?  Headache time.
  //This deletion of the shared stuff is super important 2/9/18
  //this must come before all the code below.  In short: RenderTarget automatically deletes its texure
  //TODO: in the future remove deferred/forward targets.  Make the render targets refer to a shared_ptr Texture2DSpec so that
  //deletion of the texture is natural when it gets unreferenced.  Make the creation of all render target textures be independent
  //of their framebuffers so that we can share them across multiple render stages.
  _pMsaaDeferred = nullptr;
  _pMsaaForward = nullptr;
  _pMsaaDepth = nullptr;
  _pBlittedDepth = nullptr;
  _pBlittedForward = nullptr;
  _pBlittedDeferred = nullptr;
  _pPick = nullptr;
  _pDOFFbo = nullptr;

  //Base FBOs
  _pBlittedDepth = FramebufferBase::createDepthTarget("depth blitted", iWidth, iHeight, 0, false, 0);

  //Do not cahnge "Pick" name.  This is shared.
  _pPick = FramebufferBase::createTarget("Pick", GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, iWidth, iHeight, RenderTargetType::e::Pick, 0, 0, 0);//4

  _pBlittedDeferred = std::make_shared<DeferredFramebuffer>(getGraphicsContext(), iWidth, iHeight, false, 0, _vClear);
  _pBlittedDeferred->init(iWidth, iHeight, _pBlittedDepth, _pPick);

  _pBlittedForward = std::make_shared<ForwardFramebuffer>(getGraphicsContext(), iWidth, iHeight, false, 0, _vClear);
  _pBlittedForward->init(iWidth, iHeight, _pBlittedDepth, _pPick);

  _pDOFFbo = std::make_shared<DOFFbo>(iWidth, iHeight);

  //Multisample
  if (_bMsaaEnabled == true) {
    BroLogInfo("[RenderPipe] Creating deferred MSAA lighting buffer");
    _pMsaaDepth = FramebufferBase::createDepthTarget("depth msaa", iWidth, iHeight, 0, _bMsaaEnabled, _nMsaaSamples);
    _pMsaaDeferred = std::make_shared<DeferredFramebuffer>(getGraphicsContext(), iWidth, iHeight, _bMsaaEnabled, _nMsaaSamples, _vClear);
    _pMsaaDeferred->init(iWidth, iHeight, _pMsaaDepth, _pPick);// Yeah I don't know if the "pick" here will work
    _pMsaaForward = std::make_shared<ForwardFramebuffer>(getGraphicsContext(), iWidth, iHeight, _bMsaaEnabled, _nMsaaSamples, _vClear);
    _pMsaaForward->init(iWidth, iHeight, _pMsaaDepth, _pPick);// Yeah I don't know if the "pick" here will work
  }
  else {
    BroLogInfo("[RenderPipe] Multisample not enabled.");
    _pMsaaDeferred = _pBlittedDeferred;
    _pMsaaForward = _pBlittedForward;
    _pMsaaDepth = _pBlittedDepth;
  }

  //These are here SOLELY for shadow map blending.
  //If we don't do any shadow blending then these are useless.
  int32_t iShadowMapRes = Gu::getEngineConfig()->getShadowMapResolution();
  _pShadowBoxFboMaster = std::make_shared<ShadowBox>(nullptr, iShadowMapRes, iShadowMapRes, true);
  _pShadowBoxFboMaster->init();
  _pShadowFrustumMaster = std::make_shared<ShadowFrustum>(nullptr, iShadowMapRes, iShadowMapRes, true);
  _pShadowFrustumMaster->init();

  if (StringUtil::isNotEmpty(strEnvTexturePath)) {
    _pEnvTex = getGraphicsContext()->getTexCache()->getOrLoad(strEnvTexturePath, false, true, true);
  }
}

void RenderPipeline::saveScreenshot() {
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F9)) {
    if (Gu::getInputManager()->shiftHeld()) {
      BroLogInfo("[RenderPipe] Saving all MRTs.");
      //Save all deferred textures
      int iTarget;

      //*Don't save the master shadowbox image.
      //for (int i = 0; i < 6; ++i) {
      //    t_string fname = FileSystem::getScreenshotFilename();
      //    fname = TStr(fname, "_shadowbox_MASTER_", 0, "_side_", i, "_.png");
      //    RenderUtils::saveTexture(std::move(fname), _pShadowBoxFboMaster->getGlTexId(), GL_TEXTURE_CUBE_MAP, i);
      //    BroLogInfo("[RenderPipe] Screenshot '", fname, "' saved");
      //}
      iTarget = 0;
      for (std::shared_ptr<ShadowFrustum> sf : getGraphicsContext()->getLightManager()->getAllShadowFrustums()) {
        string_t fname = FileSystem::getScreenshotFilename();
        fname = fname + "_shadow_frustum_" + iTarget + "_.png";
        RenderUtils::saveTexture(std::move(fname), sf->getGlTexId(), GL_TEXTURE_2D);
        BroLogInfo("[RenderPipe] Screenshot '" + fname + "' saved");
        iTarget++;
      }
      iTarget = 0;
      for (std::shared_ptr<ShadowBox> sb : getGraphicsContext()->getLightManager()->getAllShadowBoxes()) {
        for (int i = 0; i < 6; ++i) {
          string_t fname = FileSystem::getScreenshotFilename();

          string_t side;
          if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_POSITIVE_X) side = "+X";
          else if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_POSITIVE_Y) side = "+Y";
          else if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_POSITIVE_Z) side = "+Z";
          else if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_NEGATIVE_X) side = "-X";
          else if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y) side = "-Y";
          else if (i + GL_TEXTURE_CUBE_MAP_POSITIVE_X == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) side = "-Z";

          fname = fname + "_shadowbox_" + iTarget + "_side_" + side + "_.png";
          RenderUtils::saveTexture(std::move(fname), sb->getGlTexId(), GL_TEXTURE_CUBE_MAP, i);
          BroLogInfo("[RenderPipe] Screenshot '" + fname + "' saved");
        }
        iTarget++;
      }

      iTarget = 0;
      for (std::shared_ptr<BufferRenderTarget> pTarget : _pBlittedDeferred->getTargets()) {
        string_t fname = FileSystem::getScreenshotFilename();
        fname = fname + "_deferred_" + pTarget->getName() + "_" + iTarget++ + "_.png";
        RenderUtils::saveTexture(std::move(fname), pTarget->getGlTexId(), pTarget->getTextureTarget());
        BroLogInfo("[RenderPipe] Screenshot '" + fname + "' saved");
      }
      iTarget = 0;
      for (std::shared_ptr<BufferRenderTarget> pTarget : _pBlittedForward->getTargets()) {
        string_t fname = FileSystem::getScreenshotFilename();
        fname = fname + "_forward_" + pTarget->getName() + "_" + iTarget++ + "_.png";
        RenderUtils::saveTexture(std::move(fname), pTarget->getGlTexId(), pTarget->getTextureTarget());
        BroLogInfo("[RenderPipe] Screenshot '" + fname + "' saved");
      }

    }
    else {
      //Basic Forward Screenshot
      string_t fname = FileSystem::getScreenshotFilename();
      RenderUtils::saveTexture(std::move(fname), _pBlittedForward->getGlId(), GL_TEXTURE_2D);
      BroLogInfo("[RenderPipe] Screenshot '" + fname + "' saved");
    }
  }
}

void RenderPipeline::copyMsaaSamples(std::shared_ptr<FramebufferBase> msaa, std::shared_ptr<FramebufferBase> blitted) {
  //Downsize the MSAA sample buffer.
  if (_bMsaaEnabled) {
    getGraphicsContext()->glBindRenderbuffer(GL_RENDERBUFFER, NULL);
    getGraphicsContext()->chkErrDbg();

    getGraphicsContext()->glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa->getGlId());
    getGraphicsContext()->chkErrDbg();
    getGraphicsContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitted->getGlId());
    getGraphicsContext()->chkErrDbg();

    for (std::shared_ptr<BufferRenderTarget> inf : msaa->getTargets()) {
      GLenum blendMode;
      GLbitfield bitMask;
      if (inf->getTargetType() == RenderTargetType::e::Depth) {
        //GL_LINEAR is only a valid interpolation method for the color buffer.
        blendMode = GL_NEAREST;
      }
      else {
        glReadBuffer(inf->getAttachment());
        getGraphicsContext()->chkErrDbg();
        glDrawBuffer(inf->getAttachment());
        getGraphicsContext()->chkErrDbg();
        blendMode = GL_LINEAR;
      }

      //GL_DEPTH_BUFFER_BIT 0x00000100
      //GL_COLOR_BUFFER_BIT  0x00004000
      bitMask = inf->getBlitBit();

      //GL_DEPTH_BUFFER_BIT;
      getGraphicsContext()->glBlitFramebuffer(0, 0, _iLastWidth, _iLastHeight, 0, 0, _iLastWidth, _iLastHeight, bitMask, blendMode);
      getGraphicsContext()->chkErrDbg();
    }
    getGraphicsContext()->chkErrDbg();

  }
}
void RenderPipeline::resizeScreenBuffers(int32_t w, int32_t h) {
  //Simply called neow by new camera vp size.
  //recreate everything
  init(w, h, "");
}

void RenderPipeline::beginRenderShadows() {
  //See GLLightManager in BRO
  getGraphicsContext()->pushDepthTest();
  getGraphicsContext()->pushBlend();
  getGraphicsContext()->pushCullFace();
  glCullFace(GL_FRONT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}
void RenderPipeline::endRenderShadows() {

  glCullFace(GL_BACK);
  getGraphicsContext()->popDepthTest();
  getGraphicsContext()->popBlend();
  getGraphicsContext()->popCullFace();
}
void RenderPipeline::renderShadows(std::shared_ptr<CameraNode> cam) {
  getGraphicsContext()->getLightManager()->update(_pShadowBoxFboMaster, _pShadowFrustumMaster);

  //Force refresh teh viewport.
  cam->getViewport()->updateChanged(true);
}

void RenderPipeline::beginRenderDeferred() {
  enableDisablePipeBits();

  // _pMsaaForward->clearFb();//Must call before deferre
  _pMsaaDeferred->beginRender();
}
void RenderPipeline::endRenderDeferred() {
  _pMsaaDeferred->endRender();
}
void RenderPipeline::beginRenderForward() {
  _pMsaaForward->beginRender();
}
void RenderPipeline::endRenderForward() {
  _pMsaaForward->endRender();
}
void RenderPipeline::enableDisablePipeBits() {
  //TODO: make sure the given input window is in focus.
  //if (_pWindow!=nullptr && _pWindow->hasFocus()) {

  //    if (Gu::getFingers()->keyPress(SDL_SCANCODE_F8)) {
  //        Gu::incrementEnum<PipeBit::e>(_pipeBits, PipeBit::e::MaxPipes);
  //        if (_ePipeBit == PipeBit::e::Full) {
  //            _pipeBits.set();
  //        }
  //        else {
  //            _pipeBits.reset();
  //            _pipeBits.set(_ePipeBit);
  //        }
  //    }
  //}
}

void RenderPipeline::blitDeferredRender(std::shared_ptr<CameraNode> cam) {
  //NOTE:
  //Bind the forward framebuffer (_pBlittedForward is equal to _pMsaaForward if MSAA is disabled, if it isn't we call copyMSAASamples later)
  getGraphicsContext()->getShaderMaker()->shaderBound(nullptr);//Unbind and reset shader.
  getGraphicsContext()->glBindFramebuffer(GL_FRAMEBUFFER, _pMsaaForward->getGlId());
  getGraphicsContext()->chkErrDbg();
  getGraphicsContext()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  getGraphicsContext()->chkErrDbg();

  int rasterW = cam->getViewport()->getWidth();
  int rasterH = cam->getViewport()->getHeight();

  _pDeferredShader->beginRaster(rasterW, rasterH);
  {
    //*The clear here isn't necessary. If we're copying all of the contents of the deferred buffer.
    // - Clear the color and depth buffers (back and front buffers not the Mrts)
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (std::shared_ptr<BufferRenderTarget> inf : _pMsaaDeferred->getTargets()) {
      getGraphicsContext()->glActiveTexture(inf->getTextureChannel());
      getGraphicsContext()->chkErrDbg();
      glBindTexture(inf->getTextureTarget(), inf->getTexId());
      getGraphicsContext()->chkErrDbg();

      if (inf->getTargetType() == RenderTargetType::e::Color || inf->getTargetType() == RenderTargetType::e::Shadow) {
        //Don't set depth target.
        _pDeferredShader->setTextureUf(inf->getLayoutIndex());
        getGraphicsContext()->chkErrDbg();
      }

    }

    //Set the light uniform blocks for the deferred shader.
    _pDeferredShader->setLightUf();
    setShadowUf();
    _pDeferredShader->draw(_pQuadMesh);
    getGraphicsContext()->chkErrDbg();
  }
  _pDeferredShader->endRaster();

}
void RenderPipeline::setShadowUf() {
  int32_t iMaxTexs = 0;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iMaxTexs);

  std::vector<GLint> boxSamples;
  std::vector<GLint> frustSamples;

  int iNumGpuShadowBoxes = Gu::getEngineConfig()->getMaxCubeShadowSamples();
  int iNumGpuShadowFrustums = Gu::getEngineConfig()->getMaxFrustShadowSamples();

  int32_t iIndex = 0;
  int32_t iTextureIndex;

  //We loop this way because we MUST fill all texture units in the GPU
  if (getGraphicsContext()->getLightManager()->getGpuShadowBoxes().size() > iNumGpuShadowBoxes) {
    BroLogWarnCycle("More than " + iNumGpuShadowBoxes + " boxes - some shadows will not show.");
  }
  for (int iShadowBox = 0; iShadowBox < iNumGpuShadowBoxes; ++iShadowBox) {
    iTextureIndex = _pMsaaDeferred->getNumNonDepthTargets() + iIndex;
    if (iTextureIndex < iMaxTexs) {
      GLuint texId = getGraphicsContext()->getTexCache()->getDummy1x1TextureCube();;
      if (iShadowBox < getGraphicsContext()->getLightManager()->getGpuShadowBoxes().size()) {
        std::shared_ptr<ShadowBox> pBox = getGraphicsContext()->getLightManager()->getGpuShadowBoxes()[iShadowBox];
        if (pBox != nullptr) {
          texId = pBox->getGlTexId();
        }
      }
      boxSamples.push_back(iTextureIndex);
      getGraphicsContext()->glActiveTexture(GL_TEXTURE0 + iTextureIndex);
      Gu::checkErrorsDbg();
      glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
      Gu::checkErrorsDbg();
      iIndex++;
    }
    else {
      BroLogWarn("Deferred Step: Too many textures bound: " + iTextureIndex);
    }
  }
  _pDeferredShader->setUf("_ufShadowBoxSamples", boxSamples.data(), (GLint)boxSamples.size());
  Gu::checkErrorsDbg();

  //We loop this way because we MUST fill all texture units in the GPU
  if (getGraphicsContext()->getLightManager()->getGpuShadowBoxes().size() > iNumGpuShadowFrustums) {
    BroLogWarnCycle("More than " + iNumGpuShadowFrustums + " frustum - some shadows will not show.");
  }
  for (int iShadowFrustum = 0; iShadowFrustum < iNumGpuShadowFrustums; ++iShadowFrustum) {
    iTextureIndex = _pMsaaDeferred->getNumNonDepthTargets() + iIndex;
    if (iTextureIndex < iMaxTexs) {
      GLuint texId = getGraphicsContext()->getTexCache()->getDummy1x1Texture2D();
      if (iShadowFrustum < getGraphicsContext()->getLightManager()->getGpuShadowFrustums().size()) {
        std::shared_ptr<ShadowFrustum> pFrust = getGraphicsContext()->getLightManager()->getGpuShadowFrustums()[iShadowFrustum];
        if (pFrust != nullptr) {
          texId = pFrust->getGlTexId();
        }
      }
      frustSamples.push_back(iTextureIndex);
      getGraphicsContext()->glActiveTexture(GL_TEXTURE0 + iTextureIndex);
      Gu::checkErrorsDbg();
      glBindTexture(GL_TEXTURE_2D, texId);
      Gu::checkErrorsDbg();
      iIndex++;
    }
    else {
      BroLogWarn("Deferred Step: Too many textures bound: " + iTextureIndex);
    }
  }
  _pDeferredShader->setUf("_ufShadowFrustumSamples", frustSamples.data(), (GLint)frustSamples.size());
  Gu::checkErrorsDbg();

  //Set Mirror Environment map.
  if (_pEnvTex != nullptr) {
    iTextureIndex = _pMsaaDeferred->getNumNonDepthTargets() + iIndex;
    if (iTextureIndex < iMaxTexs) {
      getGraphicsContext()->glActiveTexture(GL_TEXTURE0 + iTextureIndex);
      glBindTexture(GL_TEXTURE_2D, _pEnvTex->getGlId());
      _pDeferredShader->setUf("_ufTexEnv0", (GLvoid*)&iTextureIndex);
      iIndex++;
    }
    else {
      BroLogWarn("Deferred Step: Too many textures bound: " + iTextureIndex);
    }
  }
  else {
    BroLogWarn("You didn't set the enviro texture.");
    Gu::debugBreak();
  }
}
DOFFbo::DOFFbo(std::shared_ptr<GLContext> ct, int32_t w, int32_t h) : HasGraphicsContext(ct) {
  //Create Quick FBO
  getGraphicsContext()->glGenFramebuffers(1, &_uiDOFFboId);
  Gu::checkErrorsDbg();
  getGraphicsContext()->glBindFramebuffer(GL_FRAMEBUFFER, _uiDOFFboId);
  Gu::checkErrorsDbg();
  getGraphicsContext()->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, w);
  Gu::checkErrorsDbg();
  getGraphicsContext()->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, h);
  Gu::checkErrorsDbg();
  getGraphicsContext()->glActiveTexture(GL_TEXTURE0);
  Gu::checkErrorsDbg();

  //Texurev
  glGenTextures(1, &_uiTexId0);
  glBindTexture(GL_TEXTURE_2D, _uiTexId0);
  Gu::checkErrorsDbg();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
  Gu::checkErrorsDbg();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  Gu::checkErrorsDbg();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  Gu::checkErrorsDbg();
  glBindTexture(GL_TEXTURE_2D, 0);

}
DOFFbo::~DOFFbo() {
  getGraphicsContext()->glDeleteFramebuffers(1, &_uiDOFFboId);
  glDeleteTextures(1, &_uiTexId0);
}
void RenderPipeline::postProcessDOF(std::shared_ptr<CameraNode> pCam) {
  //If MSAA is enabled downsize the MSAA buffer to the _pBlittedForward buffer so we can execute post processing.
  //copyMsaaSamples(_pMsaaForward, _pBlittedForward);


  std::shared_ptr<ShaderBase> pDofShader = getGraphicsContext()->getShaderMaker()->getDepthOfFieldShader();
  
  if (pDofShader == nullptr || pCam == nullptr) {
    BroLogErrorCycle("Error: nullptrs 348957");
    return;
  }
  vec3 pos = pCam->getFinalPos();
  std::shared_ptr<BufferRenderTarget> rtPos = _pMsaaDeferred->getTargetByName("Position");
  std::shared_ptr<BufferRenderTarget> rtColor = _pMsaaForward->getTargetByName("Color");//**Note** Forward

  int rasterW = pCam->getViewport()->getWidth();
  int rasterH = pCam->getViewport()->getHeight();

  //Blend color + position and store it in the color.
  pDofShader->beginRaster(rasterW,rasterH);
  {
    //This could be removed if we used Texture2DSpec for the RenderTarget texturs..
    GLuint i0;
    getGraphicsContext()->glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rtPos->getGlTexId());
    i0 = 0;
    pDofShader->setUf("_ufTextureId_i0", (GLvoid*)&i0);

    //This could be removed if we used Texture2DSpec for the RenderTarget texturs..
    getGraphicsContext()->glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, rtColor->getGlTexId());
    i0 = 1;
    pDofShader->setUf("_ufTextureId_i1", (GLvoid*)&i0);

    //Camera params
    //pDofShader->setUf("_fFocalDepth", (GLvoid*)&getGraphicsContext()->getLightManager()->getDeferredParams()->_fFocalDepth);
    pDofShader->setUf("_fFocalRange", (GLvoid*)&getGraphicsContext()->getLightManager()->getDeferredParams()->_fFocalRange);
    //View pos
    pDofShader->setUf("_ufViewPos", (void*)&pos);

    //Draw Round 1
    getGraphicsContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pDOFFbo->_uiDOFFboId);
    getGraphicsContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _pDOFFbo->_uiTexId0, 0);
    GLint horiz = 0;
    pDofShader->setUf("_ufHorizontal", (GLvoid*)&horiz);
    pDofShader->draw(_pQuadMesh);
    Gu::checkErrorsDbg();

    //Draw Round 2
    //Bind rtColor back to the output.
    getGraphicsContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pDOFFbo->_uiDOFFboId);
    getGraphicsContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtColor->getGlTexId(), 0);

    //Bind the previously rendered color to the color buffer so we can pingpong it back.
    getGraphicsContext()->glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _pDOFFbo->_uiTexId0);

    i0 = 1;
    pDofShader->setUf("_ufTextureId_i1", (GLvoid*)&i0);

    horiz = 1;
    pDofShader->setUf("_ufHorizontal", (GLvoid*)&horiz);
    pDofShader->draw(_pQuadMesh);

    Gu::checkErrorsDbg();
  }
  pDofShader->endRaster();
  Gu::checkErrorsDbg();

  //Unbind / Delete
  getGraphicsContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  getGraphicsContext()->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  getGraphicsContext()->glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

}
void RenderPipeline::endRenderAndBlit(std::shared_ptr<RenderTarget> target) {
  //Blit the render to the given target.

  //Do not bind anything - default framebuffer.
  if (target->render) {
    getGraphicsContext()->getShaderMaker()->shaderBound(nullptr);//Unbind and reset shader.
    getGraphicsContext()->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    getGraphicsContext()->chkErrDbg();
    getGraphicsContext()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    getGraphicsContext()->chkErrDbg();
  }
  else {
    BroThrowNotImplementedException();
  }

  // - Clear the color and depth buffers (back and front buffers not the Mrts)
  glClearColor(getClear().x, getClear().y, getClear().z, getClear().w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  getGraphicsContext()->chkErrDbg();

  getGraphicsContext()->glActiveTexture(GL_TEXTURE0);
  getGraphicsContext()->chkErrDbg();
  glBindTexture(GL_TEXTURE_2D, _pBlittedForward->getGlColorBufferTexId());
  getGraphicsContext()->chkErrDbg();

  int iW  = pCam->getViewport()->getWidth();
  int iH  = pCam->getViewport()->getHeight();

  _pForwardShader->beginRaster(iW,iH);
  {
    saveScreenshot();
    getGraphicsContext()->chkErrDbg();

    debugForceSetPolygonMode();
    getGraphicsContext()->chkErrDbg();

    _pForwardShader->setTextureUf(0);
    getGraphicsContext()->chkErrDbg();

    //draw a screen quad to the default OpenGL framebuffer
    _pForwardShader->draw(_pQuadMesh);
    getGraphicsContext()->chkErrDbg();
  }
  _pForwardShader->endRaster();

}
void RenderPipeline::createQuadMesh(int w, int h) {

  //    DEL_MEM(_pQuadMesh);
  _pQuadMesh = nullptr;
  _pQuadMesh = MeshUtils::createScreenQuadMesh(w, h);
}
void RenderPipeline::checkDeviceCaps(int iWidth, int iHeight) {
  //TODO: later we'll create this async.
  //  Gd::verifyRenderThread();

  //Make sure we have enough render targets
  GLint iMaxDrawBuffers;
  GLint iMaxFbWidth;
  GLint iMaxFbHeight;
  //TODO : check to see if this includes a depth MRT
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, (GLint*)&iMaxDrawBuffers);

  if (iMaxDrawBuffers < 1) {
    BroThrowException(Stz "[RenderPipe] Your GPU only supports " + iMaxDrawBuffers +
      " MRTs, the system requires at least " + 1 +
      " MRTs. Consider upgrading graphics card.");
  }

  glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, (GLint*)&iMaxFbWidth);
  glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, (GLint*)&iMaxFbHeight);

  if (iMaxFbHeight < iHeight || iMaxFbWidth < iWidth) {
    BroThrowException(Stz "[RenderPipe] Your GPU only supports MRTs at " +
      iMaxFbWidth + "x" + iMaxFbHeight +
      " pixels. The system requested " + iWidth + "x" + iHeight + ".");
  }

  checkMultisampleParams();
}
void RenderPipeline::checkMultisampleParams() {
  GLint iMaxSamples;
  glGetIntegerv(GL_MAX_SAMPLES, &iMaxSamples);
  BroLogInfo(Stz "Max OpenGL MSAA Samples " + iMaxSamples);

  if (_bMsaaEnabled) {
    if ((int)_nMsaaSamples > iMaxSamples) {
      BroLogWarn(Stz "[RenderPipe] MSAA sample count of '" + _nMsaaSamples +
        "' was larger than the card's maximum: '" + iMaxSamples + "'. Truncating.");
      _nMsaaSamples = iMaxSamples;
      Gu::debugBreak();
    }
    if (BitHacks::bitcount(_nMsaaSamples) != 1) {
      BroLogWarn(Stz "[RenderPipe] Error, multisampling: The number of samples must be 2, 4, or 8.  Setting to 2.");
      _nMsaaSamples = iMaxSamples > 2 ? 2 : iMaxSamples;
      Gu::debugBreak();
    }
  }
}
void RenderPipeline::debugForceSetPolygonMode() {

#ifdef __WINDOWS__
#ifdef _DEBUG
  //These must be #ifdef out because glPolygonMOde is not present in gl330 core 
  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_FILL);
#endif
#endif
}
void RenderPipeline::releaseFbosAndMesh() {
  getGraphicsContext()->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  _pBlittedDeferred = nullptr;
  _pBlittedForward = nullptr;
  _pBlittedDepth = nullptr;
  if (_bMsaaEnabled) {
    _pMsaaForward = nullptr;
    _pMsaaDeferred = nullptr;
    _pMsaaDepth = nullptr;
  }

  _pShadowBoxFboMaster = nullptr;
  _pShadowFrustumMaster = nullptr;
}

void RenderPipeline::renderScene(std::shared_ptr<RenderTarget> target, std::shared_ptr<Scene> pScene, PipeBits pipeBits) {
  //Re-Init
  std::shared_ptr<CameraNode> cam = pScene->getActiveCamera();

  if (cam == nullptr) {
    BroLogErrorOnce("Camera was not set for renderScene");
    return;
  }
  if (cam->getViewport() == nullptr) {
    BroLogErrorOnce("Camera Viewport was not set for renderScene");
    return;
  }
  std::shared_ptr<RenderViewport> pv = cam->getViewport();
  if (pv->getWidth() != _iLastWidth || pv->getHeight() != _iLastHeight) {
    init(pv->getWidth(), pv->getHeight(), "");
  }


  //Only render one thing at a tiem to prevent corrupting the pipe
  if (_bRenderInProgress == true) {
    BroLogError("Tried to render something while another render was currently in progress.");
    return;
  }

  _bRenderInProgress = true;
  {
    RenderParams rp;
    //rp->setWindow(gw);

    //This doesn't conform ot the new ability to render individual objects.
    //This draws all scene shadows
    //If we want shadows in the thumbnails, we'll need to fix this to be able to render
    //individual object shadows
    beginRenderShadows();
    {
      //**This shouldn't be an option ** 
      //Curetly we update lights at the same time as shadows.  this is erroneous
      if (pipeBits.test(PipeBit::e::Shadow)) {
        renderShadows(cam);
      }
    }
    endRenderShadows();

    _pMsaaForward->clearFb();

    //1. 3D rendering
    if (pipeBits.test(PipeBit::e::Deferred)) {
      beginRenderDeferred();
      {
        pScene->drawDeferred(rp);
      }
      endRenderDeferred();

      //Blit to forward FB
      blitDeferredRender(cam);

      if (pipeBits.test(PipeBit::e::Transparent)) {
        pScene->drawTransparent(rp);
      }

      //Do post processing

    }
    /*

    //Do post processing
    postProcessDeferredRender();
    }

    if (pipeBits.test(PipeBit::e::Forward)) {
    beginRenderForward();
    {
    toDraw->drawForward(rp);

    if (pipeBits.test(PipeBit::e::Debug)) {
    toDraw->drawDebug(rp);
    }
    if (pipeBits.test(PipeBit::e::NonDepth)) {
    //UI
    toDraw->drawNonDepth(rp);
    }
    }
    endRenderForward();
    }
    */
    //2. Forward Rendering
    if (pipeBits.test(PipeBit::e::Forward)) {
      beginRenderForward();

      pScene->drawForward(rp);

      //2.1 - Debug
      if (pipeBits.test(PipeBit::e::Debug)) {
        pScene->drawDebug(rp);
      }

      //**TODO: DOF should be a pipe bit.
      //2.2 - DOF
      if (pipeBits.test(PipeBit::e::DepthOfField)) {
        postProcessDOF();
      }

      //Rebind Forward FBO
      beginRenderForward();
      //  Gu::getGui()->debugForceLayoutChanged();

      //3. Orthographic, Behind the UI 
      if (pipeBits.test(PipeBit::e::NonDepth)) {
        pScene->drawNonDepth(rp);
      }

      //4. The UI
      if (pipeBits.test(PipeBit::e::UI_Overlay)) {
        Gu::getUIManager()->update(Gu::getInputManager());
        Gu::getUIManager()->drawForward();
      }

      endRenderForward();
    }
    //
    //      beginRenderTransparent();
    //      {
    //          //TP uses the forward texture.
    //        //   toDraw->drawTransparent(rp);
    //      }
    //      endRenderTransparent();

    //5. Blit
    if (pipeBits.test(PipeBit::e::BlitFinal)) {
      endRenderAndBlit();
    }
  }
  _bRenderInProgress = false;
}
std::shared_ptr<Img32> RenderPipeline::getResultAsImage() {
  std::shared_ptr<Img32> bi = std::make_shared<Img32>();

  //so wer'e having problem rendering the alpha of rthe forward buffer
  //its probably a trivial bug, but my brain is dead once again
  //so i'm just exporting the colors of the deferred for now

  std::shared_ptr<BufferRenderTarget> pTarget;
  pTarget = _pBlittedForward->getTargetByName("Color");
  if (RenderUtils::getTextureDataFromGpu(bi, pTarget->getGlTexId(), GL_TEXTURE_2D) == true) {
    //the GL tex image must be flipped to show upriht/
  }

  //Only transparent method that's working
// pTarget = _pBlittedDeferred->getTargetByName("Color");
// if (RenderUtils::getTextureDataFromGpu(bi, pTarget->getGlTexId(), GL_TEXTURE_2D) == true) {
//     //the GL tex image must be flipped to show upriht/
// }


  return bi;
}



}//ns game
