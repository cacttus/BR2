/**
*  @file PhysicsGridAwareness.h
*  @date January 11, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __PHYSICSGRIDAWARENESS_15156936891785483737_H__
#define __PHYSICSGRIDAWARENESS_15156936891785483737_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class PhysicsGridAwareness
*  @brief This "fuzzy region" tells us how many globs we can load, and unload in the scene.
*/
class PhysicsGridAwareness : public VirtualMemory {
public:
  PhysicsGridAwareness(std::shared_ptr<PhysicsManager> pm, MpFloat rxz, float incXZ, MpFloat ry, float incY);
  virtual ~PhysicsGridAwareness() override;

  void update(float dt);

  void getAwarenessBox(Box3f& c);
  void getAwarenessBox(Box3f& c, float rxz, float ry);
  vec3 getAwarenessPos();

private:
  float _fAwarenessRadiusXZ;
  float _fAwarenessRadiusY;
  vec3 _vLastAwarenessPos;
  float _incXz;
  MpFloat _mpXz;
  MpFloat _mpY;
  float _incY;
  std::shared_ptr<PhysicsManager> _pPhysicsManager = nullptr;

  void updateAwarenessSpheroidAxis(float& fAwareness, float minR, float maxR, float increment);
};

}//ns BR2



#endif
