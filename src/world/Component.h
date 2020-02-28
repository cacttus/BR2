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
public:
  Component();
  virtual ~Component() override;
  
  virtual void afterAdded() = 0;
  virtual void update(float d) = 0;

  void setNode(std::shared_ptr<SceneNode> ob) { _pWorldObject = ob;  }

  template < class Tx = SceneNode >
  std::shared_ptr<Tx> getNode() { return std::dynamic_pointer_cast<Tx>(_pWorldObject); }
private:
  std::shared_ptr<SceneNode> _pWorldObject = nullptr;
};

}//ns BR2



#endif
