#include "../base/Logger.h"
#include "../base/BinaryFile.h"
#include "../base/Img32.h"
#include "../base/Hash.h"
#include "../base/AppBase.h"
#include "../base/Perf.h"
#include "../base/GLContext.h"
#include "../math/Algorithm.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/RenderSettings.h"
#include "../model/SceneNode.h"
#include "../model/ShaderStorageBuffer.h"
#include "../model/Model.h"
#include "../model/Material.h"
#include "../model/MobFile.h"
#include "../model/ModelCache.h"
#include "../model/MeshSpec.h"
#include "../model/MeshNode.h"
#include "../model/UtilMeshInline.h"
#include "../model/FragmentBufferData.h"
#include "../model/OBB.h"

namespace BR2 {
KeyFrame::KeyFrame(mat4& mat, int32_t iSeq) : KeyFrame(iSeq) {
    //We're removing matrices because it's damn impossible to use them.
    vec4 pos, rot, scl;
    mat.decompose(pos, rot, scl);
    _pos = pos;
    _rot.construct(rot.x, rot.y, rot.z, rot.w);
    _scl = scl;
}
KeyFrame::KeyFrame(vec3& p, quat& r, vec3& s, int32_t iSeq) : KeyFrame(iSeq) {
    _pos = p;
    _rot = r;
    _scl = s;
}
KeyFrame::KeyFrame(int32_t iSeq) {
    // _eInterpolation = cpType;
    _fTime = -1;
    _iSequenceKeyFrame = iSeq;
    //vCurveGraphPoint = cp;       //TODO: this will implement bezier later
    //strCurveType = cpType;
    //vCurveLeftHandle = l_h; //TODO: this will implement bezier later
    //strCurveLeftHandleType = l_t;
    //vCurveRightHandle = r_h;//TODO: this will implement bezier later
    //strCurverRightHandleType = r_t;
}
KeyFrame::~KeyFrame() {}
void KeyFrame::scaleKey(int32_t iBaseFps, float& fMaxEndTime) {
    //scale this key from the integer (blender timeline key) to [0,1] for the animation.
    _fTime = (float)_iSequenceKeyFrame / (float)iBaseFps;

    if (_fTime > fMaxEndTime) {
        fMaxEndTime = _fTime;
    }
}
mat4 KeyFrame::compile(vec3& p, quat& r, vec3& s) {
    mat4 mRot;
    mat4 mScl;
    mat4 mPos;

    mPos = mat4::getTranslation(p);
    mRot = r.toMat4(); //mat4::getRotationRad(_fRotation, _vRotationNormal);
    mScl = mat4::getScale(s);

    //Taken from Node3base
    mat4 mRet = mScl * mRot * mPos;

    return mRet;
}
void KeyFrame::animate(float ct, std::shared_ptr<KeyFrame> pNext, mat4& mOut) {
    if (shared_from_this() == pNext)
    {
        mOut = compile(_pos, _rot, _scl);
        return;
    }
    //Compute time [0,1] between current and next keyframe
    float difftime = (ct - getTime()) / (pNext->getTime() - getTime());

    //For the sake of all this I feel like everything needs to be in PRS.
    if (pNext->_eInterpolation == KeyframeInterpolation::e::Linear) {
        //We control animationb based on the keyframe we're interpolating TO - thsu use the F2 keyframe
        vec3 p = _pos.lerpTo(pNext->_pos, difftime);
        quat r = _rot.slerpTo(pNext->_rot, difftime);
        vec3 s = _scl.lerpTo(pNext->_scl, difftime);
        mOut = compile(p, r, s);

    }
    else if (pNext->_eInterpolation == KeyframeInterpolation::e::Bezier) {
        BRThrowNotImplementedException();
    }
    else {
        BRThrowNotImplementedException();
    }
}
void KeyFrame::deserialize( std::shared_ptr<BinaryFile> fb) {
    fb->readInt32(_iSequenceKeyFrame);
    fb->readVec3(_pos);
    fb->readVec4(*(vec4*)&_rot);//hmm this doesn't look portable
    fb->readVec3(_scl);
}
void KeyFrame::serialize( std::shared_ptr<BinaryFile> fb) {
    fb->writeInt32(std::move(_iSequenceKeyFrame));
    fb->writeVec3(std::move(_pos));
    fb->writeVec4(std::move(*(vec4*)&_rot));//hmm this doesn't look portable
    fb->writeVec3(std::move(_scl));
}
//////////////////////////////////////////////////////////////////////////
ActionKeys::ActionKeys(string_t objName) {
    _iNameHashed = STRHASH(objName);
    // _strName = actName;
    _strObjectName = objName;
    // _iBaseFps = iBaseFps;
}
ActionKeys::~ActionKeys() {
    _vecKeys.resize(0);
}
void ActionKeys::animate( std::shared_ptr<Animator> tl, mat4& mOut) {
    std::shared_ptr<KeyFrame> f1, f2;
    if (tl->getIsBackward()) {
        f1 = getBFrame(tl);
        f2 = getAFrame(tl);
    }
    else {
        f1 = getAFrame(tl);
        f2 = getBFrame(tl);
    }

    f1->animate(tl->getTime(), f2, mOut);
}
std::shared_ptr<KeyFrame> ActionKeys::getAFrame( std::shared_ptr<Animator> tl) {
    AssertOrThrow2(_vecKeys.size() > 0);

    float ct = tl->getTime();

    //TODO: optimize - too many keys and this will be slow
    for (size_t i = 0; i < _vecKeys.size(); ++i)
    {
        if (_vecKeys[i]->getTime() <= ct)
        {
            if (i == _vecKeys.size() - 1 || _vecKeys[i + 1]->getTime() > ct)
                return _vecKeys[i];
        }
    }
    return _vecKeys[0];
}
std::shared_ptr<KeyFrame> ActionKeys::getBFrame( std::shared_ptr<Animator> tl) {
    float ct = tl->getTime();
    //TODO: optimize - too many keys and this will be slow
    for (size_t i = 0; i < _vecKeys.size(); ++i)
    {
        if (_vecKeys[i]->getTime() > ct)
        {
            if (i == _vecKeys.size() - 1 || _vecKeys[i + 1]->getTime() > ct)
                return _vecKeys[i];
        }
    }
    AssertOrThrow2(_vecKeys.size() > 0);
    return _vecKeys[0];
}
void ActionKeys::scaleKeys(int32_t iBasFps, float& fMaxEndTime) {
    if (_vecKeys.size() == 0) {
        BRLogError("ActionKeys is invalid, no keys present");
        Gu::debugBreak();
    }
    for (std::shared_ptr<KeyFrame> kf : _vecKeys) {
        kf->scaleKey(iBasFps, fMaxEndTime);
    }
}
void ActionKeys::addKeyFrame(int iTime, mat4& m) {
 std::shared_ptr<KeyFrame> newKf = std::make_shared<KeyFrame>(m, iTime);//fTime is zero - calc this later
    getKeyFrames().push_back(newKf);
}
void ActionKeys::addKeyFrame(int iTime, vec3& p, quat& r, vec3& s) {
 std::shared_ptr<KeyFrame> newKf = std::make_shared<KeyFrame>(p, r, s, iTime);//fTime is zero - calc this later
    getKeyFrames().push_back(newKf);
}
void ActionKeys::deserialize( std::shared_ptr<BinaryFile> fb) {
    fb->readString(_strObjectName);
    _iNameHashed = STRHASH(_strObjectName);

    int32_t nKeyframes;
    fb->readInt32(nKeyframes);
    for (int32_t iKeyframe = 0; iKeyframe < nKeyframes; ++iKeyframe) {
        std::shared_ptr<KeyFrame> kf = std::make_shared<KeyFrame>();
        kf->deserialize(fb);
        _vecKeys.push_back(kf);
    }

}
void ActionKeys::serialize( std::shared_ptr<BinaryFile> fb) {
    fb->writeString(std::move(_strObjectName));

    fb->writeInt32((int32_t)_vecKeys.size());
    for (std::shared_ptr<KeyFrame> kf : _vecKeys) {
        kf->serialize(fb);
    }

}
//////////////////////////////////////////////////////////////////////////
ActionGroup::ActionGroup(string_t strName, int32_t iBaseFps) {
    _strName = strName;
    _iBaseFps = iBaseFps;
    _iNameHashed = STRHASH(_strName);
}
void ActionGroup::addActionKeys(std::shared_ptr<ActionKeys> a) {
    Hash32 ah = a->getObjectNameHash();
    if (_mapActions.find(ah) != _mapActions.end()) {
        BRLogWarn("Tried to add action keys for '" + a->getObjectName() + "' multiple times to action group '" + getName() + "'");
        Gu::debugBreak();
    }
    else {
        _mapActions.insert(std::make_pair(ah, a));
    }
}
std::shared_ptr<ActionKeys> ActionGroup::getActionKeys(Hash32 iObjectNameHashed) {
    std::map<Hash32, std::shared_ptr<ActionKeys>>::iterator it = _mapActions.find(iObjectNameHashed);
    if (it != _mapActions.end()) {
        return it->second;
    }
    return nullptr;
}
void ActionGroup::scaleKeys() {
    _fEndTime = -FLT_MAX;
    if (_mapActions.size() == 0) {
        BRLogError("Action group is invalid, no actions present");
        Gu::debugBreak();
    }
    for (std::pair<Hash32, std::shared_ptr<ActionKeys>> p : _mapActions) {
        p.second->scaleKeys(_iBaseFps, _fEndTime);
    }
}
bool ActionGroup::isValid() {
    return _fEndTime >= 0.0 && _mapActions.size() > 0;
}
void ActionGroup::deserialize( std::shared_ptr<BinaryFile> fb) {
    fb->readString(_strName);
    _iNameHashed = STRHASH(_strName);

    int32_t nActions;
    fb->readInt32(nActions);
    for (int32_t iAction = 0; iAction < nActions; ++iAction) {
        std::shared_ptr<ActionKeys> act = std::make_shared<ActionKeys>();
        act->deserialize(fb);
        _mapActions.insert(std::make_pair(act->getObjectNameHash(), act));
    }

    //**Important
    scaleKeys();
}
void ActionGroup::serialize( std::shared_ptr<BinaryFile> fb) {
    fb->writeString(std::move(_strName));

    fb->writeInt32((int32_t)_mapActions.size());
    for (std::pair<Hash32, std::shared_ptr<ActionKeys>> p : _mapActions) {
        p.second->serialize(fb);
    }

}
//////////////////////////////////////////////////////////////////////////
BoneSpec::BoneSpec(string_t name, int32_t id) : BaseSpec(name) {
    _iBoneId = id;
}
BoneSpec::~BoneSpec() {
}

void BoneSpec::setParent(std::shared_ptr<BoneSpec> bs) {
    if (_pParent != nullptr) {
        BRLogError("Bone '" + getName() + "' Parent was already set!");
    }
    else {
        _pParent = bs;
    }
}
void BoneSpec::addChild(std::shared_ptr<BoneSpec> bs) {
    bool berr = false;
    for (std::shared_ptr<BoneSpec> bs1 : _vecChildren) {
        if (bs1 == bs) {
            BRLogError("Bone Spec '" + bs->getName() + "' was already added to bone '" + getName() + "'");
            berr = true;
        }
    }
    if (berr == false) {
        _vecChildren.push_back(bs);
    }
}
void BoneSpec::deserialize( std::shared_ptr<BinaryFile> fb) {
    BaseSpec::deserialize(fb);
    //fb->readString(_strName);
    //_iNameHashed = STRHASH(_strName);
    //
    //fb->readString(_strParentName);
    //fb->readMat4(_mBind);

    fb->readInt32(_iBoneId);
    fb->readVec3(_vHead);
    fb->readVec3(_vTail);

    _mInvBind = _mBind.inverseOf();
}
void BoneSpec::serialize( std::shared_ptr<BinaryFile> fb) {
    BaseSpec::serialize(fb);
    //fb->writeString(std::move(_strName));
    //fb->writeString(std::move(_strParentName));
    fb->writeInt32(std::move(_iBoneId));
    //fb->writeMat4(std::move(_mBind));
    fb->writeVec3(std::move(_vHead));
    fb->writeVec3(std::move(_vTail));
}
//////////////////////////////////////////////////////////////////////////
BoneNode::BoneNode(std::shared_ptr<BoneSpec> b, std::shared_ptr<ArmatureNode> pa) : SceneNode(b) {
    _pBone = b;
    _pArmatureNode = pa;
    _mLocal.setIdentity();
}
std::shared_ptr<BoneNode> BoneNode::create(std::shared_ptr<BoneSpec> b, std::shared_ptr<ArmatureNode> pa){
    std::shared_ptr<BoneNode> bn = std::make_shared<BoneNode>(b,pa);
    bn->init();
    return bn;
}

BoneNode::~BoneNode() {
}
void BoneNode::init() {
    SceneNode::init();
}
void BoneNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad){
    pBox.genResetLimits();
    //Only apply OBBs to bone and mesh nodes.
    if (getSpec()) {
        //transform by our armture
        std::shared_ptr<BoneNode> bn = getThis<BoneNode>();
        mat4 mBox = bn->getLocal() * bn->getArmatureNode()->getLocal();
        _pOBB->calc(mBox, getSpec()->getBoundBoxObject());
        for (int i = 0; i < 8; ++i) {
            pBox.genExpandByPoint(getOBB()->getVerts()[i]);
        }
    }
    SceneNode::calcBoundBox(pBox, obPos, extra_pad);
}
//////////////////////////////////////////////////////////////////////////
Animator::Animator()
{
}
Animator::~Animator() {
}
void Animator::initStaticData() {}
void Animator::update(float delta) {
    if (_eState == PlayState::e::Playing && _pAction != nullptr) {
        _fAnimTime += _fAnimSpeed * delta;

        if (_fAnimTime >= _pAction->getEndTime()) {
            if (_bLoop) {
                _fAnimTime = fmodf(_fAnimTime, _pAction->getEndTime());
            }
            else {
                _fAnimTime = _pAction->getEndTime();
                stop();
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
Armature::Armature(string_t strName, int32_t iId) : BaseSpec(strName), _iArmatureId(iId) {
}
Armature::~Armature() {
    //for (BoneCache::iterator it = _mapBoneCacheOrdered.begin();
    //    it != _mapBoneCacheOrdered.end(); it++) {
    //    std::shared_ptr<BoneSpec> ps = it->second;
    //    delete ps;
    //}
    _mapBoneCacheOrdered.clear();
}
void Armature::deserialize( std::shared_ptr<BinaryFile> fb) {
    BaseSpec::deserialize(fb);

    fb->readInt32(_iArmatureId);
    int32_t nBones;
    fb->readInt32(nBones);
    for (int32_t iBone = 0; iBone < nBones; ++iBone) {
        std::shared_ptr<BoneSpec> bs = std::make_shared<BoneSpec>();
        bs->deserialize(fb);
        _mapBoneCacheOrdered.insert(std::make_pair(bs->getBoneId(), bs));
    }

    BRLogInfo("  Compiling armature hierarchy.");
    compileHierarchy();

}
void Armature::serialize( std::shared_ptr<BinaryFile> fb) {
    BaseSpec::serialize(fb);

    fb->writeInt32(std::move(_iArmatureId));
    fb->writeInt32((int32_t)_mapBoneCacheOrdered.size());
    for (std::pair<int32_t, std::shared_ptr<BoneSpec>> p : _mapBoneCacheOrdered) {
        p.second->serialize(fb);
    }

}
bool Armature::tkArmFile(MobFile* pMobFile, std::vector<string_t>& tokens) {
    int iind = 1;

    if (pMobFile->lcmp(tokens[0], "bones_beg", 1)) {
    }
    else if (pMobFile->lcmp(tokens[0], "bones_end", 1)) {
        compileHierarchy();
    }
    else if (pMobFile->lcmp(tokens[0], "bone_beg", 5)) {
        if (_pCurBone != nullptr) {
            pMobFile->parseErr("Current bone already defined");
        }
        else {
            string_t name = pMobFile->getCleanToken(tokens, iind);
            int32_t id = TypeConv::strToInt(pMobFile->getCleanToken(tokens, iind));
            string_t parent = pMobFile->getCleanToken(tokens, iind);
            ParentType::e ep = pMobFile->parseParentType(pMobFile->getCleanToken(tokens, iind));
            _pCurBone = std::make_shared<BoneSpec>(name, id);

            _pCurBone->setParentName(parent, ep);
        }
    }
    else if (pMobFile->lcmp(tokens[0], "bone_end", 2)) {
        if (_pCurBone == nullptr) {
            pMobFile->parseErr("Current bone not defined");
        }
        else {
            if (_mapBoneCacheOrdered.find(_pCurBone->getBoneId()) != _mapBoneCacheOrdered.end()) {
                BRLogError("Bone '" + _pCurBone->getName() + "' ID:" + _pCurBone->getBoneId() + " already found in bone cache.  Animation will look weird.");
                Gu::debugBreak();
            }
            else {
                _mapBoneCacheOrdered.insert(std::make_pair(_pCurBone->getBoneId(), _pCurBone));
            }
            _pCurBone = nullptr;
        }
    }
    else if (pMobFile->lcmp(tokens[0], "bone_head", 4)) {
        if (_pCurBone == nullptr) {
            pMobFile->parseErr("Cur bone was not defined");
        }
        else {
            vec3 v(TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 1)),
                TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 2)),
                TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 3)));
            _pCurBone->setHead(v);
        }
    }
    else if (pMobFile->lcmp(tokens[0], "bone_tail", 4)) {
        if (_pCurBone == nullptr) {
            pMobFile->parseErr("Cur bone was not defined");
        }
        else {

            vec3 v(TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 1)),
                TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 2)),
                TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 3)));
            _pCurBone->setTail(v);
        }
    }
    else if (pMobFile->lcmp(tokens[0], "bone_bind_world", 3)) {
        //Object space inverse bind pose
        if (_pCurBone == nullptr) {
            pMobFile->parseErr("Cur bone was not defined");
        }
        else {
            string_t mType = pMobFile->getCleanToken(tokens, iind);
            if (pMobFile->lcmp(mType, "m44")) {
                mat4 mat = pMobFile->parseMat4(pMobFile->getCleanToken(tokens, iind)); //= bufferedFile.getTok();

                //**I'm not sure if this is the bind or the INV bind (from the file)
                _pCurBone->setBind(mat);
            }
            else {
                pMobFile->parseErr("Wrong matrix type for bone mat (must be 4x4)");
            }
        }

    }
    //else if (pMobFile->lcmp(tokens[0], "bone_bind_world_inv", 3)) {
    //    //Object space inverse bind pose
    //    //if (_pCurBone == nullptr) {
    //    //    pMobFile->parseErr("Cur bone was not defined");
    //    //}
    //    //else {
    //    //    t_string mType = pMobFile->getCleanToken(tokens, iind);
    //    //    if (pMobFile->lcmp(mType, "m44")) {
    //    //        mat4 mat = pMobFile->parseMat4(pMobFile->getCleanToken(tokens, iind)); //= bufferedFile.getTok();
    //    //        //**I'm not sure if this is the bind or the INV bind (from the file)
    //    //        _pCurBone->setInvBind(mat);
    //    //    }
    //    //    else {
    //    //        pMobFile->parseErr("Wrong matrix type for bone mat (must be 4x4)");
    //    //    }
    //    //}
    //}
    else if (pMobFile->lcmp(tokens[0], "bpoi", 3)) {
        //Joint Space Relative Bind Pose
    }

    else {
        return false;
    }

    return true;
}
std::shared_ptr<BoneSpec> Armature::getBoneSpec(string_t boneName) {
    Hash32 hash = Hash::computeStringHash32bit(boneName);
    BoneCache::iterator it = _mapBoneCacheOrdered.find(hash);

    if (it != _mapBoneCacheOrdered.end()) {
        return it->second;
    }
    return nullptr;
}
std::shared_ptr<BoneSpec> Armature::getCachedBoneByName(string_t name) {
    Hash32 par = STRHASH(name);
    for (std::pair<int32_t, std::shared_ptr<BoneSpec>> p : _mapBoneCacheOrdered) {
        if (p.second->getNameHashed() == par) {
            return p.second;
        }
    }
    return nullptr;
}
void Armature::compileHierarchy() {
    //Build the bone hierarchy after loading it from the file.

    for (std::pair<int32_t, std::shared_ptr<BoneSpec>> p : _mapBoneCacheOrdered) {//; itBone != _mapBoneCacheOrdered.end(); itBone++) {
        std::shared_ptr<BoneSpec> pBone = p.second;

        if (StringUtil::isEmpty(pBone->getParentName())){
            if (_pArmRoot != nullptr) {
                BRLogError("Arm root was not null when trying to add root!");
            }
            else {
                _pArmRoot = pBone;
            }
        }
        else {
            std::shared_ptr<BoneSpec> pParent = getCachedBoneByName(pBone->getParentName());
            if (pParent == nullptr) {
                BRLogError("Hierarchy could not find parent bone '" + pBone->getParentName());
            }
            else {
                pBone->setParent(pParent);
                pParent->addChild(pBone);
            }
        }

    }

}

//////////////////////////////////////////////////////////////////////////
ArmatureNode::ArmatureNode(std::shared_ptr<Armature> ps) : SceneNode(ps) {
}
std::shared_ptr<ArmatureNode> ArmatureNode::create(std::shared_ptr<Armature> ps){
    std::shared_ptr<ArmatureNode> a = std::make_shared<ArmatureNode>(ps);
    a->init();
    return a;
}

ArmatureNode::~ArmatureNode() {
    //**Tree node does not delete bones.
    //for (std::shared_ptr<BoneNode> bn : _vecBonesOrdered) {
    //    DEL_MEM(bn);
    //}
    _vecBonesOrdered.resize(0);
  //  _vecMeshes.resize(0);
}
void ArmatureNode::init(){
    SceneNode::init();

    std::vector<std::shared_ptr<BoneNode>> vecBonesUnordered;
    build(getData<Armature>()->getRootBone(), nullptr, vecBonesUnordered);

    //Build a list of bones ordered by the bone cache on the parent armature.
    //*This must correspond to the same ordering in MeshSpec::getGpuJointOrdinal
    for (std::pair<Hash32, std::shared_ptr<BoneSpec>> pbone : *getData<Armature>()->getBoneCacheOrdered()) {
        for (std::shared_ptr<BoneNode> bn : vecBonesUnordered) {
            if (bn->getBoneSpec() == pbone.second) {
                _vecBonesOrdered.push_back(bn);
            }
        }
    }

    AssertOrThrow2(_vecBonesOrdered.size() > 0);
}
void ArmatureNode::build(std::shared_ptr<BoneSpec> b, std::shared_ptr<BoneNode> bParent, std::vector<std::shared_ptr<BoneNode>>& vecBonesUnordered) {
    AssertOrThrow2(b != nullptr);
    std::shared_ptr<BoneNode> new_bn = BoneNode::create(b, getThis<ArmatureNode>());//getThis<BoneNode>(); 
    vecBonesUnordered.push_back(new_bn);

    if (bParent == nullptr) {
        _pBoneRoot = new_bn;
    }
    else {
        //Note: bones are separate from the scene - custom datablock
        //Noep we added back..
        bParent->attachChild(new_bn);
    }
    for (std::shared_ptr<BoneSpec> bs : b->getChildren()) {
        build(bs, new_bn, vecBonesUnordered);
    }
}
void ArmatureNode::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
    if (_pBoneRoot) {
        //Bones are separate from scene.  Bones don't get model transform.
        //This is intentional.  We would end up with 2 transforms on mesh.
        _pBoneRoot->update(delta, mapAnimators);
    }
    SceneNode::update(delta, mapAnimators);
}
void ArmatureNode::drawBones(std::shared_ptr<UtilMeshInline> mi, std::shared_ptr<BoneNode> bn, Color4f& c4) {
    ///mat4 mBind = bn->getSpec()->getBind();
    mat4 mBoneLocal = bn->getLocal();
    vec4 vHead(bn->getBoneSpec()->getHead(), 1);
    vec4 vTail(bn->getBoneSpec()->getTail(), 1);

    //mLocal removed the armature transform - so add it back to translate the
    //bone relative to the model.
    mBoneLocal = mBoneLocal * getLocal(); // Multiply by arm local matrix.

    vHead = mBoneLocal * vHead;
    vTail = mBoneLocal * vTail;

    vec3 p0 = vec3(vHead.x, vHead.y, vHead.z);
    vec3 p1 = vec3(vTail.x, vTail.y, vTail.z);

    mi->vt2(v_v3c4(p0, c4), v_v3c4(p1, c4));// Edges

    Color4f c2 = c4;
    if (c2.r() == 1) { c2.r() = 0; c2.g() = 1; }
    else if (c2.g() == 1) { c2.g() = 0; c2.r() = 1; }
    for (std::shared_ptr<TreeNode> tn : *bn->getChildren()) {
        std::shared_ptr<BoneNode> bnc = std::dynamic_pointer_cast<BoneNode>(tn);
        if (bnc != nullptr) { //bones can have meshes as children
            drawBones(mi, bnc, c2);
        }
    }

}
void ArmatureNode::drawForward(std::shared_ptr<UtilMeshInline> mi) {
    Color4f c4(1, 0, 0, 1);
    drawBones(mi, _pBoneRoot, c4);
}
void ArmatureNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad){
    pBox.genResetLimits();
    if (getSpec()) {
        std::shared_ptr<ArmatureNode> pa = getThis<ArmatureNode>();
        pa->getRoot()->iterateDepthFirst<BoneNode>([&](std::shared_ptr<BoneNode> bn) {
            bn->calcBoundBox();
            pBox.genExpandByBox(bn->getBoundBoxObject());
            return true;
        });
    }
    SceneNode::calcBoundBox(pBox, obPos, extra_pad);
}
//////////////////////////////////////////////////////////////////////////
ModelSpec::ModelSpec(string_t name, int32_t frameRate) : PhysicsSpec(name), _iFrameRate(frameRate) {
    //_iNameHash = STRHASH(name);
}
ModelSpec::~ModelSpec() {
    /*for (std::shared_ptr<ActionGroup> ag : _vecActionGroups) {
        DEL_MEM(ag);
    }*/
    _vecActionGroups.clear();
    //for (std::shared_ptr<MeshSpec> ms : _vecMeshes) {
    //    DEL_MEM(ms);
    //}
    _vecMeshes.clear();
    //for (std::shared_ptr<Armature> arm : _vecArmatures) {
    //    DEL_MEM(arm);
    //}
    _vecArmatures.clear();
    _mapArmaturesOrdered.clear();
    //_vecMeshes.clear();
    //_vecArmatures.clear();
}
void ModelSpec::deserialize( std::shared_ptr<BinaryFile> fb) {
    BRLogInfo("Reading Model..");
    PhysicsSpec::deserialize(fb);
    //fb->readString(std::move(_strName));
    //_iNameHashed = STRHASH(_strName);

    fb->readString(_strFriendlyName);

    fb->readInt32(_iFrameRate);

    int32_t nArms;
    fb->readInt32(nArms);
    for (int32_t iArm = 0; iArm < nArms; ++iArm) {
        BRLogInfo("  Arm " + iArm + " ..");
        std::shared_ptr<Armature> pArm = std::make_shared<Armature>();
        pArm->deserialize(fb);
        getArmatures().push_back(pArm);
        getArmatureMapOrdered().insert(std::make_pair(pArm->getArmatureId(), pArm));
    }

    int32_t nActionGroups;
    fb->readInt32(nActionGroups);
    for (int32_t iAction = 0; iAction < nActionGroups; ++iAction) {
        BRLogInfo("  Action " + iAction + "..");
        std::shared_ptr<ActionGroup> pg = std::make_shared<ActionGroup>();
        pg->setFps(_iFrameRate);
        pg->deserialize(fb);
        getActionGroups().push_back(pg);
    }

    int32_t nMeshes;
    fb->readInt32(nMeshes);
    for (int32_t iMesh = 0; iMesh < nMeshes; ++iMesh) {
        BRLogInfo("  Mesh " + iMesh + "..");
        std::shared_ptr<MeshSpec> pMesh = std::make_shared<MeshSpec>();
        pMesh->deserialize(fb);
        getMeshes().push_back(pMesh);
    }
    
    bool bHasThumb = false;
    fb->readBool(bHasThumb);
    if (bHasThumb) {
        _pThumb = std::make_shared<Img32>();
        _pThumb->deserialize(fb);
        if(false) {
            string_t cached = Gu::getApp()->makeAssetPath("cache", getName() + "-thumb-loaded.png");
            Gu::saveImage(cached, _pThumb);
        }
    }

    BRLogInfo("..Done");
}
void ModelSpec::serialize( std::shared_ptr<BinaryFile> fb) {
    PhysicsSpec::serialize(fb);

    fb->writeString(std::move(getFriendlyName()));

    //fb->writeString(std::move(_strName));
    fb->writeInt32(std::move(_iFrameRate));

    fb->writeInt32((int32_t)getArmatures().size());
    for (std::shared_ptr<Armature> arm : getArmatures()) {
        arm->serialize(fb);
    }

    fb->writeInt32((int32_t)getActionGroups().size());
    for (std::shared_ptr<ActionGroup> ag : getActionGroups()) {
        ag->serialize(fb);
    }

    fb->writeInt32((int32_t)getMeshes().size());
    for (std::shared_ptr<MeshSpec> ms : getMeshes()) {
        ms->serialize(fb);
    }

    
    if(_pThumb != nullptr){
        fb->writeBool(true);

        if (false) {
            string_t cached = Gu::getApp()->makeAssetPath("cache", getName() + "-thumb-saved.png");
            Gu::saveImage(cached, _pThumb);
        }

        _pThumb->serialize(fb);
    }
    else {
        fb->writeBool(false);
    }

}
std::shared_ptr<ActionGroup> ModelSpec::getAction(Hash32 actionNameHash) {
    for (std::shared_ptr<ActionGroup> ag : _vecActionGroups) {
        if (ag->getNameHash() == actionNameHash) {
            return ag;
        }
    }
    return nullptr;
}
std::shared_ptr<Armature> ModelSpec::getArmatureById(int32_t armId) {
    for (std::shared_ptr<Armature> aa : _vecArmatures) {
        if (aa->getArmatureId() == armId) {
            return aa;
        }
    }
    return nullptr;
}

void ModelSpec::postMobConversion() {
    //I mean technically the model spec doesn't care about it's spec box.  That's just for meshes.
    getBoundBoxObject()->genResetLimits();
    //for(std::shared_ptr<Armature> pa : _vecArmatures){
    //    getBoundBoxObject()->genExpandByBox(pa->getBoundBoxObject());
    //}
    for (std::shared_ptr<MeshSpec> pa : _vecMeshes) {
        //this is already called in copyspecfragments.
        //pa->computeBox();
        getBoundBoxObject()->genExpandByBox(pa->getBoundBoxObject());
    }
}
std::shared_ptr<BoneSpec> ModelSpec::getBoneByArmJointOffset(int32_t ijo) {
    int32_t ij = 0;
    for (std::pair<Hash32, std::shared_ptr<Armature>> parm : getArmatureMapOrdered()) {
        for (std::pair<Hash32, std::shared_ptr<BoneSpec>> pbone : *parm.second->getBoneCacheOrdered()) {
            if (ij == ijo) {
                return pbone.second;
            }
            ij++;
        }
    }
    return nullptr;
}
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<ModelNode> ModelNode::create(std::shared_ptr<ModelSpec> ps){
    std::shared_ptr<ModelNode> m = std::make_shared<ModelNode>(ps);
    m->init();
    m->stopAllActions();
    m->update(0.0, std::map<Hash32, std::shared_ptr<Animator>>());
    return m;
}

ModelNode::ModelNode(std::shared_ptr<ModelSpec> pModelSpec) : PhysicsNode(pModelSpec) {

}
ModelNode::~ModelNode() {
    //for (std::pair<Hash32,  std::shared_ptr<Animator>> p : _mapAnimators) {
    //    DEL_MEM(p.second);
    //}
    _mapAnimators.clear();
    //for (std::shared_ptr<MeshNode> mg : _vecMeshes) {
    //    DEL_MEM(mg);
    //}
    _vecMeshes.resize(0);
    //for (std::shared_ptr<ArmatureNode> a : _vecArmatures) {
    //    DEL_MEM(a);
    //}
    _vecArmatures.resize(0);
}
void ModelNode::init(){
    PhysicsNode::init();
    //Create armatures
    for (std::shared_ptr<Armature> pArmSpec : getData<ModelSpec>()->getArmatures()) {
        std::shared_ptr<ArmatureNode> pArmNode = ArmatureNode::create(pArmSpec);
        _vecArmatures.push_back(pArmNode);
        addNodeToCache(pArmNode);
        //Also add all bones in the case of bone parents..
        //for (std::shared_ptr<BoneNode> bn : pArmNode->getBonesOrdered()) {
        //    addNodeToCache(bn);
        //}
    }
    //Create meshes
    for (std::shared_ptr<MeshSpec> pMeshSpec : getData<ModelSpec>()->getMeshes()) {
        std::shared_ptr<MeshNode> pMeshnode = MeshNode::create(pMeshSpec, getThis<ModelNode>());
        _vecMeshes.push_back(pMeshnode);
        addNodeToCache(pMeshnode);
    }

    buildNodeParents();
    Gu::checkErrorsRt();
}
void ModelNode::buildNodeParents() {
    //Parent Armatures And Meshes And Bones (if they have parents, if no, they are parented by the modelnode
    for (std::pair <Hash32, std::shared_ptr<SceneNode>> p : _mapNodes) {
        std::shared_ptr<SceneNode> pNode = p.second;
        string_t strChild = pNode->getSpec()->getName();
        string_t strParent = pNode->getSpec()->getParentName();
        if (pNode->getSpec()->getParentType() == ParentType::e::Bone) {
            //Bone parents - we may end up not even using bone aprents.
            bool bFound = false;
            for (std::shared_ptr<ArmatureNode> pan : _vecArmatures) {
                for (std::shared_ptr<BoneNode> bn : pan->getBonesOrdered()) {

                    if (StringUtil::equals(strParent, bn->getSpecName())) {
                        if (pNode->getParent() != nullptr) {
                            BRLogError("Bone parent node already set for node " + pNode->getSpecName());
                            Gu::debugBreak();
                        }
                        else {
                            //attach to armature, and alsoa dd the bone to the parenture
                            pan->attachChild(pNode);
                            pNode->setBoneParent(bn);
                            bFound = true;
                        }
                    }
                }
            }
            if (bFound == false) {
                BRLogError("Bone parent node not found for node " + pNode->getSpecName());
                Gu::debugBreak();
            }
        }
        else if (StringUtil::isEmpty(strParent)) {
            //Attach to the model everything but the bone nodes which get their own update in the armature.
            attachChild(pNode); //Attach to the model node, no root parent
        }
        else {
            if (pNode->getParent() != nullptr) {
                std::shared_ptr<SceneNode> pParentAttached = std::dynamic_pointer_cast<SceneNode>(pNode->getParent());
                if (pParentAttached == nullptr) {
                    BRLogError("Parent already attached, but Tried to cast a base node - failed.");
                    Gu::debugBreak();
                }
                else {
                    BRLogError("Parent '" + pParentAttached->getSpec()->getName() +
                        "' already set for node '" + strChild + "' who wants to be parented by '" + strParent + "' ");
                    Gu::debugBreak();
                }
            }
            else {
                std::shared_ptr<SceneNode> pParentNode = getNodeByName(strParent);
                if (pParentNode == nullptr) {
                    BRLogError("Could not find parent '" + strParent + "' for node '" + strChild + "'");
                    Gu::debugBreak();
                }
                else {
                    //if(pNode->isMeshNode() && pParentNode->isArmatureNode()){
                    //    std::shared_ptr<ArmatureNode> pa = dynamic_cast<std::shared_ptr<ArmatureNode>>(pParentNode);
                    //    pa->addMeshNode(dynamic_cast<std::shared_ptr<MeshNode>>(pNode));
                    //}
                    //else {
                    pParentNode->attachChild(pNode);
                    //                    }
                }
            }
        }
    }
}
void ModelNode::addNodeToCache(std::shared_ptr<SceneNode> bn) {
    AssertOrThrow2(bn != nullptr);
    Hash32 bnName = STRHASH(bn->getSpec()->getName());
    if (_mapNodes.find(bnName) != _mapNodes.end()) {
        BRLogError("ModelNode: Failed to add base node '" + bn->getSpec()->getName() + "' as it already exists in node cache.");
    }
    else {
        _mapNodes.insert(std::make_pair(bnName, bn));
    }
}
std::shared_ptr<SceneNode> ModelNode::getNodeByName(string_t name) {
    Hash32 h = STRHASH(name);
    std::map<Hash32, std::shared_ptr<SceneNode>>::iterator it = _mapNodes.find(h);
    if (it == _mapNodes.end()) {
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<ModelSpec> ModelNode::getModelSpec() {
    return std::dynamic_pointer_cast<ModelSpec>(SceneNode::getSpec());
}
void ModelNode::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators) {
    Perf::pushPerf();
    //Do nothing with input animators.  We use our own animators.

    //Update animators
    for (std::pair<Hash32,  std::shared_ptr<Animator>> p : _mapAnimators) {
        p.second->update(delta);
    }

    //Update all nodes ONCE
    SceneNode::update(delta, _mapAnimators);

    //*Dispatch final skin for all meshes
    for (std::shared_ptr<MeshNode> mn : _vecMeshes) {
        mn->computeAndDispatchSkin();
    }
    Perf::popPerf();
}

void ModelNode::drawForward(RenderParams& rp) {
    SceneNode::drawForward(rp);

    //Debug Boxes and Soforth
    if (Gu::getRenderSettings()->getDebug()->getShowArmatures()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        for (std::shared_ptr<ArmatureNode> pa : _vecArmatures) {
            pa->drawForward(mi);
        }
        mi->end(rp.getCamera());
    }
    //Recursive!
    if (Gu::getRenderSettings()->getDebug()->getShowBoneBindBoxes()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        for (std::shared_ptr<ArmatureNode> an : _vecArmatures) {
                an->getRoot()->drawBoneBindBoxes(an, mi);
        }
        mi->end(rp.getCamera());
    }
    if (Gu::getRenderSettings()->getDebug()->getShowBoneBoxes()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        for(std::shared_ptr<ArmatureNode> an : _vecArmatures){
                an->getRoot()->drawBoneBoxes(mi);
        }
        mi->end(rp.getCamera());

    }
    if (Gu::getRenderSettings()->getDebug()->getShowMeshBoxes()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        drawMeshBoxes(mi);
        mi->end(rp.getCamera());
    }
    if (Gu::getRenderSettings()->getDebug()->getShowModelBoxes()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        drawBoxes(mi);
        mi->end(rp.getCamera());
    }
    if (Gu::getRenderSettings()->getDebug()->getShowModelBoundBox()) {
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline> mi = std::make_shared<UtilMeshInline>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        drawBox(mi);
        mi->end(rp.getCamera());
    }
    
    // mi.setModelMatrix(getFinal());
}


void ModelNode::playAction(string_t actName) {
    Hash32 anh = STRHASH(actName);
    std::map<Hash32,  std::shared_ptr<Animator>>::iterator it = _mapAnimators.find(anh);
    if (it != _mapAnimators.end()) {
         std::shared_ptr<Animator> pa = it->second;
        pa->stop();
        pa->start();
    }
    else {
        std::shared_ptr<ActionGroup> ag = getModelSpec()->getAction(anh);
        if (ag == nullptr) {
            BRLogError("Model '" + getSpec()->getName() + "', could not find action '" + actName + "'");
            Gu::debugBreak();
        }
        else if (ag->isValid()) {
             std::shared_ptr<Animator> pa = std::make_shared<Animator>();
            _mapAnimators.insert(std::make_pair(anh, pa));

            pa->setAction(ag);
            pa->start();
            pa->setLoop(true);
        }
        else {
            BRLogError("Action wasn't valid for playing.");
            Gu::debugBreak();
        }
    }
}
 std::shared_ptr<Animator> ModelNode::getAnimator(string_t actName) {
    Hash32 anh = STRHASH(actName);
    std::map<Hash32,  std::shared_ptr<Animator>>::iterator it = _mapAnimators.find(anh);
    if (it != _mapAnimators.end()) {
        return it->second;
    }
    return nullptr;
}
bool ModelNode::isPlaying(string_t actName) {
     std::shared_ptr<Animator> pa = getAnimator(actName);
    return pa && pa->getState() == PlayState::e::Playing;
}
bool ModelNode::isPlaying() {
    for (std::pair<Hash32,  std::shared_ptr<Animator>> p : _mapAnimators) {
        if (p.second->getState() == PlayState::e::Playing) {
            return true;
        }
    }
    return false;
}
void ModelNode::stopAllActions() {
    for (std::pair<Hash32,  std::shared_ptr<Animator>> p : _mapAnimators) {
        p.second->stop();
        //DEL_MEM(p.second);
    }
    _mapAnimators.clear();
}
void ModelNode::stopAction(string_t actName) {
    Hash32 anh = STRHASH(actName);
    std::map<Hash32,  std::shared_ptr<Animator>>::iterator it = _mapAnimators.find(anh);
    if (it != _mapAnimators.end()) {
         std::shared_ptr<Animator> a = it->second;
        _mapAnimators.erase(it);
        //DEL_MEM(a);
    }
}


void ModelSpec::cacheMeshBones() {
    //BoneBoxes BoneBox
    //Stores all bones that affect a mesh in a cache for the mesh.
    //loop all weights and store boness.
    for (std::shared_ptr<Armature> a : getArmatures()) {
        for (std::pair<int32_t, std::shared_ptr<BoneSpec>> p : *a->getBoneCacheOrdered()) {
            //calc bone boxes
            p.second->getBoundBoxObject()->genResetLimits();
        }
    }
    int nNotFound = 0;
    for (std::shared_ptr<MeshSpec> ms : getMeshes()) {
        ms->beginEdit();
        {
            ms->getBoneCache().clear();
            int32_t iIndex = 0;
            if (ms->hasSkin() && ms->getWeightOffsetsGpu() != nullptr && ms->getWeightsGpu() != nullptr) {
                GpuAnimatedMeshWeightData* wd = new GpuAnimatedMeshWeightData[ms->getWeightOffsetsGpu()->getNumElements()];
                GpuAnimatedMeshWeight* w = new GpuAnimatedMeshWeight[ms->getWeightsGpu()->getNumElements()];

                ms->getWeightOffsetsGpu()->syncRead((void*)wd, ms->getWeightOffsetsGpu()->getByteSize());
                ms->getWeightsGpu()->syncRead((void*)w, ms->getWeightsGpu()->getByteSize());

                size_t nWeights = ms->getWeightOffsetsGpu()->getNumElements();

                for (size_t iWeight = 0; iWeight < nWeights; ++iWeight) {
                    int nCount = wd[iWeight]._count;
                    for (int iCount = 0; iCount < nCount; ++iCount) {

                        GpuAnimatedMeshWeight* wpt = &w[wd[iWeight]._offset + iCount];

                        //Only apply verts who have weight.
                        if (wpt->_weight > 0) {
                            std::shared_ptr<BoneSpec> bs = getBoneByArmJointOffset(wpt->_iArmJointOffset);
                            if (bs != nullptr) {
                                //cache the bone on the mesh spec.
                                ms->getBoneCache().insert(bs);

                                //Expand the bone's bound box.
                                vec3 vt = ms->getFrags()->v3f(iWeight);
                                vec4 vv(vt, 1);
                                vv =  (/*bs->getBind() **/ ms->getBind()) * vv;
                                if (vv.z < -20 || vv.z>20) {
                                    int nnn = 0;
                                }
                                bs->getBoundBoxObject()->genExpandByPoint(vec3(vv.x, vv.y, vv.z));
                            }
                            else {
                                nNotFound++;
                                Gu::debugBreak();
                            }
                        }
                    }
                }


                delete[] wd;
                delete[] w;
            }

        }
        ms->endEdit();
    }
}
void ModelNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad) {
    pBox.genResetLimits();

    SceneNode::calcBoundBox(pBox, obPos, extra_pad);
}



}//ns Game
