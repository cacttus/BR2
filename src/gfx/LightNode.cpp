#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/EngineConfig.h"
#include "../base/FpsMeter.h"
#include "../base/Gu.h"
#include "../base/Perf.h"

#include "../gfx/LightNode.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/ShadowFrustum.h"
#include "../model/OBB.h"

namespace BR2 {
LightNodeBase::LightNodeBase(bool bShadow) : _bEnableShadows(bShadow), PhysicsNode(nullptr) {
  _color = vec4(1, 1, 1, 1);
}
LightNodeBase::~LightNodeBase() {
}
void LightNodeBase::init() {
  PhysicsNode::init();
}
void LightNodeBase::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  PhysicsNode::update(delta, mapAnimators);
}
vec3* LightNodeBase::getFinalPosPtr() {
  _vGpuBufferedPosition = getFinalPos();//getTransform()->getPos();
  return &_vGpuBufferedPosition;
}
bool LightNodeBase::shadowsEnabled() {
  return _bEnableShadows && (Gu::getEngineConfig()->getEnableObjectShadows() ||
    Gu::getEngineConfig()->getEnableTerrainShadows());
}
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<LightNodeDir> LightNodeDir::create(bool bShadow) {
  std::shared_ptr<LightNodeDir> lp = std::make_shared<LightNodeDir>(bShadow);
  lp->init();
  lp->_pSpec = std::make_shared<BaseSpec>("*LightNodeDir");

  return lp;
}
LightNodeDir::LightNodeDir(bool bShadow) : LightNodeBase(bShadow) {
}
LightNodeDir::~LightNodeDir() {
  //DEL_MEM(_pGpuLight);
  //DEL_MEM(_pShadowFrustum);
}
void LightNodeDir::setLookAt(const vec3& v) {
  _vLookAt = v;
  _pShadowFrustum->setChanged();
}
void LightNodeDir::init() {
  LightNodeBase::init();

  _vLookAt = vec3(0, 0, 0);
  _vDir = vec3(0, 0, 1);
  _vUp = vec3(0, 1, 0);
  _vRight = vec3(1, 0, 0);

  int32_t iShadowMapRes = Gu::getEngineConfig()->getShadowMapResolution();
  _pShadowFrustum = std::make_shared<ShadowFrustum>(std::dynamic_pointer_cast<LightNodeDir>(shared_from_this()),
    iShadowMapRes, iShadowMapRes, shadowsEnabled());    //TEST
  _pShadowFrustum->init();
  _pShadowFrustum->getFrustum()->setZFar(500);

  _pGpuLight = std::make_shared<GpuDirLight>();
  setMaxDistance(100.0f);
  _pShadowFrustum->setChanged();
}

void LightNodeDir::setMaxDistance(float f) {
  _pShadowFrustum->getFrustum()->setZFar(f);
  _pShadowFrustum->setChanged();
}
void LightNodeDir::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  if (getHidden() == true) {
    return;
  }
  Perf::pushPerf();
  {
    LightNodeBase::update(delta, mapAnimators);
    if (_pShadowFrustum != nullptr) {
      _pShadowFrustum->update();
    }

    _vDir = (getLookAt() - getFinalPos()).normalized();
    vec3 vUp = vec3(0, 1, 0);
    _vRight = (vUp.cross(_vDir)).normalized();
    _vUp = (_vDir.cross(_vRight)).normalized();

    //update
    _pGpuLight->_vPos = getFinalPos();
    _pGpuLight->_vDir = _vDir;
    _pGpuLight->_fMaxDistance = _pShadowFrustum->getFrustum()->getZFar();
    _pGpuLight->_vDiffuseColor = _color;
    _pGpuLight->_fLinearAttenuation = 0.0f;
    _pGpuLight->_mView = *_pShadowFrustum->getViewMatrixPtr();
    _pGpuLight->_mProj = *_pShadowFrustum->getProjMatrixPtr();
    _pGpuLight->_mPVB = *_pShadowFrustum->getPVBPtr();
  }
  Perf::popPerf();
}
bool LightNodeDir::renderShadows(std::shared_ptr<ShadowFrustum> pf) {
  //**Update shadow map frustum.
  if (shadowsEnabled()) {
    _pShadowFrustum->renderShadows(pf);
  }

  return true;
}
void LightNodeDir::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) {
  pBox.genResetLimits();
  pBox = *_pShadowFrustum->getFrustum()->getBoundBox();
  getOBB()->setInvalid();

  BaseNode::calcBoundBox(pBox, obPos, extra_pad);
}
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<LightNodePoint> LightNodePoint::create(bool bhasShadowBox) {
  std::shared_ptr<LightNodePoint> lp = std::make_shared<LightNodePoint>(bhasShadowBox);
  lp->init();
  lp->_pSpec = std::make_shared<BaseSpec>("*LightNodePoint");
  return lp;
}
LightNodePoint::LightNodePoint(bool bShadowBox) : LightNodeBase(bShadowBox) {
}
LightNodePoint::~LightNodePoint() {
  _pShadowBox = nullptr;
  _pGpuLight = nullptr;
}
void LightNodePoint::init() {
  LightNodeBase::init();
  _pGpuLight = std::make_shared<GpuPointLight>();

  int32_t iShadowMapRes = Gu::getEngineConfig()->getShadowMapResolution();
  _pShadowBox = std::make_shared<ShadowBox>(std::dynamic_pointer_cast<LightNodePoint>(shared_from_this()), iShadowMapRes, iShadowMapRes, shadowsEnabled());    //TEST
  _pShadowBox->init();
}
void LightNodePoint::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  if (getHidden() == true) {
    return;
  }
  Perf::pushPerf();
  {
    LightNodeBase::update(delta, mapAnimators);
    if (_pShadowBox != nullptr) {
      _pShadowBox->update();
    }
    updateFlicker();

    //update
    _pGpuLight->_vPos = getFinalPos();
    _pGpuLight->_fRadius = _fRadius;
    _pGpuLight->_vDiffuseColor = _color;

  }
  Perf::popPerf();
}
bool LightNodePoint::renderShadows(std::shared_ptr<ShadowBox> pf) {
  //**Update shadow map frustum.
  _pShadowBox->renderShadows(pf);
  return true;
}
void LightNodePoint::updateFlicker() {
  if (_bFlickerEnabled) {

    _fFlickerValue += (1 / 10.0f) * 1.0f / Gu::getFpsMeter()->getFps();

    if (_fFlickerValue >= 1.0f) {
      _fLastRandomValue = _fNextRandomValue;
      _fNextRandomValue = Random::nextFloat(0.0f, 1.0f);

      _fFlickerValue = 0.0f;
    }

    _fFlickerAddRadius = _fFlickerMaxRadius * (Alg::cosine_interpolate(_fLastRandomValue, _fNextRandomValue, _fFlickerValue));//pnoise in range -1,1 so make it 1,1
  }

  // optimized radius for shader.
  float f = getLightRadius();
  if (f != 0.0f) {
    _f_1_Radius_2 = (1.0f) / (f * f);
  }
}
float LightNodePoint::getLightRadius() {
  //Negative light radiuses are infinite
  if (_fRadius == _cInfiniteLightRadius)
    return _cInfiniteLightRadius;
  else
    return(_fRadius + _fFlickerAddRadius);
}
void LightNodePoint::setLightRadius(float r) {
  if (r < 0.0f) {
    r = _cInfiniteLightRadius;
  }
  _fRadius = r;
}

void LightNodePoint::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) {
  pBox.genResetLimits();
  pBox._min = pBox._max = getPos();
  pBox._min -= getLightRadius();
  pBox._max += getLightRadius();
  getOBB()->setInvalid();

  BaseNode::calcBoundBox(pBox, obPos, extra_pad);
}


}//ns game
