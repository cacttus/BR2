#include "../base/Gu.h"
#include "../base/Hash.h"
#include "../base/GLContext.h"
#include "../base/oglErr.h"
#include "../base/FileSystem.h"
#include "../base/Logger.h"
#include "../gfx/QuadBufferMesh.h"
#include "../gfx/Atlas.h"
#include "../gfx/GpuQuad3.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/ShaderBase.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"
#include "../model/IboData.h"
#include "../model/TileMesh25.h"
#include "../model/MeshNode.h"
#include "../world/Manifold.h"
#include "../world/SpriteBucket.h"
//#include "../world/World25.h"
//#include "../world/WorldObj.h"
#include "../world/WorldGrid.h"
#include "../world/BottleUtils.h"
#include "../world/WorldCellFile.h"
#include "../world/WipGrid.h"

#include "../world/W25Config.h"
#include "../world/Lair.h"
#include "../world/WorldMaker.h"
#include "../world/BlockNode.h"
#include "../world/W25GridMesh.h"
#include "../world/W25MeshMaker.h"
#include "../world/WorldCell.h"

namespace BR2 {
WorldGrid::WorldGrid(std::shared_ptr<World25> pworld, ivec3& viPos, bool bEmpty) :
  PhysicsGrid(pworld, viPos, BottleUtils::getNodeWidth(), BottleUtils::getNodeHeight(), bEmpty),
  _pWorld25(pworld) {
}
WorldGrid::~WorldGrid() {
  deleteMeshes();
  //**No need to delete WorldCell, Gn2 deletes all of this
  DEL_MEM(_pRoot);
}
///////////////////////////////////////////////////////////////////
void WorldGrid::initSync() {
  //All meshes must be created because meshes "class" handles meshes now.
  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    _pMeshes[iMatter] = std::make_shared<W25GridMesh>(getThis<WorldGrid>(), (GridMeshLayer::e) iMatter);
  }


}
void WorldGrid::getCellData(BlockNode* parent, WorldCellFile* __out pFile) {
  WorldCellData* pd;

  if (parent == nullptr) {
    parent = _pRoot;
  }

  //*If this hits then you haven't expanded the grid before saving.
  AssertOrThrow2(parent != nullptr);

  //**Note this function must follow EXCACTLY the makeNodes function
  if (parent->getIsLeaf()) {
    for (int ci = 0; ci < BlockNode::c_nCells; ++ci) {
      if (parent->getCells() != nullptr && parent->getCell(ci) != nullptr) {
        pd = parent->getCell(ci)->serialize();
      }
      else {
        pd = WorldCell::serializeEmpty();
      }
      pFile->getCells().push_back(pd);
    }
  }
  else {
    if (parent->getChildren() != nullptr) {
      //Validate grid integrity.
      for (size_t ni = 0; ni < parent->getChildren()->size(); ++ni) {
        //**All cells must be created for saving to work.
        //Because we load the save file in order.  To generate all cells call expand()
        AssertOrThrow2(parent->getChildren()->at(ni));
        getCellData(parent->getChildren()->at(ni), pFile);
      }
    }
    else {
      //The grid should be expanded before saving.  All nodes must have children, howver ty don't neave to have cells.
      BRThrowException("Grid was not expanded before saving.");
    }
  }
}
void WorldGrid::consolidate() {
  // return;

   //New fn. 
   //Remove empty nodes.
  bool bEmpty = true;
  _nPruned = 0;
  _nNodes = 0;
  _nCells = 0;
  _pRoot->consolidate(bEmpty, _nCells, _nPruned, _nNodes);
  //If bEmpty is true we logically could delete the root, however
  //We are using the root node f or it's bound box, so don't.
}
void WorldGrid::expand() {
  //Node = BVH
  //Cell =  Tile
  int iCell = 0;

  //BRLogDebugBeg("Dividing ", _viPos.toString(), " ..");

  t_timeval tv = Gu::getMicroSeconds();
  {
    _nNodes = 0;
    _nLeaves = 0;
    _nPruned = 0;
    makeRootNode();

    _pRoot->divide(tv, getThis<WorldGrid>(), _nNodes, _nLeaves, 0, nullptr, nullptr, true);

    if (_nLeaves != BottleUtils::getNumCellsPerNode()) {
      int nnn = 0;
      nnn++;
    }
    //expandV(_pRoot);
  }
  _tvDivide = (Gu::getMicroSeconds() - tv) / 1000;
  //BRLogDebugEnd("..Done ", (int)(), "ms");
}
void WorldGrid::makeRootNode() {
  if (_pRoot == nullptr) {
    _pRoot = new BlockNode(getThis<WorldGrid>());
    //_pRoot->setNodeType(NodeType::e::Vertical);
    _nNodes++;

    _pRoot->getBox()->_min = getBoundBox()->_min;
    _pRoot->getBox()->_max = getBoundBox()->_max;
    _pRoot->getBox()->validateBoundBox();
  }
}
void WorldGrid::createAllCells(WorldCellFile* pFile, World25GridGen::e eGen, WipGrid* pDat) {
  //*Note: we must already be expanded with expand()
  AssertOrThrow2(_pRoot != nullptr);
  //Creates and recreates cells
  //Generate + Fill
  int32_t iCell = 0;
  for (int iM = 0; iM < GridMeshLayer::e::MaxMatters; ++iM) {
    _nVertexCount[iM] = 0;
  }
  _nCells = 0;
  _pRoot->createCells(pFile, iCell, eGen, pDat, getThis<WorldGrid>(), _nVertexCount, _nCells);


  //We were allocating HUGE vectors for solid volumes because
  // 48 * 4096 = 190000,  We usually only have 5-6k vertexes, 
  // So this will limit it.
  for (int iM = 0; iM < GridMeshLayer::e::MaxMatters; ++iM) {
    if (_nVertexCount[iM] > 7000) {
      _nVertexCount[iM] = 7000;
    }
    _pMeshes[iM]->setVertexPreallocCount(_nVertexCount[iM]);
  }

  int nnn = 0;
  nnn++;
}
void WorldGrid::fillObjects() {
  //Once we've created the grid, next fill hte objects and do any
  //post-initialization with cells.
  //Post-init the cells
  //std::map<WorldCellData*, WorldCell*>::iterator ite = _mapPostInitCells.begin();
  //for (; ite != _mapPostInitCells.end(); ite++) {
  //    WorldCellData* pData = ite->first;
  //    WorldCell* pCell = ite->second;
  //    Tile25Spec* pSolid = nullptr;
  //    Tile25Spec* pLiquid = nullptr;

  //    if (pData != nullptr) {
  //        pSolid = getWorld25()->getMotionBucket()->getTileById(pData->_iBaseTileIdx);
  //        pLiquid = getWorld25()->getMotionBucket()->getTileById(pData->_iLiquidTileIdx);
  //    }

  //    if(pSolid == nullptr){
  //        //We couldn't find the texture.  You probably renamed it or deleted
  //        //it from sprites.dat
  //        pSolid = getWorld25()->getMotionBucket()->getDefaultTile(MatterMode::e::Solid);
  //        BRLogError("Couldn't find solid base tile, Index:", pData->_iBaseTileIdx);
  //        Gu::debugBreak();
  //    }
  //    if(pLiquid == nullptr){
  //        //We couldn't find the texture.  You probably renamed it or deleted
  //        //it from sprites.dat
  //        pLiquid = getWorld25()->getMotionBucket()->getDefaultTile(MatterMode::e::Liquid);
  //        BRLogError("Couldn't find liquid base tile, Index:", pData->_iLiquidTileIdx);
  //            Gu::debugBreak();
  //    }

  //    pCell->setTile(MatterMode::e::Solid, pSolid);
  //    pCell->setTile(MatterMode::e::Liquid, pLiquid);
  //}
  //Add objects.

//    _mapPostInitCells.clear();
}
WorldCell* WorldGrid::getCellForIndex(ivec3& idx) {
  vec3 vp = getOriginR3();
  vp.x += (float)idx.x * BottleUtils::getCellWidth() + BottleUtils::getCellWidth() * 0.5f;
  vp.y += (float)idx.y * BottleUtils::getCellHeight() + BottleUtils::getCellHeight() * 0.5f;
  vp.z += (float)idx.z * BottleUtils::getCellWidth() + BottleUtils::getCellWidth() * 0.5f;

  WorldCell* pc = getCellForPoint(vp);

  if (pc != nullptr) {
    //Validate that this works.
    AssertOrThrow2(pc->getLocalPos() == idx);
  }

  return pc;
}
void WorldGrid::drawOpaque(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn) {
  if (!getGenerated()) {
    return;
  }
  _pMeshes[GridMeshLayer::e::Opaque]->draw(rp, dbgNumTrisDrawn);
}
void WorldGrid::drawTransparent(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn) {
  if (!getGenerated()) {
    return;
  }
  _pMeshes[GridMeshLayer::e::Transparent]->draw(rp, dbgNumTrisDrawn);
}
void WorldGrid::drawGrid(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn) {
  if (!getGenerated()) {
    return;
  }

  std::shared_ptr<FrustumBase> pf = Gu::getCamera()->getFrustum();
  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    if (_pMeshes[iMatter] != nullptr) {
      if (_pMeshes[iMatter]->getMesh() != nullptr) {


        //**TODO: Toggle show grid based on whether we are picking opaque / transparent polygons.


        //This MUST match or we'll get jacked up results.
        uint32_t pick = _pMeshes[iMatter]->getMesh()->getPickId();
        rp.getShader()->setUf("_ufPickId", (void*)& pick);
        rp.setMesh(_pMeshes[iMatter]->getMesh());
        rp.draw();
      }
    }
  }

}
std::shared_ptr<World25Plane> WorldGrid::getWorld25Plane() {
  return _pWorld25->getWorld25Plane();
}
ivec3 WorldGrid::getCellIdForPoint(vec3& pt) {
  vec3 v = pt - getOriginR3();
  ivec3 cid = getWorld25()->v3Toi3CellLocal(v);
  return cid;
}
WorldCell* WorldGrid::getCellForPoint(vec3& pt) {
  //NOTE** see getCellForPoint
  WorldCell* pc = nullptr;
  AssertOrThrow2(_pRoot != nullptr);
  _pRoot->getCellForPoint(pt, pc);
  return pc;
}
void WorldGrid::settleLiquids() {
  if (!getGenerated()) {
    return;
  }
  if (_vecLiquidMod.size() > 0) {
    std::set<WorldCell*> vdCopy = _vecLiquidMod;
    _vecLiquidMod.clear();

    //for (QuadLut::iterator it = _quadLut.begin(); it != _quadLut.end(); it++) {
    for (WorldCell* pc : vdCopy) {
      W25Geom liq = pc->getGeom(GridMeshLayer::e::Transparent);

      if (liq != W25GEOM_EMPTY) //Add && _bLiquidStable here to avoid extraneous updates.
      {
        W25Geom liqNewCell = liq;
        //Check a corner.
        //v 2 3 6 7 - check for below
        spreadLiquid(2, pc, 0, pc, liqNewCell);
        spreadLiquid(3, pc, 1, pc, liqNewCell);
        spreadLiquid(6, pc, 4, pc, liqNewCell);
        spreadLiquid(7, pc, 5, pc, liqNewCell);
        if (pc->getGeom(GridMeshLayer::e::Transparent) != liqNewCell) {
          _vecLiquidMod.insert(pc);
          pc->setGeom(GridMeshLayer::e::Transparent, liqNewCell);
          _pMeshes[GridMeshLayer::e::Transparent]->updateRedoMeshForCell(pc, true);
        }

        settleLiquidsNeighbor(pc, PhysicsGridSide::e::gB);
        settleLiquidsNeighbor(pc, PhysicsGridSide::e::gL);
        settleLiquidsNeighbor(pc, PhysicsGridSide::e::gR);
        settleLiquidsNeighbor(pc, PhysicsGridSide::e::gA);
        settleLiquidsNeighbor(pc, PhysicsGridSide::e::gF);

      }
    }
  }
}
void WorldGrid::settleLiquidsNeighbor(WorldCell* pc, PhysicsGridSide::e eSide) {
  WorldCell* pn = pc->getNeighbor(eSide);
  if (pn != nullptr) {
    if (pn->getGrid() == getThis<WorldGrid>()) {  //Prevent updateRedoMeshForCell from getting non-child nodes
      W25Geom liqNewBot = pn->getGeom(GridMeshLayer::e::Transparent);
      //pn->getTile();
      for (int i = 0; i < 4; ++i) {
        int v8idx = WorldCell::getV8IndexForV4Side(eSide, i);
        int v8opp = WorldCell::getV8IndexForV4SideOpp(eSide, i);

        spreadLiquid(v8idx, pc, v8opp, pn, liqNewBot);
      }

      if (pn->getGeom(GridMeshLayer::e::Transparent) != liqNewBot) {
        _vecLiquidMod.insert(pn);
        pn->setGeom(GridMeshLayer::e::Transparent, liqNewBot);
        _pMeshes[GridMeshLayer::e::Transparent]->updateRedoMeshForCell(pc, true);
      }
    }
  }
}
void WorldGrid::spreadLiquid(int ic, WorldCell* pc, int ni, WorldCell* pn, W25Geom& __out_ val) {
  AssertOrThrow2(ic >= 0 && ic < 8 && ni >= 0 && ni < 8);
  if (w25_get(pc->getGeom(GridMeshLayer::e::Transparent), ic) &&
    !w25_get(pn->getGeom(GridMeshLayer::e::Transparent), ni) &&
    !w25_get(pn->getGeom(GridMeshLayer::e::Opaque), ni)) {
    w25_set(val, ni);
    if (pc->getTile(GridMeshLayer::e::Transparent) != W25TILE_EMPTY) {
      pn->setTile(GridMeshLayer::e::Transparent, pc->getTile(GridMeshLayer::e::Transparent));
    }
  }

}
void WorldGrid::getLocalCellRangeForBox(Box3f& box, ivec3& p0, ivec3& p1) {
  vec3 origin = this->getOriginR3();

  float relax = 0.005f;

  vec3 p0_local = (box._min - origin) + relax;
  vec3 p1_local = (box._max - origin) - relax;

  p0 = getWorld25()->v3Toi3CellLocal(p0_local);
  p1 = getWorld25()->v3Toi3CellLocal(p1_local);

  int ciMaxCellsXYZ = (int)BottleUtils::getNumCellsWidth(); // 1 bil

  //**So invalid intervals are bound to happen here because the input bound box is 
  //likely going to be outside the range of this node.
  //To fix this we adjust the intervals here.
  if (p0.x < 0) p0.x = 0;
  if (p0.y < 0) p0.y = 0;
  if (p0.z < 0) p0.z = 0;
  if (p0.x >= ciMaxCellsXYZ) p0.x = ciMaxCellsXYZ - 1;
  if (p0.y >= ciMaxCellsXYZ) p0.y = ciMaxCellsXYZ - 1;
  if (p0.z >= ciMaxCellsXYZ) p0.z = ciMaxCellsXYZ - 1;

  if (p1.x < 0) p1.x = 0;
  if (p1.y < 0) p1.y = 0;
  if (p1.z < 0) p1.z = 0;
  if (p1.x >= ciMaxCellsXYZ) p1.x = ciMaxCellsXYZ - 1;
  if (p1.y >= ciMaxCellsXYZ) p1.y = ciMaxCellsXYZ - 1;
  if (p1.z >= ciMaxCellsXYZ) p1.z = ciMaxCellsXYZ - 1;

  //Make sure it's a valid interval
  if (p1.x < p0.x) p1.x = p0.x;
  if (p1.y < p0.y) p1.y = p0.y;
  if (p1.z < p0.y) p1.z = p0.z;
}
void WorldGrid::deleteMeshes() {
  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    _pMeshes[iMatter] = nullptr;
  }
}
//bool WorldGrid::handleGridPhysics(std::shared_ptr<PhysicsNode> ob, GridPhysicsStep::e eStep, std::multimap<float, BoxCollision>* vecBoxCollisions) {
//    bool bCollided = false;
//    if (!getGenerated()) {
//        return bCollided;
//    }
//
//    handleGridPhysics_r(_pRoot, ob,  eStep, bCollided, vecBoxCollisions);
//    return bCollided ;
//}
//void WorldGrid::handleGridPhysics_r(W25BlockNode* parent, std::shared_ptr<PhysicsNode> ob, GridPhysicsStep::e eStep,
//    bool&  bCollided, std::multimap<float, BoxCollision>* vecBoxCollisions) {
//    /*
//    //Index data is present at each gn2 because of "propogate fn"
//    if(parent->getIndexCount(MatterMode::e::Solid) > 0) {
//        if (parent->getBox()->cubeIntersect_EasyOut_Inclusive(ob->getBox())) {
//            if (parent->getCell() != nullptr) {
//                if(parent->getCell()->getVisibleCell() != nullptr) {
//                    if(parent->getCell()->getVisibleCell()->getMeshIndexCount(MatterMode::e::Solid) > 0) {
//                        if(eStep == GridPhysicsStep::e::Unstick) {
//                            _pWorld25->unstick_objs(ob, nullptr, ob->getBox(), parent->getCell()->getBoundBoxBvh());
//                        }
//                        else if (eStep == GridPhysicsStep::e::Collide) {
//                            AssertOrThrow2(vecBoxCollisions);
//                            int iLevel = parent->getCell()->getLevel();
//                          //  if(iLevel < (int)_vecMeshes[MatterMode::e::Solid].size()) {
//                                int iOff = parent->getCell()->getVisibleCell()->getMeshIndexOffset(MatterMode::e::Solid);
//                                int iCount = parent->getCell()->getVisibleCell()->getMeshIndexCount(MatterMode::e::Solid);
//
//                                std::shared_ptr<W25GridMesh> pMeshLevel = _pMeshes[MatterMode::e::Solid];
//                                if(_pWorld25->collideMesh(ob,
//                                    &pMeshLevel->getVerts(),
//                                    &pMeshLevel->getIndexes(),
//                                    iOff,
//                                    iCount))
//                                {
//                                    bCollided = true;
//                                }
//                         //   }
//                        }
//                    }
//                }
//            }
//            else { //if not has cell
//                if (parent->getChildren() != nullptr) {
//                    for (size_t i = 0; i<parent->getChildren()->size(); ++i) {
//                        handleGridPhysics_r(parent->getChildren()->at(i), ob, eStep, bCollided, vecBoxCollisions);
//                    }
//                }
//            }
//        }
//    }
//    */
//}
void WorldGrid::getObjData(WorldCellFile* pFile) {
  //Fill the file with the obj data.

  //std::vector<std::shared_ptr<ModelNode>> obList;
  //for () {
  //    //This should allow for all ponits to be included only in single cells.
  //    std::shared_ptr<ModelNode> ob2 = std::dynamic_pointer_cast<ModelNode>(ob);
  //    if(ob2 != nullptr){
  //        if (getBoundBox()->containsBottomLeftInclusive(ob->getBoundBoxObject()->center())) {
  //            obList.push_back(ob2);
  //        }
  //    }
  //}

  pFile->getObjects().clear();

  World25ObjectData* obd = nullptr;
  for (std::shared_ptr<PhysicsNode> ob : *(getManifold()->getAll())) {
    obd = new World25ObjectData();
    if (ob != nullptr && ob->getSpec() != nullptr && ob->getSpec()->getNameHashed() > 0) {
      obd->_iType = ob->getSpec()->getNameHashed();
      obd->_vPos = ob->getPos();
    }
    else {
      //Cannot be zero, but store the object so we can tell it's an error
      BRLogError("Error, object was null or not found while saving glob");
      Gu::debugBreak();
      obd->_iType = 0;
      obd->_vPos = vec3(0, 0, 0);
    }

    //We must push back an invalid ID to keep the data in sync
    pFile->getObjects().push_back(obd);
  }


}
void WorldGrid::raycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& outCells) {
  if (!getGenerated()) {
    return;
  }
  _pRoot->raycastCells(pr, outCells);
}
void WorldGrid::markAllCellsDirty() {
  iterateCells([this](WorldCell* pc) {
    _vecLiquidMod.insert(pc);
    });
}
void WorldGrid::markCellDirty(WorldCell* pc) {
  _vecLiquidMod.insert(pc);
}
void WorldGrid::beginGenAsync(std::function<void()> func) {
  std::shared_ptr<WorldGrid> that = getThis<WorldGrid>();

  _genFuture = std::async(std::launch::async, [that, func] {
    try {
      func();
    }
    catch (Game::Exception* ex) {
      //Thread was killed
      if (that->getKilled()) {
        BRLogInfo("Grid generation cancelled.");
      }
      else {
        BRLogInfo("During Grid generation, There was an error somewhere.");
      }

    }
    return true;
    });
}
void WorldGrid::generateStage1() {
  std::shared_ptr<WorldGrid> that = getThis<WorldGrid>();
  _eGenState = GridGenState::e::Stage1;

  beginGenAsync([that]() {
    that->generateStage1Sync();
    });
}
void WorldGrid::generateStage2() {
  std::shared_ptr<WorldGrid> that = getThis<WorldGrid>();
  _eGenState = GridGenState::e::Stage2;

  beginGenAsync([that]() {
    that->generateStage2Sync();
    });
}
void WorldGrid::killGen() {
  _bKilled = true;
}
bool WorldGrid::getKilled() {
  return _bKilled;
}
void WorldGrid::checkKilled() {
  //Throw an exception in the generating thread to stop generating.
  if (_bKilled && getIsGenerating()) {
    //*** THIS IS NOT AN ERROR**
    //*** THIS IS NOT AN ERROR**
    throw new Exception("Grid generation was cancelled.", __LINE__, __FILE__, false);
    //*** THIS IS NOT AN ERROR**
    //*** THIS IS NOT AN ERROR**
  }
}
bool WorldGrid::getIsGenerating() {

  if (_genFuture.valid()) {
    std::future_status status = _genFuture.wait_for(std::chrono::milliseconds(0));

    if (status == std::future_status::ready) {
      return false;
    }
    else {
      return true;
    }
  }
  else {
    return false;
  }
}
void WorldGrid::generateStage1Sync() {
  // initSync();//This is already called, but whatever, it's cheap
  // checkKilled();
  //
  linkGrids();
  expand();
  // checkKilled();

  //loadFromDisk();
  //checkKilled();

  linkCellsInternal();
#ifdef _DEBUG
  debugVerifyAllInternalCellsLinked();
#endif
  checkKilled();

  consolidate();//Bug somewhere here.
  checkKilled();

  markAllCellsDirty();
  checkKilled();

  fillObjects(); //Must come before make mesh.
  checkKilled();
}
void WorldGrid::postGenerateStage1() {
  t_timeval tv = Gu::getMicroSeconds();
  {
    stitchNodes(false);
    _eGenState = GridGenState::e::Stage1Complete;
  }
  _tvPostGen2 = (Gu::getMicroSeconds() - tv) / 1000;
}
void WorldGrid::generateStage2Sync() {
  //Only called after all 6 neighbors of this grid have been through stage 1
  //This will make it so that we don't have to call "retopo" for all the sides.
  makeMesh(true);
}
void WorldGrid::postGenerateStage2() {
  t_timeval tv = Gu::getMicroSeconds();
  {
    //This must be done synchronously.

    //Removing this in favor of 2-stage generation
    // redoNeighborConfigs();

    //Finish generating synchronous opengl stuff (render thread)
    for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
      Gu::checkErrorsRt();//_pWorld25->getContext()->chkErrRt();
      if (_pMeshes[iMatter] != nullptr) {
        _pMeshes[iMatter]->sendMeshToGpu();
      }
    }

    _eGenState = GridGenState::e::Generated;
  }
  _tvPostGen1 = (Gu::getMicroSeconds() - tv) / 1000;
}
void WorldGrid::debugVerifyAllInternalCellsLinked(BlockNode* parent) {
  if (isEmpty() == true) {
    return;
  }
  iterateCells([](WorldCell* pc) {
    ivec3 lpos = pc->getLocalPos();

    for (int ni = 0; ni < WorldCell::c_nNeighbors; ++ni) {
      WorldCell* pn = pc->getNeighbor((PhysicsGridSide::e)ni);
      if (pn == nullptr) {
        ivec3 iRPos;
        WorldCell::getRelativeNeighborPos((PhysicsGridSide::e)ni, iRPos);
        iRPos += pc->getLocalPos();
        if (WorldCell::isValidCellPos(iRPos)) {
          if (!WorldCell::isBorderCellPos(iRPos)) {
            //Neighbor is internal, but it's null.  All internals must be linked.
            //Assuming we didn't call consolidate before this
            Gu::debugBreak();
          }
        }
      }

    }
    });

}
void WorldGrid::redoNeighborConfigs() {

  for (int iNeighbor = 0; iNeighbor < c_nNeighbors; ++iNeighbor) {
    std::shared_ptr<WorldGrid> gn = std::dynamic_pointer_cast<WorldGrid>(getNeighbor(iNeighbor));
    //Both this grid and the other must redo their sides.
    if (gn != nullptr) {
      if (gn->getGenState() == GridGenState::e::Generated) {
        redoSide((PhysicsGridSide::e)iNeighbor);
        gn->redoSide(PhysicsGrid::getOppNeighborIndex((PhysicsGridSide::e)iNeighbor), nullptr);
      }
    }
  }
}
void WorldGrid::redoSide(PhysicsGridSide::e eSide, BlockNode* parent) {

  iterateCells([this, eSide](WorldCell* pc) {
    if (pc->isBorderSide(eSide)) {
      //We shouldn't have to redo the neighbor cells, since we're only culling the given side.
      updateRedoMeshForCell(pc, false);
    }
    });
}
void WorldGrid::deflateCellIndexes(GridMeshLayer::e eMatter, int32_t iOldOff, int32_t iOldCount, WorldCell* pRemoved, BlockNode* parent) {
  //Updates the mesh indexes for the cells, called when we edit the grid (remove a cell).
  iterateCells([eMatter, iOldOff, iOldCount, pRemoved](WorldCell* pc) {
    if (pc->getVisibleCell()) {
      if (pc == pRemoved) {
        pc->deleteVisibleCell();
      }
      else {
        int32_t noff = pc->getVisibleCell()->getMeshIndexOffset(eMatter);
        if (noff > iOldOff) {
          int32_t new_off = noff - iOldCount;
          if (new_off < 0) {
            Gu::debugBreak();
            new_off = 0;    //**Error
          }
          pc->getVisibleCell()->setMeshIndexOffset(eMatter, new_off);
        }
      }
    }
    });
}
void WorldGrid::iterateCells(std::function<void(WorldCell*)> func, BlockNode* parent) {
  //Iterates all NON-NULL cells.

  //TODO: we should put a ==Generated check here, and disable this for self-grids to make sure
  //we don't try to access cells during generation.
  if (parent == nullptr) {
    if (_pRoot == nullptr) {
      //Consolidated.
      return;
    }
    parent = _pRoot;
  }

  if (parent->getIsLeaf()) {
    for (int ci = 0; ci < BlockNode::c_nCells; ++ci) {
      if (parent->getCells() != nullptr) {
        WorldCell* pc = parent->getCell(ci);
        if (pc != nullptr) {
          func(pc);
        }
      }
    }
  }
  else {
    if (parent->getChildren() != nullptr) {
      for (size_t ni = 0; ni < parent->getChildren()->size(); ++ni) {
        BlockNode* pcn = parent->getChildren()->at(ni);

        if (pcn != nullptr) {
          iterateCells(func, pcn);
        }
      }
    }
  }
}
size_t WorldGrid::getSizeKb() {
  return getNodeSizeKb() + getMeshSizeKb() + getCellSizeKb();
}
size_t WorldGrid::getNodeSizeKb() {
  int iNodeSizeKb = 0;
  int nodeSize = (int)sizeof(BlockNode);
  int gridSize = (int)sizeof(WorldGrid);
  iNodeSizeKb += _nNodes * nodeSize;
  iNodeSizeKb += _nCells * sizeof(WorldCell*); //_setCells.size() ;
  iNodeSizeKb += gridSize;
  iNodeSizeKb /= 1024;
  return iNodeSizeKb;
}
size_t WorldGrid::getMeshSizeKb() {
  int iMeshSizeKb = 0;
  int gridSize = (int)sizeof(WorldGrid);
  for (int i = 0; i < GridMeshLayer::e::MaxMatters; ++i) {
    if (_pMeshes[i] != nullptr) {
      iMeshSizeKb += _pMeshes[i]->getVerts().size() * sizeof(W25MeshVert);
      iMeshSizeKb += _pMeshes[i]->getIndexes().size() * sizeof(v_index32);
    }
  }
  iMeshSizeKb /= 1024;

  return iMeshSizeKb;
}
size_t WorldGrid::getCellSizeKb() {
  int iCellSizeKb = 0;
  int cellSize = (int)sizeof(WorldCell);
  iCellSizeKb = _nCells * cellSize;
  iCellSizeKb /= 1024;

  return iCellSizeKb;
}
string_t WorldGrid::getGenProfileString() {
  int iSizeKb;
  int iMeshSizeKb = getMeshSizeKb();
  int iNodeSizeKb = getNodeSizeKb();
  int iCellSizeKb = getCellSizeKb();

  t_timeval iTimeMs;
  int32_t nVerts = 0, nIndexes = 0;
  int cellSize = (int)sizeof(WorldCell);
  int nodeSize = (int)sizeof(BlockNode);
  int gridSize = (int)sizeof(WorldGrid);

  for (int i = 0; i < GridMeshLayer::e::MaxMatters; ++i) {
    if (_pMeshes[i] != nullptr) {
      nVerts += (int32_t)_pMeshes[i]->getVerts().size();
      nIndexes += (int32_t)_pMeshes[i]->getIndexes().size();
    }
  }

  iSizeKb = iNodeSizeKb + iMeshSizeKb + iCellSizeKb;

  iTimeMs = _tvDivide
    + _tvSave
    + _tvLoad
    + _tvLink
    + _tvMesh
    + _tvProp
    + _tvPostGen1
    + _tvPostGen2;

  return Stz getGridPos().toString() + "  " + iSizeKb + "kb (" + iNodeSizeKb + " node+ " + iCellSizeKb + " cell+ " + iMeshSizeKb + " mesh) " + iTimeMs + "ms \r\n " +
    "#CellSiz " + cellSize + "+ " +
    "#NodeSiz " + nodeSize + "+ " +
    "#GridSiz " + gridSize + "+ " +
    "\r\n" +
    "#Verts " + nVerts + "+ " +
    "#Indexes " + nIndexes + "(" + nIndexes / 3 + " tris)+ " +
    "\r\n" +
    "#Cells " + (int)_nCells + "+ " +
    "#Nodes " + (int)_nNodes + "+ " +
    "#Pruned " + (int)_nPruned + "+ " +
    "\r\n" +
    "   Div " + (int)_tvDivide + "ms+ " +
    "Save " + (int)_tvSave + "ms+ " +
    "Load " + (int)_tvLoad + "ms+ " +
    "Link " + (int)_tvLink + "ms+ " +
    "\r\n" +
    "   Mesh " + (int)_tvMesh + "ms+ " +
    "Prop " + (int)_tvProp + "ms+ " +
    "Post1 " + (int)_tvPostGen1 + "ms " +
    "Post2 " + (int)_tvPostGen2 + "ms " +
    "\n";
}
void WorldGrid::cleanEmptyNodes() {
  //TODO:
}
void WorldGrid::cleanEmptyNodes_r(BlockNode* pParent) {
  //TODO:
}
//void WorldGrid::unloadObjects() {
//    if (getIsGenerating()) {
//        return;
//    }
//    t_timeval tv = Gu::getMicroSeconds();
//
//    std::vector<std::shared_ptr<ModelNode>> obs;
//
//    //Make a copy of the manifold objects (because it's going to get modified)
//    for (std::shared_ptr<ModelNode> ob : *getManifold()->getAll()) {
//        obs.push_back(ob);
//    }
//
//    for (std::shared_ptr<ModelNode> ob : obs) {
//        //This may remove the object, which is good.
//        _pWorld25->refreshObjectManifold(ob);
//    }
//
//    if (obs.size() > 0) {
//        BRLogInfo("   Deleted ", obs.size(), " objects, ",(int) (Gu::getMicroSeconds() - tv) / 1000, "ms");
//    }
//
//}
void WorldGrid::unlinkCellsAndGridsSync() {
  //Do not call "stitchnodes" when linking It must be synchroous
  //However we can call it when unlinking because that's async.
  unlinkGrids();
  stitchNodes(true);
}
void WorldGrid::linkCellsInternal() {
  t_timeval tv = Gu::getMicroSeconds();
  {
    //***ASYNC
    //the unlink for cells will unlink all the center cells.
    //This isn't needed... we can just unlink the outer cells (stitchnodes).
    linkCellsInternal(false);
  }
  _tvLink = (Gu::getMicroSeconds() - tv) / 1000;
}
//Call this on the root node.
void WorldGrid::linkCellsInternal(bool bUnlink) {
  //L to R
  stitchBoundariesX_r(_pRoot->getChild(BlockPos8::e::NBP3_ABL), _pRoot->getChild(BlockPos8::e::NBP3_ABR), bUnlink, false);
  stitchBoundariesX_r(_pRoot->getChild(BlockPos8::e::NBP3_ATL), _pRoot->getChild(BlockPos8::e::NBP3_ATR), bUnlink, false);
  stitchBoundariesX_r(_pRoot->getChild(BlockPos8::e::NBP3_FBL), _pRoot->getChild(BlockPos8::e::NBP3_FBR), bUnlink, false);	// - Link order same as in Phase 1
  stitchBoundariesX_r(_pRoot->getChild(BlockPos8::e::NBP3_FTL), _pRoot->getChild(BlockPos8::e::NBP3_FTR), bUnlink, false);
  //B to T
  stitchBoundariesY_r(_pRoot->getChild(BlockPos8::e::NBP3_ABL), _pRoot->getChild(BlockPos8::e::NBP3_ATL), bUnlink, false);
  stitchBoundariesY_r(_pRoot->getChild(BlockPos8::e::NBP3_ABR), _pRoot->getChild(BlockPos8::e::NBP3_ATR), bUnlink, false);
  stitchBoundariesY_r(_pRoot->getChild(BlockPos8::e::NBP3_FBL), _pRoot->getChild(BlockPos8::e::NBP3_FTL), bUnlink, false);
  stitchBoundariesY_r(_pRoot->getChild(BlockPos8::e::NBP3_FBR), _pRoot->getChild(BlockPos8::e::NBP3_FTR), bUnlink, false);
  //A to F
  stitchBoundariesZ_r(_pRoot->getChild(BlockPos8::e::NBP3_ABL), _pRoot->getChild(BlockPos8::e::NBP3_FBL), bUnlink, false);
  stitchBoundariesZ_r(_pRoot->getChild(BlockPos8::e::NBP3_ABR), _pRoot->getChild(BlockPos8::e::NBP3_FBR), bUnlink, false);
  stitchBoundariesZ_r(_pRoot->getChild(BlockPos8::e::NBP3_ATL), _pRoot->getChild(BlockPos8::e::NBP3_FTL), bUnlink, false);
  stitchBoundariesZ_r(_pRoot->getChild(BlockPos8::e::NBP3_ATR), _pRoot->getChild(BlockPos8::e::NBP3_FTR), bUnlink, false);
}
BlockNode* WorldGrid::rootNeighbor(PhysicsGridSide::e n) {
  //
  //Stitches between GLOB nodes
  //
  std::shared_ptr<WorldGrid> g = std::dynamic_pointer_cast<WorldGrid>(getNeighbor((int)n));
  if (g != nullptr) {
    if (g->getGenState() >= GridGenState::e::Stage1Complete) {
      return g->getRoot();
    }
  }
  return nullptr;
}
void WorldGrid::stitchNodes(bool bUnlink) {
  if (bUnlink && _pRoot == nullptr) {
    return;
  }
  AssertOrThrow2(_pRoot != nullptr);
  // - Don't call this method in the async loop.  This modifies other grids' data.

  // - Stitch outside the node on the grid graph.
  // stitch the three positive with this node as giver
  if (rootNeighbor(PhysicsGridSide::e::gR)) {
    stitchNodesX(_pRoot, rootNeighbor(PhysicsGridSide::e::gR), bUnlink);
  }
  if (rootNeighbor(PhysicsGridSide::e::gT)) {
    stitchNodesY(_pRoot, rootNeighbor(PhysicsGridSide::e::gT), bUnlink);
  }
  if (rootNeighbor(PhysicsGridSide::e::gF)) {
    stitchNodesZ(_pRoot, rootNeighbor(PhysicsGridSide::e::gF), bUnlink);
  }
  // stitch the three negative neighbors with this node as taker
  if (rootNeighbor(PhysicsGridSide::e::gL)) {
    stitchNodesX(rootNeighbor(PhysicsGridSide::e::gL), _pRoot, bUnlink);
  }
  if (rootNeighbor(PhysicsGridSide::e::gB)) {
    stitchNodesY(rootNeighbor(PhysicsGridSide::e::gB), _pRoot, bUnlink);
  }
  if (rootNeighbor(PhysicsGridSide::e::gA)) {
    stitchNodesZ(rootNeighbor(PhysicsGridSide::e::gA), _pRoot, bUnlink);
  }
}
void WorldGrid::stitchNodesX(BlockNode* giver, BlockNode* taker, bool bUnlink) {
  if (bUnlink && (giver == nullptr || taker == nullptr)) {
    return;
  }
  AssertOrThrow2(giver != nullptr);
  AssertOrThrow2(taker != nullptr);
  stitchBoundariesX_r(giver->getChild(BlockPos8::e::NBP3_ABR), taker->getChild(BlockPos8::e::NBP3_ABL), bUnlink, true);
  stitchBoundariesX_r(giver->getChild(BlockPos8::e::NBP3_ATR), taker->getChild(BlockPos8::e::NBP3_ATL), bUnlink, true);
  stitchBoundariesX_r(giver->getChild(BlockPos8::e::NBP3_FBR), taker->getChild(BlockPos8::e::NBP3_FBL), bUnlink, true);
  stitchBoundariesX_r(giver->getChild(BlockPos8::e::NBP3_FTR), taker->getChild(BlockPos8::e::NBP3_FTL), bUnlink, true);
}
void WorldGrid::stitchNodesY(BlockNode* giver, BlockNode* taker, bool bUnlink) {
  if (bUnlink && (giver == nullptr || taker == nullptr)) {
    return;
  }
  AssertOrThrow2(giver != nullptr);
  AssertOrThrow2(taker != nullptr);
  stitchBoundariesY_r(giver->getChild(BlockPos8::e::NBP3_ATL), taker->getChild(BlockPos8::e::NBP3_ABL), bUnlink, true);
  stitchBoundariesY_r(giver->getChild(BlockPos8::e::NBP3_ATR), taker->getChild(BlockPos8::e::NBP3_ABR), bUnlink, true);
  stitchBoundariesY_r(giver->getChild(BlockPos8::e::NBP3_FTL), taker->getChild(BlockPos8::e::NBP3_FBL), bUnlink, true);
  stitchBoundariesY_r(giver->getChild(BlockPos8::e::NBP3_FTR), taker->getChild(BlockPos8::e::NBP3_FBR), bUnlink, true);
}
void WorldGrid::stitchNodesZ(BlockNode* giver, BlockNode* taker, bool bUnlink) {
  if (bUnlink && (giver == nullptr || taker == nullptr)) {
    return;
  }
  AssertOrThrow2(giver != nullptr);
  AssertOrThrow2(taker != nullptr);
  stitchBoundariesZ_r(giver->getChild(BlockPos8::e::NBP3_FTL), taker->getChild(BlockPos8::e::NBP3_ATL), bUnlink, true);
  stitchBoundariesZ_r(giver->getChild(BlockPos8::e::NBP3_FTR), taker->getChild(BlockPos8::e::NBP3_ATR), bUnlink, true);
  stitchBoundariesZ_r(giver->getChild(BlockPos8::e::NBP3_FBL), taker->getChild(BlockPos8::e::NBP3_ABL), bUnlink, true);
  stitchBoundariesZ_r(giver->getChild(BlockPos8::e::NBP3_FBR), taker->getChild(BlockPos8::e::NBP3_ABR), bUnlink, true);
}
void WorldGrid::stitchBoundariesX_r(BlockNode* giver, BlockNode* taker, bool bUnlink, bool bQueue) {
  BlockPos8::e othr[] = {
      BlockPos8::NBP3_ABR, BlockPos8::NBP3_ATR, BlockPos8::NBP3_FBR, BlockPos8::NBP3_FTR
  };
  stitchBoundaries_r(giver, taker, bUnlink, bQueue, 0, PhysicsGridSide::e::gR, othr);
}
void WorldGrid::stitchBoundariesY_r(BlockNode* giver, BlockNode* taker, bool bUnlink, bool bQueue) {
  BlockPos8::e othr[] = {
      BlockPos8::NBP3_ATL, BlockPos8::NBP3_ATR, BlockPos8::NBP3_FTL, BlockPos8::NBP3_FTR
  };
  stitchBoundaries_r(giver, taker, bUnlink, bQueue, 1, PhysicsGridSide::e::gT, othr);
}
void WorldGrid::stitchBoundariesZ_r(BlockNode* giver, BlockNode* taker, bool bUnlink, bool bQueue) {
  BlockPos8::e othr[] = {
      BlockPos8::NBP3_FBL, BlockPos8::NBP3_FBR, BlockPos8::NBP3_FTL, BlockPos8::NBP3_FTR
  };
  stitchBoundaries_r(giver, taker, bUnlink, bQueue, 2, PhysicsGridSide::e::gF, othr);
}
void WorldGrid::stitchBoundaries_r(BlockNode* giver, BlockNode* taker, bool bUnlink, bool bQueue,
  int ax, PhysicsGridSide::e gLink,
  BlockPos8::e(&othr)[4]
) {
  if (!giver || !taker) {
    return;
  }
  if (giver == taker) {
    Gu::debugBreak();
  }
  if (giver->getIsLeaf() && !taker->getIsLeaf() || taker->getIsLeaf() && !giver->getIsLeaf()) {
    //Error
    Gu::debugBreak();
  }

  //Get the opposite cell for the given axis. 0=x, 1=y 2=z
  BlockPos8::e othrOpp[4];

  for (int i = 0; i < 4; ++i) {
    othrOpp[i] = oppAx(othr[i], ax);
  }

  if (giver->getIsLeaf() && taker->getIsLeaf()) {
    if (isEmpty() == true) {
      //Don't link cells if empty.  There won't be any
    }
    else {
      if (giver->getCells() && taker->getCells()) {//New - thsi is for debugging, really all leaves should have cells
          // A->B
        for (int i = 0; i < 4; ++i) {
          linkBlocks(giver->getCell(othr[i]), taker->getCell(othrOpp[i]), gLink, bUnlink, bQueue);
        }
        //giver self
        for (int i = 0; i < 4; ++i) {
          linkBlocks(giver->getCell(othrOpp[i]), giver->getCell(othr[i]), gLink, bUnlink, bQueue);
        }
        //taker self
        for (int i = 0; i < 4; ++i) {
          linkBlocks(taker->getCell(othrOpp[i]), taker->getCell(othr[i]), gLink, bUnlink, bQueue);
        }
      }
    }
  }
  else {
    for (int i = 0; i < 4; ++i) {
      // A->B
      stitchBoundaries_r(giver->getChild(othr[i]), taker->getChild(othrOpp[i]), bUnlink, bQueue, ax, gLink, othr);
      //giver self
      stitchBoundaries_r(giver->getChild(othrOpp[i]), giver->getChild(othr[i]), bUnlink, bQueue, ax, gLink, othr);
      //taker self
      stitchBoundaries_r(taker->getChild(othrOpp[i]), taker->getChild(othr[i]), bUnlink, bQueue, ax, gLink, othr);
    }
  }
}
void WorldGrid::linkBlocks(WorldCell* b1, WorldCell* b2, PhysicsGridSide::e nrp, bool bUnlink, bool bQueue) {
  // - Link two block nodes.
  // The direction of the link is considering v2 is relative to v1. Meaning that
  // the "nrp" indexes into v1
  if (b1 == nullptr || b2 == nullptr) {
    return;
  }
  //  AssertOrThrow2(b1 != nullptr);
  //  AssertOrThrow2(b2 != nullptr);
  AssertOrThrow2(b1->getLocalPos() != b2->getLocalPos());
  if (bUnlink) {
    if (b1 != nullptr) {
      b1->setNeighbor(nrp, nullptr);
    }
    if (b2 != nullptr) {
      b2->setNeighbor(PhysicsGrid::getOppNeighborIndex(nrp), nullptr);
    }void redoSide(PhysicsGridSide::e eSide);
  }
  else {
    if (b1->getNeighbor(nrp) != nullptr) {
      if (b1->getNeighbor(nrp) != b2) {
        Gu::debugBreak();
        BRLogError("1 Tried to link block when block already had link.");
      }
    }
    else {
      b1->setNeighbor(nrp, b2);
    }
    PhysicsGridSide::e opp = PhysicsGrid::getOppNeighborIndex(nrp);
    if (b2->getNeighbor(opp) != nullptr) {
      if (b2->getNeighbor(PhysicsGrid::getOppNeighborIndex(nrp)) != b1) {
        Gu::debugBreak();
        BRLogError("2 Tried to link block when block already had link.");
      }
    }
    else {
      b2->setNeighbor(opp, b1);
    }
    debugBlocksAreSpatiallyCoherent(b1, b2);
  }
}
BlockPos8::e WorldGrid::oppAx(BlockPos8::e ePos, int ax) {
  if (ax == 0) return BottleUtils::oppBlockPosX(ePos);
  else if (ax == 1) return BottleUtils::oppBlockPosY(ePos);
  else if (ax == 2) return BottleUtils::oppBlockPosZ(ePos);
  BRThrowNotImplementedException();
}
void WorldGrid::debugBlocksAreSpatiallyCoherent(WorldCell* b1, WorldCell* b2) {
#ifdef _DEBUG
  //Validate X Z
  vec2 b1c = b1->getCenterR3().xz();
  vec2 b2c = b2->getCenterR3().xz();
  float dist2 = b1c.distance2(b2c);
  float epsilon = BottleUtils::getCellWidth() * 0.01f;
  float maxdist = (BottleUtils::getCellWidth() * BottleUtils::getCellWidth());

  //TODO: validate Y

  //This will fail if we change the blocks to not be cubes.  If it does, change maxdist above.
  AssertOrThrow2(dist2 <= (maxdist + epsilon));

#endif
}
void WorldGrid::makeMesh(bool bAsync) {
  t_timeval tv = Gu::getMicroSeconds();
  {
    for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
      _pMeshes[iMatter]->makeMeshImmediately(bAsync);
    }
  }
  _tvMesh = (Gu::getMicroSeconds() - tv) / 1000;
}
void WorldGrid::updateTopology() {


  //Update the grid topology without redoing "the whole thing"
  for (size_t i = 0; i < GridMeshLayer::e::MaxMatters; ++i) {
    _pMeshes[i]->updateTopology();
  }

}
void WorldGrid::updateRedoMeshForCell(WorldCell* pc, bool bNeighbors) {
  //All matters
  for (size_t i = 0; i < GridMeshLayer::e::MaxMatters; ++i) {
    updateRedoMeshForCell(pc, (GridMeshLayer::e)i, bNeighbors);
  }
}
void WorldGrid::updateRedoMeshForCell(WorldCell* pc, GridMeshLayer::e eMatter, bool bNeighbors) {
  AssertOrThrow2(pc->getGrid() == getThis<WorldGrid>());
  _pMeshes[eMatter]->updateRedoMeshForCell(pc, bNeighbors);
}
void WorldGrid::updateRedoMesh(GridMeshLayer::e eMatter) {
  _pMeshes[eMatter]->updateRedoMesh();
}
std::shared_ptr<MeshNode> WorldGrid::getMesh() {
  return _pMeshes[GridMeshLayer::e::Opaque]->getMesh();
}




}//ns Game





