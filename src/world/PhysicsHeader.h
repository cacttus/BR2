/**
*  @file PhysicsHeader.h
*  @date 20200220
*  @author MetalMario971
*/
#pragma once
#ifndef __PHYSICSHEADER_158226034357574117_H__
#define __PHYSICSHEADER_158226034357574117_H__

#include "../world/WorldHeader.h"
#include <set>


namespace BR2 {
class BoxCollision : public VirtualMemory {
public:
  BoxCollision() {
    clear();
  }
  virtual ~BoxCollision() {
  }
  void clear() {
    _ob1 = nullptr;
    _ob2 = nullptr;
    _bCollided = false;
    _t = FLT_MAX;
    _nStuck = 0;
    _bCollided = false;
  }

  std::shared_ptr<PhysicsNode> _ob1;
  std::shared_ptr<PhysicsNode> _ob2;
  float _t;        // time to collision between 0 and 1, 20151212 - NOW if we are stuck T will give us the distance to the volume (plane dist, or sphere distance)
  float _tx, _ty, _tz;
  bool _bCollided; // if collided
  bool _bStuck; // if stuck
  uint64_t _frameID;    // - Checks weather this possibility was tested.
  int32_t _nStuck;
  int32_t _iType; // plane=0 edge=1 pt=2
  CollisionResult::e _cx, _cy, _cz;

  int _ax_t0;//box3 SAT axis
};
class CheckedSet {
  std::map<std::shared_ptr<PhysicsNode>, std::set<std::shared_ptr<PhysicsNode>>> _mapChecked;
public:
  void clear() {

    _mapChecked.clear();
  }
  bool hasPair(std::shared_ptr<PhysicsNode> a, std::shared_ptr<PhysicsNode> b) {
    std::map<std::shared_ptr<PhysicsNode>, std::set<std::shared_ptr<PhysicsNode>>>::iterator iteA = _mapChecked.find(a);
    if (iteA != _mapChecked.end()) {
      std::set<std::shared_ptr<PhysicsNode>>::iterator iteB = iteA->second.find(b);
      if (iteB != iteA->second.end()) {
        return true;
      }
    }
    return false;
  }
  void addPair(std::shared_ptr<PhysicsNode> a, std::shared_ptr<PhysicsNode> b) {
    std::map<std::shared_ptr<PhysicsNode>, std::set<std::shared_ptr<PhysicsNode>>>::iterator iteA = _mapChecked.find(a);
    if (iteA == _mapChecked.end()) {
      std::set<std::shared_ptr<PhysicsNode>> bb;
      _mapChecked.insert(std::make_pair(a, bb));
      iteA = _mapChecked.find(a);
      bb.insert(b);
    }
    else {
      iteA->second.insert(b);
    }
  }
};
class StuckPair {
public:
  std::shared_ptr<PhysicsNode> _objA;
  std::shared_ptr<PhysicsNode> _objB;
};

}//ns Game



#endif
