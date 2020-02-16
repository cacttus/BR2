/**
*  @file PhysicsNode.h
*  @date March 31, 2017- Phy25
*  @date Jan 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __PHYSICS_NODE_14910105823111128827_H__
#define __PHYSICS_NODE_14910105823111128827_H__

#include "../model/SceneNode.h"
#include "../world/WorldHeader.h"
#include "../model/NodeData.h"

namespace BR2 {
//It doesn't make sense to have the "shape" on the model, even though the model has velocity.  Same goes for mesh.
//TODO: remove spec
class PhysicsData : public NodeData {
public:
  PhysicsData() {} //deserialize ctor
  PhysicsData(string_t strName) : NodeData(strName) {}
  virtual ~PhysicsData() override {}

  bool getIsPersistent() { return false; }
  float getMass() { return 0.0f; } //PHY_STATIC_MASS
  float getHollow() { return false; }
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
};
/**
*  @class PhysicsNode
*  @brief Movement and physics properties for node objects.
*/
class PhysicsNode : public SceneNode {
public:
  PhysicsNode(std::shared_ptr<PhysicsData> ps);
  virtual ~PhysicsNode();

  virtual std::shared_ptr<TreeNode> attachChild(std::shared_ptr<TreeNode> pChild) override;
  std::shared_ptr<GridManifold> getManifold() { return _pManifold; }
  void setTemps(vec3& vAccel, uint64_t frameId);
  void setTempPos(vec3& v) { _vTempPos = v; }
  void setTempVelocity(vec3& v) { _vTempVel = v; }
  vec3 getTempPos() { return _vTempPos; }
  vec3 getTempVelocity() { return _vTempVel; }
  float getFriction() { return _fFriction; }
  void setIsAtRest(bool b) { _bIsAtRest = b; }
  uint64_t getLastMovedFrame() { return _iLastMovedFrame; }
  void setLastMovedFrame(uint64_t u) { _iLastMovedFrame = u; }
  uint64_t getLastAccFrame() { return _iLastAccFrame; }
  void setLastAccFrame(uint64_t u) { _iLastAccFrame = u; }
  vec3 getLastPos() { return _vHistoryPos[0]; }
  void calcSpeedBox();
  bool getIsHollow() { return _bHollow; }
  bool getIsAtRest() { return _bIsAtRest; }
  bool getIsStatic() { return _fMass <= 0.0f; }
  Box3f* getSpeedbox() { return _pSpeedbox; }
  vec3 getVelocity() { return _vVelocity; }
  void setVelocity(vec3& v);
  void validateSanePhysics();

  uint64_t _iStamp;

private:
  vec3 _vVelocity = vec3(0, 0, 0);
  Box3f* _pSpeedbox = nullptr;
  bool _bIsAtRest = true;
  float _fMass = 1.0f;
  bool _bHollow = false;

  float _fFriction = 0.90f; // 0.9 m/s
  uint64_t _iLastMovedFrame = 0;
  uint64_t _iLastAccFrame = 0;
  vec3 _vTempPos;
  vec3 _vTempVel;
  const int c_vHistorySize = 10;
  const int c_vHistoryPosSize = 10;
  std::vector<vec3> _vHistoryPos;
  const int c_vHistoryVelSize = 10;
  std::vector<vec3> _vHistoryVel;

  std::shared_ptr<GridManifold> _pManifold = nullptr;



};

}//ns Game



#endif
