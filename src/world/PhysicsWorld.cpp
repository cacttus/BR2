#include "../base/Logger.h"
#include "../base/Perf.h"
#include "../base/Gu.h"
#include "../base/FileSystem.h"
#include "../base/FpsMeter.h"
#include "../base/Hash.h"
#include "../base/GLContext.h"
#include "../gfx/GraphicsContext.h"
#include "../math/CollisionEquations.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/LightNode.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/RenderParams.h"
#include "../gfx/RenderSettings.h"
#include "../model/MeshUtils.h"
#include "../model/MeshNode.h"
#include "../model/UtilMeshInline.h"
#include "../model/Model.h"
#include "../model/OBB.h"
#include "../world/WorldHeader.h"
#include "../world/PhysicsNode.h"
#include "../world/PhysicsWorld.h"
#include "../world/Manifold.h"
#include "../world/PhysicsGrid.h"
#include "../world/PhysicsGridAwareness.h"
#include "../world/RenderBucket.h"
#include "../world/Scene.h"


namespace BR2 {
PhysicsWorld::PhysicsWorld(std::shared_ptr<Scene> pscene) {
  _pScene = pscene;
}
PhysicsWorld::~PhysicsWorld() {
  //DEL_MEM(_pWorldBox);
  //DEL_MEM(_pAwareness);
  //DEL_MEM(_pRenderBucket);
  //DEL_MEM(_pRenderBucket);
}

std::shared_ptr<PhysicsWorld> PhysicsWorld::create(std::shared_ptr<Scene> s, float fNodeWidth, float fNodeHeight, vec3& vUp,
  MpFloat awXZ, float awXZInc, MpFloat awY, float awYInc,
  MpInt mpNodesY, uint32_t iGridCountLimit) {
  std::shared_ptr<PhysicsWorld> w = std::make_shared<PhysicsWorld>(s);
  w->init(fNodeWidth, fNodeHeight, vUp, awXZ, awXZInc, awY, awYInc, mpNodesY, iGridCountLimit);
  return w;
}
std::multimap<float, std::shared_ptr<PhysicsGrid>>& PhysicsWorld::getVisibleGrids() { return _pRenderBucket->getGrids(); }
std::multimap<float, std::shared_ptr<SceneNode>>& PhysicsWorld::getVisibleNodes() { return _pRenderBucket->getObjs(); }
void PhysicsWorld::init(float fNodeWidth, float fNodeHeight, vec3& vUp,
  MpFloat awXZ, float awXZInc, MpFloat awY, float awYInc,
  MpInt mpNodesY, uint32_t iGridCountLimit) {
  _vUp = vUp;
  _fNodeWidth = fNodeWidth;
  _fNodeHeight = fNodeHeight;
  _pAwareness = nullptr;
  _iGridCountLimit = iGridCountLimit;
  _mpNodesY = mpNodesY;
  _pAwareness = std::make_shared<PhysicsGridAwareness>(getThis<PhysicsWorld>(), awXZ, awXZInc, awY, awYInc);
  _pWorldBox = std::make_unique<Box3f>();
  _pWorldBox->_min = _pWorldBox->_max = vec3(0, 0, 0);
  _pRenderBucket = std::make_shared<RenderBucket>();
}
void PhysicsWorld::getNodeRangeForBox(Box3f* c, ivec3* __out_ p0, ivec3* __out_ p1, bool bLimitByWorldBox) {
  //CreateCellsForVolume.
  *p0 = v3Toi3Node(c->_min);
  *p1 = v3Toi3Node(c->_max);

  if (bLimitByWorldBox) {
    //limit the returend "grid of grids" by the actual 'bound box' of our grid.
    //this speeds up computation rather than simply trying to calculate every box for an object.
    ivec3 v0 = v3Toi3Node(_pWorldBox->_min);
    ivec3 v1 = v3Toi3Node(_pWorldBox->_max);
    *p0 = p0->clampTo(v0, v1);
    *p1 = p1->clampTo(v0, v1);
  }

  //Validate
  int32_t ciMaxCellsXYZ = 999999999; // 999mil
  int32_t x, y, z;
  x = (p1->x - p0->x);
  y = (p1->y - p0->y);
  z = (p1->z - p0->z);

  AssertOrThrow2(x >= 0);
  AssertOrThrow2(y >= 0);
  AssertOrThrow2(z >= 0);
  AssertOrThrow2(x < ciMaxCellsXYZ);//theoretically we can have more than this.
  AssertOrThrow2(y < ciMaxCellsXYZ);
  AssertOrThrow2(z < ciMaxCellsXYZ);
}
ivec3 PhysicsWorld::v3Toi3Node(vec3& v) {
  return v3Toi3Any(v, 1.0f / _fNodeWidth, 1.0f / _fNodeHeight);
}
ivec3 PhysicsWorld::v3Toi3Any(vec3& v, float w1, float h1) {
  ivec3 ret;
  float ex, ey, ez;

  ex = v.x * w1; //** w1 is the RECIPROCAL width of the cell/node 
  ey = v.y * h1;
  ez = v.z * w1;

  ret.x = (int32_t)floorf(ex);
  ret.y = (int32_t)floorf(ey);
  ret.z = (int32_t)floorf(ez);

  return ret;
}
std::shared_ptr<PhysicsGrid> PhysicsWorld::getNodeAtPos(ivec3& pos) {
  GridMap::iterator ite = _gridMap.find(&pos);

  if (ite == _gridMap.end()) {
    return nullptr;
  }

  return ite->second;
}
void PhysicsWorld::addGrid(std::shared_ptr<PhysicsGrid> pGrid, const ivec3& cv) {
  ivec3* idx = new ivec3(cv);
  if (pGrid != nullptr) {
    debugMakeSureNoDupes(pGrid->getGridPos());
    _gridMap.insert(std::make_pair(idx, pGrid));

    //Loop over objects to see if their manifold intersects this.
    calcGridManifold(pGrid);
  }
  else {
    _setEmpty.insert(idx);
  }

  updateWorldBox();
}
void PhysicsWorld::updateWorldBox() {
  _pWorldBox->genResetLimits();
  for (std::pair<ivec3*, std::shared_ptr<PhysicsGrid>> p : _gridMap) {
    _pWorldBox->genExpandByBox(p.second->getBoundBox());
  }
}
void PhysicsWorld::debugMakeSureNoDupes(const ivec3& vv) {

  for (GridMap::iterator it = _gridMap.begin(); it != _gridMap.end(); it++) {
    ivec3* v = it->first;
    if (v->x == vv.x && v->y == vv.y && v->z == vv.z) {
      BRThrowException("Duplicate node found.");
    }
  }

}
void PhysicsWorld::unloadWorld() {
  for (ivec3* iv : _setEmpty) {
    DEL_MEM(iv);
  }
  _setEmpty.clear();
  _gridMap.clear();
}
void PhysicsWorld::reparentObjectByCustomBox(std::shared_ptr<PhysicsNode> ob, Box3f* pBox) {
  bool bSet = false;
  bool bSet2 = false;
  //clear
  clearObjectManifoldAndRemoveFromGrid(ob);

  //Gather nodes
  Box3f c;
  c._min.x = pBox->_min.x;
  c._min.y = pBox->_min.y;
  c._min.z = pBox->_min.z;
  c._max.x = pBox->_max.x;
  c._max.y = pBox->_max.y;
  c._max.z = pBox->_max.z;

  ivec3 p0, p1;
  getNodeRangeForBox(&c, &p0, &p1, true);

  for (int32_t node_z = p0.z; node_z <= p1.z; ++node_z) {
    for (int32_t node_y = p0.y; node_y <= p1.y; ++node_y) {
      for (int32_t node_x = p0.x; node_x <= p1.x; ++node_x) {
        ivec3 nodePos(node_x, node_y, node_z);
        std::shared_ptr<PhysicsGrid> pGrid;
        std::vector<std::shared_ptr<PhysicsGrid>> gen;
        pGrid = getNodeAtPos(nodePos);
        //f (pGrid == nullptr) {
        //   makeOrCollectGridForPos(nodePos, gen);
        //
        pGrid = getNodeAtPos(nodePos);
        if (pGrid != nullptr) {
          //Now get cells

          ivec3 cell_p0, cell_p1;
          addNodeToGrid(ob, pGrid);


          //Invalid ranges here we'll simply ignore them, because they're going to happen.
          //pGrid->getLocalCellRangeForBox(c, cell_p0, cell_p1);
          //for (int32_t cell_z = cell_p0.z; cell_z <= cell_p1.z; ++cell_z) {
          //for (int32_t cell_y = cell_p0.y; cell_y <= cell_p1.y; ++cell_y) {
          //for (int32_t cell_x = cell_p0.x; cell_x <= cell_p1.x; ++cell_x) {
          //    ivec3 cellPos(cell_x, cell_y, cell_z);
          //    WorldCell* pCell = pGrid->getCellForIndex(cellPos);

          //    if (pCell != nullptr) {
          //        //Cell cannot ever be null.

          //        //Add to Grid, Cell, and Cell to OB
          //        //*20170503 removed cell manifolds
          //        //   pCell->getCellManifold()->add(ob);
          //        ob->getManifold()->addCell(pCell);
          //    }
          //    else {
          //        //Object is outside of the grid.
          //        //We should allow this because this will happen with physics anomolies.
          //        if (_pContext->getFpsMeter()->frameMod(30) && bSet == false) {
          //            BroLogInfo("OBject outside Cells!");
          //            bSet = true;
          //        }
          //        //AssertOrThrow2(pCell != nullptr);
          //        // Gu::debugBreak();
          //    }


          //}
          //}
          //}

        }
        else {
          //if (Gu::getFpsMeter()->frameMod(30)) {
          //    BroLogWarn("OBject outside grid!");
          //}
          //**Object is approaching outside grid bounds.
          //Gu::debugBreak();
        }
      }
    }
  }


}
void PhysicsWorld::calcGridManifold(std::shared_ptr<PhysicsGrid> pGrid) {
  //do this when we add grids so that teh grids can 'capture' objects that weren't added.
  for (std::pair<NodeId, std::shared_ptr<PhysicsNode>> p : _mapObjects) {
    std::shared_ptr<PhysicsNode> pn = p.second;
    if (pn->getBoundBoxObject()->cubeIntersect_EasyOut_Inclusive(pGrid->getBoundBox())) {
      addNodeToGrid(pn, pGrid);
    }
  }
}
void PhysicsWorld::addNodeToGrid(std::shared_ptr<PhysicsNode> ob, std::shared_ptr<PhysicsGrid> pGrid) {
  pGrid->getManifold()->add(ob);
  ob->getManifold()->addGrid(pGrid);
}
void PhysicsWorld::clearObjectManifoldAndRemoveFromGrid(std::shared_ptr<PhysicsNode> ob) {
  //Clear the manifold.
  //std::set<WorldCell*>* setCells = ob->getManifold()->getCells();
  //std::set<WorldCell*>::iterator itCell = setCells->begin();

  //*20170503 removed cell manifolds
  //20170110 removed cells manifolds (again?)
  //Remove object from cells.
  //for (; itCell != setCells->end(); itCell++) {
  //    WorldCell* pCell = *itCell;
  //    pCell->getCellManifold()->remove(ob);
  //}

  //Remove object from grids.
  std::set<std::shared_ptr<PhysicsGrid>>* setGrids = ob->getManifold()->getGrids();
  std::set<std::shared_ptr<PhysicsGrid>>::iterator itGrid = setGrids->begin();
  for (; itGrid != setGrids->end(); itGrid++) {
    std::shared_ptr<PhysicsGrid> pGrid = *itGrid;
    pGrid->getManifold()->remove(ob);
  }

  //Clear everything
  ob->getManifold()->clear();
}
void PhysicsWorld::refreshCache() {
  // _vecPlayerChars.clear();
  // _vecLights.clear();

  //Hacky way to gather chars
  //ObjMap::iterator obit = _mapObjects.begin();
  //for (; obit != _mapObjects.end(); obit++) {
  //    std::shared_ptr<PhysicsNode> ob = obit->second;
  //    if (ob->getSpec()->isPlayerChar()) {
  //        _vecPlayerChars.insert(ob);
  //    }
  //    if (ob->getSpec()->isEmitter()) {
  //        _vecLights.insert(ob);
  //    }
  //}
}
bool PhysicsWorld::tryRemoveObj(std::shared_ptr<PhysicsNode> ob) {
  if (ob == nullptr) {
    BRLogError("Tried to remove a null object.");
    return false;
  }
  ObjMap::iterator it = _mapObjects.find(ob->getId());
  if (it != _mapObjects.end()) {
    _mapObjects.erase(it);
    std::set<std::shared_ptr<PhysicsNode>>::iterator act = _vecActiveFrame.find(ob);
    if (act != _vecActiveFrame.end()) {
      _vecActiveFrame.erase(act);
    }
    clearObjectManifoldAndRemoveFromGrid(ob);

    refreshCache();
    return true;
  }
  return false;
}
void PhysicsWorld::addObj(std::shared_ptr<PhysicsNode> ob, bool bActivate, bool bRefreshCache) {
  if (ob->detachFromParent() == true) {
    BRLogWarn(ob->getSpecName() + "Added a parented OBJ - you may get invaid results.");
    Gu::debugBreak();
  }
  calc_obj_manifold(ob);

  //We can remove this to optimize each load
  if (bActivate) {
    activate(ob);
  }

  //Add to ?
  ObjMap::iterator it = _mapObjects.find(ob->getId());
  if (it != _mapObjects.end()) {
    BRLogError("ERROR - Found duplicate grid object in world.");
    Gu::debugBreak();
  }
  else {
    _mapObjects.insert(std::make_pair(ob->getId(), ob));
  }

  if (bRefreshCache) {
    refreshCache();
  }
}
void PhysicsWorld::activate(std::shared_ptr<PhysicsNode> ob) {
  if (ob->getIsStatic() == false) {
    _vecActiveFrame.insert(ob);
    ob->setIsAtRest(false);
  }
}
std::shared_ptr<PhysicsNode> PhysicsWorld::getObj(NodeId id) {
  ObjMap::iterator ite = _mapObjects.find(id);
  if (ite == _mapObjects.end()) {
    return nullptr;
  }
  return ite->second;
}
void PhysicsWorld::update(float delta) {
  Perf::pushPerf();
  for (std::pair<NodeId, std::shared_ptr<PhysicsNode>> p : _mapObjects) {
    //Still update hidden objects (i.e., correct boundbox computation), but don't draw them.
    p.second->update(delta, std::map<Hash32, std::shared_ptr<Animator>>());
  }

  _pAwareness->update(delta);
  makeGrid();
  collisionLoopDual(delta);

  //Collect all nodes
  BvhCollectionParams bcp;
  bcp._pFrustum = _pScene->getActiveCamera()->getFrustum();
  bcp._pRenderBucket = _pRenderBucket;
  float nw = getNodeWidth();
  float nh = getNodeHeight();
  float dd = Gu::getRenderSettings()->drawDistance();
  if (dd < 1) {
    dd = 1;
  }
  if (dd > 1000) {
    dd = 1000;
  }
  bcp._fMaxDist = vec3(nw, nh, nw).length2() * dd; // 15 cubes I guess.
  collectVisibleNodes(&bcp);
  Perf::popPerf();
}
void PhysicsWorld::collisionLoopDual(float delta) {
  //Begin Dual Step
  vec3 vGrav = _vUp * getGravityAmount();
  //  Physics* that = this;
  int32_t nIterV = 0, nIterA = 0;
  uint64_t frameId = Gu::getFpsMeter()->getFrameNumber();
  vec3 vDa = vGrav * delta;

  //Accelleration
  for (std::shared_ptr<PhysicsNode> ob : _vecActiveFrame) {
    ob->setTemps(vDa, frameId);
    nIterA++;
  }
  collide();
  postCollide(frameId, true);

  //Velcoity
  for (std::shared_ptr<PhysicsNode> ob : _vecActiveFrame) {
    ob->setTemps(ob->getVelocity(), frameId);
    nIterV++;
  }
  collide();
  postCollide(frameId, false);

}
void PhysicsWorld::postCollide(uint64_t frameId, bool bAccel) {

  std::set<std::shared_ptr<PhysicsNode>> vecFinalActive;
  int32_t  nIter2 = 0;
  for (std::shared_ptr<PhysicsNode> ob : _vecActiveFrame) {
    vec3 p_cur = ob->getTempPos();
    vec3 p_last = ob->getLastPos();
    vec3 v_cur = ob->getTempVelocity();
    //vec3 acc = ob->getTempAccelleration();

    //if (acc.squaredLength() > 0.00001f) {
    //    int nnn = 0;
    //    nnn++;
    //}
    if (checkMoved(p_last, p_cur, v_cur)) {
      vecFinalActive.insert(ob);

      ob->setPos(p_cur + v_cur);

      //Ok so ACC is an issue. We need to limit the ACC here. And don't set the ACC unless we're
      //a couple of frames without "ground".  This allows us to avoid "false air time" because
      //The ob sometimes doesn't collide, then gets an ACC, then "stops" when it hits a wall because
      // it thinks it needs to slide against the ground.
      // We could also use seconds.. like 0.4 seconds until we have gravity.
      //Similar to "On Ground" boolean from bullet

      //**So the resoulion to this is to do the accelleration velocity a second time
      // and double the collision routine.  THat's the only way to fix the "sticking" issue.
      /*
      //We are on the ground.
      if(acc.squaredLength() < 0.00003f){
      ob->setLastAccFrame(frameId);
      }
      if (frameId - ob->getLastAccFrame() < 10) {
      acc = 0.0f;
      }
      else {
      //If this hits, we should be in the air.
      //If not, then it's an error.
      int nnn=0;
      nnn++;
      }*/

      if (bAccel) {
        //Acc step.  Add the final ACc to the velocity 
        ob->setVelocity(ob->getVelocity() + v_cur);
      }

      ob->setLastMovedFrame(frameId);
      ob->setIsAtRest(false);
    }
    else if (frameId - ob->getLastMovedFrame() == 32) {
      //We didn't move.
      v_cur = 0.0f;
      ob->setVelocity(v_cur);
      ob->setPos(std::move(p_last));
      ob->setIsAtRest(true);
    }
    else {
      int nnn = 0;
      nnn++;
      ob->setVelocity(v_cur);
      ob->setPos(std::move(p_cur));
      vecFinalActive.insert(ob);
    }


    ob->validateSanePhysics();
    nIter2++;
  }

  _vecActiveFrame.clear();
  _vecActiveFrame = vecFinalActive;
}
FORCE_INLINE void limit_ax(float& __inout_ v_ax, float amt, bool& bActive) {
  float vsl;
  //v_ax = velocity along axis. 
  // if v_ax ^2 > amt^2 then we are active.
  vsl = v_ax * v_ax;
  if (vsl < amt) {
    v_ax = 0.0f;
  }
  else {
    bActive = true;
  }
}
bool PhysicsWorld::checkMoved(vec3& p_last, vec3& p_cur, vec3& v_cur) {
  bool bIsActive = false;

  limit_ax(v_cur.x, PHY_MIN_VEL_LEN_2, bIsActive);
  limit_ax(v_cur.y, PHY_MIN_VEL_LEN_2, bIsActive);
  limit_ax(v_cur.z, PHY_MIN_VEL_LEN_2, bIsActive);

  vec3 p_new = p_last + v_cur;
  vec3 p_d = (p_new - p_last);

  limit_ax(p_d.x, PHY_DELTA_P_LEN_2, bIsActive);
  limit_ax(p_d.y, PHY_DELTA_P_LEN_2, bIsActive);
  limit_ax(p_d.z, PHY_DELTA_P_LEN_2, bIsActive);

  return bIsActive;
}
void PhysicsWorld::collide() {
  sortObjects_CalculateSpeedboxes_And_CollectManifolds();

  unstick_loop();
  collide_loop();
}
void PhysicsWorld::unstick_loop() {
  //Unstick everything.
  int iMaxStickIter = 32;
  int iStickIter = 0;
  int nStuck = 0;

  do {
    nStuck = unstick();
    sortObjects_CalculateSpeedboxes_And_CollectManifolds();

    // _iStamp++;
  } while (nStuck > 0 && (iStickIter++ < iMaxStickIter));
}
void PhysicsWorld::collide_loop() {
  int32_t intDebugNumSteps = 0;//DEBUG
  int maxIter = 32;
  int32_t collided;

  // Time Physics
  do {
    //Increment until either there are no more collisions or we just need to fail out because we are stuck
    collided = collect_and_resolve_collisions();
    sortObjects_CalculateSpeedboxes_And_CollectManifolds();

    intDebugNumSteps++;
  } while (collided > 0 && maxIter-- > 0);
}
void PhysicsWorld::sortObjects_CalculateSpeedboxes_And_CollectManifolds() {
  //See also PhyscisaManager.cpp
  for (std::set<std::shared_ptr<PhysicsNode>>::iterator ite = _vecActiveFrame.begin(); ite != _vecActiveFrame.end(); ite++) {
    std::shared_ptr<PhysicsNode> ob = *ite;
    calc_obj_manifold(ob);
  }
}
void PhysicsWorld::calc_obj_manifold(std::shared_ptr<PhysicsNode> ob) {
  //assume bound box is calculated already.
  //ob->calcBoundBox();
  if (ob->getBoundBoxObject()->limitSizeForEachAxis(ob->getPos(), 20000.0f)) {
    //Restrict bound box size, this will prevent us from trying to reparent an object
    //that has an invalid bound box
    BRLogWarnCycle("Bound Box Of Object " + ob->getSpecName() + " was too large, >20000, check meshes to ensure box is accurate.");
  }

  //Update Speedbox
  ob->calcSpeedBox();
  ob->validateSanePhysics();

  //Same as reparentObjectByCustomBox
  reparentObjectByCustomBox(ob, ob->getSpeedbox());

  ob->validateSanePhysics();
}
int32_t PhysicsWorld::unstick() {
  //Collect all collisions that are already boxed.
  //Order them by distance from Z=0
  //Then loop over them from top to bottom and "unstick"
  //Loop until everything is unstuck.  This basically works because
  //we have unlimited space in the -Z direction.

  int32_t nStuck = 0;
  int32_t numComparisons = 0;
  std::multimap<float, StuckPair> mapZOrder;

  CheckedSet setChecked;

  for (std::shared_ptr<PhysicsNode> objA : _vecActiveFrame) {

    //**First collide with ground plane.
    std::set<std::shared_ptr<PhysicsGrid>>::iterator gridIt = objA->getManifold()->getGrids()->begin();
    for (; gridIt != objA->getManifold()->getGrids()->end(); gridIt++) {
      std::shared_ptr<PhysicsGrid> pGrid = *gridIt;
      //Phy25* objGrid = pGrid->getGroundPhy();
      //if (setChecked.hasPair(objA, objGrid) == false) {
      //    pGrid->handleGridPhysics(objA, GridPhysicsStep::e::Unstick);
      //    setChecked.addPair(objA, objGrid);
      //    numComparisons++;
      //}

      //**Then collide against all objects.
      //**TODO: this needs to be optimized with some kind of axis sweep
      std::set<std::shared_ptr<PhysicsNode>>::iterator objIte2 = pGrid->getManifold()->getAll()->begin();
      for (; objIte2 != pGrid->getManifold()->getAll()->end(); objIte2++) {
        std::shared_ptr<PhysicsNode> objB = (*objIte2);

        if (objB->getIsHollow()) {
          continue;
        }
        if (objA == objB) {
          continue;
        }

        //No static or rest checks because the first iterator is alwyas the active objects.
        if (setChecked.hasPair(objA, objB) == false) {
          setChecked.addPair(objA, objB);
          if (unstick_objs(objA, objB, objA->getBoundBoxObject(), objB->getBoundBoxObject())) {
            nStuck++;
          }
          numComparisons++;
        }

      }
    }


  }
  setChecked.clear();

  return nStuck;
}
bool PhysicsWorld::unstick_objs(std::shared_ptr<PhysicsNode> objA, std::shared_ptr<PhysicsNode> objB, Box3f* in_bA, Box3f* in_bB) {
  //Box3f* boxA = objA->getBox();
  //Box3f* boxB = objB->getBox();

  if (in_bA->cubeIntersect_EasyOut_Inclusive(in_bB)) {
    vec3 ac = in_bA->center();
    vec3 bc = in_bB->center();
    vec3 ar = (in_bA->_max - in_bA->_min) * 0.5f;
    vec3 br = (in_bB->_max - in_bB->_min) * 0.5f;

    vec3 ar_pad = ar + PHY_COLLIDE_PADDING_EPSILON;
    vec3 br_pad = br + PHY_COLLIDE_PADDING_EPSILON;


    //TODO:
    //this algorithm only works if the two cube centers are not at teh same position
    //SO - if they are we want to set one cube higher than the other. along the -z

    float out_t0, out_t1;
    //  bool count_xyz;
    bool b_stuck;
    int ax_t0, ax_t1;

    vec3 av_c;
    vec3 bv_c;

    if (objA == nullptr || objA->getIsStatic()) {
      bv_c = (bc - ac);
      av_c = 0.0f;
    }
    else if (objB == nullptr || objB->getIsStatic()) {
      av_c = (ac - bc);
      bv_c = 0.0f;
    }
    else {
      av_c = (ac - bc) * 0.5f;
      bv_c = (bc - ac) * 0.5f;
    }

    Ceq::sat_box3_box3_t(ac, bc, ar_pad, br_pad, av_c, bv_c, out_t0, out_t1, ax_t0, ax_t1, b_stuck, true);
    float move_t = fabsf(out_t0) > fabsf(out_t1) ? out_t1 : out_t0;

    if (move_t == 0.0f) {
      //This was causing us to get stuck because with 0 T we can't move p +v
      return false;
    }
    else {
      //Run a second time, without the padding.
      unstick_if_moved(objA, objB, in_bA, in_bB, av_c, bv_c, move_t);
    }

    return true;
  }
  return false;
}
void PhysicsWorld::unstick_if_moved(std::shared_ptr<PhysicsNode> obA, std::shared_ptr<PhysicsNode> obB, Box3f* in_bA, Box3f* in_bB,
  vec3& velA_new, vec3& velB_new, float move_t) {
  //update _vecActiveObjects if the objects have moved a significant amount.

  Box3f boxATemp;
  Box3f boxBTemp;
  vec3 v_tmp_a;
  vec3 v_tmp_b;
  vec3 p_new_a;
  vec3 p_new_b;
  vec3 p_last_a;
  vec3 p_last_b;
  float ob_fric_a;
  float ob_fric_b;
  vec3 v_new_a;
  vec3 v_new_b;

  Box3f* bA = nullptr, * bB = nullptr;

  //calc new positions (must come first)
  if (obA != nullptr && obA->getIsStatic() == false) {
    ob_fric_a = obA->getFriction();
    p_last_a = obA->getTempPos();
    v_tmp_a = velA_new * move_t;
    //p_new_a = p_last_a + v_tmp_a;
    float vl; vec3 vn;
    v_tmp_a.len_and_norm(vn, vl);
    p_new_a = p_last_a + vn * (vl);
    obA->calcBoundBox(boxATemp, p_new_a, PHY_COLLIDE_PADDING_EPSILON);
    bA = &boxATemp;
  }
  else {
    bA = in_bA;
  }

  if (obB != nullptr && obB->getIsStatic() == false) {
    ob_fric_b = obB->getFriction();
    p_last_b = obB->getTempPos();
    v_tmp_b = velB_new * move_t;
    // p_new_b = p_last_b + v_tmp_b;
    float vl; vec3 vn;
    v_tmp_b.len_and_norm(vn, vl);
    p_new_b = p_last_b + vn * (vl);

    obB->calcBoundBox(boxBTemp, p_new_b, PHY_COLLIDE_PADDING_EPSILON);
    bB = &boxBTemp;
  }
  else {
    bB = in_bB;
  }

  //calc new vel
  if (obA != nullptr && obA->getIsStatic() == false) {
    //We multiply the velocity by -1 to invert it, since the "stuck"
    //formula uses a velocity that points AWAY from the given cube.
    unstick_ob_v2(p_new_a, v_tmp_a * -1.0f, bA, bB, move_t, ob_fric_a, v_new_a);

    //*wE NEED TO S 
    obA->setTempPos(p_new_a);
    obA->setTempVelocity(v_new_a);
  }
  if (obB != nullptr && obB->getIsStatic() == false) {
    unstick_ob_v2(p_new_b, v_tmp_b * -1.0f, bB, bA, move_t, ob_fric_b, v_new_b);
    obB->setTempPos(p_new_b);
    obB->setTempVelocity(v_new_b);
  }

}
void PhysicsWorld::unstick_ob_v2(vec3& ob_in_p, vec3& ob_in_v, Box3f* boxA, Box3f* boxB,
  float move_t, float ob_friction, vec3& __out_ out_new_v) {
  //The position must already be added by ct, and 
  //Assume the bounding boxes are recalculated

  vec3 plane_n;
  //    float plane_d;
      //20170407 NOTE: just switched this up, this may be wrong.  Using nthe new contact pnlane here
  vec3 dv(0, 0, 0);
  Ceq::aa_box_contact_plane_3(boxA, boxB, plane_n, dv);

  vec3 cv = ob_in_v;

#ifdef _DEBUG
  vec3 orig_v = cv;
  cv = orig_v;
#endif

  vec3 cvr = cv - (plane_n * cv.dot(plane_n) * 2.0f); // reflect velocity along plane
  vec3 o_pt2 = ob_in_p + cv + cvr; // lateral point
  vec3 cv_lat = o_pt2 - ob_in_p; // planar direction;

  float cv_len;
  vec3 cv_n;
  cv.len_and_norm(cv_n, cv_len);

  vec3 cv_lat_n = cv_lat.normalized();

  float friction = plane_n.dot(cv_n); // "slide factor."
  friction = MathUtils::brMax(-friction, 0.0f);
  friction -= ob_friction;
  friction = MathUtils::brClamp(friction, 0.0f, 1.0f);

  if (friction > 0) {
    //lateral velocity old velocity length times friction amount.
    cv = cv_lat_n * cv_len * friction;
    out_new_v = cv;
  }
  else {
    out_new_v = ob_in_v;
  }
}
int32_t PhysicsWorld::collect_and_resolve_collisions() {
  int32_t numCollided = 0;
  int32_t nStuck = 0;
  int32_t numComparisons = 0;
  std::multimap<float, StuckPair> mapZOrder;

  CheckedSet setChecked;

  //Clear active, we'll update it during the collision
  std::multimap<float, BoxCollision> vecBoxCollisions;

  //Copy _vecActiveFrame
  //  std::set<Phy25*> vecNextActive = _vecActiveFrame;

  for (std::shared_ptr<PhysicsNode> objA : _vecActiveFrame) {
    std::set<std::shared_ptr<PhysicsGrid>>::iterator gridIt = objA->getManifold()->getGrids()->begin();
    for (; gridIt != objA->getManifold()->getGrids()->end(); gridIt++) {
      std::shared_ptr<PhysicsGrid> pGrid = *gridIt;
      //Phy25* objGrid = pGrid->getGroundPhy();
      ////**First collide with ground .
      //if (setChecked.hasPair(objA, objGrid) == false) {
      //    if (pGrid->handleGridPhysics(objA, GridPhysicsStep::e::Collide, &vecBoxCollisions)) {
      //        numCollided++;
      //    }
      //    setChecked.addPair(objA, objGrid);
      //    numComparisons++;
      //}

      //Phy25* objGrid = pGrid->getGroundPhy();
      //if (setChecked.hasPair(objA, objGrid) == false) {
      //    pGrid->collideOb(objA);
      //    setChecked.addPair(objA, objGrid);
      //    numComparisons++;
      //}

      //**Then collide against all objects.
      //**TODO: this needs to be optimized with some kind of axis sweep
      std::set<std::shared_ptr<PhysicsNode>>::iterator objIte2 = pGrid->getManifold()->getAll()->begin();
      for (; objIte2 != pGrid->getManifold()->getAll()->end(); objIte2++) {
        std::shared_ptr<PhysicsNode> objB = (*objIte2);

        if (objB->getIsHollow()) {
          continue;
        }
        if (objA == objB) {
          continue;
        }

        //No static or rest checks because the first iterator is alwyas the active objects.
        if (setChecked.hasPair(objA, objB) == false) {
          //check_stuck(objA, objB, mapZOrder);
          // nStuck += unstick_objs(objA, objB, nextActive);
          if (collisionTestBox(objA, objB, vecBoxCollisions)) {
            numCollided++;
          }
          setChecked.addPair(objA, objB);
          numComparisons++;


        }
      }
    }
  }
  setChecked.clear();

  // _vecActiveFrame = vecNextActive;
  stabalizeCollisionSystem(vecBoxCollisions);

  return numCollided;
}
int32_t PhysicsWorld::collisionTestBox(std::shared_ptr<PhysicsNode> objA, std::shared_ptr<PhysicsNode> objB, std::multimap<float, BoxCollision>& vecBoxCollisions) {
  Box3f* ba = objA->getBoundBoxObject();
  Box3f* bb = objB->getBoundBoxObject();
  vec3 va = objA->getTempVelocity();
  vec3 vb = objB->getTempVelocity();
  vec3 ca = ba->center();
  vec3 cb = bb->center();
  vec3 ra = ba->_max - ca;
  vec3 rb = bb->_max - cb;
  vec3 ra_pad = ra + PHY_COLLIDE_PADDING_EPSILON;
  vec3 rb_pad = rb + PHY_COLLIDE_PADDING_EPSILON;

  float t0, t1;
  int ax_t0, ax_t1;
  bool bStuck = false;
#ifdef _DEBUG
  if (objB->getBoundBoxObject() && objA->getBoundBoxObject()) {
    if (objA->getBoundBoxObject()->cubeIntersect_EasyOut_Inclusive(objB->getBoundBoxObject())) {
      //already touching
      //Note here that we won't hit this but we'll still be collided because of the 
      // box "padding" above.
      int nnn = 0;
      nnn++;
    }
  }
#endif
  if (Ceq::sat_box3_box3_t(ca, cb, ra, rb, va, vb, t0, t1, ax_t0, ax_t1, bStuck, false)) {
#ifdef _DEBUG
    if (objB->getSpeedbox() && objA->getSpeedbox()) {
      if (objA->getSpeedbox()->cubeIntersect_EasyOut_Inclusive(objB->getSpeedbox())) {
        int nnn = 0;
        nnn++;
      }
      else {
        //Error
        int nnn = 0;
        nnn++;
      }
    }
#endif

    //Run the equation a second time without padding and consider that the correct Time.
    // Ceq::sat_box3_t(ca, cb, ra, rb, va, vb, t0, t1, ax_t0, ax_t1, bStuck, false);

    BoxCollision bb;// = new BoxCollision();

    bb._bStuck = bStuck;
    bb._bCollided = true;
    bb._ob1 = objA;
    bb._ob2 = objB;
    bb._t = t0;
    bb._ax_t0 = ax_t0;

    vecBoxCollisions.insert(std::make_pair(bb._t, bb));

    if (vecBoxCollisions.size() > 1) {
      int nnn = 0;
      nnn++;
    }

    return 1;
  }
  else {
#ifdef _DEBUG
    if (objB->getSpeedbox() && objA->getBoundBoxObject()) {
      if (objA->getBoundBoxObject()->cubeIntersect_EasyOut_Inclusive(objB->getSpeedbox())) {
        //for grids - error
        //Error
        int nnn = 0;
        nnn++;
      }
    }
    else if (objB->getBoundBoxObject() && objA->getSpeedbox()) {
      if (objA->getSpeedbox()->cubeIntersect_EasyOut_Inclusive(objB->getBoundBoxObject())) {
        //for grids - error
        //Error
        int nnn = 0;
        nnn++;
      }
    }
    else if (objB->getBoundBoxObject() && objA->getBoundBoxObject()) {
      if (objA->getBoundBoxObject()->cubeIntersect_EasyOut_Inclusive(objB->getBoundBoxObject())) {
        //for grids - error
        int nnn = 0;
        nnn++;
      }
    }
#endif
    return 0;
  }

}
int32_t PhysicsWorld::stabalizeCollisionSystem(std::multimap<float, BoxCollision>& vecBoxCollisions) {
  // Staballize
  int nResolved = 0;
  std::multimap<float, BoxCollision>::iterator ite = vecBoxCollisions.begin();
  //_iStamp++;

  for (; ite != vecBoxCollisions.end(); ite++) {
    BoxCollision& bc = ite->second;
    resolve_pair_t(bc);

    nResolved++;
  }

  // _vecActiveFrame.clear();
  //_vecActiveFrame = nextActive;

  return nResolved;
}
void PhysicsWorld::resolve_pair_t(BoxCollision& pCol) {
  if (pCol._bStuck) {
    int nnn = 0;
    nnn++;
  }

  std::shared_ptr<PhysicsNode> objA = pCol._ob1;
  std::shared_ptr<PhysicsNode> objB = pCol._ob2;
  Box3f* boxA = objA->getBoundBoxObject();
  Box3f* boxB = objB->getBoundBoxObject();

  vec3 av = objA->getTempVelocity();
  vec3 bv = objB->getTempVelocity();
  // vec3 aa = objA->getTempAccelleration();
  //vec3 ba = objB->getTempAccelleration();

  //if a is box && b is box
  bb_resolve_setup_data(objA, objB, av, bv, boxA, boxB, pCol._t, pCol._ax_t0);
}
void PhysicsWorld::bb_resolve_setup_data(std::shared_ptr<PhysicsNode> obA, std::shared_ptr<PhysicsNode> obB,
  vec3& v_last_a, vec3& v_last_b,
  Box3f* boxA, Box3f* boxB, float move_t, int ax_t0) {
  //Calc box, move out, then slide

  Box3f boxATemp;
  Box3f boxBTemp;
  vec3 v_move_a;
  vec3 v_move_b;
  vec3 p_new_a;
  vec3 p_new_b;
  vec3 p_last_a;
  vec3 p_last_b;
  float ob_fric_a;
  float ob_fric_b;
  vec3 v_new_a;
  vec3 v_new_b;
  //  vec3 a_new_a;
  // vec3 a_new_b;

  Box3f* bA, * bB;

  //calc new positions and boxes (must come first)
  if (obA->getIsStatic() == false) {
    ob_fric_a = obA->getFriction();
    p_last_a = obA->getTempPos();
    v_move_a = v_last_a * move_t;
    p_new_a = p_last_a + v_move_a * move_t;// * (vl* move_t);
    obA->calcBoundBox(boxATemp, p_new_a, 0);//Do not add padding - we cannot be intersecting when we calc the normal
    bA = &boxATemp;
  }
  else {
    bA = obA->getBoundBoxObject();
    p_new_a = obA->getBoundBoxObject()->center(); //ONly used for calculating the collision plane
  }

  if (obB->getIsStatic() == false) {
    ob_fric_b = obB->getFriction();
    p_last_b = obB->getTempPos();
    v_move_b = v_last_b * move_t;
    p_new_b = p_last_b + v_move_b * move_t;
    obB->calcBoundBox(boxBTemp, p_new_b, 0);//Do not add padding - we cannot be intersecting when we calc the normal
    bB = &boxBTemp;
  }
  else {
    bB = obB->getBoundBoxObject();
    p_new_b = obB->getBoundBoxObject()->center();
  }


  //calc new vel
  if (obA->getIsStatic() == false) {
    //We multiply the velocity by -1 to invert it, since the "stuck"
    //formula uses a velocity that points AWAY from the given cube.
    bb_move_and_slide(p_last_a, v_last_a, p_new_a, v_move_a, bA, bB, move_t, ob_fric_a, v_new_a, ax_t0);
    obA->setTempPos(p_new_a);
    obA->setTempVelocity(v_new_a);
  }
  if (obB->getIsStatic() == false) {
    bb_move_and_slide(p_last_b, v_last_b, p_new_b, v_move_b, bB, bA, move_t, ob_fric_b, v_new_b, ax_t0);
    obB->setTempPos(p_new_b);
    obB->setTempVelocity(v_new_b);
  }

}
void PhysicsWorld::bb_move_and_slide(const vec3& ob_last_p, const vec3& ob_last_v,
  const vec3& ob_new_p, const vec3& ob_move_v, Box3f* boxA, Box3f* boxB,
  float move_t, float ob_friction, vec3& __out_ out_new_v, const int ax_t0) {
  //The position must already be added by ct, and 
  //Assume the bounding boxes are recalculated

  vec3 plane_n;
  Ceq::aa_box_contact_plane_3(boxA, boxB, plane_n, ob_last_v);

  vec3 cv = ob_last_v;

#ifdef _DEBUG
  vec3 orig_v = cv;
  cv = orig_v;
#endif

  vec3 rem_v = (ob_last_v * (1.0f - move_t));
  vec3 cvr = rem_v - (plane_n * rem_v.dot(plane_n) * 2.0f);//reflect
  vec3 o_pt2 = ob_new_p + rem_v + cvr; // add both to get the lateral PT
  vec3 cv_lat = o_pt2 - ob_new_p;
  vec3 cv_lat_n = cv_lat.normalized();

  float cv_len;
  vec3 cv_n;
  cv.len_and_norm(cv_n, cv_len);

  float len_rem = cv_len - cv_len * move_t;//Remaining length "energy" from the collision

  float friction = 1;
  friction = plane_n.dot(cv_n); // "slide factor."
  friction = 1.0f + (MathUtils::brMin(friction * 0.8f, 0.0f));// we remove some friction to make the char slide more.
  friction = MathUtils::brClamp(friction, 0.0f, 1.0f);
  friction *= ob_friction;
  friction = MathUtils::brClamp(friction, 0.0f, 1.0f);

  if (friction > 0) {
    //lateral velocity old velocity length times friction amount.
    out_new_v = cv_lat_n * len_rem * friction;
  }
  else {
    out_new_v = 0.0;//ob_last_v;
  }

  ////Accellertaion
  //vec3 ob_a_n = ob_last_a.normalized();
  //friction = plane_n.dot(ob_a_n); // "slide factor."
  //friction = 1.0 + (broMin(friction, 0.0f));// Use hard friction for accel
  //out_new_a = ob_last_a * friction;
}
bool PhysicsWorld::collideMesh(std::shared_ptr<PhysicsNode> ob, vec3* verts, size_t vOffBytes, size_t vStrideBytes, size_t vCount, v_index32* inds, int32_t iOff, int32_t iCount) {
  bool bRet = false;
  Box3f* obBox = ob->getBoundBoxObject();
  v_index32 ind[3];
  vec3 v[3];

  //loop triangles 
  for (int iInd = iOff; iInd < iCount; iInd += 3) {
    ind[0] = *(inds + iInd + 0);
    ind[1] = *(inds + iInd + 1);
    ind[2] = *(inds + iInd + 2);
    v[0] = *(MeshUtils::getVertexElementOffset(verts, ind[0], vOffBytes, vStrideBytes, vCount));
    v[1] = *(MeshUtils::getVertexElementOffset(verts, ind[1], vOffBytes, vStrideBytes, vCount));
    v[2] = *(MeshUtils::getVertexElementOffset(verts, ind[2], vOffBytes, vStrideBytes, vCount));

    if (Ceq::triangleAabbTest(v, obBox->_min, obBox->_max)) {

    }
  }

  return bRet;
}
void PhysicsWorld::refreshObjectManifold(std::shared_ptr<PhysicsNode> ob) {
  calc_obj_manifold(ob);

  //If we have no grids in our manifold we are "orphaned"
  // Delete this object if it is not persistent.
  if (ob->getManifold()->getGrids()->size() == 0) {
    std::shared_ptr<PhysicsSpec> ps = std::dynamic_pointer_cast<PhysicsSpec>(ob->getSpec());

    BRLogWarn("Object is outside of grid, wes hould probably destroy it");
    //if (ps && ps->getIsPersistent() == false) {
    //    _pWorld25->delObj(ob->getId());
    //}
  }

}
void PhysicsWorld::limitVelocity(vec3& __inout_ v) {
  float c_maxVelLength = 1.0; //meters per second squared.
  float c_maxVelLength2 = c_maxVelLength * c_maxVelLength; //meters per second squared.
  if (v.squaredLength() >= c_maxVelLength2) {
    v = v.normalize() * c_maxVelLength;
  }
}
void PhysicsWorld::limitAccelleration(vec3& __inout_ va) {
  float c_maxAccLength = 2.0; //meters per second squared.
  float c_maxAccLength2 = c_maxAccLength * c_maxAccLength; //meters per second squared.
  if (va.squaredLength() > c_maxAccLength2) {
    va = va.normalize() * c_maxAccLength;
  }
}
void PhysicsWorld::makeGrid() {
  //This is the main grid thing.
  //We may be able to limit the Y axis here.
  //_setVisibleGrids.clear();

  Box3f c;
  getAwareness()->getAwarenessBox(c);
  ivec3 p0, p1;
  getNodeRangeForBox(&c, &p0, &p1, true);

  std::vector<std::shared_ptr<PhysicsGrid>> vecGen;
  sweepGridBox(
    [&](ivec3& cv) {
      if (getGrids().size() < _iGridCountLimit) {
        makeOrCollectGridForPos(cv, vecGen);
      }
    }
  , p0, p1);

  vecGen.resize(0);

}
void PhysicsWorld::sweepGridBox(std::function<void(ivec3&)> func, ivec3& viMin, ivec3& viMax) {
  int yi = _mpNodesY.getMin();
  int ya = _mpNodesY.getMax();
  AssertOrThrow2(yi <= ya);

  int dbg_nGrids = abs(viMax.x - viMin.x + 1) * abs(viMax.z - viMin.z + 1);
  if (dbg_nGrids > 5000) {
    BRLogWarnCycle("sweepGridBox, too many grids, looping over: " + dbg_nGrids);
    static int n = 0;
    if (n == 0) {
      Gu::debugBreak();
    }
  }
  for (int32_t k = viMin.z; k <= viMax.z; ++k) {
    for (int32_t j = yi; j <= ya; ++j) {//p0.y; j <= p1.y; ++j) {
      for (int32_t i = viMin.x; i <= viMax.x; ++i) {
        ivec3 cv(i, j, k);
        func(cv);
      }
    }
  }
}
void PhysicsWorld::getNodeBoxForGridPos(const ivec3& pt, Box3f& __out_ box) const {
  box._min.x = (float)(pt.x + 0) * getNodeWidth();
  box._min.y = (float)(pt.y + 0) * getNodeHeight();
  box._min.z = (float)(pt.z + 0) * getNodeWidth();
  box._max.x = (float)(pt.x + 1) * getNodeWidth();
  box._max.y = (float)(pt.y + 1) * getNodeHeight();
  box._max.z = (float)(pt.z + 1) * getNodeWidth();
}

void PhysicsWorld::makeOrCollectGridForPos(ivec3& cv, std::vector<std::shared_ptr<PhysicsGrid>>& vecGenerated) {
  std::shared_ptr<PhysicsGrid> pGrid = getNodeAtPos(cv);

  //Separate grid generation into 2 phases.  First always create so that we have the "grid".
  //Second, if the grid isn't
  //generated/ing, then put it in the generation queue.
  if (pGrid == nullptr) {

    //find grid.  if we can't find then add to "empty" set of ivec3
    if (_setEmpty.find(&cv) == _setEmpty.end()) {

      pGrid = loadGrid(cv);//May return nullptr for empty grid
      addGrid(pGrid, cv);
    }
  }


}
std::shared_ptr<PhysicsGrid> PhysicsWorld::loadGrid(const ivec3& pos) {
  std::shared_ptr<PhysicsGrid> p = nullptr;
  BRThrowNotImplementedException();
  return p;
}
std::shared_ptr<PhysicsGrid> PhysicsWorld::getNodeForPoint(vec3& pt) {
  ivec3 iv;
  std::shared_ptr<PhysicsGrid> gn = nullptr;

  iv = v3Toi3Node(pt);
  gn = getNodeAtPos(iv);
  return gn;
}
vec3 PhysicsWorld::i3tov3Node(const ivec3& iNode) {
  vec3 vOut;
  vOut.x = getNodeWidth() * (float)iNode.x;
  vOut.y = getNodeHeight() * (float)iNode.y;
  vOut.z = getNodeWidth() * (float)iNode.z;
#ifdef _DEBUG
  //This is called a lot so we're ifdefing it
  ivec3 v1 = v3Toi3Node(vOut);
  AssertOrThrow2(v1 == iNode);
#endif

  return vOut;
}

void PhysicsWorld::collectVisibleNodes(BvhCollectionParams* parms) {
  AssertOrThrow2(parms->_pFrustum != nullptr);
  AssertOrThrow2(parms->_pRenderBucket != nullptr);
  AssertOrThrow2(parms->_pVisibleCamera != nullptr);

  
  if (parms->_pRenderBucket->hasItems()) {
    BRLogWarn("Render bucket had items while collecting nodes.");
  }

  parms->_pRenderBucket->clear(parms->_pVisibleCamera);
  std::set<ivec3*, ivec3::Vec3xCompLess> grids;

  //Get All Grids
  sweepGridFrustum([&](ivec3& cv) {
    std::shared_ptr<PhysicsGrid> pGrid = getNodeAtPos(cv);
    if (pGrid != nullptr) {
      std::shared_ptr<FrustumBase> frust = parms->_pVisibleCamera->getFrustum();
      Box3f* box = pGrid->getBoundBox();
      if (frust->hasBox(box)) {
        parms->_pRenderBucket->addGrid(pGrid);
      }
    }
    }, parms->_pFrustum, parms->_fMaxDist);

  //Add meshes to render bucket
  int32_t dbgHidden = 0;
  for (std::pair<NodeId, std::shared_ptr<PhysicsNode>> p : _mapObjects) {
    if (p.second->getHidden() == false) {
      if (parms->_pFrustum->hasBox(p.second->getBoundBoxObject())) {
        float fDist2 = (parms->_pFrustum->getNearPlaneCenterPoint() - p.second->getBoundBoxObject()->center()).length2();
        parms->_pRenderBucket->addObj(p.second);
      }
      else {
        dbgHidden++;
      }
    }
  }

}
void PhysicsWorld::sweepGridFrustum(std::function<void(ivec3&)> func, std::shared_ptr<FrustumBase> pf, float fMaxDist2) {
  vec3 cp = pf->getNearPlaneCenterPoint();
  int32_t iDebugSweepCount = 0;
  std::vector<ivec3> toCheck;
  std::set<ivec3*, ivec3::Vec3xCompLess> checked;

  toCheck.reserve(2000);

  //Seed
  toCheck.push_back(v3Toi3Node(cp));

  while (toCheck.size() > 0) {
    Box3f box;
    ivec3 vi = toCheck[0];
    toCheck.erase(toCheck.begin() + 0);
    iDebugSweepCount++;

    if (checked.find(&vi) == checked.end()) {
      //TODO: fix this because we're getting stack overflows
      //Shitty.. but what else could we do?
      checked.insert(new ivec3(vi));

      // if the grid right here intersects the frustum
      getNodeBoxForGridPos(vi, box);

      vec3 node_center = box.center();

      float fDist2 = (pf->getNearPlaneCenterPoint() - node_center).length2();

      if (fDist2 < fMaxDist2) {
        if (pf->hasBox(&box)) {
          func(vi);

          //Sweep Neighbors
          vec3 n[6];
          n[0] = node_center + vec3(-getNodeWidth(), 0, 0);
          n[1] = node_center + vec3(getNodeWidth(), 0, 0);
          n[2] = node_center + vec3(0, -getNodeHeight(), 0);
          n[3] = node_center + vec3(0, getNodeHeight(), 0);
          n[4] = node_center + vec3(0, 0, -getNodeWidth());
          n[5] = node_center + vec3(0, 0, getNodeWidth());
          toCheck.push_back(v3Toi3Node(n[0]));
          toCheck.push_back(v3Toi3Node(n[1]));
          toCheck.push_back(v3Toi3Node(n[2]));
          toCheck.push_back(v3Toi3Node(n[3]));
          toCheck.push_back(v3Toi3Node(n[4]));
          toCheck.push_back(v3Toi3Node(n[5]));

        }
        else {
          int nnn = 0;
          nnn++;
        }
      }
      else {
        int nnn = 0;
        nnn++;
      }
    }
  }


  for (ivec3* vi : checked) {
    delete vi;
  }
}
void PhysicsWorld::sweepGridFrustum_r(std::function<void(ivec3&)> func, std::shared_ptr<FrustumBase> pFrust, float fMaxDist2,
  vec3& pt, std::set<ivec3*, ivec3::Vec3xCompLess>& grids, int32_t& iDebugSweepCount) {
  //What this does:
  //for point P - snap to the ivec3 grid.
  // check if box intersects frustum, 
  // repeat for all 6 neighbors 
  // *we don't use physicsgrid here!*  it's a 'virtual' box because the grids are not contiguous
  // Testing: enters about 400 times.

  //Box3f box;
  //ivec3 vi = v3Toi3Node(pt);
  //iDebugSweepCount++;

  //if (grids.find(&vi) == grids.end()) {
  //    //TODO: fix this because we're getting stack overflows
  //    //Shitty.. but what else could we do?
  //    grids.insert(new ivec3(vi));

  //    // if the grid right here intersects the frustum
  //    getNodeBoxForGridPos(vi, box);

  //    vec3 node_center = box.center();

  //    float fDist2 = (pFrust->getNearPlaneCenterPoint() - node_center).length2();

  //    if (fDist2 < fMaxDist2) {
  //        if (pFrust->hasBox(&box)) {
  //            func(vi);

  //            //Sweep Neighbors
  //            vec3 n[6];
  //            n[0] = node_center + vec3(-getNodeWidth(), 0, 0);
  //            n[1] = node_center + vec3(getNodeWidth(), 0, 0);
  //            n[2] = node_center + vec3(0, -getNodeHeight(), 0);
  //            n[3] = node_center + vec3(0, getNodeHeight(), 0);
  //            n[4] = node_center + vec3(0, 0, -getNodeWidth());
  //            n[5] = node_center + vec3(0, 0, getNodeWidth());
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[0], grids, iDebugSweepCount);
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[1], grids, iDebugSweepCount);
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[2], grids, iDebugSweepCount);
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[3], grids, iDebugSweepCount);
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[4], grids, iDebugSweepCount);
  //            sweepGridFrustum_r(func, pFrust, fMaxDist2, n[5], grids, iDebugSweepCount);

  //        }
  //        else {
  //            int nnn = 0;
  //            nnn++;
  //        }
  //    }
  //    else {
  //        int nnn = 0;
  //        nnn++;
  //    }
  //}

}

//void World25::getAllNodesForRayBoxSLOW(Ray_t* pr, float fBoxWidth2, float fBoxHeight2,
//    std::vector<std::shared_ptr<WorldGrid>>& __out_ grids, bool bLoadIfNotLoaded) {
//
//    if (getWorld25Plane()->intersectRay(pr)) {
//        vec3 pt = pr->getHitPoint();
//        Box3f c;
//
//        c._min = pt - vec3(fBoxWidth2, fBoxHeight2, fBoxWidth2);
//        c._max = pt + vec3(fBoxWidth2, fBoxHeight2, fBoxWidth2);
//
//        ivec3 p0, p1;
//        getNodeRangeForBox(&c, &p0, &p1);
//
//        int yi = CongaUtils::getNodesY().getMin();
//        int ya = CongaUtils::getNodesY().getMax();
//
//        std::vector<std::shared_ptr<WorldGrid>> vecGen;
//        int dbgn = 0;
//        int dbgp = 0;
//        int dbg_nGrids = abs(p1.x - p0.x + 1) * abs(p1.z - p0.z + 1);
//        for (int32_t k = p0.z; k <= p1.z; ++k) {
//            for (int32_t j = yi; j <= ya; ++j) {
//                for (int32_t i = p0.x; i <= p1.x; ++i) {
//                    ivec3 cv(i, j, k);
//                    std::shared_ptr<WorldGrid> g = SafeCast(getNodeAtPos(cv), std::shared_ptr<WorldGrid>);
//
//                    if (g == nullptr) {
//                        //Let's not do this for now, it coudl cause issues
//                        //if(bLoadIfNotLoaded) {
//                        //    createNode(cv, bInRange);
//                        //    grids.push_back(g);
//                        //}
//                    }
//                    else {
//                        grids.push_back(g);
//                    }
//
//                }
//            }
//        }
//    }
//}
std::shared_ptr<PhysicsNode> PhysicsWorld::findNode(string_t specName) {
  //Returns the FIRST node by the given spec name.
  Hash32 h = STRHASH(specName);
  for (std::pair<NodeId, std::shared_ptr<PhysicsNode>> p : _mapObjects) {
    if (h == p.second->getSpecNameHashed()) {
      return p.second;
    }
  }
  return nullptr;
}
void PhysicsWorld::drawShadows() {

}
void PhysicsWorld::drawForward() {
  RenderParams rp;
  for (std::pair<float, std::shared_ptr<SceneNode>> p : getVisibleNodes()) {
    std::shared_ptr<SceneNode> pm = p.second;
    pm->drawForward(rp);
  }
}
void PhysicsWorld::drawDeferred() {
  Perf::pushPerf();

  Gu::getCoreContext()->pushDepthTest();
  Gu::getCoreContext()->pushCullFace();
  Gu::getCoreContext()->pushBlend();
  {
    Gu::getCoreContext()->enableBlend(false);
    Gu::getCoreContext()->enableCullFace(true);
    Gu::getCoreContext()->enableDepthTest(true);

    //**
    //2/24 in order to set up the rendering system to be instanced we gotta change a lot around, like merge all the uniform buffers, skin joint buffers, and stuff.  Then reference by gl_InstanceID
    //so -- this is a huge task and it's nto really needed yet.
    //**

    //If we don't have skin, draw instanced, 
    //if we have skin draw normally.
    //_pRenderBucket->sortAndDrawMeshes(
    //    [](std::shared_ptr<VertexFormat> vf) {
    //        return Gu::getShaderMaker()->getDiffuseShader(vf);
    //    },
    //    [&](std::shared_ptr<ShaderBase> sb) {
    //        sb->bind();
    //        sb->setUf("_ufProj", (void*)&_projMatrix, 1, false);
    //        sb->setUf("_ufView", (void*)&_viewMatrix, 1, false);
    //        sb->setUf("_ufShadowLightPos", (void*)&vFinal, 1, false);
    //    },
    //    [](std::shared_ptr<ShaderBase> sb, std::shared_ptr<MeshNode> bn) {
    //        RenderParams rp;
    //        rp.setShader(sb);
    //        bn->drawDeferred(std::move(rp));
    //    }
    //);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    ////3D Models
    ////  glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //for (std::pair<float, std::shared_ptr<BaseNode>> p : getVisibleNodes()) {
    //    std::shared_ptr<BaseNode> pm = p.second;
    //    std::shared_ptr<WorldObject> wo = std::dynamic_pointer_cast<WorldObject>(pm);
    //    if (wo != nullptr) {
    //        wo->drawDeferred(std::move(rp)); //Don't pass a matrix in, the model is the same now
    //    }
    //}
    //3D Models
    //  glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshes()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawDeferred(rp);
    }
  }
  Gu::getCoreContext()->popBlend();
  Gu::getCoreContext()->popCullFace();
  Gu::getCoreContext()->popDepthTest();

  Perf::popPerf();
}
void PhysicsWorld::drawTransparent() {
  Perf::pushPerf();

  Gu::getCoreContext()->pushDepthTest();
  Gu::getCoreContext()->pushCullFace();
  Gu::getCoreContext()->pushBlend();
  {
    Gu::getCoreContext()->enableDepthTest(true);
    Gu::getCoreContext()->enableCullFace(true);
    Gu::getCoreContext()->enableBlend(false);

    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshesTransparent()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawTransparent(rp);
    }
  }
  Gu::getCoreContext()->popBlend();
  Gu::getCoreContext()->popCullFace();
  Gu::getCoreContext()->popDepthTest();

  Perf::popPerf();

}


}//ns Game
