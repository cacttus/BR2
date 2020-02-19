#include "../model/SceneNode.h"
#include "../model/NodeData.h"
#include "../model/MeshNode.h"
#include "../gfx/CameraNode.h"
#include "../gfx/LightNode.h"
#include "../model/MeshData.h"
#include "../model/OBB.h"
#include "../model/UtilMeshInline.h"
#include "../model/MeshNode.h"
#include "../model/Model.h"
#include "../world/RenderBucket.h"
#include "../world/Scene.h"
#include "../math/Matrix4x4.h"
#include "../base/Logger.h"


namespace BR2 {
SceneNode::SceneNode() {
  _pNodeData = std::make_shared<NodeData>();

  _pBox = new Box3f();
  _pOBB = new OBB();
  _vPos = vec3(0, 0, 0);
  _mWorld = mat4::identity();
  _mLocal = mat4::identity();
  _vRotationNormal = vec3(0, 1, 0);
  _fRotation = 0;
  _vScale = vec3(1, 1, 1);
  _bTransformChanged = true;

  static NodeId id = 1;
  _iNodeId = id++;
}
SceneNode::SceneNode(std::shared_ptr<NodeData> data) : SceneNode() {
  _pNodeData = data;
}
SceneNode::~SceneNode() {
  //    _setShadowInfluences.clear();
  DEL_MEM(_pBox);
  DEL_MEM(_pOBB);
}
string_t SceneNode::getName() {
  if (getNodeData()) {
    return getNodeData()->getName();
  }
  else {
    Br2LogError("Treid to return a name of a model without a spec");
    Gu::debugBreak();
    return std::string("no spec");
  }
}
Hash32 SceneNode::getSpecNameHashed() {
  if (getNodeData()) {
    return getNodeData()->getNameHashed();
  }
  else {
    Br2LogError("Treid to return a hash of a model without a spec");
    Gu::debugBreak();
    return 0;
  }
}
void SceneNode::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  //    clearShadowInfluences();
      //We shouldn't need this
      //if (pParent == nullptr) {
      //    pParent = dynamic_cast<Node3base*>(getParent());
      //}
      //static int b = 0;

      //Force bind in case we don't have animations.
  setLocalBind();

  animate(mapAnimators);

  compileWorldMatrix();

  //So this is technically only ever going to 
  //be used by ModelNode, however it allows us to
  //modify ALL nodes if we need to.
  _mLocal = _mLocal * _mWorld;

  applyParent();

  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->update(delta, mapAnimators);
    }
  }

  calcBoundBox();//Must come after matrix calc
}
void SceneNode::compileWorldMatrix() {
  //So this is technically only ever going to 
  //be used by ModelNode, however it allows us to
  //modify ALL nodes if we need to.
  if (getTransformChanged() == false) {
    return;
  }

  mat4 mRot;
  mat4 mScl;
  mat4 mPos;

  mPos = mat4::getTranslation(_vPos);
  mRot = mat4::getRotationRad(_fRotation, _vRotationNormal);
  mScl = mat4::getScale(_vScale);
  _mWorld = mScl * mRot * mPos;
}
bool SceneNode::isCameraNode() {
  return getThis<CameraNode>() != nullptr;
}
bool SceneNode::isMeshNode() {
  return getThis<MeshNode>() != nullptr;
}
bool SceneNode::isSkinnedMesh() {
  return isMeshNode() && getThis<MeshNode>()->getMeshData()->hasSkin();
}
bool SceneNode::isBoneNode() {
  return getThis<BoneNode>() != nullptr;
}
bool SceneNode::isArmatureNode() {
  return getThis<ArmatureNode>() != nullptr;
}
bool SceneNode::isLightNode() {
  return getThis<LightNodeBase>() != nullptr;
}
bool SceneNode::isModelNode() {
  return getThis<ModelNode>() != nullptr;
}
void SceneNode::setLocalBind() {
  //Node3base* pParent = SafeCast(getParent(), Node3base*);

  if (isBoneNode()) {
    //Force bone nodes to animate in their own space without world transforms.
    _mLocal = mat4::identity();
    //std::shared_ptr<BoneNode> bn = dynamic_cast<std::shared_ptr<BoneNode>>(this);
    //bn->getBoneMat() = mat4::identity();
  }
  else if (isMeshNode() || isArmatureNode()) {
    if (isSkinnedMesh()) {
      _mLocal = mat4::identity();
    }
    else {
      _mLocal = getNodeData()->getBind();
    }
  }
  else {
    _mLocal = mat4::identity();
  }
}
void SceneNode::animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {

  for (std::pair<Hash32, std::shared_ptr<Animator>> p : mapAnimators) {
    applyLocalAnimation(p.second);
  }
}
void SceneNode::applyParent() {
  std::shared_ptr<SceneNode> pParent = std::dynamic_pointer_cast<SceneNode>(getParent());
  //Apply the parent transform.
  if (isBoneNode()) {
    _mLocal = getNodeData()->getInvBind() * _mLocal * getNodeData()->getBind();
    if (pParent != nullptr) {
      _mLocal = _mLocal * pParent->getLocal();
    }
  }
  else if (isMeshNode() || isArmatureNode() || isLightNode() || isModelNode() || isCameraNode()) {
    if (getNodeData() == nullptr) {
      if (pParent != nullptr) {
        _mLocal = _mLocal * pParent->getLocal();
      }
    }
    else if (
      getNodeData()->getParentType() == ParentType::e::Armature ||
      getNodeData()->getParentType() == ParentType::e::Object ||
      getNodeData()->getParentType() == ParentType::e::None) {

      if (isSkinnedMesh()) {
        //Setting this to ident here so we do't get confused.
        //We're doing all multiplies in CopyJointsToGPU no 1/20/18
        //see CopyJointsToGPU
      }
      else {
        _mLocal = _mLocal * getNodeData()->getParentInverse();
        if (pParent != nullptr) {
          _mLocal = _mLocal * pParent->getLocal();
        }
      }
    }
    else if (getNodeData()->getParentType() == ParentType::e::Bone) {
      if (getBoneParent() != nullptr) {
        static int n = 0;
        if (n == 0) {

          //TODO remove this debug stuff
          if (pParent != nullptr) {
            static int a = 4;
            if (a == 0)
              _mLocal = getBoneParent()->getNodeData()->getInvBind() * _mLocal * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 1)
              _mLocal = _mLocal * getBoneParent()->getNodeData()->getInvBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 2)
              _mLocal = getBoneParent()->getNodeData()->getBind() * _mLocal * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 3)
              _mLocal = _mLocal * getBoneParent()->getNodeData()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 4)
              //**ALMOST
              _mLocal = getBoneParent()->getNodeData()->getInvBind() * _mLocal * getBoneParent()->getNodeData()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 5)
              //Didn't work
              _mLocal = getBoneParent()->getNodeData()->getInvBind() * _mLocal * getBoneParent()->getLocal() * getBoneParent()->getNodeData()->getBind() * pParent->getLocal();//Armature
            if (a == 6)
              _mLocal = _mLocal * getBoneParent()->getNodeData()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
          }
        }
        if (n == 1) {
          _mLocal = getBoneParent()->getNodeData()->getInvBind() * getBoneParent()->getAnimated() * _mLocal * getBoneParent()->getNodeData()->getBind();
          if (getBoneParent()->getParent() != nullptr) {
            _mLocal = _mLocal * std::dynamic_pointer_cast<SceneNode>(getBoneParent()->getParent())->getLocal();
          }
          _mLocal = _mLocal * pParent->getLocal();//Armature
        }
        if (n == 2)
          _mLocal = _mLocal * getBoneParent()->getLocal();

        if (n == 3)
          _mLocal = pParent->getLocal() * getBoneParent()->getLocal() * _mLocal;
      }
    }
    else {
      Br2ThrowNotImplementedException();
    }

  }
  else {
    Br2LogWarnCycle("Unrecognized node type when updating parent.");
  }

  if (false) {
    vec4 p; vec4 r; vec4 s;
    _mLocal.decompose(p, r, s, true);
  }
}
void SceneNode::applyLocalAnimation(std::shared_ptr<Animator> anm) {
  std::shared_ptr<SceneNode> pParent = std::dynamic_pointer_cast<SceneNode>(getParent());
  if (anm != nullptr) {
    std::shared_ptr<ActionGroup> aa = anm->getAction();
    if (aa != nullptr) {
      if (getThis<SceneNode>()->getNodeData() != nullptr) {
        //*Since we can have multiple animations for a model, this will silently fail on nodes
        // who don't have this particular animation name.
        std::shared_ptr<ActionKeys> ak = aa->getActionKeys(getThis<SceneNode>()->getNodeData()->getNameHashed());
        if (ak != nullptr) {

          ak->animate(anm, _mAnimated);

          _mLocal = _mAnimated;

          if (false) {
            vec4 p; vec4 r; vec4 s;
            _mLocal.decompose(p, r, s, true);
          }
        }
      }
    }
  }
}

void SceneNode::calcBoundBox() {
  Box3f* pBox = getBoundBoxObject();
  calcBoundBox(*pBox, getPos(), 0.0f);
}
void SceneNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) {
  _pOBB->setInvalid();

  if (!isBoneNode()) {
    //aggregate all boxes, don'd do that for bone nodes we need them to have local boxes
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> bn = std::dynamic_pointer_cast<SceneNode>(tn);
      if (bn != nullptr) {
        pBox.genExpandByBox(bn->getBoundBoxObject());
      }
    }
  }

  float volume = pBox.getVolumePositiveOnly();

  //Default to 1 unit box if not calculated.
  if (pBox.validateBoundBox() == false) {
    //no spec... so we have no bound box to base the OBB transform..
    float radius = 1.0f;
    pBox._min = obPos - vec3(radius + extra_pad, radius + extra_pad, radius + extra_pad);
    pBox._max = obPos + vec3(radius + extra_pad, radius + extra_pad, radius + extra_pad);
  }

}
void SceneNode::drawBoneBindBoxes(std::shared_ptr<ArmatureNode> an, std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<SceneNode> that = getThis<SceneNode>();
  iterateDepthFirst<SceneNode>([&](std::shared_ptr<SceneNode> tx) {
    if (tx != that) {
      tx->drawBoneBindBoxes(an, mi);
    }
    return true;
    });
  if (isBoneNode()) {
    vec4 cOBB(1, 1, 0, 1);
    OBB ob;
    ob.calc(an->getLocal(), getNodeData()->getBoundBoxObject());
    mi->addBox(ob.getVerts(), &cOBB);
  }
}
void SceneNode::drawBoneBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<SceneNode> that = getThis<SceneNode>();

  iterateDepthFirst<SceneNode>([&](std::shared_ptr<SceneNode> tx) {
    if (tx != that) {
      tx->drawBoneBoxes(mi);
    }
    return true;
    });
  if (isBoneNode()) {
    vec4 cOBB(0.181, 0.8, 1, 1);
    mi->addBox(getOBB()->getVerts(), &cOBB);
  }
}
void SceneNode::drawMeshBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<SceneNode> that = getThis<SceneNode>();

  iterateDepthFirst<SceneNode>([&](std::shared_ptr<SceneNode> tx) {
    if (tx != that) {
      tx->drawMeshBoxes(mi);
    }
    return true;
    });
  if (isMeshNode()) {
    vec4 cOBB(1, 0, 1, 1);
    mi->addBox(getOBB()->getVerts(), &cOBB);
  }
}
void SceneNode::drawBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<SceneNode> that = getThis<SceneNode>();

  iterateDepthFirst<SceneNode>([&](std::shared_ptr<SceneNode> tx) {
    if (tx != that) {
      tx->drawBoxes(mi);
    }
    return true;
    });
  drawBox(mi);

}
void SceneNode::drawBox(std::shared_ptr<UtilMeshInline> mi) {
  vec4 cBB(1, 1, 0, 1);
  vec4 cOBB(0, 1, 1, 1);
  mi->addBox(getBoundBoxObject(), &cBB);
  if (getOBB()->getInvalid() == false) {
    mi->addBox(getOBB()->getVerts(), &cOBB);
  }
}
void SceneNode::drawDeferred(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawDeferred(rp);
    }
  }
}
void SceneNode::drawForward(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawForward(rp);
    }
  }
}
void SceneNode::drawShadow(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawShadow(rp);
    }
  }
}
void SceneNode::drawForwardDebug(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawForwardDebug(rp);
    }
  }
}
void SceneNode::drawNonDepth(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawNonDepth(rp);
    }
  }
}
void SceneNode::drawTransparent(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      pc->drawTransparent(rp);
    }
  }
}
vec3 SceneNode::getFinalPos() {
  vec3 v;
  v = getLocal().getTranslation();
  return v;
}
void SceneNode::collect(std::shared_ptr<RenderBucket> rb) {
  rb->collect(getThis<SceneNode>());
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<SceneNode> pc = std::dynamic_pointer_cast<SceneNode>(tn);
      if (pc) {
        pc->collect(rb);
      }
    }
  }
}
std::shared_ptr<Scene> SceneNode::getScene() {
  std::shared_ptr<Scene> x = findParent<Scene>();
  return x;
}


}//ns BR2
