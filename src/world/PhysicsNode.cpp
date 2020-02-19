#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../world/PhysicsNode.h"
#include "../world/Manifold.h"
#include "../world/PhysicsManager.h"
#include "../world/Scene.h"

namespace BR2 {
PhysicsNode::PhysicsNode() : SceneNode(ps) {
  _pSpeedbox = new Box3f();
  _pManifold = std::make_shared<GridManifold>();
  _vHistoryPos.push_back(vec3(0, 0, 0));
  _vHistoryVel.push_back(vec3(0, 0, 0));
  //  _vTempAcc = 0.0f;S
  _vTempPos = getPos();
  _vTempVel = 0.0f;
  //calcBoundBox();//don't call this here, tTHIIS is null
}
PhysicsNode::~PhysicsNode() {
  //   DEL_MEM(_pCellManifold);
 // DEL_MEM(_pManifold);
  DEL_MEM(_pSpeedbox);
}
//void PhysicsNode::init() {
//  BaseNode::init();
//}
void PhysicsNode::setVelocity(vec3& v) {
  _vVelocity = v;
  //Don't update anything here. we set this often.
}
std::shared_ptr<TreeNode> PhysicsNode::attachChild(std::shared_ptr<TreeNode> pChild) {
  if (std::dynamic_pointer_cast<PhysicsNode>(pChild) != nullptr) {
    //*Make sure the node is not in the scene.
    getScene()->getPhysicsManager()->tryRemoveObj(std::dynamic_pointer_cast<PhysicsNode>(pChild));
  }
  else {
    //This is not an error - like ArmatureNode will come here it's not a physicsnode.
    //THis is just an extra safe check I guess
    //cast fail
//    Gu::debugBreak();
  }
  return TreeNode::attachChild(pChild);
}

void PhysicsData::serialize(std::shared_ptr<BinaryFile> fb) {
  NodeData::serialize(fb);
}
void PhysicsData::deserialize(std::shared_ptr<BinaryFile> fb) {
  NodeData::deserialize(fb);
}
void PhysicsNode::validateSanePhysics() {
  vec3 pos = getPos();
  vec3 origpos = pos;

  //Make this a huge number.
#define PHY_MAX_VELOCITY_LENGTH (120.0)
#define PHY_MAX_VELOCITY_LENGTH_SQR (PHY_MAX_VELOCITY_LENGTH*PHY_MAX_VELOCITY_LENGTH)
#define PHY_MAX_OBJECT_DISTANCE (10000000.0f)

    //if (_vTempAcc.squaredLength() > PHY_MAX_VELOCITY_LENGTH_SQR) {
    //    //You're going too fast, stap.//Stop moving.
    //    _vTempAcc = 0;
    //    Gu::debugBreak();
    //}
  if (_vVelocity.squaredLength() > PHY_MAX_VELOCITY_LENGTH_SQR) {
    //You're going too fast, stap.//Stop moving.
//    Gu::debugBreak();
    _vVelocity = 0;
  }
  if (!pos.isNormalFloat()) {
    //    Gu::debugBreak();
    pos = 0;
  }

#define pos_msg(aa) BroLogWarn("Object has reached the edge of the world!! p=("+ aa.x+ " "+ aa.y+ " "+ aa.z + ") resetting position.")

  // if the object is out of bounds throw it up to the sky
  if (pos.x < -PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.x = PHY_MAX_OBJECT_DISTANCE - getBoundBoxObject()->getWidth(); }
  if (pos.y < -PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.y = PHY_MAX_OBJECT_DISTANCE - getBoundBoxObject()->getHeight(); }
  if (pos.z < -PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.z = PHY_MAX_OBJECT_DISTANCE - getBoundBoxObject()->getDepth(); }
  if (pos.x > PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.x = -PHY_MAX_OBJECT_DISTANCE + getBoundBoxObject()->getWidth(); }
  if (pos.y > PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.y = -PHY_MAX_OBJECT_DISTANCE + getBoundBoxObject()->getHeight(); }
  if (pos.z > PHY_MAX_OBJECT_DISTANCE) { pos_msg(pos); pos.z = -PHY_MAX_OBJECT_DISTANCE + getBoundBoxObject()->getDepth(); }

  if (pos != origpos) {
    setPos(std::move(pos));
  }

}
//void PhysicsNode::calcBoundBox() {
//    Box3f* pBox = getBoundBoxObject();
//    calcBoundBox(pBox, getPos());
//}

void PhysicsNode::calcSpeedBox() {
  vec3 vVel = getVelocity();
  Box3f* pSpeedbox = getSpeedbox();
  pSpeedbox->_min = getBoundBoxObject()->_min;
  pSpeedbox->_max = getBoundBoxObject()->_max;

  if (vVel.x > 0) {
    pSpeedbox->_max.x += vVel.x;
  }
  else {
    pSpeedbox->_min.x += vVel.x;
  }

  if (vVel.y > 0) {
    pSpeedbox->_max.y += vVel.y;
  }
  else {
    pSpeedbox->_min.y += vVel.y;
  }

  if (vVel.z > 0) {
    pSpeedbox->_max.z += vVel.z;
  }
  else {
    pSpeedbox->_min.z += vVel.z;
  }

  pSpeedbox->validateBoundBox();
}

void PhysicsNode::setTemps(vec3& vVel, uint64_t frameId) {
  //Last Pos
  _vHistoryPos.push_back(getPos());
  if ((int)_vHistoryPos.size() > c_vHistoryPosSize) {
    _vHistoryPos.erase(_vHistoryPos.begin());
  }
  _vHistoryVel.push_back(getVelocity());
  if ((int)_vHistoryVel.size() > c_vHistoryVelSize) {
    _vHistoryVel.erase(_vHistoryVel.begin());
  }
  //Store the temps that get modified in Smasher
  _vTempPos = getPos();
  //20
  _vTempVel = vVel; //getVelocity();

//   _vTempAcc = vAccel;

  PhysicsManager::limitVelocity(_vTempVel);

  validateSanePhysics();
}
//void Phy25::unstickOb(Phy25* other){
//}
//void PhysicsNode::calcBoundBox() {
//    Box3f* pBox = getBoundBoxObject();
//    calcBoundBox(pBox, getPos());
//}
//void PhysicsNode::calcBoundBox(Box3f* __out_ pBox, vec3& obPos, float extra_pad) {
//
//    //if (_pGrid != nullptr)
//    //{
//    //    pBox->_min = _pGrid->getBoundBox()->_min - extra_pad;
//    //    pBox->_max = _pGrid->getBoundBox()->_max + extra_pad;
//
//    //    //Assuming the grid Z is set at 0
//    //    pBox->_max.y = 0 - extra_pad;
//    //    pBox->_min.y = -CongaUtils::getCellHeight() * 0.5f + extra_pad;
//    //}
//    //else if (_pPixObj != nullptr) {
//    float cd2 = CongaUtils::getObjDefaultBoxHeight2() + extra_pad;
//    float boxR;
//
//    //Objs can have custom boxes, otherwise use the cell width for box width.
//    if (_pPixObj->getSpec()->getBoundBoxRadiusXY() > 0.0f) {
//        boxR = _pPixObj->getSpec()->getBoundBoxRadiusXY() + extra_pad;
//    }
//    else {
//        boxR = CongaUtils::getObjDefaultBoxWidth2() + extra_pad;//If we use 0.5 the boxes will stick.
//    }
//    pBox->_min = vec3(obPos.x - boxR, obPos.y - boxR, obPos.z - cd2);
//    pBox->_max = vec3(obPos.x + boxR, obPos.y + boxR, obPos.z + cd2);
//}
//
//pBox->validateBoundBox();
//}




}//ns BR2
