/**
*  @file Component.h
*  @date 20200213
*  @author MetalMario971
*/
#pragma once
#ifndef __COMPONENT_15816346511616768678_H__
#define __COMPONENT_15816346511616768678_H__

#include "../base/BaseHeader.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class Component
*  @brief Component of a gameobject.
*/
class Component : public VirtualMemoryShared<Component> {
  friend class SceneNode;
public:
  Component();
  virtual ~Component() override;

  virtual void onAddedToNode() = 0;
  virtual void onUpdate(float dt) = 0;

  template < class Tx = SceneNode >
  std::shared_ptr<Tx> getNode() { return std::dynamic_pointer_cast<Tx>(_pWorldObject); }
  std::shared_ptr<Scene> getScene() { return _pScene; }

private:
  std::shared_ptr<SceneNode> _pWorldObject = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr; //This is a shortcut for the scene node which we will access a lot.
};

}//ns BR2



#endif
