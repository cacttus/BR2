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
  SceneNode(std::shared_ptr<NodeData> nd = nullptr);
  virtual ~SceneNode() override;

  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  void collect(std::shared_ptr<RenderBucket> rb);

  void compileWorldMatrix();
  virtual void calcBoundBox();
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad);
  template <class Tx> bool findNode(std::shared_ptr<Tx>& __out_ node);

  std::shared_ptr<SceneNode> clone();
  void copy(std::shared_ptr<SceneNode> rhs);

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
  void setNodeData(std::shared_ptr<NodeData> nd) { _pNodeData = nd; }
  std::shared_ptr<NodeData> getNodeData() { return _pNodeData; }
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
  string_t getName();
  Hash32 getSpecNameHashed();
  vec3 getFinalPos();

  //mat4& getFinal() {return _mFinal;}
  bool isMeshNode();
  bool isBoneNode();
  bool isArmatureNode();
  bool isSkinnedMesh();
  bool isLightNode();
  bool isModelNode();
  bool isCameraNode();

protected:
  std::shared_ptr<BoneNode> _pBoneParent = nullptr;
  std::shared_ptr<NodeData> _pNodeData = nullptr;
  Box3f* _pBox = nullptr; //Computed AABB.
  OBB* _pOBB = nullptr; //Computed OBB.
  vec3 _vViewNormal; //TODO: use an orthonormal matrix.
  vec3 _vRotationNormal;//User manipulated rotation normal.
  float _fRotation; //User manipulated rotation.
  vec3 _vScale; //User manipulated scale.
  mat4 _mWorld; //User manipulated PRS that has been compiled.
  mat4 _mLocal; //Local animation outside of the manipulated PRS.
  float _fCullDistance2 = FLT_MAX;//squared cull distance.  Defaults to ALWAYS show
  mat4 _mAnimated; //The computed animation matrix.
  bool _bHidden = false; //Visibility

  void setLocalBind();
  void animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
  void applyLocalAnimation(std::shared_ptr<Animator>);
  void applyParent();

private:
  NodeId _iNodeId = 0;//Note: this is also use for picking and must therefore be 32 bits (not 64)
  vec3 _vPos;
  vec3 _vLastPos;
  bool _bTransformChanged = true;
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
