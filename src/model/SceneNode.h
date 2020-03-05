/**
*  @file Node3base.h
*  @date November 25, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __NODE3BASE_14801270783276389065_H__
#define __NODE3BASE_14801270783276389065_H__

#include "../base/BaseHeader.h"
#include "../math/MathHeader.h"
#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../world/WorldHeader.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"
#include "../base/TreeNode.h"

namespace BR2 {
class UtilMeshInline;
class BaseSpec : public VirtualMemoryShared<BaseSpec> {
public:
  BaseSpec() {} //Serialized version
  BaseSpec(string_t strName);
  virtual ~BaseSpec() override;
  ParentType::e getParentType() { return _eParentType; }
  Box3f* getBoundBoxObject() { return _pBox; }

  string_t getName() { return _strName; }
  Hash32 getNameHashed() { return _iNameHashed; }
  //void setBasis(mat4& basis){
  //    _mBasis = basis;
  //}
 // mat4& getBasis() { return _mBasis; }
  mat4& getBind() { return _mBind; }
  mat4& getInvBind() { return _mInvBind; }
  mat4& getParentInverse() { return _mParentInverse; }
  void setParentInverse(mat4& m) { _mParentInverse = m; }
  void setBind(mat4& bind) {
    _mBind = bind;
    _mInvBind = _mBind.inverseOf();
  }
  void setInvBind(mat4& bind) {
    //_mBind = bind;
    _mInvBind = bind;
  }
  void setParentName(string_t str, ParentType::e ee) { _strParentName = str; setParentType(ee); }
  string_t getParentName() { return _strParentName; }
  virtual void serialize(std::shared_ptr<BinaryFile> fb);
  virtual void deserialize(std::shared_ptr<BinaryFile> fb);

protected:
  Box3f* _pBox = nullptr;//Base Box.
  string_t _strName;
  Hash32 _iNameHashed;
  mat4 _mInvBind;
  mat4 _mBind;
  mat4 _mParentInverse;   //This is just for mesh objects that have mesh parents.
  //mat4 _mBasis;//Where the object is - base rotation &c.
  string_t _strParentName;
  ParentType::e _eParentType = ParentType::e::None;
  void setParentType(ParentType::e pt) { _eParentType = pt; }
};
/**
*    @class BaseNode
*    @brief The base of the node system.
*/
class SceneNode : public TreeNode {
public:
  SceneNode(std::shared_ptr<BaseSpec>);
  virtual ~SceneNode() override;

  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  virtual void calcBoundBox();
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad);

  virtual void afterAddedToScene(std::shared_ptr<Scene> scene);
  virtual void afterRemovedFromScene(std::shared_ptr<Scene> scene);

  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp) override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawForwardDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override;
  virtual void drawTransparent(RenderParams& rp) override;
  virtual void drawUI(RenderParams& rp) override {}

  void addComponent(std::shared_ptr<Component> comp);
  void compileWorldMatrix();

  std::shared_ptr<Scene> getScene();
  std::shared_ptr<InputManager> getInput(); 
  NodeId getId();
  OBB* getOBB();
  Box3f* getBoundBoxObject();
  const vec3& getPos();
  void setPos(const vec3&& p);
  vec3 getViewNormal();
  void setViewNormal(vec3& p);
  void setScale(vec3& v);
  vec3 getScale();
  bool getTransformChanged();
  void setRot(vec4&& axis_angle_radians);
  std::shared_ptr<BaseSpec> getSpec();
  mat4& getLocal();
  mat4& getWorld();
  mat4& getAnimated();
  vec3 getVelocity();
  void setVelocity(vec3& vel);
  void setBoneParent(std::shared_ptr<BoneNode> bn);
  std::shared_ptr<BoneNode> getBoneParent();
  bool isMeshNode();
  bool isBoneNode();
  bool isArmatureNode();
  bool isSkinnedMesh();
  bool isLightNode();
  bool isModelNode();
  bool isCameraNode();
  string_t getSpecName();
  Hash32 getSpecNameHashed();
  vec3 getFinalPos();
  void show();
  void hide();
  bool isHidden();

  void drawBoxes(std::shared_ptr<UtilMeshInline> mi);
  void drawBox(std::shared_ptr<UtilMeshInline> mi);
  void drawBoneBindBoxes(std::shared_ptr<ArmatureNode> an, std::shared_ptr<UtilMeshInline> mi);
  void drawMeshBoxes(std::shared_ptr<UtilMeshInline> mi);
  void drawBoneBoxes(std::shared_ptr<UtilMeshInline> mi);
  //void addShadowInfluence(std::shared_ptr<ShadowBox> psb);
  //void clearShadowInfluences();
  void collect(std::shared_ptr<RenderBucket> rb);

  template < typename Tx > bool findNode(std::shared_ptr<Tx>& __out_ node);
  template < typename Tx > std::shared_ptr<Tx> getData() {return std::dynamic_pointer_cast<Tx>(_pSpec);}

protected:
  void setLocalBind();
  void animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  void applyLocalAnimation(std::shared_ptr<Animator>);
  void applyParent();
  virtual void init();
protected:
  std::vector<std::shared_ptr<Component>> _vecComponents;
  std::shared_ptr<BaseSpec> _pSpec = nullptr;
  Box3f* _pBox = nullptr;
  OBB* _pOBB = nullptr;
  vec3 _vViewNormal;
  vec3 _vRotationNormal;
  float _fRotation;
  vec3 _vScale;
  mat4 _mWorld; //User manipulated PRS
  mat4 _mLocal;//Local animation outside of the manipulated PRS
  float _fCullDistance2 = FLT_MAX;//squared cull distance.  Defaults to ALWAYS show
  mat4 _mAnimated;
  std::shared_ptr<BoneNode> _pBoneParent = nullptr;
  //  std::set<std::shared_ptr<ShadowBox>> _setShadowInfluences;
  bool _bHidden = false;

  vec3 _velocity;

private:
  NodeId _iNodeId = 0;//Note: this is also use for picking and must therefore be 32 bits (not 64)
  vec3 _vPos;
  vec3 _vLastPos;
  bool _bTransformChanged = true;
  bool _bInitialized = false;
  void updateComponents(float delta);
  void startComponent(std::shared_ptr<Component> c, std::shared_ptr<Scene> s);
  void endComponent(std::shared_ptr<Component> c);
};


template < typename Tx >
bool SceneNode::findNode(std::shared_ptr<Tx>& __out_ node) {
  //Find a node (starting at this node) which matches the given node type: Tx
  std::shared_ptr<Tx> pc = std::dynamic_pointer_cast<Tx>(shared_from_this());
  if (pc != nullptr) {
    node = pc;
    return true;
  }

  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      if (pc != nullptr) {
        if (pc->findNode<Tx>(node) == true) {
          return true;
        }
      }
    }
  }
  return false;
}

}//ns Game



#endif
