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
/**
*  @class SceneNode
*  @brief The base class for nodes in the scenegraph. @sa Scene
*/
class SceneNode : public TreeNode, public ISerializable<SceneNode> {
public:
  SceneNode();
  virtual ~SceneNode() override;

  virtual void update(float delta, std::shared_ptr<CameraNode> cam, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  void collect(std::shared_ptr<RenderBucket> rb);

  void compileWorldMatrix();
  virtual void calcBoundBox();
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad);
  template <class Tx> bool findNode(std::shared_ptr<Tx>& __out_ node);

  virtual std::shared_ptr<SceneNode> clone(bool deep);
  virtual void copy(std::shared_ptr<SceneNode> rhs, bool deep);

  virtual void serialize(std::shared_ptr<BinaryFile> fb);
  virtual void deserialize(std::shared_ptr<BinaryFile> fb);

  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp) override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawForwardDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override;
  virtual void drawTransparent(RenderParams& rp) override;
  virtual void drawUI(RenderParams&) override {}
  void drawBoxes(std::shared_ptr<UtilMeshInline> mi);
  void drawBox(std::shared_ptr<UtilMeshInline> mi);
  void drawBoneBindBoxes(std::shared_ptr<ArmatureNode> an, std::shared_ptr<UtilMeshInline> mi);
  void drawMeshBoxes(std::shared_ptr<UtilMeshInline> mi);
  void drawBoneBoxes(std::shared_ptr<UtilMeshInline> mi);

  void setBoneParent(std::shared_ptr<BoneNode> bn) { _pBoneParent = bn; }
  std::shared_ptr<BoneNode> getBoneParent() { return _pBoneParent; }
  void setHidden(bool bHidden) { _bHidden = bHidden; }
  bool getHidden() { return _bHidden; }
  NodeId getId() { return _iNodeId; }
  OBB* getOBB() { return _pOBB; }
  Box3f* getBoundBoxObject() { return _pBox; }
  const vec3& getPos() { return _vPos; }
  void setPos(const vec3&& p);
  vec3 getViewNormal() { return _vViewNormal; }
  void setViewNormal(vec3& p);
  void setScale(vec3& v);
  vec3 getScale() { return _vScale; }
  bool getTransformChanged() { return _bTransformChanged; }
  void setRot(vec4&& axis_angle_radians);
  mat4& getLocal() { return _mLocal; }
  mat4& getWorld() { return _mWorld; }
  mat4& getAnimated() { return _mAnimated; }
  std::shared_ptr<Scene> getScene();
  Hash32 getSpecNameHashed() { return _iNameHashed; }
  vec3 getFinalPos();
  ParentType getParentType() { return _eParentType; }
  Box3f* getBindingBoundBox() { return _pBindingBoundBox; }
  string_t getName() { return _strName; }
  mat4& getBind() { return _mBind; }
  mat4& getInvBind() { return _mInvBind; }
  mat4& getParentInverse() { return _mParentInverse; }
  void setParentInverse(mat4& m) { _mParentInverse = m; }
  void setBind(mat4& bind);
  void setInvBind(mat4& bind);
  void setParentName(string_t str, ParentType ee) { _strParentName = str; setParentType(ee); }
  string_t getParentName() { return _strParentName; }
  Hash32 getNameHashed() { return _iNameHashed; }

  bool isMeshNode();
  bool isBoneNode();
  bool isArmatureNode();
  bool isSkinnedMesh();
  bool isLightNode();
  bool isModelNode();
  bool isCameraNode();

  Box3f* getBoundBox() { return _pBox; }

  void addComponent(std::shared_ptr<Component> comp);
  vec3 getVelocity() { return _vVelocity; }
protected:
  mat4 _mWorld; //User manipulated PRS that has been compiled.
  mat4 _mLocal; //Local animation outside of the manipulated PRS.
  vec3 _vViewNormal = vec3(0, 0, -1);; //TODO: use an orthonormal matrix.
  vec3 _vRotationNormal = vec3(0,1 , 0);//User manipulated rotation normal.
  float _fRotation = 0; //User manipulated rotation.
  vec3 _vScale = vec3(1, 1, 1);; //User manipulated scale.
  vec3 _vVelocity = vec3(0, 0, 0);

private:
  NodeId _iNodeId = 0;//Note: this is also use for picking and must therefore be 32 bits (not 64)
  vec3 _vPos;
  vec3 _vLastPos;
  bool _bTransformChanged = true;

  Box3f* _pBindingBoundBox = nullptr;//Base Box.
  string_t _strName;
  Hash32 _iNameHashed;
  mat4 _mInvBind;
  mat4 _mBind;
  mat4 _mParentInverse;   //This is just for mesh objects that have mesh parents.
  string_t _strParentName;
  ParentType _eParentType = ParentType::None;

  std::vector<std::shared_ptr<Component>> _vecComponents;

  std::shared_ptr<BoneNode> _pBoneParent = nullptr;
  Box3f* _pBox = nullptr; //Computed AABB.
  OBB* _pOBB = nullptr; //Computed OBB.
  float _fCullDistance2 = FLT_MAX;//squared cull distance.  Defaults to ALWAYS show
  mat4 _mAnimated; //The computed animation matrix.
  bool _bHidden = false; //Visibility

  void setLocalBind();
  void animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  void applyLocalAnimation(std::shared_ptr<Animator>);
  void applyParent();
  void setParentType(ParentType pt) { _eParentType = pt; }
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



}//ns BR2



#endif
