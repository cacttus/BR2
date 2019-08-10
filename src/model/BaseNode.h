/**
*
*    @file Node3base.h
*    @date November 25, 2016
*    @author Derek Page
*
*    © 2016
*
*
*/
#pragma once
#ifndef __NODE3BASE_14801270783276389065_H__
#define __NODE3BASE_14801270783276389065_H__

#include "../base/BaseHeader.h"
#include "../math/MathHeader.h"
#include "../gfx/DisplayHeader.h"
#include "../model/ModelHeader.h"
#include "../world/WorldHeader.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"
#include "../base/TreeNode.h"

namespace Game {
class UtilMeshInline;
class BaseSpec : public VirtualMemoryShared<BaseSpec> {
protected:
    Box3f* _pBox = nullptr;//Base Box.
    t_string _strName;
    Hash32 _iNameHashed;
    mat4 _mInvBind;
    mat4 _mBind;
    mat4 _mParentInverse;   //This is just for mesh objects that have mesh parents.
    //mat4 _mBasis;//Where the object is - base rotation &c.
    t_string _strParentName;
    ParentType::e _eParentType = ParentType::e::None;
    void setParentType(ParentType::e pt) { _eParentType = pt; }

public:
    BaseSpec() { } //Serialized version
    BaseSpec(t_string strName);
    virtual ~BaseSpec() override;
    ParentType::e getParentType() { return _eParentType; }
    Box3f* getBoundBoxObject() { return _pBox; }

    t_string getName() { return _strName; }
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
    void setParentName(t_string str, ParentType::e ee) { _strParentName = str; setParentType(ee); }
    t_string getParentName() { return _strParentName; }
    virtual void serialize(std::shared_ptr<BinaryFile> fb);
    virtual void deserialize(std::shared_ptr<BinaryFile> fb);

};

/**
*    @class Node3base
*    @brief
*/
class BaseNode : public TreeNode {
private:
    NodeId _iNodeId = 0;//Note: this is also use for picking and must therefore be 32 bits (not 64)
    vec3 _vPos;
    vec3 _vLastPos;
    bool _bTransformChanged = true;
    bool _bInitialized = false;
protected:
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
protected:
    void setLocalBind();
    void animate(std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
    void applyLocalAnimation(std::shared_ptr<Animator>);
    void applyParent();
    virtual void init();

public:
    BaseNode(std::shared_ptr<BaseSpec>);
    virtual ~BaseNode() override;

    void setHidden(bool bHidden) { 
        _bHidden = bHidden; 
    }
    bool getHidden() { 
        return _bHidden; 
    }

    virtual void update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& mapAnimators);
    void compileWorldMatrix();
    virtual void calcBoundBox();
    virtual void calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad);
    void setBoneParent(std::shared_ptr<BoneNode> bn) { _pBoneParent = bn; }
    std::shared_ptr<BoneNode> getBoneParent() { return _pBoneParent; }
    
    virtual void drawDeferred(RenderParams& rp) override;
    virtual void drawForward(RenderParams& rp) override;
    virtual void drawShadow(RenderParams& rp) override;
    virtual void drawDebug(RenderParams& rp) override;
    virtual void drawNonDepth(RenderParams& rp) override;
    virtual void drawTransparent(RenderParams& rp) override;

    template < typename Tx > std::shared_ptr<Tx> getData() {
        return std::dynamic_pointer_cast<Tx>(_pSpec);
    }
    NodeId getId() { return _iNodeId; }
    OBB* getOBB() { return _pOBB; }
    Box3f* getBoundBoxObject() { return _pBox; }
    const vec3& getPos() { return _vPos; }
    void setPos(const vec3&& p) {
        _vLastPos = _vPos;
        _vPos = p;
        _bTransformChanged = true;
    }
    vec3 getViewNormal() { return _vViewNormal; }
    void setViewNormal(vec3& p) {
        _vViewNormal = p;
        _bTransformChanged = true;
    }
    void setScale(vec3& v) {
        _vScale = v;
        _bTransformChanged = true;
    }
    vec3 getScale() { return _vScale; }

    bool getTransformChanged() { return _bTransformChanged; }
    //  void resetTransformChanged() { _bTransformChanged = false; }

    void setRot(vec4&& axis_angle_radians){
        _vRotationNormal = axis_angle_radians.xyz();
        _fRotation = axis_angle_radians.w;
    }
    std::shared_ptr<BaseSpec> getSpec() { return _pSpec; }
    mat4& getLocal() { return _mLocal; }
    mat4& getWorld() { return _mWorld; }
    mat4& getAnimated() { return _mAnimated; }
    //mat4& getFinal() {return _mFinal;}
    bool isMeshNode();
    bool isBoneNode();
    bool isArmatureNode();
    bool isSkinnedMesh();
    bool isLightNode();
    bool isModelNode();
    bool isCameraNode();
    t_string getSpecName();
    Hash32 getSpecNameHashed();
    void drawBoxes(std::shared_ptr<UtilMeshInline> mi);
    void drawBox(std::shared_ptr<UtilMeshInline> mi);
    void drawBoneBindBoxes(std::shared_ptr<ArmatureNode> an, std::shared_ptr<UtilMeshInline> mi);
    void drawMeshBoxes(std::shared_ptr<UtilMeshInline> mi);
    void drawBoneBoxes(std::shared_ptr<UtilMeshInline> mi);
    //void addShadowInfluence(std::shared_ptr<ShadowBox> psb);
    //void clearShadowInfluences();

    vec3 getFinalPos();

    void collect(std::shared_ptr<RenderBucket> rb);
    template < typename Tx > bool findNode(std::shared_ptr<Tx>& __out_ node) {
        //Find a node (starting at this node) which matches the given node type: Tx
        std::shared_ptr<Tx> pc = std::dynamic_pointer_cast<Tx>(shared_from_this());
        if (pc != nullptr) {
            node = pc;
            return true;
        }

        if (getChildren()) {
            for (std::shared_ptr<TreeNode> tn : *getChildren()) {
                std::shared_ptr<BaseNode> pc = std::dynamic_pointer_cast<BaseNode>(tn);
                if (pc != nullptr) {
                    if (pc->findNode<Tx>(node) == true) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};


}//ns Game



#endif
