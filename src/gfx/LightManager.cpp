#include "../base/BaseAll.h"
#include "../base/EngineConfig.h"
#include "../base/InputManager.h"
#include "../base/Perf.h"

#include "../gfx/CameraNode.h"
#include "../gfx/LightManager.h"
#include "../gfx/LightNode.h"

#include "../model/ShaderStorageBuffer.h"
#include "../world/PhysicsWorld.h"
#include "../world/RenderBucket.h"


namespace Game {
LightManager::LightManager(std::shared_ptr<GLContext> tc) :
    _pContext(tc) {
    _pGpuDeferredParams = std::make_shared<GpuDeferredParams>();

    if (Gu::getConfig()->getColorSpace() == ColorSpace::e::SRGB) {
        //If SRGB is enabled we want to nullify the exponent
        //_pGpuDeferredParams->_fHdrToneMapExp = 0.899999976;
        //_pGpuDeferredParams->_fHdrSampleExp = 1.30000007;
        _pGpuDeferredParams->_fHdrToneMapExp = 1.2500010f;
        _pGpuDeferredParams->_fHdrSampleExp = 1.6700005f;
    }
    else {
        _pGpuDeferredParams->_fHdrToneMapExp = 1.2500010f;
        _pGpuDeferredParams->_fHdrSampleExp = 1.6700005f;
    }


    BroLogDebug("Light Manager Initializing");
    initializeDeferredFogData();

}
LightManager::~LightManager() {

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void LightManager::setHdrToneMapExponent(float val) {
    if (val < 0.0f) {
        val = 0.0f;
    }
    _pGpuDeferredParams->_fHdrToneMapExp = val;
}
void LightManager::setHdrSampleSpaceExponent(float val) {
    if (val < 0.0f)
        val = 0.0f;
    _pGpuDeferredParams->_fHdrSampleExp = val;
}
void LightManager::initializeDeferredFogData() {
    _pGpuDeferredParams->_fFogDamp = 1.8f;//0.627001286;//0.91783465f;
//    _pGpuDeferredParams->_iFogSampleCount = 10;//400.000000;//dist;
//    _pGpuDeferredParams->_fFogSampleRadius = 1.0f;//0.927198410;//1.0072;
    _pGpuDeferredParams->_fFogBlend = 0.56361f;//640.000000;  
    _pGpuDeferredParams->_vFogColor = vec4(0.8407f, 0.89349f, 0.981054f, 1.0f);//vec4(0.000002,0.0000021,0.00055,1);
    //Fog distance VVVV Lower this value to get nearer fog
    _pGpuDeferredParams->_fFogDivisor = 20.0f; // This is divided by teh frag position to get [0,1].  Technically it's the beginning fog distance
    _pGpuDeferredParams->_vAmbientColor = vec3(0.24, 0.26, 0.261);
    _pGpuDeferredParams->_fAmbientIntensity = 0.5f;

    _pGpuDeferredParams->_fFocalDepth = 0.0f;//Not Used
    _pGpuDeferredParams->_fFocalRange = 25.0f;

}
void LightManager::updateRenderData() {
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_N)) {
        _pGpuDeferredParams->_fFocalDepth -= 0.5f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_M)) {
        _pGpuDeferredParams->_fFocalDepth += 0.5f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_J)) {
        _pGpuDeferredParams->_fFocalRange -= 0.01f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_K)) {
        _pGpuDeferredParams->_fFocalRange += 0.01f;
    }

    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_I)) {
        _pGpuDeferredParams->_fHdrToneMapExp -= 0.01f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_O)) {
        _pGpuDeferredParams->_fHdrToneMapExp += 0.01f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_T)) {
        _pGpuDeferredParams->_fHdrSampleExp -= 0.01f;
    }
    if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_Y)) {
        _pGpuDeferredParams->_fHdrSampleExp += 0.01f;
    }
    _pGpuDeferredParams->_fFogDamp = MathUtils::broClamp(_pGpuDeferredParams->_fFogDamp, 0.0f, 1.0f);
    _pGpuDeferredParams->_fFogBlend = MathUtils::broClamp(_pGpuDeferredParams->_fFogBlend, 0.0f, 1.0f);
    _pGpuDeferredParams->_fFogDivisor = MathUtils::broClamp(_pGpuDeferredParams->_fFogDivisor, 0.0f, FLT_MAX);
    _pGpuDeferredParams->_fAmbientIntensity = MathUtils::broClamp(_pGpuDeferredParams->_fAmbientIntensity, 0.0f, 1.0f);



    //    _pGpuDeferredParams->_iPointLightCount
    std::shared_ptr<CameraNode> pCam = Gu::getCamera();
    //Set world view / proj
    if (pCam != nullptr) {
        //  _pGpuDeferredParams->_mWorldProj = pCam->getProj();
        //  _pGpuDeferredParams->_mWorldView = pCam->getView();
        _pGpuDeferredParams->_vViewPos = pCam->getPos();
        _pGpuDeferredParams->_vViewDir = pCam->getViewNormal();
    }
    else {
        //  _pGpuDeferredParams->_mWorldProj = mat4::identity();
        //  _pGpuDeferredParams->_mWorldView = mat4::identity();
    }
    _pGpuDeferredParams->_fTimeSeconds = 0;

}

void LightManager::update(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf) {
    setupLights(pf, mf);
    updateRenderData();
}
// - Main function to be called on the scene to set up all the lights.
void LightManager::setupLights(std::shared_ptr<ShadowBox> pf, std::shared_ptr<ShadowFrustum> mf) {
    //Physics world may not be instantiated.
    if (Gu::getPhysicsWorld() != nullptr && Gu::getPhysicsWorld()->getRenderBucket() != nullptr) {

        Perf::pushPerf();
        int32_t nMaxPointLights = Gu::getConfig()->getMaxPointLights();//shadowmapmaxinfluences
        int32_t nMaxDirLights = Gu::getConfig()->getMaxDirLights();//shadowmapmaxinfluences

        _pGpuDeferredParams->_iPointLightCount = 0;
        _pGpuDeferredParams->_iDirLightCount = 0;
        _vecGpuPointLights.clear();
        _vecGpuDirLights.clear();
        _vecGpuShadowBoxes.clear();
        _vecGpuShadowFrustums.clear();

        //Note: we collect all lights in the physics world collection step.
        //Update all lights that collide with the main frustum
        for (std::pair<float, std::shared_ptr<LightNodePoint>> p : Gu::getPhysicsWorld()->getRenderBucket()->getPointLights()) {
            std::shared_ptr<LightNodePoint> pPointLight = p.second;
            if (_pGpuDeferredParams->_iPointLightCount < nMaxPointLights) {
                pPointLight->renderShadows(pf);

                _pGpuDeferredParams->_iPointLightCount++;

                _vecGpuShadowBoxes.push_back(pPointLight->getShadowBox());
                _vecGpuPointLights.push_back(*(pPointLight->getGpuLight().get()));
            }
        }
        for (std::pair<float, std::shared_ptr<LightNodeDir>> p : Gu::getPhysicsWorld()->getRenderBucket()->getDirLights()) {
            std::shared_ptr<LightNodeDir> pDirLight = p.second;
            if (_pGpuDeferredParams->_iDirLightCount < nMaxDirLights) {
                pDirLight->renderShadows(mf);

                _pGpuDeferredParams->_iDirLightCount++;

                _vecGpuShadowFrustums.push_back(pDirLight->getShadowFrustum());
                _vecGpuDirLights.push_back(*(pDirLight->getGpuLight().get()));
            }
        }

        Gu::checkErrorsDbg();
        Perf::popPerf();
    }

}
std::vector<std::shared_ptr<ShadowBox>> LightManager::getAllShadowBoxes() {
    std::vector<std::shared_ptr<ShadowBox>> sbs;
    for (std::pair<float, std::shared_ptr<LightNodePoint>> p : Gu::getPhysicsWorld()->getRenderBucket()->getPointLights()) {
        if (p.second->getShadowBox()) {
            sbs.push_back(p.second->getShadowBox());
        }
    }

    return sbs;
}
std::vector<std::shared_ptr<ShadowFrustum>> LightManager::getAllShadowFrustums() {
    std::vector<std::shared_ptr<ShadowFrustum>> sbs;
    for (std::pair<float, std::shared_ptr<LightNodeDir>> p : Gu::getPhysicsWorld()->getRenderBucket()->getDirLights()) {
        if (p.second->getShadowFrustum()) {
            sbs.push_back(p.second->getShadowFrustum());
        }
    }

    return sbs;
}







}//ns Game
