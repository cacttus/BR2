/**
*  @file BoneSpec.h
*  @date December 11, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __BONE_14815063041061475745_H__
#define __BONE_14815063041061475745_H__

#include "../base/BaseHeader.h"
#include "../base/Serializable.h"
#include "../model/NodeData.h"
#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../model/SceneNode.h"
#include "../world/PhysicsNode.h"

namespace BR2 {
/**
*  @class KeyFrame
*  @brief A keyframe for a 3D mesh animation.
*/
class KeyFrame : public ISerializable, public VirtualMemoryShared<KeyFrame> {
public:
  KeyFrame(mat4& mat, int32_t iSeq);
  KeyFrame(vec3& p, quat& r, vec3& s, int32_t iSeq);
  KeyFrame() {}// for deserialization
  virtual ~KeyFrame() override;

  float getTime() { return _fTime; }
  void scaleKey(int32_t iBaseFps, float& fMaxEndTime);
  int getSequenceKeyFrame() { return _iSequenceKeyFrame; }
  void animate(float ct, std::shared_ptr<KeyFrame> pNext, mat4& mOut);

  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;

private:
  KeyframeInterpolation::e _eInterpolation = KeyframeInterpolation::e::Linear;
  vec3 _pos;
  quat _rot;//quaternion
  vec3 _scl;
  float _fTime; //Time [0,1]
  int32_t _iSequenceKeyFrame;//the keyframe that we loaded from the file
  mat4 compile(vec3& p, quat& r, vec3& s);
  KeyFrame(int32_t iSeq);

  //TODO:
  // vec2 vCurveGraphPoint;       //TODO: this will implement bezier later
  //t_string strCurveType;
  //vec2 vCurveLeftHandle; //TODO: this will implement bezier later
  //t_string strCurveLeftHandleType;
  //vec2 vCurveRightHandle;//TODO: this will implement bezier later
  // t_string strCurverRightHandleType;
  //KeyFrame(std::shared_ptr<ActionKeys> pa,  int iSeq, vec2& cp, KeyframeInterpolation::e cpType, vec2& l_h, t_string l_t, vec2&  r_h, t_string r_t);
};

/**
*  @class ActionKeys
*  @brief Key frames for each object: mesh, armature, or bone
*/
class ActionKeys : public ISerializable, public VirtualMemoryShared<ActionKeys> {
public:
  ActionKeys() {}
  ActionKeys(string_t objName);
  virtual ~ActionKeys() override;

  string_t getObjectName() { return _strObjectName; }
  int32_t getObjectNameHash() { return _iNameHashed; }
  std::vector<std::shared_ptr<KeyFrame>>& getKeyFrames() { return _vecKeys; }
  void scaleKeys(int32_t iBasFps, float& fMaxEndTime);
  void addKeyFrame(int iTime, mat4& m);
  void addKeyFrame(int iTime, vec3& p, quat& r, vec3& s);
  void animate(std::shared_ptr<Animator> tl, mat4& mOut);
  void deserialize(std::shared_ptr<BinaryFile> fb);
  void serialize(std::shared_ptr<BinaryFile> fb);

private:
  string_t _strObjectName;//armature or mesh
  Hash32 _iNameHashed;
  std::vector<std::shared_ptr<KeyFrame>> _vecKeys;
  //t_string _sName;

  std::shared_ptr<KeyFrame> getAFrame(std::shared_ptr<Animator> tl);
  std::shared_ptr<KeyFrame> getBFrame(std::shared_ptr<Animator> tl);
  // void slerpKeys(float ct, std::shared_ptr<KeyFrame> f1, std::shared_ptr<KeyFrame> f2, mat4& mOut);
};

/**
*  @class ActionGroup
*  @brief A list of actions for an animated model.
*  @note We had to change this from Action because it got ambiguous with ActionKeys
*/
class ActionGroup : public ISerializable, public VirtualMemoryShared<ActionGroup> {
public:
  ActionGroup() {}
  ActionGroup(string_t strName, int32_t iBaseFps);
  int32_t getFps() { return _iBaseFps; }
  void setFps(int32_t x) { _iBaseFps = x; }
  string_t getName() { return _strName; }
  int32_t getNameHash() { return _iNameHashed; }
  void addActionKeys(std::shared_ptr<ActionKeys> a);
  std::shared_ptr<ActionKeys> getActionKeys(Hash32 iObjectNameHashed);
  void scaleKeys();
  float getEndTime() { return _fEndTime; }
  bool isValid();
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;

private:
  int32_t _iBaseFps = 0;
  string_t _strName;
  Hash32 _iNameHashed = 0;
  std::map<Hash32, std::shared_ptr<ActionKeys>> _mapActions;
  float _fEndTime = 1.0f;
};

/**
*  @class Animator
*  @brief, In Blender a Timeline.  Timer or stopwatch that controls animation for a set of objects in a blender scene.
*/
class Animator : public VirtualMemory {
public:
  Animator();
  virtual ~Animator() override;

  //TODO: update this with the FPS int he MOB file.
  void update(float delta);
  std::shared_ptr<ActionGroup> getAction() { return _pAction; }

  PlayState::e getState() { return _eState; }
  bool getIsBackward() { return _bBackward; }
  float getTime() { return _fAnimTime; }
  int32_t getActionNameHash() { return _iAnimationId; }
  void setAnimationId(int32_t id) { _iAnimationId = id; }
  void reset() { _fAnimTime = 0.0f; }
  void start() { reset(); _eState = PlayState::e::Playing; }
  void setAction(std::shared_ptr<ActionGroup> pa) { _pAction = pa; }
  void stop() { reset(); _eState = PlayState::e::Stopped; }
  void pause() { _eState = PlayState::e::Paused; }
  void setDuration(float d) { _fAnimSpeed = 1.0f / d; }
  bool isEnded();
  void setLoop(bool b) { _bLoop = b; }

private:
  int32_t _iAnimationId = -1;
  float _fAnimTime = 0.0f; // anim time [0,1]
  float _fAnimSpeed = 1.0f; // base speed = 1.0f = the frames per second of the action.
  bool _bBackward = false;
  bool _bLoop = true;
  PlayState::e _eState = PlayState::e::Stopped;
  std::shared_ptr<ActionGroup> _pAction = nullptr;
};

/**
*  @class BoneData
*  @brief A bone (matrix) for skinned animation.
*/
class BoneData : public NodeData {
public:
  BoneData() {}
  BoneData(string_t name, int32_t id);
  virtual ~BoneData() override;
  void setParent(std::shared_ptr<BoneData> bs);
  void addChild(std::shared_ptr<BoneData> bs);
  //t_string getParentName() { return _sParentName; }
  vec3 getTail() { return _vTail; }
  vec3 getHead() { return _vHead; }
  void setHead(vec3& v) { _vHead = v; }
  void setTail(vec3& v) { _vTail = v; }
  int32_t getBoneId() { return _iBoneId; }
  //   std::set<Hash32>* getActions() { return &_setActions; }
  std::vector<std::shared_ptr<BoneData>>& getChildren() { return _vecChildren; }
  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override;
  virtual void serialize(std::shared_ptr<BinaryFile> bf) override;

  string_t getName() { return _name; }
private:
  Hash32 _iBoneName = 0;
  int32_t _iBoneId = -1;//**Bone ordinal
  std::vector<std::shared_ptr<BoneData>> _vecChildren;
  std::shared_ptr<BoneData> _pParent = nullptr;
  string_t _name;
  vec3 _vHead;
  vec3 _vTail;

  Hash32 getHashedBoneName() { return _iBoneName; }
  std::shared_ptr<BoneData> getParent() { return _pParent; }
};

/**
*  @class BoneNode
*  @brief An instance of an armature Bone.
*/
class BoneNode : public SceneNode {
public:
  BoneNode() {}
  BoneNode(std::shared_ptr<BoneData> b, std::shared_ptr<ArmatureNode> pa);
  static std::shared_ptr<BoneNode> create(std::shared_ptr<BoneData> b, std::shared_ptr<ArmatureNode> pa);

  virtual ~BoneNode() override;
  std::shared_ptr<BoneData> getBoneData() { return _pBone; }
  std::shared_ptr<ArmatureNode> getArmatureNode() { return _pArmatureNode; }
  void setAdded() {
    //Use this to avoid referencing bones in multiple meshes
    //**Remove once we start using bone instances.
    AssertOrThrow2(_bAdded == false);
    _bAdded = true;
  }
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
  virtual void calcBoundBox() override { SceneNode::calcBoundBox(); }

private:
  std::shared_ptr<BoneData> _pBone = nullptr;
  bool _bAdded = false;
  std::shared_ptr<ArmatureNode> _pArmatureNode = nullptr;
  //  mat4 _mBone;

};

/**
*  @class ArmatureData
*  @brief An armature (skeleton) similar to an armature in Blender. Container for a bone armature, also parses it from the mobstr file
*/
class ArmatureData : public NodeData {
public:
  typedef std::map<int32_t, std::shared_ptr<BoneData>> BoneCache; //bones  map<Bone ID, Bone*>
public:
  ArmatureData() {}
  ArmatureData(string_t strName, int32_t iId);
  virtual ~ArmatureData();

  int32_t getArmatureId() { return _iArmatureId; }
  BoneCache* getBoneCacheOrdered() { return &_mapBoneCacheOrdered; }
  std::shared_ptr<BoneData> getRootBone() { return _pArmRoot; }
  std::shared_ptr<BoneData> getBoneSpec(string_t boneName);
  bool tkArmFile(MobFile* pMobFile, std::vector<string_t>& tokens);
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;

private:
  int32_t _iArmatureId = -1;
  BoneCache _mapBoneCacheOrdered;
  std::shared_ptr<BoneData> _pArmRoot = nullptr;
  std::shared_ptr<BoneData> _pCurBone = nullptr;   //Temps * no significance
  std::shared_ptr<BoneData> _pCurKFBone = nullptr; //Temps * no significance

  void compileHierarchy();
  std::shared_ptr<BoneData> getCachedBoneByName(string_t name);
};

/**
*  @class ArmatureNode
*  @brief An instance of an armature (skeleton) similar to an armature in Blender.
*/
class ArmatureNode : public SceneNode {
public:
  ArmatureNode(std::shared_ptr<ArmatureData> ps);
  virtual ~ArmatureNode();

  std::shared_ptr<BoneNode> getRoot() { return _pBoneRoot; }
  std::vector<std::shared_ptr<BoneNode>>& getBonesOrdered() { return _vecBonesOrdered; }
  void drawForward(std::shared_ptr<UtilMeshInline> mi);
  std::shared_ptr<ArmatureData> getArmatureData() { return _pArmatureData; }
  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;

private:
  std::shared_ptr<BoneNode> _pBoneRoot = nullptr;
  std::vector<std::shared_ptr<BoneNode>> _vecBonesOrdered;
  std::shared_ptr<ArmatureData> _pArmatureData = nullptr;
  // std::vector<std::shared_ptr<MeshNode>> _vecMeshes;
  void build(std::shared_ptr<BoneData> b, std::shared_ptr<BoneNode> bParent, std::vector<std::shared_ptr<BoneNode>>& vecBonesUnordered);
  void drawBones(std::shared_ptr<UtilMeshInline> mi, std::shared_ptr<BoneNode> bn, Color4f& c);
};

/**
* @class ModelData
* @brief A renderable model that exists in the world, with a mesh.
*/
class ModelData : public PhysicsData {
public:
  ModelData() {}//serialize only.
  ModelData(string_t name, int32_t frameRate);
  virtual ~ModelData();

  int32_t getFrameRate() { return _iFrameRate; }
  std::vector<std::shared_ptr<MeshData>>& getMeshes() { return _vecMeshes; }
  std::vector<std::shared_ptr<ArmatureData>>& getArmatures() { return _vecArmatures; }
  std::vector<std::shared_ptr<ActionGroup>>& getActionGroups() { return _vecActionGroups; }
  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override;
  virtual void serialize(std::shared_ptr<BinaryFile> bf) override;
  std::shared_ptr<ActionGroup> getAction(Hash32 actionNameHash);
  std::shared_ptr<ArmatureData> getArmatureById(int32_t armId);
  std::map<Hash32, std::shared_ptr<ArmatureData>>& getArmatureMapOrdered() { return _mapArmaturesOrdered; }
  void postMobConversion();
  void cacheMeshBones();
  void setThumb(std::shared_ptr<Img32> thumb) { _pThumb = thumb; }
  std::shared_ptr<Img32> getThumb() { return _pThumb; }
  std::string getFriendlyName() { return _strFriendlyName; }
  void setFriendlyName(std::string s) { _strFriendlyName = s; }

private:
  int32_t _iFrameRate = 24;
  std::vector<std::shared_ptr<MeshData>> _vecMeshes;
  std::vector<std::shared_ptr<ArmatureData>> _vecArmatures;
  std::vector<std::shared_ptr<ActionGroup>> _vecActionGroups;
  std::map<Hash32, std::shared_ptr<ArmatureData>> _mapArmaturesOrdered; //Calculated from input vertex weights.
  std::shared_ptr<Img32> _pThumb = nullptr;
  std::shared_ptr<BoneData> getBoneByArmJointOffset(int32_t ijo);
  std::string _strFriendlyName;
};

/**
*  @class ModelNode
*  @brief A 3D Model, defined as Meshes, Skeleton, and Animations.
*/
class ModelNode : public PhysicsNode {
public:
  ModelNode(std::shared_ptr<GLContext> ct, std::shared_ptr<ModelData>);
  virtual ~ModelNode();

  void playAction(string_t name);
  bool isPlaying();
  bool isPlaying(string_t actName);
  void stopAction(string_t actName);
  void stopAllActions();
  std::shared_ptr<ModelData> getModelSpec();
  void attachToNode(std::shared_ptr<SceneNode> pNode) { _pWorldNode = pNode; }
  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;
  virtual void drawForward(RenderParams& rp) override;
  std::vector<std::shared_ptr<ArmatureNode>>& getArmatureNodes() { return _vecArmatures; }
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
  std::shared_ptr<GLContext> getContext() { return _pContext; }

private:
  std::vector<std::shared_ptr<MeshNode>> _vecMeshes;
  std::vector<std::shared_ptr<ArmatureNode>> _vecArmatures;
  std::shared_ptr<SceneNode> _pWorldNode = nullptr; // Reference to the world node *not owned
  // std::shared_ptr<Animator> _pAnimator = nullptr;
  std::map<Hash32, std::shared_ptr<Animator>> _mapAnimators;
  std::map<Hash32, std::shared_ptr<SceneNode>> _mapNodes;//Cache of all nodes appended including bones.  
  std::shared_ptr<SceneNode> getNodeByName(string_t name);
  void addNodeToCache(std::shared_ptr<SceneNode> bn);
  std::shared_ptr<Animator> getAnimator(string_t actName);
  void buildNodeParents();
  std::shared_ptr<GLContext> _pContext = nullptr;
};


}//ns BR2



#endif

