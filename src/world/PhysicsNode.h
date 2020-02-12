/**
*
*    @file PhysicsNode.h
*    @date March 31, 2017- Phy25
*    @date Jan 4, 2018
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __PHYSICS_NODE_14910105823111128827_H__
#define __PHYSICS_NODE_14910105823111128827_H__

#include "../model/BaseNode.h"
#include "../world/WorldHeader.h"

namespace BR2 {
//It doesn't make sense to have the "shape" on the model, even though the model has velocity.  Same goes for mesh.
class PhysicsSpec : public BaseSpec {
public:
    bool getIsPersistent() { return false; }
    float getMass() { return 0.0f; } //PHY_STATIC_MASS
    float getHollow() { return false; }
    PhysicsSpec() { } //deserialize ctor
    PhysicsSpec(string_t strName) : BaseSpec(strName) { }
    virtual ~PhysicsSpec() override { }
    virtual void serialize( std::shared_ptr<BinaryFile> fb) override;
    virtual void deserialize( std::shared_ptr<BinaryFile> fb) override;
};
//The physics data is independent of the PixObj only because we need to
//collide with the World Node plane so we need a separate box that isn't an object.
class PhysicsNode : public BaseNode {
private:
    //Where is the PhysicsShape?  It's on the MeshSpec.  Because Blender..

    vec3 _vVelocity = vec3(0, 0, 0);
  //  vec3 _vAccel = vec3(0, 0, 0); //This is only ever used as a temp
    Box3f* _pSpeedbox = nullptr;
 //   CellManifold* _pCellManifold = nullptr;
    bool _bIsAtRest = true;
    //  bool _bStatic = false;  // if the obj is static.  FOr now this is only the world plane.
    float _fMass = 1.0f;
    bool _bHollow = false;

    float _fFriction = 0.90f; // 0.9 m/s
   // vec3 _vLastPos;
    uint64_t _iLastMovedFrame = 0;
    uint64_t _iLastAccFrame = 0;
    vec3 _vTempPos;
    vec3 _vTempVel;
  //  vec3 _vTempAcc;
    const int c_vHistorySize = 10;
    const int c_vHistoryPosSize = 10;
    std::vector<vec3> _vHistoryPos;
    const int c_vHistoryVelSize = 10;
    std::vector<vec3> _vHistoryVel;

    std::shared_ptr<GridManifold> _pManifold = nullptr;
protected:
    virtual void init() override;

public:
    PhysicsNode(std::shared_ptr<PhysicsSpec>);
    virtual ~PhysicsNode();
    virtual std::shared_ptr<TreeNode> attachChild(std::shared_ptr<TreeNode> pChild) override;
    

    std::shared_ptr<GridManifold> getManifold() { return _pManifold; }
    void setTemps(vec3& vAccel, uint64_t frameId);
    void setTempPos(vec3& v) {  _vTempPos = v; }
    void setTempVelocity(vec3& v) {  _vTempVel = v; }
  //  void setTempAccelleration(vec3& v) { _vTempAcc = v; }
    vec3 getTempPos() { return _vTempPos; }
    vec3 getTempVelocity() { return _vTempVel; }
 //   vec3 getTempAccelleration() { return _vTempAcc; }
    uint64_t _iStamp;
    float getFriction() { return _fFriction; }
    void setIsAtRest(bool b) { _bIsAtRest = b; }
    uint64_t getLastMovedFrame() { return _iLastMovedFrame; }
    void setLastMovedFrame(uint64_t u) { _iLastMovedFrame = u; }
    uint64_t getLastAccFrame() { return _iLastAccFrame; }
    void setLastAccFrame(uint64_t u) { _iLastAccFrame = u; }
    vec3 getLastPos() { return _vHistoryPos[0]; }
    //void updateLastPos() {  }
    //void calcBoundBox(); //Do not call.  Used by physics only.
    //void calcBoundBox(Box3f* __out_ pBox, vec3& obPos, float extra_pad = 0); //Do not call.  Used by physics only.
    void calcSpeedBox();
    bool getIsHollow() { return _bHollow; }
   // WorldObj* getPixObj() { return _pPixObj; }
   // vec3 getPos();
   // void setPos(vec3& vp);
    //Box3f* getBox();
    //void updateRestState();
    bool getIsAtRest() { return _bIsAtRest; }
    bool getIsStatic() { return _fMass <= 0.0f; }
   // CellManifold* getManifold() { return _pCellManifold; }
    Box3f* getSpeedbox() { return _pSpeedbox; }
    vec3 getVelocity() { return _vVelocity; }
    void setVelocity(vec3& v);
   //vec3 getAccelleration() { return _vAccel; }
   //void setAccelleration(vec3& v) { _vAccel = v; }
    void validateSanePhysics();
    //void unstickFrom(Phy25* other);
};

}//ns Game



#endif
