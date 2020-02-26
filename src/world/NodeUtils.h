/**
*  @file NodeUtils.h
*  @date 20200225
*  @author MetalMario971
*/
#pragma once
#ifndef __NODEUTILS_1582685968660193936_H__
#define __NODEUTILS_1582685968660193936_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class NodeUtils
*  @brief
*/
class NodeUtils : public VirtualMemoryShared<NodeUtils> {
public:
  static std::shared_ptr<CameraNode> getActiveCamera(std::shared_ptr<LightManager> lm);
  static std::shared_ptr<CameraNode> getActiveCamera(std::shared_ptr<PhysicsWorld> pw);
  static std::shared_ptr<CameraNode> getActiveCamera(std::shared_ptr<SceneNode> lm);
  static std::shared_ptr<PhysicsWorld> getPhysicsWorld(std::shared_ptr<SceneNode> lm);
  static std::shared_ptr<Picker> getPicker(std::shared_ptr<SceneNode> lm);
};

}//ns Game



#endif
