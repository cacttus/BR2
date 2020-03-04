/**
*  @file BoneSpec.h
*  @date December 11, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __BONE_14815063041061475745_H__
#define __BONE_14815063041061475745_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../model/SceneNode.h"
#include "../world/PhysicsNode.h"

namespace BR2 {
/**
*  @class KeyFrame
*  @brief Key frame of  an animation track.
*/
class KeyFrame : public VirtualMemoryShared<KeyFrame> {
public:
  KeyFrame(mat4& mat, int32_t iSeq);
  KeyFrame(vec3& p, quat& r, vec3& s, int32_t iSeq);
  KeyFrame() {} //deserialize ctor
  //KeyFrame(std::shared_ptr<ActionKeys> pa, mat4& mat, int iSeq, vec2& cp, KeyframeInterpolation::e cpType, vec2& l_h, string_t l_t, vec2&  r_h, string_t r_t);
  //KeyFrame(std::shared_ptr<ActionKeys> pa, vec3& p, quat& r, vec3& s, int iSeq, vec2& cp, KeyframeInterpolation::e cpType, vec2& l_h, string_t l_t, vec2&  r_h, string_t r_t);
  virtual ~KeyFrame() override;

  //  Quaternion& getQuat() { return _mQuat; }
  float getTime() { return _fTime; }

  // mat4& getMat() { return _mMat; }

  // mat4& getMatCompiled() { return _mMatCompiled; }

  void scaleKey(int32_t iBaseFps, float& fMaxEndTime);
  int getSequenceKeyFrame() { return _iSequenceKeyFrame; }
  void animate(float ct, std::shared_ptr<KeyFrame> pNext, mat4& mOut);
  //KeyframeDataType::e getDataType() { return _eDataType;}
   // KeyframeInterpolation::e getInterpolation() { return _eInterpolation; }
  void deserialize(std::shared_ptr<BinaryFile> fb);
  void serialize(std::shared_ptr<BinaryFile> fb);
private:
  // KeyframeDataType::e _eDataType = KeyframeDataType::e::Undefined;
   //KeyframeType::e _eInterpolation = KeyframeInterpolation::e::Undefined;
  KeyframeInterpolation::e _eInterpolation = KeyframeInterpolation::e::Linear;
  //mat4 _mMat;
  //Quaternion _mQuat;
 // mat4 _mMatCompiled; //P + R + S

//  mat4 _mat; 
  vec3 _pos;
  quat _rot;//quaternion
  vec3 _scl;

  float _fTime; //Time [0,1]
  int32_t _iSequenceKeyFrame;//the keyframe that we loaded from the file

 // vec2 vCurveGraphPoint;       //TODO: this will implement bezier later
  //string_t strCurveType;
  //vec2 vCurveLeftHandle; //TODO: this will implement bezier later
  //string_t strCurveLeftHandleType;
  //vec2 vCurveRightHandle;//TODO: this will implement bezier later
 // string_t strCurverRightHandleType;
  //KeyFrame(std::shared_ptr<ActionKeys> pa,  int iSeq, vec2& cp, KeyframeInterpolation::e cpType, vec2& l_h, string_t l_t, vec2&  r_h, string_t r_t);
  mat4 compile(vec3& p, quat& r, vec3& s);
  KeyFrame(int32_t iSeq);
};

/**
*    @class ActionKeys
    Key frames for each object: mesh, armature, or bone
*/
class ActionKeys : public VirtualMemory {
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
  //string_t _sName;

  std::shared_ptr<KeyFrame> getAFrame(std::shared_ptr<Animator> tl);
  std::shared_ptr<KeyFrame> getBFrame(std::shared_ptr<Animator> tl);
  // void slerpKeys(float ct, std::shared_ptr<KeyFrame> f1, std::shared_ptr<KeyFrame> f2, mat4& mOut);
};
/**
*    @class ActionGroup
    We had to change this from Action because it got ambiguous with ActionKeys
*/
class ActionGroup : public VirtualMemory {
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
  void deserialize(std::shared_ptr<BinaryFile> fb);
  void serialize(std::shared_ptr<BinaryFile> fb);
private:
  int32_t _iBaseFps = 0;
  string_t _strName;
  Hash32 _iNameHashed = 0;
  std::map<Hash32, std::shared_ptr<ActionKeys>> _mapActions;
  float _fEndTime = 1.0f;
};

/**
*    @class Animator
*   Blender: Timeline.  timer or stopwatch that controls animation for a set of objects in a blender scene.
*/
class Animator : public VirtualMemory {
public:
  static void initStaticData();
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

  bool isEnded() {
    if (_bBackward == false) {
      return _fAnimTime >= _pAction->getEndTime();
    }
    else {
      return _fAnimTime <= 0;
    }
  }

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
class BoneSpec : public BaseSpec {
public:
  //typedef std::set<Hash32> AnimMap; //map of animation name hash to animation
public:
  BoneSpec() {}
  BoneSpec(string_t name, int32_t id);
  virtual ~BoneSpec() override;
  void setParent(std::shared_ptr<BoneSpec> bs);
  void addChild(std::shared_ptr<BoneSpec> bs);
  //string_t getParentName() { return _sParentName; }
  vec3 getTail() { return _vTail; }
  vec3 getHead() { return _vHead; }
  void setHead(vec3& v) { _vHead = v; }
  void setTail(vec3& v) { _vTail = v; }
  int32_t getBoneId() { return _iBoneId; }
  //   std::set<Hash32>* getActions() { return &_setActions; }
  std::vector<std::shared_ptr<BoneSpec>>& getChildren() { return _vecChildren; }
  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override;
  virtual void serialize(std::shared_ptr<BinaryFile> bf) override;

private:
  Hash32 _iBoneName = 0;
  int32_t _iBoneId = -1;//**Bone ordinal
  std::vector<std::shared_ptr<BoneSpec>> _vecChildren;
  std::shared_ptr<BoneSpec> _pParent = nullptr;

  //  AnimMap _setActions; // map of animation to ID.
    //string_t _sParentName;
  vec3 _vHead;
  vec3 _vTail;

  Hash32 getHashedBoneName() { return _iBoneName; }
  std::shared_ptr<BoneSpec> getParent() { return _pParent; }
};
class BoneNode : public SceneNode {
public:
  BoneNode(std::shared_ptr<BoneSpec> b, std::shared_ptr<ArmatureNode> pa);
  static std::shared_ptr<BoneNode> create(std::shared_ptr<BoneSpec> b, std::shared_ptr<ArmatureNode> pa);

  virtual ~BoneNode() override;
  //   mat4& getBoneMat() { return _mBone; }
  std::shared_ptr<BoneSpec> getBoneSpec() { return _pBone; }
  std::shared_ptr<ArmatureNode> getArmatureNode() { return _pArmatureNode; }
  void setAdded() {
    //Use this to avoid referencing bones in multiple meshes
    //**Remove once we start using bone instances.
    AssertOrThrow2(_bAdded == false);
    _bAdded = true;
  }
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
  virtual void calcBoundBox() override { SceneNode::calcBoundBox(); }
protected:
  virtual void init() override;

private:
  std::shared_ptr<BoneSpec> _pBone = nullptr;
  bool _bAdded = false;
  std::shared_ptr<ArmatureNode> _pArmatureNode = nullptr;
  //  mat4 _mBone;
};
class Armature : public BaseSpec {
public:
  typedef std::map<int32_t, std::shared_ptr<BoneSpec>> BoneCache; //bones  map<Bone ID, Bone*>

public:
  int32_t getArmatureId() { return _iArmatureId; }
  Armature() {}
  Armature(string_t strName, int32_t iId);
  virtual ~Armature();

  BoneCache* getBoneCacheOrdered() { return &_mapBoneCacheOrdered; }
  std::shared_ptr<BoneSpec> getRootBone() { return _pArmRoot; }
  std::shared_ptr<BoneSpec> getBoneSpec(string_t boneName);

  bool tkArmFile(MobFile* pMobFile, std::vector<string_t>& tokens);
  //std::set<Hash32>* getActions() {  return &_setActions; }
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;

private:
  int32_t _iArmatureId = -1;
  BoneCache _mapBoneCacheOrdered;
  std::shared_ptr<BoneSpec> _pArmRoot = nullptr;
  std::shared_ptr<BoneSpec> _pCurBone = nullptr;   //Temps * no significance
  std::shared_ptr<BoneSpec> _pCurKFBone = nullptr; //Temps * no significance

  //std::set<Hash32> _setActions;

 // void processKeyframes(std::shared_ptr<BoneSpec> pb, string_t animName);
  void compileHierarchy();
  std::shared_ptr<BoneSpec> getCachedBoneByName(string_t name);
};
class ArmatureNode : public SceneNode {
public:
  ArmatureNode(std::shared_ptr<Armature> ps);
  static std::shared_ptr<ArmatureNode> create(std::shared_ptr<Armature> ps);
  virtual ~ArmatureNode();
  // void addMeshNode(std::shared_ptr<MeshNode> mn) { _vecMeshes.push_back(mn); }
  std::shared_ptr<BoneNode> getRoot() { return _pBoneRoot; }
  std::vector<std::shared_ptr<BoneNode>>& getBonesOrdered() { return _vecBonesOrdered; }
  void drawForward(std::shared_ptr<UtilMeshInline> mi);

  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;
private:
  std::shared_ptr<BoneNode> _pBoneRoot = nullptr;
  std::vector<std::shared_ptr<BoneNode>> _vecBonesOrdered;
  // std::vector<std::shared_ptr<MeshNode>> _vecMeshes;
  void build(std::shared_ptr<BoneSpec> b, std::shared_ptr<BoneNode> bParent, std::vector<std::shared_ptr<BoneNode>>& vecBonesUnordered);
  void drawBones(std::shared_ptr<UtilMeshInline> mi, std::shared_ptr<BoneNode> bn, Color4f& c);
protected:
  virtual void init() override;
};

#pragma region WorldObjectSpec
class WorldObjectSpec : public PhysicsSpec {
public:
  WorldObjectSpec() {}//serialize only.
  WorldObjectSpec(string_t name, int32_t frameRate);
  virtual ~WorldObjectSpec();

  static std::shared_ptr<WorldObjectSpec> create(std::string mobFolder, uint32_t typeID, std::string friendlyName, std::string strBox, std::string strPlace);

  int32_t getFrameRate() { return _iFrameRate; }
  std::vector<std::shared_ptr<MeshSpec>>& getMeshes() { return _vecMeshes; }
  std::vector<std::shared_ptr<Armature>>& getArmatures() { return _vecArmatures; }
  std::vector<std::shared_ptr<ActionGroup>>& getActionGroups() { return _vecActionGroups; }
  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override;
  virtual void serialize(std::shared_ptr<BinaryFile> bf) override;
  std::shared_ptr<ActionGroup> getAction(Hash32 actionNameHash);
  std::shared_ptr<Armature> getArmatureById(int32_t armId);
  std::map<Hash32, std::shared_ptr<Armature>>& getArmatureMapOrdered() { return _mapArmaturesOrdered; }
  void postMobConversion();//init
  void cacheMeshBones();
  void setThumb(std::shared_ptr<Img32> thumb) { _pThumb = thumb; }
  std::shared_ptr<Img32> getThumb() { return _pThumb; }
  std::string getFriendlyName() { return _strFriendlyName; }
  void setFriendlyName(std::string s) { _strFriendlyName = s; }

private:
  int32_t _iFrameRate = 24;
  std::vector<std::shared_ptr<MeshSpec>> _vecMeshes;
  std::vector<std::shared_ptr<Armature>> _vecArmatures;
  std::vector<std::shared_ptr<ActionGroup>> _vecActionGroups;
  std::map<Hash32, std::shared_ptr<Armature>> _mapArmaturesOrdered; //Calculated from input vertex weights.
  std::shared_ptr<Img32> _pThumb = nullptr;
  std::shared_ptr<BoneSpec> getBoneByArmJointOffset(int32_t ijo);
  std::string _strFriendlyName;

  //WorldObj integration
public:
  string_t getFriendlyName() { return _strFriendlyName; }
  string_t getMobName() { return _strMobName; }
  uint32_t getTypeId() { return _iTypeId; }

  //std::shared_ptr<WorldObjectSpec> getOrLoadModel();
  std::shared_ptr<WorldObject> createInstance(std::shared_ptr<PhysicsWorld> w, vec3& r3Pos);

private:
  vec3 _vBoxFit;
  string_t _strMobName;
  //std::shared_ptr<ModelSpec> _pModelSpec = nullptr; this is the same thing
  uint32_t _iTypeId;
  std::string _strFriendlyName;
  std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> _placeOptions;//indexed by side

  static vec3 boxFit(vec3& vBoxFit);
  static std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> parsePlacementOptions(std::string strPlace);
  static vec3 parseBoxFit(std::string strBox);
  void place(const vec3& r3, vec3& outPos, vec4& outRot);
};
#pragma endregion

#pragma region WorldObject
class WorldObject : public PhysicsNode {
public:
  WorldObject(std::shared_ptr<WorldObjectSpec>);
  virtual ~WorldObject();
  
  static std::shared_ptr<WorldObject> create(std::shared_ptr<WorldObjectSpec> spec);

  void playAction(string_t name);
  bool isPlaying();
  bool isPlaying(string_t actName);
  void stopAction(string_t actName);
  void stopAllActions();
  std::shared_ptr<WorldObjectSpec> getModelSpec();
  void attachToNode(std::shared_ptr<SceneNode> pNode) { _pWorldNode = pNode; }
  //This is the physics update.  It's the main update.  modelNode drops animation on subsequent nodes.
  virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) override;

  virtual void drawForward(RenderParams& rp) override;
  std::vector<std::shared_ptr<ArmatureNode>>& getArmatureNodes() { return _vecArmatures; }
  virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) override;

protected:
  virtual void init() override;

private:
  string_t _mobName = "";
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
};
#pragma endregion

}//ns Game



#endif

