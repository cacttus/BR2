#include "../model/BaseNode.h"
#include "../base/Hash.h"
#include "../base/BinaryFile.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/EngineConfig.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShadowBoxSide.h"
#include "../model/MeshNode.h"
#include "../model/Model.h"
#include "../model/OBB.h"
#include "../model/UtilMeshInline.h"
#include "../model/MeshData.h"
#include "../gfx/LightNode.h"
#include "../math/MathAll.h"
#include "../world/RenderBucket.h"

namespace BR2 {
BaseSpec::BaseSpec(string_t strName) {
  _strName = strName;
  _iNameHashed = STRHASH(_strName);
  _mBind = mat4::identity();
  _mInvBind = _mBind.inverseOf();
  _mParentInverse = mat4::identity();
  //    _mBasis = mat4::identity();
  _pBox = new Box3f();
}
BaseSpec::~BaseSpec() {
  DEL_MEM(_pBox);
  //_pBox = nullptr;
}

void BaseSpec::serialize(std::shared_ptr<BinaryFile> fb) {
  fb->writeString(std::move(_strName));
  fb->writeString(std::move(_strParentName));
  fb->writeInt32(std::move((int32_t)_eParentType));
  fb->writeMat4(std::move(_mBind));
  fb->writeMat4(std::move(_mParentInverse));
  fb->writeVec3(std::move(_pBox->_min));
  fb->writeVec3(std::move(_pBox->_max));
}
void BaseSpec::deserialize(std::shared_ptr<BinaryFile> fb) {
  fb->readString(std::move(_strName));
  _iNameHashed = STRHASH(_strName);
  fb->readString(std::move(_strParentName));
  int32_t pt;
  fb->readInt32(pt);
  _eParentType = (ParentType::e)pt;
  fb->readMat4(std::move(_mBind));
  _mInvBind = _mBind.inverseOf();
  fb->readMat4(std::move(_mParentInverse));

  _pBox = new Box3f();
  fb->readVec3(std::move(_pBox->_min));
  fb->readVec3(std::move(_pBox->_max));
}

///////////////////////////////////////////////////////////////////
BaseNode::BaseNode(std::shared_ptr<BaseSpec> pspec) {
  _pSpec = pspec;
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
BaseNode::~BaseNode() {
  //    _setShadowInfluences.clear();
  DEL_MEM(_pBox);
  DEL_MEM(_pOBB);
}
string_t BaseNode::getSpecName() {
  if (getSpec()) {
    return getSpec()->getName();
  }
  else {
    BroLogError("Treid to return a name of a model without a spec");
    Gu::debugBreak();
    return std::string("no spec");
  }
}
Hash32 BaseNode::getSpecNameHashed() {
  if (getSpec()) {
    return getSpec()->getNameHashed();
  }
  else {
    BroLogError("Treid to return a hash of a model without a spec");
    Gu::debugBreak();
    return 0;
  }
}
void BaseNode::init() {
  //Nodes now need to be initialized due to shared_ptr constructors not having this ptr
  if (_bInitialized == true) {
    BroLogWarnCycle(getSpecName() + "..Node already initializd..errors will result");
  }
  _bInitialized = true;
}
void BaseNode::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
  if (_bInitialized == false) {
    BroLogWarnCycle("Node was not initialized (init())");
    Gu::debugBreak();
    return;
  }
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
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->update(delta, mapAnimators);
    }
  }

  calcBoundBox();//Must come after matrix calc
}
void BaseNode::compileWorldMatrix() {
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
bool BaseNode::isCameraNode() {
  return getThis<CameraNode>() != nullptr;
}
bool BaseNode::isMeshNode() {
  return getThis<MeshNode>() != nullptr;
}
bool BaseNode::isSkinnedMesh() {
  return isMeshNode() && getThis<MeshNode>()->getMeshSpec()->hasSkin();
}
bool BaseNode::isBoneNode() {
  return getThis<BoneNode>() != nullptr;
}
bool BaseNode::isArmatureNode() {
  return getThis<ArmatureNode>() != nullptr;
}
bool BaseNode::isLightNode() {
  return getThis<LightNodeBase>() != nullptr;
}
bool BaseNode::isModelNode() {
  return getThis<ModelNode>() != nullptr;
}
void BaseNode::setLocalBind() {
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
      _mLocal = getSpec()->getBind();
    }
  }
  else {
    _mLocal = mat4::identity();
  }
}
void BaseNode::animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {

  for (std::pair<Hash32, std::shared_ptr<Animator>> p : mapAnimators) {
    applyLocalAnimation(p.second);
  }
}
void BaseNode::applyParent() {
  std::shared_ptr<BaseNode> pParent = std::dynamic_pointer_cast<BaseNode>(getParent());
  //Apply the parent transform.
  if (isBoneNode()) {
    _mLocal = getSpec()->getInvBind() * _mLocal * getSpec()->getBind();
    if (pParent != nullptr) {
      _mLocal = _mLocal * pParent->getLocal();
    }
  }
  else if (isMeshNode() || isArmatureNode() || isLightNode() || isModelNode() || isCameraNode()) {
    if (getSpec() == nullptr) {
      if (pParent != nullptr) {
        _mLocal = _mLocal * pParent->getLocal();
      }
    }
    else if (
      getSpec()->getParentType() == ParentType::e::Armature ||
      getSpec()->getParentType() == ParentType::e::Object ||
      getSpec()->getParentType() == ParentType::e::None) {

      if (isSkinnedMesh()) {
        //Setting this to ident here so we do't get confused.
        //We're doing all multiplies in CopyJointsToGPU no 1/20/18
        //see CopyJointsToGPU
      }
      else {
        _mLocal = _mLocal * getSpec()->getParentInverse();
        if (pParent != nullptr) {
          _mLocal = _mLocal * pParent->getLocal();
        }
      }
    }
    else if (getSpec()->getParentType() == ParentType::e::Bone) {
      if (getBoneParent() != nullptr) {
        static int n = 0;
        if (n == 0) {

          if (pParent != nullptr) {
            static int a = 4;
            if (a == 0)
              _mLocal = getBoneParent()->getSpec()->getInvBind() * _mLocal * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 1)
              _mLocal = _mLocal * getBoneParent()->getSpec()->getInvBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 2)
              _mLocal = getBoneParent()->getSpec()->getBind() * _mLocal * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 3)
              _mLocal = _mLocal * getBoneParent()->getSpec()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 4)
              //**ALMOST
              _mLocal = getBoneParent()->getSpec()->getInvBind() * _mLocal * getBoneParent()->getSpec()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
            if (a == 5)
              //Didn't work
              _mLocal = getBoneParent()->getSpec()->getInvBind() * _mLocal * getBoneParent()->getLocal() * getBoneParent()->getSpec()->getBind() * pParent->getLocal();//Armature
            if (a == 6)
              _mLocal = _mLocal * getBoneParent()->getSpec()->getBind() * getBoneParent()->getLocal() * pParent->getLocal();//Armature
          }
        }
        if (n == 1) {
          _mLocal = getBoneParent()->getSpec()->getInvBind() * getBoneParent()->getAnimated() * _mLocal * getBoneParent()->getSpec()->getBind();
          if (getBoneParent()->getParent() != nullptr) {
            _mLocal = _mLocal * std::dynamic_pointer_cast<BaseNode>(getBoneParent()->getParent())->getLocal();
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
      BroThrowNotImplementedException();
    }

  }
  else {
    BroLogWarnCycle("Unrecognized node type when updating parent.");
  }

  if (false) {
    vec4 p; vec4 r; vec4 s;
    _mLocal.decompose(p, r, s, true);
  }
}
void BaseNode::applyLocalAnimation(std::shared_ptr<Animator> anm) {
  std::shared_ptr<BaseNode> pParent = std::dynamic_pointer_cast<BaseNode>(getParent());
  if (anm != nullptr) {
    std::shared_ptr<ActionGroup> aa = anm->getAction();
    if (aa != nullptr) {
      if (getThis<BaseNode>()->getSpec() != nullptr) {
        //*Since we can have multiple animations for a model, this will silently fail on nodes
        // who don't have this particular animation name.
        std::shared_ptr<ActionKeys> ak = aa->getActionKeys(getThis<BaseNode>()->getSpec()->getNameHashed());
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

void BaseNode::calcBoundBox() {
  Box3f* pBox = getBoundBoxObject();
  calcBoundBox(*pBox, getPos(), 0.0f);
}
void BaseNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) {
  _pOBB->setInvalid();

  if (!isBoneNode()) {
    //aggregate all boxes, don'd do that for bone nodes we need them to have local boxes
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> bn = std::dynamic_pointer_cast<BaseNode>(tn);
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
void BaseNode::drawBoneBindBoxes(std::shared_ptr<ArmatureNode> an, std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<BaseNode> that = getThis<BaseNode>();
  iterateDepthFirst<BaseNode>([&](std::shared_ptr<BaseNode> tx) {
    if (tx != that) {
      tx->drawBoneBindBoxes(an, mi);
    }
    return true;
    });
  if (isBoneNode()) {
    vec4 cOBB(1, 1, 0, 1);
    OBB ob;
    ob.calc(an->getLocal(), getSpec()->getBoundBoxObject());
    mi->addBox(ob.getVerts(), &cOBB);
  }
}
void BaseNode::drawBoneBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<BaseNode> that = getThis<BaseNode>();

  iterateDepthFirst<BaseNode>([&](std::shared_ptr<BaseNode> tx) {
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
void BaseNode::drawMeshBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<BaseNode> that = getThis<BaseNode>();

  iterateDepthFirst<BaseNode>([&](std::shared_ptr<BaseNode> tx) {
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
void BaseNode::drawBoxes(std::shared_ptr<UtilMeshInline> mi) {
  std::shared_ptr<BaseNode> that = getThis<BaseNode>();

  iterateDepthFirst<BaseNode>([&](std::shared_ptr<BaseNode> tx) {
    if (tx != that) {
      tx->drawBoxes(mi);
    }
    return true;
    });
  drawBox(mi);

}
void BaseNode::drawBox(std::shared_ptr<UtilMeshInline> mi) {
  vec4 cBB(1, 1, 0, 1);
  vec4 cOBB(0, 1, 1, 1);
  mi->addBox(getBoundBoxObject(), &cBB);
  if (getOBB()->getInvalid() == false) {
    mi->addBox(getOBB()->getVerts(), &cOBB);
  }
}
void BaseNode::drawDeferred(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawDeferred(rp);
    }
  }
}
void BaseNode::drawForward(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawForward(rp);
    }
  }
}
void BaseNode::drawShadow(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawShadow(rp);
    }
  }
}
void BaseNode::drawDebug(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawDebug(rp);
    }
  }
}
void BaseNode::drawNonDepth(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawNonDepth(rp);
    }
  }
}
void BaseNode::drawTransparent(RenderParams& rp) {
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      pc->drawTransparent(rp);
    }
  }
}
vec3 BaseNode::getFinalPos() {
  vec3 v;
  v = getLocal().getTranslation();
  return v;
}
void BaseNode::collect(std::shared_ptr<RenderBucket> rb) {
  rb->collect(getThis<BaseNode>());
  if (getChildren()) {
    for (std::shared_ptr<TreeNode> tn : *getChildren()) {
      std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
      if (pc) {
        pc->collect(rb);
      }
    }
  }
}


}//ns Game
