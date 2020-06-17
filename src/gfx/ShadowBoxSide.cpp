#include "../base/Logger.h"
#include "../base/Perf.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderParams.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShadowBoxSide.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/LightNode.h"
#include "../gfx/LightManager.h"
#include "../gfx/RenderSettings.h"
#include "../model/MeshSpec.h"
#include "../model/MeshNode.h"
#include "../world/RenderBucket.h"
#include "../world/PhysicsWorld.h"
#include "../world/PhysicsGrid.h"
#include "../world/NodeUtils.h"

namespace BR2 {
ShadowBoxSide::ShadowBoxSide(std::shared_ptr<ShadowBox> pParentBox, std::shared_ptr<LightNodePoint> pLightSource, BoxSide::e eSide) {
  _eSide = eSide;
  _pParentBox = pParentBox;
  _pLightSource = pLightSource;

  _pViewport = std::make_shared<RenderViewport>(_pParentBox->getFboWidth(), _pParentBox->getFboHeight(), ViewportConstraint::Full);

  _pVisibleSet = std::make_shared<RenderBucket>();
  _pFrustum = std::make_unique<FrustumBase>(_pViewport, 90.0f);
}
ShadowBoxSide::~ShadowBoxSide() {
}
void ShadowBoxSide::updateView(/*send junk in instead*/) {
  AssertOrThrow2(_pLightSource != nullptr);
  AssertOrThrow2(_pParentBox != nullptr);

  if (Gu::getRenderSettings()->getDebug()->getShadowHelpVisible()) {
    //Debug - If shadow help is visible
    //we alter the viewport here to show shadow lines
    //around the edges which demarcate the shadow box frustum boundaries.
    int pad = 15;
    _pViewport->updateBox(pad, pad, _pParentBox->getFboWidth() - pad * 2, _pParentBox->getFboHeight() - pad * 2);
  }
  else {
    _pViewport->updateBox(0, 0, _pParentBox->getFboWidth(), _pParentBox->getFboHeight());
  }

  float fNear = _pParentBox->getSmallBoxSize() * 0.5f;
  float fFar = 0;

  // make sure frustum isn't **d uip
  if (_pLightSource->getLightRadius() < fNear) {
    std::shared_ptr<CameraNode> cam = NodeUtils::getActiveCamera(_pLightSource->getLightManager());
    fFar = cam->getFrustum()->getZFar();
  }
  else {
    fFar = _pLightSource->getLightRadius();
  }
  _pFrustum->setZNear(fNear);
  _pFrustum->setZFar(fFar);

  vec3 vLightPos = _pLightSource->getFinalPos();
  vec3 vCamNormal = BoxUtils::normalForSide(_eSide);

  //**This value is super important.  If we render with teh camera oriented down the wrong axis
  //the render texture will be oriented incorrectly.
  vec3 vCamUp;

  if (vCamNormal.x > 0.0f) { vCamUp = vec3(0, 1, 0); }
  else if (vCamNormal.x < 0.0f) { vCamUp = vec3(0, 1, 0); }
  else if (vCamNormal.y > 0.0f) { vCamUp = vec3(0, 0, -1); }
  else if (vCamNormal.y < 0.0f) { vCamUp = vec3(0, 0, 1); }
  else if (vCamNormal.z > 0.0f) { vCamUp = vec3(0, 1, 0); }
  else if (vCamNormal.z < 0.0f) { vCamUp = vec3(0, 1, 0); }

  //Calculate geometrric frustum
  _pFrustum->update(
    vCamNormal
    , vLightPos
    , vCamUp
    , ProjectionMode::e::Perspective
  );

  //calc view matrix
  vec3 vLookAt = vLightPos + vCamNormal;
  _viewMatrix.lookAt(
    vLightPos
    , vLookAt
    , vCamUp);

  // _projMatrix = _pFrustum->getProjectionMatrix();
  float vpWidth_2 = _pFrustum->getTanFov2() * fNear;
  float arat_1 = 1.0 / _pViewport->getAspectRatio();
  float vw = vpWidth_2;
  float vh = vpWidth_2 * arat_1;
  _projMatrix = mat4::getProjection(
    fNear, fFar,
    vw, -vw,
    -vh, vh
  );
}
bool ShadowBoxSide::computeIsVisible(std::shared_ptr<FrustumBase> pCamFrustum) {
  //TODO:
  // This is very inaccurate.  Try to intersect the two frustums instead.
  // Actually since all shadow box sides are axis-aligned this isn't too inaccurate..

  return pCamFrustum->hasFrustum(_pFrustum);
}
void ShadowBoxSide::cullObjectsAsync(CullParams& cp) {
  // AssertOrThrow2(_pLightSource != nullptr);
  //AssertOrThrow2(_pVisibleSet != nullptr);

  updateView();

  _pVisibleSet->start(MathUtils::brMin(
    cp.getMaxObjectDistance() * cp.getMaxObjectDistance(),
    Gu::getEngineConfig()->getMaxPointLightShadowDistance() * Gu::getEngineConfig()->getMaxPointLightShadowDistance()),
    cp.getCamera());

  if (std::shared_ptr<PhysicsWorld> physics = NodeUtils::getPhysicsWorld(_pLightSource)) {
    physics->collectVisibleNodes(_pVisibleSet);
  }

  //////////////////////////////////////////////////////////////////////////
  //Loop through objects and find whether they have changed since last update.
  _bMustUpdate = false;

  if (_pVisibleSet->getGrids().size() > 0 || _pVisibleSet->getObjs().size() > 0) {
    _bMustUpdate = true;
  }
}
void ShadowBoxSide::renderShadows(std::shared_ptr<ShadowBox> pMasterBox, bool bForce) {
  Perf::pushPerf();
  static int n = 0; if (n == 1) { return; }

  //Return if we don't need to update.
  if (_bMustUpdate == false && bForce == false) {
    return;
  }

  getViewport()->bind();

  pMasterBox->beginRenderSide(_eSide);
  {
    _pVisibleSet->sortAndDrawMeshes(
      [](std::shared_ptr<VertexFormat> fmt) {
        return Gu::getShaderMaker()->getShadowShader(fmt);
      },
      [&](std::shared_ptr<ShaderBase> sb) {
        sb->bind();
        sb->setUf("_ufProj", (void*)getProjectionMatrix(), 1, false);
        sb->setUf("_ufView", (void*)getViewMatrix(), 1, false);
        sb->setUf("_ufShadowLightPos", (void*)getLight()->getFinalPosPtr(), 1, false);
      },
        [&](std::shared_ptr<ShaderBase> sb, std::shared_ptr<SceneNode> n) {
        RenderParams rp;
        rp.setShader(sb);
        n->drawShadow(rp);
      }
      );

    //Find an appropriate shader for the terrain meshes
    std::shared_ptr<ShaderBase> sb;
    for (auto p : _pVisibleSet->getGrids()) {
      std::shared_ptr<MeshNode> mn = p.second->getMesh();
      if (mn != nullptr) {
        if (mn->getMeshSpec() != nullptr) {
          std::shared_ptr<VertexFormat> fmt = mn->getMeshSpec()->getVertexFormat();
          sb = Gu::getShaderMaker()->getShadowShader(fmt);
          if (sb != nullptr) {
            break;
          }
        }
      }
    }
    //Draw the terrain meshes
    if (sb != nullptr) {
      sb->bind();
      sb->setUf("_ufProj", (void*)getProjectionMatrix(), 1, false);
      sb->setUf("_ufView", (void*)getViewMatrix(), 1, false);
      sb->setUf("_ufShadowLightPos", (void*)getLight()->getFinalPosPtr(), 1, false);
      //if shadow topology
      for (auto p : _pVisibleSet->getGrids()) {
        std::shared_ptr<MeshNode> mn = p.second->getMesh();
        if (mn != nullptr) {
          if (mn->getMeshSpec() != nullptr) {
            std::shared_ptr<VertexFormat> fmt = mn->getMeshSpec()->getVertexFormat();
            std::shared_ptr<ShaderBase> sb = Gu::getShaderMaker()->getShadowShader(fmt);
            if (sb != nullptr) {
              RenderParams rp;
              rp.setShader(sb);
              mn->drawShadow(rp);
            }
          }
        }
      }
    }
  }
  pMasterBox->endRenderSide();

  Gu::checkErrorsDbg();
  Perf::popPerf();
}
}//ns Game