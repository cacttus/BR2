#include "../base/Gu.h"
#include "../gfx/CameraNode.h"
#include "../base/GLContext.h"
#include "../world/PhysicsGridAwareness.h"


namespace BR2 {
///////////////////////////////////////////////////////////////////
PhysicsGridAwareness::PhysicsGridAwareness(MpFloat rxz, float incXZ, MpFloat ry, float incY) {
    _vLastAwarenessPos = getAwarenessPos();
    _mpXz = rxz;
    _mpY = ry;
    _incY = incY;
    _incXz = incXZ;
    _fAwarenessRadiusXZ = rxz.getMin();
    _fAwarenessRadiusY = ry.getMin();
}
PhysicsGridAwareness::~PhysicsGridAwareness() {

}
void PhysicsGridAwareness::update(float dt){
    updateAwarenessSpheroidAxis(_fAwarenessRadiusY,
        _mpY.getMin(),
        _mpY.getMax(),
        _incY);
    updateAwarenessSpheroidAxis(_fAwarenessRadiusXZ,
        _mpXz.getMin(),
        _mpXz.getMax(),
        _incXz
    );

}
void PhysicsGridAwareness::updateAwarenessSpheroidAxis(float& fAwareness, float minR, float maxR, float increment) {
    float fLen = (_vLastAwarenessPos - Gu::getCamera()->getPos()).squaredLength();
    //Reset awareness when we move some small value.
    if (fLen > 0.05) {
        fAwareness = minR;
        _vLastAwarenessPos = Gu::getCamera()->getPos();
    }

    // float aMax = CongaUtils::getBvhAwarenessMaxRadius();
    // float aMin = CongaUtils::getBvhAwarenessMinRadius();
    fAwareness = MathUtils::broClamp(fAwareness, minR, maxR);

    //TODO: fix this
    //if (_setGenStage1.size() == 0 && _setGenStage2.size() == 0) {
        fAwareness += increment;//CongaUtils::getBvhAwarenessIncrement();
    //}
}
void PhysicsGridAwareness::getAwarenessBox(Box3f& c) {
    return getAwarenessBox(c, _fAwarenessRadiusXZ, _fAwarenessRadiusY);
}
void PhysicsGridAwareness::getAwarenessBox(Box3f& c, float rxz, float ry) {
    vec3 vb = getAwarenessPos();//getRaycastViewCenter(); //getContext()->getCamera()->getPos();
    c._min.x = vb.x - rxz;
    c._min.y = vb.y - ry;
    c._min.z = vb.z - rxz;
    c._max.x = vb.x + rxz;
    c._max.y = vb.y + ry;
    c._max.z = vb.z + rxz;
}
vec3 PhysicsGridAwareness::getAwarenessPos() {
    //***Don't dork with this because we use it to collect visible nodes and
    // all nodes now contain positions.
    //This is a debatable position.  If we use the "raycast" version we end up
    //creating / deleting tons of cells. 
    vec3 vp = Gu::getCamera()->getPos();//getRaycastViewCenter(); //->getCamera()->getPos();//getProjectedViewCenter();
                                                      // vp.y = 0;

    return vp;
}








}//ns BR2
