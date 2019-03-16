#include "WorldEditState.h"
#include "../bottle/World25.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/WorldObj.h"
#include "../base/Logger.h"


namespace Game {
///////////////////////////////////////////////////////////////////
WorldEditState::WorldEditState()
{

}
WorldEditState::~WorldEditState()
{

}
///////////////////////////////////////////////////////////////////

NodeId WorldEditState::getSelectedObject() {
    return _pSelectedObj; 
}
void WorldEditState::setSelectedObject(NodeId ob) {
    _pSelectedObj = ob;
}
NodeId WorldEditState::getHoveredObject() {
    return _pHoveredObj; 
}
void WorldEditState::setEditMode(EditMode::e em){
    _eEditMode = em;
   // editModeChanged();
}

void WorldEditState::setHoverVertex(WorldCell* pc, vec3& normal, vec3& point, int i) {
    _pHoveredTile = pc;
    if(_pHoveredTile != nullptr){
        _bIsHovering = true;
        _vHoverNorm = normal;
        _vHoverPt = point;
        _iHoverVertex = i;

        if (GetAsyncKeyState(VK_F9) & 0x8000) {
            int nnn = 0;
            nnn++;
        }

        //Box
        Box3f* bnew = addHoverBox(pc->getBoundBoxBvh());
        vec3 c = bnew->center();

        if(i==0 || i==2 || i==4 || i==6) { bnew->_max.x = c.x; }
        else { bnew->_min.x = c.x; }

        if (i == 0 || i == 1 || i == 4 || i == 5) { bnew->_max.y = c.y; }
        else { bnew->_min.y = c.y; }

        if (i == 0 || i == 1 || i == 2 || i == 3) { bnew->_max.z = c.z; }
        else { bnew->_min.z = c.z; }
    }
}
void WorldEditState::setHoverTile(WorldCell* pc, vec3& normal, vec3& point) {
    _pHoveredTile = pc;
    if (_pHoveredTile != nullptr) {
        _bIsHovering = true;
        _vHoverNorm = normal;
        _vHoverPt = point;
        //Box
        addHoverBox(pc->getBoundBoxBvh());
    }
}
void WorldEditState::setHoverObject(std::shared_ptr<ModelNode> ob) {
    if(ob == nullptr){
        _pHoveredObj = NO_NODE_ID; 
    }
    else {
        _bIsHovering = true;
        _pHoveredObj = ob->getId();
        //Box
        addHoverBox(ob->getBoundBoxObject());
    }
}
void WorldEditState::clearHover() {
    _bIsHovering = false;
    _pHoveredObj = NO_NODE_ID;
    _pHoveredTile = nullptr;
    _vHoverNorm.construct(0, 0, 0);
    _vHoverPt.construct(0, 0, 0);
    _iHoverVertex = -1;
    _vecHoveredNodes.clear();
    for(size_t i=0; i<_vecHoverBoxes.size(); ++i){
        delete _vecHoverBoxes[i];
    }
    _vecHoverBoxes.clear();
}

W25Tile WorldEditState::getSelectedTile() {
    if(_eGridMeshLayer == GridMeshLayer::e::Opaque){
        return _pSelectedBaseTile;
    }
    else if (_eGridMeshLayer == GridMeshLayer::e::Transparent){
        return _pSelectedLiquidTile;
    }
    else {
        BroThrowNotImplementedException();
    }

}

Box3f* WorldEditState::addHoverBox(Box3f* b) {
    Box3f* bNew = new Box3f();
    *(bNew) = *(b);
    _vecHoverBoxes.push_back(bNew);
    return bNew;
}
Box3f* WorldEditState::addHoverBox(Box3f b) {
    Box3f* bNew = new Box3f();
    *(bNew) = b;
    _vecHoverBoxes.push_back(bNew);
    return bNew;
}










}//ns Game
