#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../world/PhysicsGrid.h"
#include "../world/PhysicsWorld.h"
#include "../world/Manifold.h"
#include "../world/PhysicsNode.h"
#include "../gfx/ShaderMaker.h"


namespace BR2 {
///////////////////////////////////////////////////////////////////
PhysicsGrid::PhysicsGrid(std::shared_ptr<PhysicsWorld> pw, const ivec3& viPos, float fNodeWidth, float fNodeHeight, bool bEmpty) : _bEmpty(bEmpty),
    _pPhysicsWorld(pw), _viPos(viPos)
{
    _fNodeHeight = fNodeHeight;
    _fNodeWidth = fNodeWidth;


    //New Box
    _pBoundBox = new Box3f();

    pw->getNodeBoxForGridPos(viPos, *_pBoundBox);

    _pManifold = std::make_unique<NodeManifold>();

    for (int ni = 0; ni < c_nNeighbors; ++ni) {
        _pNeighbor[ni] = nullptr;
    }
}
PhysicsGrid::~PhysicsGrid()
{
    DEL_MEM(_pBoundBox);
    //DEL_MEM(_pManifold);

}
///////////////////////////////////////////////////////////////////
std::shared_ptr<PhysicsGrid> PhysicsGrid::getNeighborOffset(float off_x, float off_y, float off_z) {
    vec3 pt = getBoundBox()->center();
    vec3 pt2(
        pt.x + off_x
        , pt.y + off_y
        , pt.z + off_z
    );
    ivec3 ipt = _pPhysicsWorld->v3Toi3Node(pt2);
    return _pPhysicsWorld->getNodeAtPos(ipt);
}
bool PhysicsGrid::canDelete() {
    if (getIsGenerating() == true) {
        return false;
    }

    //Do not unload persistent objects.
    for (std::shared_ptr<PhysicsNode> ob : *getManifold()->getAll()) {
        if (std::dynamic_pointer_cast<PhysicsSpec>(ob->getSpec())->getIsPersistent() == true) {
            return false;
        }
    }

    return true;
}
void PhysicsGrid::linkGrids() {
    //Boudbox is not constructed here.
    vec3 cp = getBoundBox()->center();
    float nw = _pPhysicsWorld->getNodeWidth();
    float nh = _pPhysicsWorld->getNodeHeight();

    vec3 nL = cp + vec3(-nw, 0, 0);
    vec3 nR = cp + vec3(nw, 0, 0);
    vec3 nB = cp + vec3(0, -nh, 0);
    vec3 nT = cp + vec3(0, nh, 0);
    vec3 nA = cp + vec3(0, 0, -nw);
    vec3 nF = cp + vec3(0, 0, nw);

    ivec3 np[6];
    np[0] = _pPhysicsWorld->v3Toi3Node(nL);
    np[1] = _pPhysicsWorld->v3Toi3Node(nR);
    np[2] = _pPhysicsWorld->v3Toi3Node(nB);
    np[3] = _pPhysicsWorld->v3Toi3Node(nT);
    np[4] = _pPhysicsWorld->v3Toi3Node(nA);
    np[5] = _pPhysicsWorld->v3Toi3Node(nF);

    std::shared_ptr<PhysicsGrid> n[6];
    //This should be OK since this happens asynchronously.
    n[0] = _pPhysicsWorld->getNodeAtPos(np[0]);
    n[1] = _pPhysicsWorld->getNodeAtPos(np[1]);
    n[2] = _pPhysicsWorld->getNodeAtPos(np[2]);
    n[3] = _pPhysicsWorld->getNodeAtPos(np[3]);
    n[4] = _pPhysicsWorld->getNodeAtPos(np[4]);
    n[5] = _pPhysicsWorld->getNodeAtPos(np[5]);

    linkToGrid(n[0], PhysicsGridSide::e::gL);
    linkToGrid(n[1], PhysicsGridSide::e::gR);
    linkToGrid(n[2], PhysicsGridSide::e::gB);
    linkToGrid(n[3], PhysicsGridSide::e::gT);
    linkToGrid(n[4], PhysicsGridSide::e::gA);
    linkToGrid(n[5], PhysicsGridSide::e::gF);
}
void PhysicsGrid::unlinkGrids() {
    for (int i = 0; i < c_nNeighbors; ++i) {
        if (_pNeighbor[i] != nullptr) {
            PhysicsGridSide::e opp = getOppNeighborIndex((PhysicsGridSide::e)i);
            _pNeighbor[i]->unsetGridNeighbor(opp);
        }
    }
}

void PhysicsGrid::unsetGridNeighbor(PhysicsGridSide::e eSide) {
    //we must not already be nullptr or there's an integrity problem.
    AssertOrThrow2(_pNeighbor[eSide] != nullptr);
    _pNeighbor[eSide] = nullptr;
}
void PhysicsGrid::setGridNeighbor(std::shared_ptr<PhysicsGrid> pNeighbor, PhysicsGridSide::e eNeighbor) {
    if (_pNeighbor[eNeighbor] != nullptr) {
        //Integrity check.
        AssertOrThrow2(_pNeighbor[eNeighbor] == pNeighbor);
    }
    _pNeighbor[eNeighbor] = pNeighbor;
}
void PhysicsGrid::linkToGrid(std::shared_ptr<PhysicsGrid> pNeighbor, PhysicsGridSide::e eNeighbor) {

    //We were getting this becvause of the w/h mixup
    AssertOrThrow2(pNeighbor != shared_from_this());

    if (_pNeighbor[eNeighbor] != nullptr) {
        //Neighbors must match.
        AssertOrThrow2(_pNeighbor[eNeighbor] == pNeighbor);
    }
    else {
        //We must not call this to unset neighbors
        AssertOrThrow2(_pNeighbor[eNeighbor] == nullptr);
    }

    if (pNeighbor != nullptr && (eNeighbor == PhysicsGridSide::e::gT || eNeighbor == PhysicsGridSide::e::gB)) {
        int nnn = 0;
        nnn++;
    }

    this->setGridNeighbor(pNeighbor, eNeighbor);
    if (pNeighbor != nullptr) {
        pNeighbor->setGridNeighbor(shared_from_this(), getOppNeighborIndex(eNeighbor));
    }
}
PhysicsGridSide::e PhysicsGrid::getOppNeighborIndex(PhysicsGridSide::e iNeighbor) {
    if (iNeighbor == PhysicsGridSide::e::gB) { return  PhysicsGridSide::e::gT; }
    else if (iNeighbor == PhysicsGridSide::e::gR) { return  PhysicsGridSide::e::gL; }
    else if (iNeighbor == PhysicsGridSide::e::gT) { return  PhysicsGridSide::e::gB; }
    else if (iNeighbor == PhysicsGridSide::e::gL) { return  PhysicsGridSide::e::gR; }
    else if (iNeighbor == PhysicsGridSide::e::gA) { return  PhysicsGridSide::e::gF; }
    else if (iNeighbor == PhysicsGridSide::e::gF) { return  PhysicsGridSide::e::gA; }

    BroThrowNotImplementedException();
    //return  World25Side::e::gT;
}
std::shared_ptr<PhysicsGrid> PhysicsGrid::getNeighbor(int iSide) {
    AssertOrThrow2(iSide < c_nNeighbors);
    return _pNeighbor[iSide];
}
vec3 PhysicsGrid::getNodeCenterR3() {
    vec3 ret = getBoundBox()->center();

    return ret;
}
vec3 PhysicsGrid::getOriginR3() {
    vec3 ret;
    ret.x = (float)_viPos.x * _pPhysicsWorld->getNodeWidth();
    ret.y = (float)_viPos.y * _pPhysicsWorld->getNodeHeight();
    ret.z = (float)_viPos.z * _pPhysicsWorld->getNodeWidth();
    return ret;
}


}//ns Game
