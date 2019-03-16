#include "../base/GLContext.h"
#include "../display/CameraNode.h"
#include "../display/Viewport.h"
#include "../base/TouchInfo.h"
#include "../bottle/GodCam.h"
#include "../display/CameraNode.h"
#include "../display/FrustumBase.h"
#include "../bottle/BottleUtils.h"


namespace Game {
GodCam::GodCam(std::shared_ptr<GLContext> ct, std::shared_ptr<Viewport> pv): _pContext(ct) {
    BroLogInfo("Creating Camera.");
    _pCamera = CameraNode::create(pv);
    _pCamera->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  
    _vCamNormal.construct(-.6,1,-.6);
    _vCamNormal.normalize();
    _vLookAt.construct(0,0,0);
    updateCameraPosition();
    ct->setCamera(_pCamera);
    _pCamera->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());
}
GodCam::~GodCam() { 
    _pCamera = nullptr;
    //DEL_MEM(_pCamera);
}
///////////////////////////////////////////////////////////////////

void GodCam::update(std::shared_ptr<Fingers> pFingers, float dt){
    updateRotateAndZoom(dt);
    updateTouches(pFingers, dt);
    _pCamera->update(dt, std::map<Hash32, std::shared_ptr<Animator>>());
}
void GodCam::updateTouches(std::shared_ptr<Fingers> pFingers, float dt){
    ButtonState::e eLmb = pFingers->getLmbState();
    ButtonState::e eRmb = pFingers->getRmbState();
    vec2 vMouse = pFingers->getMousePos();
    vec2 vLast = pFingers->getLastMousePos();
    t_timeval us = Gu::getMicroSeconds();

    //RMB
    if (eRmb == ButtonState::e::Press) {
        _lastTouchRmb = _curTouchRmb;
        _curTouchRmb.touchBegin(vMouse, eRmb);
        _bUnsnapY = _bUnsnapX = false;
        vec2 delta = vMouse - vLast;
        userRotateAndZoom(delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
    }
    else if (eRmb == ButtonState::e::Down) {
        vec2 delta = vMouse - vLast;
        userRotateAndZoom(delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
   //     pFingers->warpMouse(_pCamera->getViewport()->getWidth()*0.5, _pCamera->getViewport()->getHeight()*0.5 );
    }
    else if (eRmb == ButtonState::e::Release) {
        _curTouchRmb.touchEnd(vMouse, eRmb);
        _bUnsnapY = _bUnsnapX = false;
    }

    else if (eRmb == ButtonState::e::Up) {
        if (_curTouchRmb._eState == ButtonState::e::Press || _curTouchRmb._eState == ButtonState::e::Down) {
            _curTouchRmb.touchEnd(vMouse, eRmb);
        }
        _bUnsnapY = _bUnsnapX = false;
    }
}
//void SnapRooter::rotateAndZoomCamera(std::shared_ptr<Fingers> pFingers, float dt) {
//    vec2 vMouse = pFingers->getMousePos();
//    vec2 vLast = pFingers->getLastMousePos();
//
//    ButtonState eRmb = pFingers->getRmbState();
//    if (eRmb == ButtonState::Press) {
//        _lastTouchRmb = _curTouchRmb;
//        _curTouchRmb.touchBegin(vMouse, eRmb);
//        _bUnsnapY = _bUnsnapX = false;
//        vec2 delta = vMouse - vLast;
//        userRotateAndZoom(delta, vMouse,vLast, _fPerUnitRotate, _fPerUnitZoom);
//    }
//    else if (eRmb == ButtonState::Down) {
//        vec2 delta = vMouse - vLast;
//        userRotateAndZoom(delta, vMouse, vLast, _fPerUnitRotate, _fPerUnitZoom);
//    }
//    else if (eRmb == ButtonState::Release) {
//        _curTouchRmb.touchEnd(vMouse, eRmb);
//        _bUnsnapY = _bUnsnapX = false;
//    }
//}
void GodCam::userRotateAndZoom(vec2& v, vec2& vMouse, vec2& vLast, float perUnitRotationRads, float perUnitZoom) {
    //Calculate snap controls - so that clicking the user doesn't incidedntally rotate/zoom the camera.
    vec2 vdelta = _curTouchRmb.getMousePosPress() - vMouse;
    float fZoomControlY = _pCamera->getViewport()->getHeight() * 0.07f; //1%
    if (fabsf(vdelta.y) > fZoomControlY) {
        _bUnsnapY = true;
    }
    float fZoomControlX = _pCamera->getViewport()->getWidth() * 0.023f; //1%
    if (fabsf(vdelta.x) > fZoomControlX) {
        _bUnsnapX = true;
    }

    //*Note: we apply rotate/zoom in the update loop in order to smooth out the velocity.

    //Rotate (X)
    if (_bUnsnapX) {
        _fRotationVel += perUnitRotationRads * v.x;
        _fRotationVel = MathUtils::broClamp(_fRotationVel, _fRotationVelMin, _fRotationVelMax);
    }

    //Zoom (Y)
    if (_bUnsnapY) {
        _fZoomVel += perUnitZoom * v.y;
        _fZoomVel = MathUtils::broClamp(_fZoomVel, _fZoomVelMin, _fZoomVelMax);
    }
}
void GodCam::updateRotateAndZoom(float delta) {
    float stop_e = 0.000001f;

    if (_fRotationVel*_fRotationVel > 0.0f) {
        doRotate(_fRotationVel);
        _fRotationVel = _fRotationVel - (_fRotationVel * _fRotationEase * delta);
        _fRotationVel = MathUtils::broClamp(_fRotationVel, _fRotationVelMin, _fRotationVelMax);

        if (_fRotationVel*_fRotationVel < stop_e) {
            _fRotationVel = 0.0f;
        }
    }

    if (_fZoomVel*_fZoomVel > 0.0f) {
        //doZoom(_fZoomVel);
        doRotateZ(_fZoomVel);
        _fZoomVel = _fZoomVel - (_fZoomVel * _fZoomEase * delta);
        _fZoomVel = MathUtils::broClamp(_fZoomVel, _fZoomVelMin, _fZoomVelMax);
        if (_fZoomVel*_fZoomVel < stop_e) {
            _fZoomVel = 0.0f;
        }
    }
}
void GodCam::doRotate(float dRot) {

    mat4 rot = mat4::getRotationRad(dRot, vec3(0, 1, 0));

    vec3 origPos = _pContext->getCamera()->getPos();

    vec4 mul =
        vec4(_vCamNormal.x,
            _vCamNormal.y,
            _vCamNormal.z,
            1.0f);
    vec4 res = rot * mul;

    _vCamNormal = vec3(res.x, res.y, res.z);
    updateCameraPosition(); // Recalc the camera position.
    //updateCameraFollow();
}
void GodCam::doRotateZ(float dRot) {

    vec3 rn = _pContext->getCamera()->getRightNormal();

    mat4 rot = mat4::getRotationRad(dRot, rn);

    vec3 origPos = _pContext->getCamera()->getPos();

    vec4 mul =
        vec4(_vCamNormal.x,
            _vCamNormal.y,
            _vCamNormal.z,
            1.0f);
    vec4 res = rot * mul;
    int n=0;
    _vCamNormal = vec3(res.x, res.y, res.z);
    updateCameraPosition(); // Recalc the camera position.
                            //updateCameraFollow();
}
void GodCam::doZoom(float amt) {
    float fZoomVal = 3.0;
    _fCamDist -= amt * fZoomVal;
    
    updateCameraPosition(); // Recalc the camera position.
}
void GodCam::updateCameraPosition() {
    if (_fCamDist >= _fCamDistMax) {
        _fCamDist = _fCamDistMax;
    }
    if (_fCamDist < _fCamDistMin) {
        _fCamDist = _fCamDistMin;
    }
    vec3 cp = _vLookAt + (_vCamNormal * _fCamDist);

    _pCamera->setPos(std::move(cp));
    _pCamera->setLookAt(std::move(_vLookAt));
}
void GodCam::userZoom(float amt){
    doZoom(amt);
}
void GodCam::moveCameraWSAD(std::shared_ptr<Fingers> pFingers, float delta) {
    //**Camera
    //Damp Slows us a bit when we zoom out.
    float damp = fabsf(_fCamDist)*0.001f;
    float factor = 4.0f;
    if (pFingers->shiftHeld()) {
        factor = 12.0f;
    }
    float strafeAmt = fabsf(_fCamDist) / (BottleUtils::getNodeWidth() + damp) * factor * delta;
    std::function<void()> moveUp = [this, strafeAmt]() {
        vec3 dir = Gu::getContext()->getCamera()->getViewNormal().xz().normalized();
        _vLookAt += dir * strafeAmt;
    };
    std::function<void()> moveDown = [this, strafeAmt]() {
        vec3 dir = Gu::getContext()->getCamera()->getViewNormal().xz().normalized();
        _vLookAt += dir * -strafeAmt;
    };
    std::function<void()> moveLeft = [this, strafeAmt]() {
        vec3 dir = Gu::getContext()->getCamera()->getRightNormal();
        _vLookAt += dir * -strafeAmt;
    };
    std::function<void()> moveRight = [this, strafeAmt]() {
        vec3 dir = Gu::getContext()->getCamera()->getRightNormal();
        _vLookAt += dir * strafeAmt;
    };

    //Mouse scroll
    float amt = 0.03f;
    float vw = (float)Gu::getContext()->getCamera()->getViewport()->getWidth();
    float vh = (float)Gu::getContext()->getCamera()->getViewport()->getHeight();
    float wa = vw * amt;
    float ha = vh  * amt;

    vec2 mp = pFingers->getMousePos();

    if (mp.x < wa) {
        moveLeft();
    }
    if (mp.y < ha) {
        moveUp();
    }
    if (mp.x > vw-wa) {
        moveRight();
    }
    if (mp.y > vh - ha) {
        moveDown();
    }

    //Keyboard
    if (pFingers->keyPressOrDown(SDL_SCANCODE_W)) {
        moveUp();
    }
    if (pFingers->keyPressOrDown(SDL_SCANCODE_S)) {
        moveDown();
    }
    if (pFingers->keyPressOrDown(SDL_SCANCODE_A)) {
        moveLeft();
    }
    if (pFingers->keyPressOrDown(SDL_SCANCODE_D)) {
        moveRight();
    }
    updateCameraPosition();
}
void GodCam::setTarget(vec3& v){
    _vLookAt = v;
    updateCameraPosition();
}
void GodCam::setActive() {
    _pContext->setCamera(_pCamera);
}



}//ns Game
