#include "../base/Stopwatch.h"
#include "../base/FpsMeter.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Perf.h"
#include "../math/BoxUtils.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBoxSide.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/LightNode.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderSettings.h"
#include "../world/RenderBucket.h"
#include "../model/MeshUtils.h"

namespace BR2 {
#pragma region ShadowBox_Internal
class ShadowBox_Internal {
public:
  int64_t _iShadowBoxId;
  std::shared_ptr<ShadowBoxSide> _pShadowBoxSide[6] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
  std::shared_ptr<LightNodePoint> _pLightSource = nullptr;
  vec3 _vCachedLastPos;
  float _fCachedLastRadius;

  float _fSmallBoxSize = 0.2f;
  GLuint _glFrameBufferId = 0;
  GLuint _glDepthTextureId;
  GLuint _glShadowCubeMapId = 0;
  uint32_t _iFboWidthPixels;
  uint32_t _iFboHeightPixels;
  Box2f _screenQuadPos;
  Box2f _screenQuadTCoords;
  std::shared_ptr<MeshNode> _pScreenQuadMesh = nullptr;
  bool _bMustUpdate = false;
  bool _bForceUpdate = false;
  bool _bShadowMapEnabled = false;

  ShadowBox_Internal(std::shared_ptr<LightNodePoint> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadowMapEnabled);
  ~ShadowBox_Internal();
  void deleteFbo();
  void createFbo();
  //void updateLightMatrix();
  void smoothShadows();

};
ShadowBox_Internal::ShadowBox_Internal(std::shared_ptr<LightNodePoint> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadowMapEnabled) {
  _bShadowMapEnabled = bShadowMapEnabled;
  _vCachedLastPos = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
  _fCachedLastRadius = FLT_MAX;
  _iFboHeightPixels = iFboHeight;
  _iFboWidthPixels = iFboWidth;
  _pLightSource = pLightSource; ;

  //used for interpolating frames.
  static int64_t idGen = 0;
  _iShadowBoxId = ++idGen;
  
}
ShadowBox_Internal::~ShadowBox_Internal() {
  deleteFbo();
  for (int i = 0; i < 6; ++i) {
    _pShadowBoxSide[i] = nullptr;
  }
}
void ShadowBox_Internal::createFbo() {
  if (_bShadowMapEnabled == false) {
    return;
  }
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
  glGenTextures(1, &_glShadowCubeMapId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _glShadowCubeMapId);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE); 
  //THe following parameters are for depth textures only
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);// ** This is if you sample the depth map, however we're sampling a color buffer.
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
  //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  Gu::checkErrorsRt();


  //Note make sure these match the other FBOs
  for (int i = 0; i < 6; ++i) {
    //Note values here must match ShadowBox due to glCopyTexSubImage
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT,
      _iFboWidthPixels, _iFboHeightPixels, 0, SHADOW_CUBE_MAP_TEX_FORMAT, SHADOW_CUBE_MAP_TEX_TYPE, nullptr);
  }
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, _iFboWidthPixels, _iFboHeightPixels, 0, GL_RED, GL_FLOAT, nullptr);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, _iFboWidthPixels, _iFboHeightPixels, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  Gu::checkErrorsRt();


  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _glShadowCubeMapId, 0);
  Gu::checkErrorsRt();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _glDepthTextureId, 0);
  Gu::checkErrorsRt();


  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
void ShadowBox_Internal::deleteFbo() {
  if (_bShadowMapEnabled == false) {
    return;
  }

  if (_glFrameBufferId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteFramebuffers(1, &_glFrameBufferId);
  }
  if (_glDepthTextureId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteTextures(1, &_glDepthTextureId);
  }
  if (_glShadowCubeMapId != 0) {
    std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glDeleteTextures(1, &_glShadowCubeMapId);
  }

}
void ShadowBox_Internal::smoothShadows() {
  //if (Gu::getRenderSettings()->getSmoothShadows()) {

  //    std::shared_ptr<CameraNode> pCam = Gu::getCamera();
  //                                                                                    
  //    //Create Quick FBO
  //    GLuint uiBLendFboId;
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glGenFramebuffers(1, &uiBLendFboId);
  //    Gu::checkErrorsDbg();
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glBindFramebuffer(GL_FRAMEBUFFER, uiBLendFboId);
  //    Gu::checkErrorsDbg();
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, _iFboWidthPixels);
  //    Gu::checkErrorsDbg();
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, _iFboHeightPixels);
  //    Gu::checkErrorsDbg();
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glActiveTexture(GL_TEXTURE0);
  //    Gu::checkErrorsDbg();

  //    //Texure
  //    GLuint uiTexId0;
  //    glGenTextures(1, &uiTexId0);
  //    glBindTexture(GL_TEXTURE_2D, uiTexId0);
  //    Gu::checkErrorsDbg();
  //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _iFboWidthPixels, _iFboHeightPixels, 0, GL_RGBA, GL_FLOAT, nullptr);
  //    Gu::checkErrorsDbg();
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //    Gu::checkErrorsDbg();
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //    Gu::checkErrorsDbg();
  //    glBindTexture(GL_TEXTURE_2D, 0);



  //    //Unbind / Delete
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glActiveTexture(GL_TEXTURE0);
  //    glBindTexture(GL_TEXTURE_2D, 0);
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glActiveTexture(GL_TEXTURE1);
  //    glBindTexture(GL_TEXTURE_2D, 0);
  //    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glDeleteFramebuffers(1, &uiBLendFboId);
  //    glDeleteTextures(1, &uiTexId0);
  //    Gu::checkErrorsDbg();
  //}
}


#pragma endregion

#pragma region ShadowBox
ShadowBox::ShadowBox(std::shared_ptr<LightNodePoint> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadowMapEnabled) {
  _pint = std::make_unique<ShadowBox_Internal>(pLightSource, iFboWidth, iFboHeight, bShadowMapEnabled);
}
ShadowBox::~ShadowBox() {
  _pint = nullptr;
}
std::shared_ptr<LightNodePoint> ShadowBox::getLightSource() { return _pint->_pLightSource; }
float& ShadowBox::getSmallBoxSize() { return _pint->_fSmallBoxSize; }
uint32_t ShadowBox::getFboWidth() { return _pint->_iFboWidthPixels; }
uint32_t ShadowBox::getFboHeight() { return _pint->_iFboHeightPixels; }
GLuint ShadowBox::getGlTexId() { return _pint->_glShadowCubeMapId; }

void ShadowBox::init() {
  for (int i = 0; i < 6; ++i) {
    _pint->_pShadowBoxSide[i] = std::make_shared<ShadowBoxSide>(shared_from_this(), _pint->_pLightSource, (BoxSide::e)i, _pint->_bShadowMapEnabled);
  }
  _pint->createFbo();

  _pint->_screenQuadPos = Box2f(vec2(0, 0), vec2(0, 0));
  _pint->_screenQuadTCoords = Box2f(vec2(0, 0), vec2(1, 1));

  Gu::checkErrorsRt(); //Rt error check - this is called only once.
}
void ShadowBox::updateAndCullAsync(CullParams& cp) {
  AssertOrThrow2(_pint->_pLightSource != nullptr);
  // 2 optimizations here
  // frame modulus
  // checking for changed objects (per-side)
  int iStartDebug = 0;
  _pint->_bForceUpdate = false;
  //Note: even better performance would be to poll object
  //positions - if nothing moved then don't update the shadowmap.
  //TEST - update every x frames. usign ID allows sequential frame updates for shadow boxes.
  
  //** Don't do any metering in here, we will do this in controlling threads. **
  //if (!Gu::getFpsMeter()->frameMod(1 + _iShadowBoxId)) {
  //  _bMustUpdate = false;
  //  return;
  //}

  std::shared_ptr<LightManager> pLightMan = _pint->_pLightSource->getLightManager();

  //Update the camera for each shadowbox side if the light has changed position or radius.
  //See also: debugInvalidateAllLightProjections
  if (_pint->_vCachedLastPos != _pint->_pLightSource->getFinalPos() || _pint->_fCachedLastRadius != _pint->_pLightSource->getLightRadius()) {
    _pint->_vCachedLastPos = _pint->_pLightSource->getFinalPos();
    _pint->_fCachedLastRadius = _pint->_pLightSource->getLightRadius();

    for (int iFace = iStartDebug; iFace < 6; ++iFace) {
      _pint->_pShadowBoxSide[iFace]->updateView();
    }

    _pint->_bMustUpdate = true;
    _pint->_bForceUpdate = true;
  }

  // Collect all objects for each frustum.
  // if objects don't change don't render that frustum (bMustUpate)
  for (int iFace = iStartDebug; iFace < 6; ++iFace) {
    _pint->_pShadowBoxSide[iFace]->cullObjectsAsync(cp);
    _pint->_bMustUpdate = _pint->_bMustUpdate || _pint->_pShadowBoxSide[iFace]->getMustUpdate();
  }

  // Tell the viewport we've changed
  _pint->_pShadowBoxSide[0]->getViewport()->bind(nullptr);

}
void ShadowBox::renderShadows(std::shared_ptr<ShadowBox> pShadowBoxMaster) {
  Perf::pushPerf();
  if (_pint->_bMustUpdate == false) {
    return;
  }

  int iStartDebug = 0;
  // Update the shadow box.
  pShadowBoxMaster->beginRenderShadowBox();
  //beginRenderShadowBox();
  {
    for (int iFace = iStartDebug; iFace < 6; ++iFace) {
      _pint->_pShadowBoxSide[iFace]->renderShadows(pShadowBoxMaster, _pint->_bForceUpdate);
      //_pShadowBoxSide[iFace]->renderShadows(shared_from_this(), _bForceUpdate);
    }
  }
  //endRenderShadowBox();
  pShadowBoxMaster->endRenderShadowBox();

  // Copy and blend this into 
  pShadowBoxMaster->copyAndBlendToShadowMap(shared_from_this());
  Perf::popPerf();
}

void ShadowBox::debugRender(RenderParams& rp) {
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Right]->getFrustum(), Color4f(1, 0, 0, 1));
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Left]->getFrustum(), Color4f(1, 0, 0, 1));
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Bottom]->getFrustum(), Color4f(0, 1, 0, 1));
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Top]->getFrustum(), Color4f(0, 1, 0, 1));
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Back]->getFrustum(), Color4f(0, 0, 1, 1));
  RenderUtils::drawFrustumShader(rp.getCamera(), _pint->_pShadowBoxSide[BoxSide::Front]->getFrustum(), Color4f(0, 0, 1, 1));
}


void ShadowBox::copyAndBlendToShadowMap(std::shared_ptr<ShadowBox> pBox) {
  if (_pint->_bShadowMapEnabled == false) {
    return;
  }
  //Here we can do shadowmap operations.

  if (pBox->getGlTexId() != 0) {
    if (Gu::getRenderSettings()->getSmoothShadows()) {
      std::shared_ptr<ShaderBase> pDofShader = Gu::getShaderMaker()->getSmoothGenShader();
      std::shared_ptr<MeshNode> pQuadMesh = MeshUtils::createScreenQuadMesh(_pint->_iFboWidthPixels, _pint->_iFboHeightPixels);

      //Blend color + position and store it in the color.
      pDofShader->beginRaster(_pint->_iFboWidthPixels, _pint->_iFboHeightPixels);
      {
        pBox->beginRenderShadowBox();
        for (int iSide = 0; iSide < 6; ++iSide) {
          pBox->beginRenderSide((BoxSide::e)iSide);
          {
            //This could be removed if we used Texture2DSpec for the RenderTarget texturs..
            GLuint i0;
            std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glActiveTexture(GL_TEXTURE0);
            Gu::checkErrorsDbg();
            glBindTexture(GL_TEXTURE_CUBE_MAP, _pint->_glShadowCubeMapId);
            Gu::checkErrorsDbg();
            i0 = 0;
            pDofShader->setUf("_ufTexture0", (GLvoid*)&i0);
            Gu::checkErrorsDbg();

            pDofShader->draw(pQuadMesh);
            Gu::checkErrorsDbg();

          }
          pBox->endRenderSide();

        }
        pBox->endRenderShadowBox();
        Gu::checkErrorsDbg();
      }
      pDofShader->endRaster();
      Gu::checkErrorsDbg();



    }
    else {
      std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glCopyImageSubData(
        _pint->_glShadowCubeMapId//FROM
        , GL_TEXTURE_CUBE_MAP //GL_TEXTURE_CUBE_MAP_POSITIVE_X+iFace
        , 0
        , 0, 0, 0 //xyZ
        , pBox->getGlTexId()//TO
        , GL_TEXTURE_CUBE_MAP //GL_TEXTURE_CUBE_MAP_POSITIVE_X+iFace
        , 0
        , 0, 0, 0
        , _pint->_iFboWidthPixels
        , _pint->_iFboHeightPixels
        , 6
      );
    }



    Gu::checkErrorsDbg();
  }

}
//Basically implementing the screen quead deferred lighting here.
void ShadowBox::updateScreenQuad() {
  //DEL_MEM(_pScreenQuadMesh);
  //_pScreenQuadMesh = MeshUtils::createScreenQuadMesh(
  //    (int)Gu::getViewport()->getWidth(),
  //    (int)Gu::getViewport()->getHeight()
  //);
  //Quad2f c = Gu::getViewport()->getClientQuad();//DirectCast(Gu::GetRenderManager(), GLRenderSystem*)->getViewport()->getClientQuad();
  //_pScreenQuadMesh->updateScreenQuad(&c);
}

void ShadowBox::beginRenderShadowBox() {
  if (_pint->_bShadowMapEnabled == false) {
    return;
  }
  //Gd::verifyRenderThread();
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pint->_glFrameBufferId);
  Gu::checkErrorsDbg();

  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  Gu::checkErrorsDbg();

  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  Gu::checkErrorsDbg();
}
void ShadowBox::beginRenderSide(BoxSide::e side) {
  if (_pint->_bShadowMapEnabled == false) {
    return;
  }
  //Gd::verifyRenderThread();
  GLenum glSide = (GLenum)BoxUtils::cubeSideToGlCubeMapEnum(side);

  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glSide, _pint->_glShadowCubeMapId, 0);

  Gu::checkErrorsDbg();

  //Clear depth MUST BE CALLED EVERY TIME.  Man that sucked.
//    if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
  glClearColor(0, 0, 0, 1);//This could be moved out of here.
//}
//else {
//    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);//This could be moved out of here.
//}
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Gu::checkErrorsDbg();

  /*
  Alternatively, if you want to render the cube in a single pass, you can attach the entire cubemap using
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id, 0);
  The second method is a single pass approach. You bind the entire cubemap as a target, and only perform a single render pass. This method requires a geometry
  shader that will duplicate each primitive six times, projecting it onto all six cubemap
  faces simultaneously (through your six view-projection matrices). The geometry shader
  directs each duplicated primitive to the appropriate cube target face using the gl_Layer variable.
  */
  //Note: we enable/disable everything in the lightmanager before rendering
}
void ShadowBox::endRenderSide() {
  if (_pint->_bShadowMapEnabled == false) {
    return;
  }
  Gu::checkErrorsDbg();

}
void ShadowBox::endRenderShadowBox() {
  if (_pint->_bShadowMapEnabled == false) {
    return;
  }

  //Gd::verifyRenderThread();
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext())->glBindFramebuffer(GL_FRAMEBUFFER, 0);
  Gu::checkErrorsDbg();
}


#pragma endregion

}//ns Game
