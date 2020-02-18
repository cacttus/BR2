/**
*  @file PhysicsShapes.h
*  @date January 12, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __PHYSICSSHAPES_1515775014582190114_H__
#define __PHYSICSSHAPES_1515775014582190114_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class PhysicsShapes
*  @brief
*
*/
class PhysicsShape : public VirtualMemory {
public:
  PhysicsShape() {}
  virtual ~PhysicsShape() override {}
  virtual void deserialize(std::shared_ptr<BinaryFile> fb);
  virtual void serialize(std::shared_ptr<BinaryFile> fb);
private:
  bool _bKinematic = false;//Is bound to armature, and animates.
  bool _bDynamic = true; // reacts physically with scenery. (otherwise it's just a trigger)

};
class SphereShape : public PhysicsShape {
public:
  float& getRadius() { return _fRadius; }
  SphereShape() {}
  SphereShape(VertexBufferPointer& vbPointer, float r);
  virtual ~SphereShape() override;
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;
private:
  float _fRadius;

};
class HullShape : public PhysicsShape {
public:
  std::vector<vec3>& getPoints() { return _vecPoints; }
  HullShape() {}
  HullShape(VertexBufferPointer& vbPointer);
  virtual ~HullShape() override;
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;
private:
  std::vector<vec3> _vecPoints;
};
//class BoxShape : public PhysicsShape {
//    Box3f* _pBox = nullptr;
//public:
//    Box3f* getBox() { return _pBox; }
//    BoxShape() { _pBox = std::make_shared<Box3f>();}
//    virtual ~BoxShape() override {
//        DEL_MEM(_pBox);
//    }
//};

}//ns BR2



#endif
