#include "../base/Stopwatch.h"
#include "../base/Base.h"

#include "../math/BoxUtils.h"
#include "../gfx/ShadowFrustum.h"

#include "../gfx/FrustumBase.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/LightManager.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/LightNode.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderManager.h"
#include "../model/MeshNode.h"
#include "../world/RenderBucket.h"
#include "../world/PhysicsManager.h"

namespace BR2 {
ShadowFrustum::ShadowFrustum(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadowMapEnabled)
{
    _bShadowMapEnabled = bShadowMapEnabled;
    _vCachedLastPos = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    _fCachedLastRadius = FLT_MAX;
    _iFboHeightPixels = iFboHeight;
    _iFboWidthPixels = iFboWidth;
    _pLightSource = pLightSource;

    //used for interpolating frames.
    static int32_t idGen = 0;
    _iShadowFrustumId = ++idGen;

}
ShadowFrustum::~ShadowFrustum()
{
    deleteFbo();

}
///////////////////////////////////////////////////////////////////
void ShadowFrustum::init() {

    _pViewport = std::make_shared<RenderViewport>(60, 60
        , VIEWPORT_LOCATION::VIEWPORT_CENTER_NONE
        , ViewportConstraint::VP_FILL_WINDOW //VP_DONOTCHANGE
        );

    _pVisibleSet = std::make_shared<RenderBucket>();
    _pFrustum = std::make_shared<FrustumBase>(_pViewport, 90.0f);

    createFbo();


    getContext()->chkErrRt(); //Rt error check - this is called only once.
}
void ShadowFrustum::update()
{
    static int n = 0;
    if (n == 1) { return; }
    AssertOrThrow2(_pLightSource != nullptr);
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

    std::shared_ptr<LightManager> pLightMan = getContext()->getLightManager();

    //Update the camera for each ShadowFrustum side if the light has changed position or radius.
    //See also: debugInvalidateAllLightProjections
 //   if (_vCachedLastPos != _pLightSource->getFinalPos() ) {
 //       _vCachedLastPos = _pLightSource->getFinalPos();

    //it's skippin this..ujust whatever..
    updateView();

    //       bForceUpdate = true;
    //       bMustUpdate = true;
    //   }

       // Collect all objects for each frustum.
       // if objects don't change don't render that frustum (bMustUpate)

    collect();

    //If we don't need to update then return.
 //   if (bMustUpdate == false){
 //       return;
 //   }

    // Tell the viewport we've changed


    // Copy and blend this into 
}

void ShadowFrustum::updateView() {
    AssertOrThrow2(_pLightSource != nullptr);

    _pViewport->setY(0);
    _pViewport->setX(0);
    _pViewport->setWidth(_iFboWidthPixels);
    _pViewport->setHeight(_iFboHeightPixels);
    _pViewport->updateChanged(true);

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
    static int nnn=0;
    float f[] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0 };
    mat4 mBias = mat4(f );
   // if(nnn==0)
    _PVB = _projMatrix * _viewMatrix *  mBias;;//= _pFrustum->getProjectionMatrix();
   // if (nnn == 1)
    _PVB = _viewMatrix * _projMatrix *   mBias;//= _pFrustum->getProjectionMatrix();
    _PVB = mBias * _projMatrix *   _viewMatrix;//= _pFrustum->getProjectionMatrix();
    _PVB = mBias * _viewMatrix *  _projMatrix;//= _pFrustum->getProjectionMatrix();
        static int nx=0;
        if (nx == 1) {
            _PVB.transpose();
        }
        if(false){
            vec4 t = vec4(pos + dir * 100, 1);
            t = _PVB * t;
            int nn=0;
            nn++;
        }
   // if (nnn == 2)
   //     vProj = _projMatrix.transpose() * _viewMatrix.transpose() *  vec4(vLookAt, 1);//= _pFrustum->getProjectionMatrix();

    int n=0;
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
void ShadowFrustum::collect() {
    if (_bShadowMapEnabled == false) {
        return;
    }
    AssertOrThrow2(_pLightSource != nullptr);
    AssertOrThrow2(_pVisibleSet != nullptr);
    // AssertOrThrow2(_pBvhCollectionResults!=nullptr);

    _pVisibleSet->clear();

    if (Gu::getPhysicsWorld() == nullptr) {
        return;
    }

    BvhCollectionParams p;
    p._fMaxDist = MathUtils::broMin(_pFrustum->getZFar(), Gu::getEngineConfig()->getMaxPointLightShadowDistance());
    p._pFrustum = _pFrustum;
    p._pRenderBucket = _pVisibleSet;
    Gu::getPhysicsWorld()->collectVisibleNodes(&p);

    //////////////////////////////////////////////////////////////////////////
    //Loop through objects and find whether they have changed since last update.
    _bMustUpdate = false;

    if (_pVisibleSet->getGrids().size() > 0 || _pVisibleSet->getObjs().size() > 0) {
        _bMustUpdate = true;
    }
}
void ShadowFrustum::debugRender()
{
    RenderUtils::drawFrustumShader(_pFrustum, Color4f(1, 0, 0, 1));
}
void ShadowFrustum::createFbo()
{
    if (_bShadowMapEnabled == false) {
        return;
    }
    deleteFbo();

    RenderUtils::debugGetRenderState();
    // Create the depth buffer
    RenderUtils::createDepthTexture(&_glDepthTextureId, _iFboWidthPixels, _iFboHeightPixels, false, 0, GL_DEPTH_COMPONENT24);

    //Bind framebuffer and attach depth texture.
    getContext()->glGenFramebuffers(1, &_glFrameBufferId);
    getContext()->chkErrRt();
    getContext()->glBindFramebuffer(GL_FRAMEBUFFER, _glFrameBufferId);
    getContext()->chkErrRt();

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
    getContext()->chkErrRt();

    glTexImage2D(GL_TEXTURE_2D, 0, SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT,
        _iFboWidthPixels, _iFboHeightPixels, 0, SHADOW_CUBE_MAP_TEX_FORMAT, SHADOW_CUBE_MAP_TEX_TYPE, nullptr);
    getContext()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _glShadowMapId, 0);
    getContext()->chkErrRt();
   // getContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glSide, _glShadowCubeMapId, 0);


    getContext()->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _glDepthTextureId, 0);
    getContext()->chkErrRt();


    glBindTexture(GL_TEXTURE_2D, 0);
    getContext()->chkErrRt();

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);

    // Disable reads from the color buffer
    glReadBuffer(GL_NONE);
    getContext()->chkErrRt();


    GLenum status = getContext()->glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Br2ThrowException("Point Light Shadow Map Framebuffer encountered an error during setup: " + status);
        getContext()->chkErrRt();
    }

    getContext()->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    getContext()->chkErrRt();

}

void ShadowFrustum::deleteFbo() {
    if (_bShadowMapEnabled == false) {
        return;
    }
    if (_glFrameBufferId != 0) {
        getContext()->glDeleteFramebuffers(1, &_glFrameBufferId);
    }
    if (_glDepthTextureId != 0) {
        getContext()->glDeleteTextures(1, &_glDepthTextureId);
    }
    if (_glShadowMapId != 0) {
        getContext()->glDeleteTextures(1, &_glShadowMapId);
    }

}
void ShadowFrustum::copyAndBlendToShadowMap(std::shared_ptr<ShadowFrustum> pBox)
{
    if (_bShadowMapEnabled == false) {
        return;
    }
    //Here we can do shadowmap operations.

    if (pBox->getGlTexId() != 0) {
        getContext()->glCopyImageSubData(
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
        getContext()->chkErrDbg();
    }
}
void ShadowFrustum::beginRenderShadowFrustum()
{
    if (_bShadowMapEnabled == false) {
        return;
    }
    //Gd::verifyRenderThread();
    getContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _glFrameBufferId);
    getContext()->chkErrDbg();

    getContext()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    getContext()->chkErrDbg();

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    getContext()->chkErrDbg();
    getContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _glShadowMapId, 0);

    //if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
    //    glClearColor(0, 0, 0, 1);//This could be moved out of here.
    //}
    //else {
        glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);//This could be moved out of here.
//    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    getContext()->chkErrDbg();
}
void ShadowFrustum::renderShadows(std::shared_ptr<ShadowFrustum> pShadowFrustumMaster) {
    if (_bShadowMapEnabled == false) {
        return;
    }

    // Update the shadow box.
    //pShadowFrustumMaster->beginRenderShadowFrustum();
    beginRenderShadowFrustum();
    {
        vec3 vFinal = *_pLightSource->getFinalPosPtr();

        _pVisibleSet->sortAndDrawMeshes(
            [](std::shared_ptr<VertexFormat> vf) {
            return getContext()->getShaderManager()->getShadowShader(vf);
        },
            [&](std::shared_ptr<ShaderBase> sb) {
            sb->bind();
            sb->setUf("_ufProj", (void*)&_projMatrix, 1, false);
            sb->setUf("_ufView", (void*)&_viewMatrix, 1, false);
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
void ShadowFrustum::endRenderShadowFrustum()
{
    if (_bShadowMapEnabled == false) {
        return;
    }
    //Gd::verifyRenderThread();
    glBindTexture(GL_TEXTURE_2D, 0);
    getContext()->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    getContext()->chkErrDbg();
}










}//ns BR2
