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
enum class AttachState { NotAttached, Attached, Detached };
/**
*  @class Component
*  @brief Component of a gameobject.
*/
class Component : public VirtualMemoryShared<Component> {
  friend class SceneNode;
public:
  Component();
  virtual ~Component() override;

  //Called after added to node.
  virtual void onStart() = 0;
  //Called every *update* frame
  virtual void onUpdate(float dt) = 0;
  //Called after removed from node, just before node is destroyed
  virtual void onExit() = 0;

  virtual void onDrawForward(RenderParams& rp) {}
  virtual void onDrawDeferred(RenderParams& rp) {}
  virtual void onDrawShadow(RenderParams& rp) {}
  virtual void onDrawTransparent(RenderParams& rp) {}
  virtual void onDrawDebug(RenderParams& rp) {}
  virtual void onDrawNonDepth(RenderParams& rp) {}

  template < class Tx = SceneNode >
  std::shared_ptr<Tx> getNode() { return std::dynamic_pointer_cast<Tx>(_pWorldObject); }
  std::shared_ptr<Scene> getScene() { return _pScene; }

private:
  std::shared_ptr<SceneNode> _pWorldObject = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr; //This is a shortcut for the scene node which we will access a lot.
  AttachState _eAttachState = AttachState::NotAttached;
};

}//ns BR2



#endif
