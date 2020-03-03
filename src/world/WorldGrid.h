/**
*  @file World25Grid.h
*  @date March 2, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLD25GRID_14884757121318293260_H__
#define __WORLD25GRID_14884757121318293260_H__

#include "../world/PhysicsGrid.h"
#include "../world/WorldHeader.h"

//**TODO: move this file to its IMP_file and remove these headers.
#include <future>
#include <atomic>

namespace BR2 {
/**
*  @class World25Grid
*  @brief World25Grid owns Gn2 owns  WorldCell
*/
class WorldGrid : public PhysicsGrid {
public:
  WorldGrid(std::shared_ptr<World25> pworld, ivec3& viPos, bool bEmpty);
  virtual ~WorldGrid() override;

  void setQueued() { _eGenState = GridGenState::e::Queued; }
  bool getIsGenerating();
  size_t getSizeKb();
  ClimateInstance* getClimateInstance() { return _pClimateInstance; }
  void setClimateInstance(ClimateInstance* pi) { _pClimateInstance = pi; }
  string_t getGenProfileString();
  void killGen();
  bool getKilled();
  void checkKilled();
  LairSpec* getLairSpec() { return _pLairSpec; }
  void setLairSpec(LairSpec* i) { _pLairSpec = i; }
  int64_t getRaycastGrids2Stamp() { return _iRaycastGrids2Stamp; }
  void setRaycastGrids2Stamp(int64_t i) { _iRaycastGrids2Stamp = i; }
  void initSync();
  void drawOpaque(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn);
  void drawTransparent(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn);
  void drawGrid(RenderParams& rp, int32_t& __out_ dbgNumTrisDrawn);
  void settleLiquids();
  std::shared_ptr<PhysicsWorld> getWorld25() { return _pWorld25; }
  void getLocalCellRangeForBox(Box3f& box, ivec3& p0, ivec3& p1);
  WorldCell* getCellForIndex(ivec3& idx);
  std::shared_ptr<PhysicsNode> getGroundPhy() { return nullptr; }
  t_timeval getCollectStamp() { return _iCollectStamp; }
  void setCollectStamp(t_timeval x) { _iCollectStamp = x; }
  std::shared_ptr<World25Plane> getWorld25Plane();
  WorldCell* getCellForPoint(vec3& pt);
  BlockNode* getRoot() { return _pRoot; }
  bool getGenerated() { return getGenState() == GridGenState::e::Generated; }
  GridGenState::e getGenState() { return _eGenState; }
  void fillObjects();
  void unlinkCellsAndGridsSync();
  void linkCellsInternal();
  void raycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& outCells);
  void markAllCellsDirty();
  void markCellDirty(WorldCell* pc);
  void generateStage1();
  void generateStage2();
  void generateStage1Sync();
  void generateStage2Sync();
  void postGenerateStage1();
  void postGenerateStage2();
  void updateTopology();
  void updateRedoMeshForCell(WorldCell* pc, GridMeshLayer::e eMatter, bool bNeighbors);
  void updateRedoMesh(GridMeshLayer::e eMatter);
  void deflateCellIndexes(GridMeshLayer::e eMatter, int32_t iOldOff, int32_t iOldCount, WorldCell* pRemoved, BlockNode* parent = nullptr);
  void expand();
  void consolidate();
  void createAllCells(WorldCellFile* pFile, World25GridGen::e eGen, WipGrid* pDat);
  void getCellData(BlockNode* parent, WorldCellFile* __out cellData);
  void getObjData(WorldCellFile* pFile);
  virtual std::shared_ptr<MeshNode> getMesh() override;

private:
  std::set<WorldCell*> _vecLiquidMod; // Cells that need to be updated via liquid or some other.

  BlockNode* _pRoot = nullptr;
  std::shared_ptr<PhysicsWorld> _pWorld25 = nullptr;
  t_timeval _iCollectStamp;
  LairSpec* _pLairSpec = nullptr;
  std::shared_ptr<W25GridMesh> _pMeshes[GridMeshLayer::e::MaxMatters];//Transparent vs opaque
  size_t _nVertexCount[GridMeshLayer::e::MaxMatters]; //This is an optimization that doesn't work very well, it's confusing and could be removed.
  int64_t _iRaycastGrids2Stamp = -1;

  //Values to query perofrmacne.
  t_timeval _tvSave = 0;
  t_timeval _tvLink = 0;
  t_timeval _tvMesh = 0;
  t_timeval _tvDivide = 0;
  t_timeval _tvPostGen1 = 0;
  t_timeval _tvPostGen2 = 0;
  t_timeval _tvProp = 0;
  t_timeval _tvLoad = 0;
  int32_t _nNodes = 0;
  int32_t _nCells = 0;
  //int32_t _nCells = 0;
  int32_t _nLeaves = 0;
  int32_t _nPruned = 0;

  ClimateInstance* _pClimateInstance = nullptr;

  std::future<bool> _genFuture;
  std::atomic_bool _bKilled = false;
  GridGenState::e _eGenState = GridGenState::e::PreGen;

  void deleteMeshes();
  ivec3 getCellIdForPoint(vec3& v);
  void settleLiquidsNeighbor(WorldCell* pc, PhysicsGridSide::e eSide);
  void spreadLiquid(int ic, WorldCell* pc, int ni, WorldCell* pn, W25Geom& __out_ val);
  void makeRootNode();
  void cleanEmptyNodes();
  void cleanEmptyNodes_r(BlockNode* pParent);
  //Link
  void linkCellsInternal(bool bUnlink);
  void stitchBoundariesX_r(BlockNode* bn, BlockNode* bn2, bool bUnlink, bool bQueue);
  void stitchBoundariesY_r(BlockNode* bn, BlockNode* bn2, bool bUnlink, bool bQueue);
  void stitchBoundariesZ_r(BlockNode* bn, BlockNode* bn2, bool bUnlink, bool bQueue);
  void stitchNodes(bool bUnlink);
  void stitchNodesX(BlockNode* giver, BlockNode* taker, bool bUnlink);
  void stitchNodesY(BlockNode* giver, BlockNode* taker, bool bUnlink);
  void stitchNodesZ(BlockNode* giver, BlockNode* taker, bool bUnlink);
  void linkBlocks(WorldCell* b1, WorldCell* b2, PhysicsGridSide::e nrp, bool bUnlink, bool bQueue);
  void debugBlocksAreSpatiallyCoherent(WorldCell*, WorldCell*);
  BlockPos8::e oppAx(BlockPos8::e ePos, int ax);
  BlockNode* rootNeighbor(PhysicsGridSide::e n);
  void makeMesh(bool bAsync = false);
  void updateRedoMeshForCell(WorldCell* pc, bool bNeighbors);
  void WorldGrid::debugVerifyAllInternalCellsLinked(BlockNode* parent = nullptr);
  void WorldGrid::stitchBoundaries_r(BlockNode* giver, BlockNode* taker, bool bUnlink, bool bQueue,
    int ax, PhysicsGridSide::e gLink, BlockPos8::e(&othr)[4]);
  size_t getNodeSizeKb();
  size_t getMeshSizeKb();
  size_t getCellSizeKb();
  void redoNeighborConfigs();
  void redoSide(PhysicsGridSide::e eSide, BlockNode* pParent = nullptr);
  void iterateCells(std::function<void(WorldCell*)> func, BlockNode* parent = nullptr);
  void beginGenAsync(std::function<void()> func);


};

}//ns Game



#endif
