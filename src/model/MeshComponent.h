/**
*  @file MeshNode.h
*  @date March 26, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __MeshNode_1490572482630426783_H__
#define __MeshNode_1490572482630426783_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../world/Component.h"

namespace BR2 {
/**
*  @class MeshNode
*  @brief Basically this is MeshNode*
*     We carete this from tnhse meshspec class..
* Mesh should be a component, really.
* TODO: we can change this to "mesh" or something.
*/
class MeshComponent : public Component {
public:
  MeshComponent(std::shared_ptr<GLContext> context, std::shared_ptr<MeshData> ps, std::shared_ptr<SceneNode> owner = nullptr);
  virtual ~MeshComponent() override;

  void update(float delta, std::shared_ptr<CameraNode> cam, std::map<Hash32, std::shared_ptr<Animator>>& pAnimator);

  void drawDeferred(RenderParams& rp);
  void drawTransparent(RenderParams& rp);
  void drawForward(RenderParams& rp);
  void drawShadow(RenderParams& rp);
  void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad);

  // virtual void afterChildInserted(std::shared_ptr<TreeNode>) override;

  std::vector<std::shared_ptr<BoneNode>>& getBoneNodesOrdered() { return _vecBoneNodesOrdered; }//Bone nodes ordered by, for each armature, Armature ID, then Bone ID 
  void printDataToStdout();
  void computeAndDispatchSkin();

  std::shared_ptr<Material> getMaterial() { return _pMaterial; }
  std::shared_ptr<MeshData> getMeshData() { return _pMeshData; }
  uint32_t getPickId() { return _iPickId; }
  std::shared_ptr<GLContext> getContext() { return _pContext; }

protected:
  uint32_t _iPickId = INVALID_PICK_ID; //Pick Id must be separate from NodeId because non-node things can be picked.
  std::shared_ptr<MeshData> _pMeshData = nullptr;
  std::shared_ptr<ModelNode> _pModelNode = nullptr;
  std::shared_ptr<Material> _pMaterial = nullptr;
  std::shared_ptr<ShaderStorageBuffer> _pArmJoints = nullptr; //Skin 12/9/2017
  std::vector<std::shared_ptr<BoneNode>> _vecBoneNodesOrdered;//Bone nodes ordered by, for each armature, Armature ID, then Bone ID 
  std::vector<std::shared_ptr<ArmatureNode>> _vecArmaturesOrdered;//Bone nodes ordered by, for each armature, Armature ID, then Bone ID 
  std::shared_ptr<GLContext> _pContext = nullptr;

  //If we wanted to be picky we could make a "modifier" class to mimic 3D editors
  void createSkin();
  void computeSkinFrame();
  void copyJointsToGpu();
  void orderBoneNodesForGpu();
  void getMeshLocalMatrix(mat4& __out_ matMesh);
  void bindSkin(std::shared_ptr<ShaderBase> shader);
  void showNoMaterialError();
  void draw(RenderParams& rp, bool bTransparent);

};

}//ns BR2



#endif
