#include "../base/Stopwatch.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../math/BoxUtils.h"
#include "../gfx/ShadowFrustum.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/LightManager.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/LightNode.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/GraphicsContext.h"
#include "../model/MeshNode.h"
#include "../world/RenderBucket.h"
#include "../world/NodeUtils.h"
#include "../world/PhysicsWorld.h"

namespace BR2 {
#pragma region ShadowFrustum_Internal
class ShadowFrustum_Internal {
public:
  std::shared_ptr<LightNodeDir> _pLightSource = nullptr;
  vec3 _vCachedLastPos;
  float _fCachedLastRadius;

  float _fSmallBoxSize = 0.2f;
  GLuint _glFrameBufferId = 0;
  GLuint _glDepthTextureId;
  GLuint _glShadowMapId = 0;
  uint32_t _iFboWidthPixels;
  uint32_t _iFboHeightPixels;
  std::shared_ptr<MeshNode> _pScreenQuadMesh = nullptr;

  int32_t _iShadowFrustumId;
  std::shared_ptr<FrustumBase> _pFrustum = nullptr;
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
  mat4 _projMatrix;    // Frustum, Proj, View - basic camera parameters
  mat4 _viewMatrix;    //
  mat4 _PVB;
  std::shared_ptr<RenderBucket> _pVisibleSet;
  std::shared_ptr<RenderBucket> _pLastSet;//TODO: check vs current set _bShadowDirty in BvhNOde
  //bool _bMustUpdate;

  ShadowFrustum_Internal(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight);
  ~ShadowFrustum_Internal();
  void deleteFbo();
  void createFbo();
  void cullObjectsAsync(CullParams& rp);
  void updateView();
  void copyAndBlendToShadowMap(std::shared_ptr<ShadowFrustum> pBox);
};
ShadowFrustum_Internal::ShadowFrustum_Internal(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight) {
  _vCachedLastPos = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
  _fCachedLastRadius = FLT_MAX;
  _iFboHeightPixels = iFboHeight;
  _iFboWidthPixels = iFboWidth;
  _pLightSource = pLightSource;

  //used for interpolating frames.
  static int32_t idGen = 0;
  _iShadowFrustumId = ++idGen;
}
ShadowFrustum_Internal::~ShadowFrustum_Internal() {
  deleteFbo();
}
void ShadowFrustum_Internal::updateView() {
  AssertOrThrow2(_pLightSource != nullptr);

  //TODO: like in the shadowbox, side, we might want to add debug padding.
  _pViewport->updateBox(0,0,_iFboWidthPixels, _iFboHeightPixels);
  _pViewport->bind();

  float fNear = _pFrustum->getZNear();
  float fFar = _pFrustum->getZFar();

  vec3 dir = _pLightSource->getDir();
  vec3 pos = _pLightSource->getFinalPos();
  vec3 up = _pLightSource->getUp();

  //Calculate geometrric frustum
  //const float tanfov90_2 = tanf(MathUtils::degToRad(90 / 2));//90 degree field of view for a box
  _pFrustum->update(
    dir
    , pos
    , up
    , ProjectionMode::e::Perspective
  );

  //calc view matrix
  vec3 vLookAt = pos + dir;
  _viewMatrix.lookAt(
    pos
    , vLookAt
    , up);

  _projMatrix = _pFrustum->getProjectionMatrix();
  vec4 vProj;
  static int nnn = 0;
  float f[] = {
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0 };
  mat4 mBias = mat4(f);
  // if(nnn==0)
  _PVB = _projMatrix * _viewMatrix * mBias;;//= _pFrustum->getProjectionMatrix();
 // if (nnn == 1)
  _PVB = _viewMatrix * _projMatrix * mBias;//= _pFrustum->getProjectionMatrix();
  _PVB = mBias * _projMatrix * _viewMatrix;//= _pFrustum->getProjectionMatrix();
  _PVB = mBias * _viewMatrix * _projMatrix;//= _pFrustum->getProjectionMatrix();
  static int nx = 0;
  if (nx == 1) {
    _PVB.transpose();
  }
  if (false) {
    vec4 t = vec4(pos + dir * 100, 1);
    t = _PVB * t;
    int nn = 0;
    nn++;
  }
  // if (nnn == 2)
  //     vProj = _projMatrix.transpose() * _viewMatrix.transpose() *  vec4(vLookAt, 1);//= _pFrustum->getProjectionMatrix();

  int n = 0;
  n++;
  //float vpWidth_2 = _pFrustum->getTanFov2()*fNear;
  //float arat_1 = _pViewport->getAspectRatio_1();
  //float vw = vpWidth_2;
  //float vh = vpWidth_2*arat_1;
  //_projMatrix = mat4::getProjection(
  //    fNear, fFar,
  //    vw, -vw,
  //    -vh, vh
  //);

//  static int n=1;
//    if(n==0)
  //  _viewProjMatrix = _projMatrix * _viewMatrix;
    //if(n==1)
    //_viewProjMatrix = _viewMatrix * _projMatrix;
    //if(n==2)
    //    _viewProjMatrix = mat4(_projMatrix * _viewMatrix).transposed();
    //if(n==3)
    //_viewProjMatrix = mat4(_viewMatrix * _projMatrix).transposed();
}
void ShadowFrustum_Internal::cullObjectsAsync(CullParams& cp) {
  AssertOrThrow2(_pLightSource != nullptr);
  AssertOrThrow2(_pVisibleSet != nullptr);

  _pVisibleSet->clear(cp.getCamera());

  std::shared_ptr<PhysicsWorld> physics = NodeUtils::getPhysicsWorld(_pLightSource);

  BvhCollectionParams p;
  p._fMaxDist = MathUtils::brMin(_pFrustum->getZFar(), Gu::getEngineConfig()->getMaxPointLightShadowDistance());
  p._pFrustum = _pFrustum;
  p._pRenderBucket = _pVisibleSet;
  p._pVisibleCamera = cp.getCamera();
  physics->collectVisibleNodes(&p);

  ////Loop through objects and find whether they have changed since last update.
  //_bMustUpdate = false;

  //if (_pVisibleSet->getGrids().size() > 0 || _pVisibleSet->getObjs().size() > 0) {
  //  _bMustUpdate = true;
  //}
}
void ShadowFrustum_Internal::createFbo() {
  deleteFbo();

  RenderUtils::debugGetRenderState();
  // Create the depth buffer
  RenderUtils::createDepthTexture(&_glDepthTextureId, _iFboWidthPixels, _iFboHeightPixels, false, 0, GL_DEPTH_COMPONENT24);

  //Bind framebuffer and attach depth texture.
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glGenFramebuffers(1, &_glFrameBufferId);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_FRAMEBUFFER, _glFrameBufferId);
  Gu::checkErrorsRt();

  // Create the cube map
  glGenTextures(1, &_glShadowMapId);
  glBindTexture(GL_TEXTURE_2D, _glShadowMapId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);//GL_NONE
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  Gu::checkErrorsRt();

  glTexImage2D(GL_TEXTURE_2D, 0, SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT,
    _iFboWidthPixels, _iFboHeightPixels, 0, SHADOW_CUBE_MAP_TEX_FORMAT, SHADOW_CUBE_MAP_TEX_TYPE, nullptr);
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _glShadowMapId, 0);
  Gu::checkErrorsRt();
  // std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glSide, _glShadowCubeMapId, 0);

  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _glDepthTextureId, 0);
  Gu::checkErrorsRt();

  glBindTexture(GL_TEXTURE_2D, 0);
  Gu::checkErrorsRt();

  // Disable writes to the color buffer
  glDrawBuffer(GL_NONE);

  // Disable reads from the color buffer
  glReadBuffer(GL_NONE);
  Gu::checkErrorsRt();

  GLenum status = std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    BRThrowException("Point Light Shadow Map Framebuffer encountered an error during setup: " + status);
    Gu::checkErrorsRt();
  }

  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  Gu::checkErrorsRt();
}
void ShadowFrustum_Internal::deleteFbo() {
  if (_glFrameBufferId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteFramebuffers(1, &_glFrameBufferId);
  }
  if (_glDepthTextureId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteTextures(1, &_glDepthTextureId);
  }
  if (_glShadowMapId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteTextures(1, &_glShadowMapId);
  }
}
void ShadowFrustum_Internal::copyAndBlendToShadowMap(std::shared_ptr<ShadowFrustum> pBox) {
  //Here we can do shadowmap operations.

  if (pBox->getGlTexId() != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glCopyImageSubData(
      _glShadowMapId
      , GL_TEXTURE_2D
      , 0
      , 0, 0, 0 //xyZ
      , pBox->getGlTexId()
      , GL_TEXTURE_2D
      , 0
      , 0, 0, 0
      , _iFboWidthPixels
      , _iFboHeightPixels
      , 0
    );
    Gu::checkErrorsDbg();
  }
}
#pragma endregion

#pragma region ShadowFrustum
ShadowFrustum::ShadowFrustum(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight) {
  _pint = std::make_unique<ShadowFrustum_Internal>(pLightSource, iFboWidth, iFboHeight);
}
ShadowFrustum::~ShadowFrustum() {
  _pint = nullptr;
}

std::shared_ptr<FrustumBase> ShadowFrustum::getFrustum() { return _pint->_pFrustum; }
GLint ShadowFrustum::getGlTexId() { return _pint->_glShadowMapId; }
mat4* ShadowFrustum::getViewMatrixPtr() { return &_pint->_viewMatrix; }
mat4* ShadowFrustum::getProjMatrixPtr() { return &_pint->_projMatrix; }
mat4* ShadowFrustum::getPVBPtr() { return &_pint->_PVB; }
void ShadowFrustum::setChanged() {
  _pint->_vCachedLastPos.construct(FLT_MAX, FLT_MAX, FLT_MAX);
}

void ShadowFrustum::init() {
  //**TODO: updated viewport, make sure it works with this.
  _pint->_pViewport = std::make_shared<RenderViewport>(60, 60, ViewportConstraint::Full);
  _pint->_pVisibleSet = std::make_shared<RenderBucket>();
  _pint->_pFrustum = std::make_shared<FrustumBase>(_pint->_pViewport, 90.0f);

  _pint->createFbo();

  Gu::checkErrorsRt(); //Rt error check - this is called only once.
}
void ShadowFrustum::updateAndCullAsync(CullParams& cp) {
  static int n = 0;
  if (n == 1) { return; }
  AssertOrThrow2(_pint->_pLightSource != nullptr);
  // 2 optimizations here
  // frame modulus
  // checking for changed objects (per-side)
  bool bMustUpdate = false;
  bool bForceUpdate = false;
  int iStartDebug = 0;

  //Note: even better performance would be to poll object
  //positions - if nothing moved then don't update the shadowmap.
  //TEST - update every x frames. usign ID allows sequential frame updates for shadow boxes.
  //if (!Gu::getFpsMeter()->frameMod(1 + _iShadowFrustumId)) {
  //    return;
  //}

  //std::shared_ptr<LightManager> pLightMan = _pint->_pLightSource->getLightManager();

  //Update the camera for each ShadowFrustum side if the light has changed position or radius.
  //See also: debugInvalidateAllLightProjections
//   if (_vCachedLastPos != _pLightSource->getFinalPos() ) {
//       _vCachedLastPos = _pLightSource->getFinalPos();

   //it's skippin this..ujust whatever..
  _pint->updateView();

  //       bForceUpdate = true;
  //       bMustUpdate = true;
  //   }

     // Collect all objects for each frustum.
     // if objects don't change don't render that frustum (bMustUpate)

  _pint->cullObjectsAsync(cp);

  //If we don't need to update then return.
//   if (bMustUpdate == false){
//       return;

//   }

   // Tell the viewport we've changed

   // Copy and blend this into
}

void ShadowFrustum::debugRender(RenderParams& rp) {
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pFrustum, Color4f(1, 0, 0, 1));
}

void ShadowFrustum::beginRenderShadowFrustum() {
  //Gd::verifyRenderThread();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pint->_glFrameBufferId);
  Gu::checkErrorsDbg();

  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  Gu::checkErrorsDbg();

  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  Gu::checkErrorsDbg();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _pint->_glShadowMapId, 0);

  //if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
  //    glClearColor(0, 0, 0, 1);//This could be moved out of here.
  //}
  //else {
  glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);//This could be moved out of here.
//    }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Gu::checkErrorsDbg();
}
void ShadowFrustum::renderShadows(std::shared_ptr<ShadowFrustum> pShadowFrustumMaster) {
  // Update the shadow box.
  //pShadowFrustumMaster->beginRenderShadowFrustum();
  beginRenderShadowFrustum();
  {
    vec3 vFinal = *_pint->_pLightSource->getFinalPosPtr();

    _pint->_pVisibleSet->sortAndDrawMeshes(
      [](std::shared_ptr<VertexFormat> vf) {
        return Gu::getShaderMaker()->getShadowShader(vf);
      },
      [&](std::shared_ptr<ShaderBase> sb) {
        sb->bind();
        sb->setUf("_ufProj", (void*)&_pint->_projMatrix, 1, false);
        sb->setUf("_ufView", (void*)&_pint->_viewMatrix, 1, false);
        //TODO: - technically we need to sendt he light's model matrix into the shader as well.
        sb->setUf("_ufShadowLightPos", (void*)&vFinal, 1, false);
      },
        [](std::shared_ptr<ShaderBase> sb, std::shared_ptr<MeshNode> bn) {
        RenderParams rp;
        rp.setShader(sb);
        bn->drawShadow(rp);
      }
      );
  }
  endRenderShadowFrustum();
  //pShadowFrustumMaster->endRenderShadowFrustum();

  // pShadowFrustumMaster->copyAndBlendToShadowMap(shared_from_this());
}
void ShadowFrustum::endRenderShadowFrustum() {
  //Gd::verifyRenderThread();
  glBindTexture(GL_TEXTURE_2D, 0);
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  Gu::checkErrorsDbg();
}

#pragma endregion
}//ns Game