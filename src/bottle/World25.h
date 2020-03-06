/**
*  @file World25d.h
*  @date March 2, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLD25D_14884745582594951236_H__
#define __WORLD25D_14884745582594951236_H__


#include "../world/PhysicsWorld.h"
#include "../bottle/BottleHeader.h"
namespace BR2 {
/**
*  @class World25d
*  @brief Manages the Bottle world grid hierarchy, and physics interaction with world.  Inherits generic physics management for iterative physics solutions.
*/
class World25 : public VirtualMemoryShared<World25> {
public:

  /* TODO: Pass PhysicsWorld in as a member*/
  World25(std::shared_ptr<Scene> pscene, std::shared_ptr< BottleScript> pCallingScript);
  virtual ~World25() override;
  void init(std::shared_ptr<ObFile> ob);

  void updateTouch(std::shared_ptr<InputManager> pFingers);
  void update(float delta) ;
   void drawDeferred(RenderParams& rp) ;
   void drawForward(RenderParams& rp) ;

  //*Raycast Tests**
  Ray_t getMouseRay(vec2& vMouse);
  void getRaycastGrids2(Ray_t* pr, std::multimap<float, std::shared_ptr<WorldGrid>>& grids, std::shared_ptr<WorldGrid> pn = nullptr, int64_t iMarchStamp = 0);
  void getRaycastObjects(Ray_t* pr, std::multimap<float, std::shared_ptr<ModelNode>>& grids);
  void getRaycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& __out_ vecCells);
  bool getCellOrObjectUnderRay(Ray_t* pr, std::shared_ptr<ModelNode>& out_ob, WorldCell*& out_cell);
  WorldCell* getGlobalCellForPoint(vec3& pt);
  void getRayHoverBlock(Ray_t* pr, WorldCell*& out_hit, WorldCell*& out_hover, vec3& out_n, vec3& out_pt);
  void getRayHoverVertex(Ray_t* pr, WorldCell*& out_hit, int& out_vert, vec3& out_n, vec3& out_pt);
  std::shared_ptr<ModelNode> getClosestObjectUnderRay(Ray_t* pr);
  WorldCell* hitCellGroundUnderRay(Ray_t* __inout_ inout_pr, bool bHitPlaneIfNoneFound);


  //Get/set cheap
  size_t getNumGridsGenerating() { return _setGenStage1.size(); }
  std::shared_ptr<World25Plane> getWorld25Plane() { return _pWorld25Plane; }
  std::shared_ptr<Atlas> getWorldAtlas() { return _pWorldAtlas; }
  std::shared_ptr<W25MeshMaker> getMeshMaker() { return _pMeshMaker; }
  std::shared_ptr<SpriteBucket> getSpriteBucket();
  std::shared_ptr<PhysicsWorld> getPhysics() { return _pPhysics; }

  std::shared_ptr<ModelNode> makeObj(string_t mobName, vec3& vR3PosToSnap);

  int32_t getMeshTrisFrame() { return _nMeshTrisFrame; }
  int32_t getQuadTrisFrame() { return _nQuadTrisFrame; }
  t_timeval getAverageOfflineDuration() { return _tvAverageOffline; }
  std::shared_ptr<WorldMaker> getWorldMaker() { return _pWorldMaker; }
  std::shared_ptr<W25Config> getConfig() { return _pConfig; }
  std::shared_ptr<BottleScript> getRoom() { return _pBottleRoom; }
  std::shared_ptr<Scene> getScene() { return _pScene; }

  void createObjFromFile(World25ObjectData* obData);//, WorldCell* pDestCell, int32_t iStackIndex);
  void remakeGridMeshes();
  void idleProcessing(t_timeval us);

  void unloadWorld();
  void loadOrCreateWorld(string_t strWorldName);
  void initWorld(string_t worldName);//select edefaul lair
  void createNewWorld();
  void loadWorld();
  string_t getWorldName() { return _strWorldName; }
  vec3 i3tov3Cell(const ivec3& iNode, const ivec3& iCell);
  ivec3 v3Toi3CellLocal(vec3& v);
  GridShow::e toggleShowGrid();

protected:

private:
  bool _bAsyncGen = false;

  std::shared_ptr<Material> _pWorldMaterial = nullptr;
  std::shared_ptr<LightNodePoint> _pLight0 = nullptr;
  std::shared_ptr<LightNodePoint> _pLight1 = nullptr;
  std::shared_ptr<LightNodePoint> _pLight2 = nullptr;
  std::shared_ptr<Atlas> _pWorldAtlas = nullptr;
  std::shared_ptr<World25Plane> _pWorld25Plane = nullptr;
  std::shared_ptr<ShaderBase> _pTileShader = nullptr;
  std::shared_ptr<ShaderBase> _pGridShader = nullptr;
  std::shared_ptr<HappySky> _pSkyBox = nullptr;
  std::shared_ptr<Atlas> _pSkyAtlas = nullptr;
  std::shared_ptr<W25MeshMaker> _pMeshMaker = nullptr;
  std::shared_ptr<WorldMaker> _pWorldMaker = nullptr;
  std::shared_ptr<BottleScript> _pBottleRoom = nullptr;
  std::shared_ptr<W25Config> _pConfig = nullptr;
  std::shared_ptr<ObFile> _pGameFile = nullptr;
  std::shared_ptr<PhysicsWorld> _pPhysics = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr;

  std::set<std::shared_ptr<WorldGrid>> _setGenStage1;
  std::set<std::shared_ptr<WorldGrid>> _setGenStage2;

  bool _bDisableLighting = true;
  
  int32_t _nMeshTrisFrame = 0;
  int32_t _nQuadTrisFrame = 0;
  t_timeval _tvAverageOffline = 0;
  int64_t _iMarchStamp = 1;
  string_t _strGameName;//Portals 12/22/17
  string_t _strWorldName;
  GridShow::e _eShowGrid = GridShow::e::None;

  std::shared_ptr<PhysicsGrid> loadGrid(const ivec3& pos);
  void updateHandCursorAndAddToRenderList(float);
  void generateStage1(std::vector<std::shared_ptr<WorldGrid>>& vecGen, bool synchronous);
  void generateStage2();
  void postGenerateNodes();

  void notifyStage1Complete(std::shared_ptr<WorldGrid> pg);
  void attemptStage2(std::shared_ptr<WorldGrid> pg);

  void makeShaders();
  void initializeWorldGrid();

  void turnOffLamp();
  void turnOffSun();

  void makeSky();
  void settleLiquids(bool bForce);
  void updateAwarenessSpheroidAxis(float& fAwareness, float minR, float maxR, float increment);
  void makeAtlas();
  void updateAndCopyVisibleObjects(float delta, std::multimap<float, std::shared_ptr<ModelNode>> vecCollected);
  void collectObjects(std::multimap<float, std::shared_ptr<ModelNode>>& vecCollected);
  WorldCell* getBaseCellAtRay(Ray_t* pr);
  WorldCell* getMidCellAtRay(Ray_t* pr);
  void updateTopology();

  void drawSky(RenderParams& rp);
  void drawWorld(RenderParams& rp);
  void createHandCursor();

  void convertMobs();
};

}//ns Game



#endif
