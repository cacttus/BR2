/**
*  @file BaseCollider.h
*  @date March 31, 2017- Phy25
*  @date Jan 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __BASE_COLLIDER_14910105823111128827_H__
#define __BASE_COLLIDER_14910105823111128827_H__

#include "../world/WorldHeader.h"
#include "../world/Component.h"

namespace BR2 {
/**
*  @class BaseCollider
*  @brief Movement and physics properties for node objects.
*/
class BaseCollider : public Component {
public:
  BaseCollider();
  virtual ~BaseCollider();

  //virtual std::shared_ptr<TreeNode> attachChild(std::shared_ptr<TreeNode> pChild) override;
  
  GridManifold* getManifold() { return _pManifold; }
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
  Box3f* getBoundBox() { return _pBoundBox; }
  
  //Since we're not implementing this yet, let's just blank these out.
  void setNodeVelocity(vec3& nodeVel);
  void setNodePosition(vec3& nodePos);
  
  //**these are on the parent SceneNode
  //vec3 getVelocity() { return _vVelocity; }
  //void setVelocity(vec3& v);

  void validateSanePhysics();

  uint64_t _iStamp;

private:
  Box3f* _pSpeedbox = nullptr;
  Box3f* _pBoundBox = nullptr;
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
  GridManifold* _pManifold = nullptr;
};

}//ns BR2



#endif
