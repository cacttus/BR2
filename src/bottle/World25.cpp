#include "../base/GLContext.h"
#include "../base/ApplicationPackage.h"
#include "../base/Hash.h"
#include "../base/FrameSync.h"
#include "../base/DebugHelper.h"
#include "../base/SoundCache.h"
#include "../base/FpsMeter.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../base/GraphicsWindow.h"
#include "../math/Random.h"
#include "../math/Algorithm.h"
#include "../math/CollisionEquations.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/UiControls.h"
#include "../gfx/Atlas.h"
#include "../gfx/QuadBufferMesh.h"
#include "../gfx/GpuQuad3.h"
#include "../gfx/HappySky.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/LightNode.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBox.h"
#include "../model/Material.h"
#include "../model/MobFile.h"
#include "../model/ModelCache.h"
#include "../model/ModelCache.h"
#include "../model/MeshNode.h"
#include "../model/TileMesh25.h"
#include "../model/Model.h"
#include "../world/PhysicsGridAwareness.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/WorldCellFile.h"
#include "../bottle/World25.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/Brain25.h"
#include "../bottle/Goal25MoveTo.h"
#include "../bottle/ObFile.h"
#include "../bottle/W25MeshMaker.h"
#include "../bottle/WipGrid.h"
#include "../bottle/WorldMaker.h"
#include "../bottle/W25Config.h"
#include "../bottle/WorldObj.h"
#include "../world/Scene.h"

#define FST_PT(xx) _pContext->getCamera()->getFrustum()->PointAt(xx)

namespace BR2 {
World25::World25(std::shared_ptr<Scene> pscene, std::shared_ptr<BottleScript> pCallingScript) {
  AssertOrThrow2(pscene);
  _pScene = pscene;
  AssertOrThrow2(pCallingScript);
  _pPhysics = pscene->getPhysicsManager();
  AssertOrThrow2(_pPhysics);

  //Validation
  _pBottleRoom = pCallingScript;
  if (!Alg::isPow2((int32_t)BottleUtils::getNumCellsWidth())) {
    BRThrowException("Cells width was not a power of 2.");
  }

  //if (!isPow2((int32_t)CongaUtils::getCellWidth())) {
  //    BRThrowException("Cells width was not a power of 2.");
  //}
  //The plane that our world stays on.
  _pWorld25Plane = std::make_shared<World25Plane>();
  _pWorld25Plane->n = vec3(0, 1, 0);
  _pWorld25Plane->d = 0;
  _pWorld25Plane->getBasisU() = vec3(1, 0, 0);
  _pWorld25Plane->getBasisV() = vec3(0, 0, 1);

  Gu::getSoundCache()->tryPlay(Gu::getPackage()->makeAssetPath("sounds", "nature_0.ogg"), SoundPlayInfo(true, 0.1f));
  //_pNoiseField = new W25NoiseField(this, 999); // Test value

//  _pObjectQuads = new TileMesh25(getContext(), 8192);

  // _pSmasher = new Physics(getContext(), this, _pWorld25Plane->n);
}
World25::~World25() {
  getPhysics()->unloadWorld();

  _pWorldMaker = nullptr;
  _pSkyBox = nullptr;
  _pSkyAtlas = nullptr; // do not delete any textures because the tex manager will delete them
  _pConfig = nullptr;
  _pMeshMaker = nullptr;
  _pTileShader = nullptr;
  _pWorld25Plane = nullptr;
  _pWorldAtlas = nullptr; // do not delete any textures because the tex manager will delete them
}

std::shared_ptr<SpriteBucket> World25::getSpriteBucket() { return _pGameFile->getBucket(); }
void World25::init(std::shared_ptr<ObFile> obFile) {
  //PhysicsWorld::init(BottleUtils::getNodeWidth(), BottleUtils::getNodeHeight(), std::move(vec3(0, 1, 0)),
  //  BottleUtils::getAwarenessRadiusXZ(), BottleUtils::getAwarenessIncrementXZ(),
  //  BottleUtils::getAwarenessRadiusY(), BottleUtils::getAwarenessIncrementY(),
  //  BottleUtils::getNodesY(), BottleUtils::getMaxGridCount());
  _pGameFile = obFile;
  _pConfig = _pGameFile->getW25Config();

  //Shader
  BRLogInfo("World25 - Making Shaders");
  makeShaders();

  BRLogInfo("World25 - Making Atlas");
  makeAtlas();//**MUST COME FIRST because it establishes the TILE ids.
  /*
  need to load the file before the UI
  make Asset Images
  _pApp->createAssetWindow(imgs)
  */

  BRLogInfo("World25 - Making Mesh Conf");
  _pMeshMaker = std::make_shared<W25MeshMaker>(_pWorldAtlas);//Must come before grid
  _pMeshMaker->init();

  BRLogInfo("World25 - Making Dungeons");
  _pWorldMaker = std::make_shared<WorldMaker>(getThis<World25>(), _pGameFile->getBucket(), _pGameFile->getLairSpecs(), _pGameFile->getWalkerSpecs());

  t_timeval _tvInitTime = Gu::getMicroSeconds();

  BRLogInfo("World25 - Making sky");
  makeSky();

  //*Make the first grid for the player's pos.
  BRLogInfo("World25 - Initializing");
  initializeWorldGrid();

  convertMobs();

  createHandCursor();

  Gu::checkErrorsRt();

  BRLogInfo("World25 - Done");
}
void World25::convertMobs() {
  for (std::shared_ptr<WorldObj> ws : _pGameFile->getMobSpecs()) {
    Gu::getModelCache()->convertMobToBin(ws->getMobName(), true, ws->getFriendlyName());
  }
}
void World25::initializeWorldGrid() {
  // ivec3 playerNode = CongaUtils::v3Toi3Node(ap);
}
void World25::unloadWorld() {
  int nStopped = 0;
  int nForfeit = 0;
  size_t iForfeit = 0;
  if (_bAsyncGen) {
    ////Initially kill everything
    //for (GridMap::iterator it = _gridMap.begin(); it != _gridMap.end(); it++) {
    //    std::shared_ptr<WorldGrid> pg = it->second;
    //    if (pg->getGenState() == GridGenState::e::Stage1 || pg->getGenState() == GridGenState::e::Stage2) {
    //        pg->killGen();
    //    }
    //}
    ////Now loop and wait for all the threads to die.
    //bool bIsGen = true;
    //t_timeval t0 = Gu::getMilliSeconds();
    //t_timeval iMaxWaitTimeMs = 3000;
    //while (bIsGen && Gu::getMilliSeconds() - t0 < iMaxWaitTimeMs) {
    //    for (GridMap::iterator it = _gridMap.begin(); it != _gridMap.end(); it++) {
    //        std::shared_ptr<WorldGrid> pg = it->second;
    //        if (pg->getGenState() == GridGenState::e::Stage1 || pg->getGenState() == GridGenState::e::Stage2) {
    //            bIsGen = true;
    //            break;
    //        }
    //    }
    //}
  }
  //Delete the cells that are dead.  Forfeit some memory.  Boo hoo.
  //for (GridMap::iterator it = _gridMap.begin(); it != _gridMap.end(); it++) {
  //    ivec3* pv = it->first;
  //    std::shared_ptr<WorldGrid> pg = it->second;

  //    if (pg->getGenState() == GridGenState::e::Stage1 || pg->getGenState() == GridGenState::e::Stage2) {
  //        iForfeit += pg->getSizeKb();
  //        nForfeit++;
  //    }
  //    else {
  //        DEL_MEM(pv);
  //        DEL_MEM(pg);
  //    }
  //}

  //BRLogWarn("Forfeited ", nForfeit, " globs (", iForfeit, "kb) of memory.  Fix this later.");

  //for (ivec3* iv : _setEmpty) {
  //    DEL_MEM(iv);
  //}
  //_setEmpty.clear();

  //_gridMap.clear();
}
void World25::initWorld(string_t worldName) {
  _strWorldName = worldName;
}
void World25::createNewWorld() {
  t_timeval tv;
  std::set<std::shared_ptr<WorldGrid>> grids;
  BRLogInfo("Creating world..");
  tv = Gu::getMicroSeconds();
  {
    _pWorldMaker->makeNewWorld();
  }
  BRLogInfo(".." + (Gu::getMicroSeconds() - tv) / 1000 + "ms");
}
//std::shared_ptr<PhysicsGrid> World25::makeGrid(ivec3& cv) {
//    //we manually set empty here because all grids are pre-loaded
//    return new WorldGrid(this, cv, true);
//}
void World25::loadWorld() {
  //Unload whole world.
  getPhysics()->unloadWorld();
  if (_bAsyncGen == false) {
    t_timeval tv;
    std::set<std::shared_ptr<WorldGrid>> grids;

    BRLogInfo("Loading..");
    tv = Gu::getMicroSeconds();
    {
      _pWorldMaker->loadAllGrids(grids);
    }
    BRLogInfo(".." + (Gu::getMicroSeconds() - tv) / 1000 + "ms");
    BRLogInfo("Generating " + grids.size() + " grids..");
    tv = Gu::getMicroSeconds();
    {
      BRLogInfo("Add..");
      for (std::shared_ptr<WorldGrid> pg : grids) {
        _pPhysics->addGrid(pg, pg->getGridPos());
        // pg->linkGrids();
      }
      BRLogInfo("Stage 1..");
      for (std::shared_ptr<WorldGrid> pg : grids) {
        pg->generateStage1Sync();
      }
      BRLogInfo("Post 1..");
      for (std::shared_ptr<WorldGrid> pg : grids) {
        pg->postGenerateStage1();
      }
      BRLogInfo("Stage 2..");
      for (std::shared_ptr<WorldGrid> pg : grids) {
        pg->generateStage2Sync();
      }
      BRLogInfo("Post 2..");
      for (std::shared_ptr<WorldGrid> pg : grids) {
        pg->postGenerateStage2();
        Gu::print(pg->getGenProfileString());
      }
    }
    BRLogInfo(grids.size() + " grids.." + (Gu::getMicroSeconds() - tv) / 1000 + "ms");
  }
}
void World25::createHandCursor() {
  std::shared_ptr<ModelSpec> ms;
  std::shared_ptr<WorldObj> s;
  std::shared_ptr<ModelNode> wo;
  /*
  find chest
  open
  get gem
  buy ice cavern = gem x 1
  place ice cavern
  click ice cavern
  enter ice cavern
  load ice cavern world.

  */
  //makeObj("cuddles_01b", vec3(-6, 0, 6), vec3(0.6f, 0.6f, 0.6f), "cuddles_01b.Idle");

  auto skel = makeObj("skeledug", vec3(BottleUtils::getCellWidth(), BottleUtils::getCellHeight() * 8, BottleUtils::getCellWidth()));
  getScene()->attachChild(skel);
  skel->playAction(Stz "skeledug.Idle");


  //   makeObj("grave_0", vec3(-10, 0, -10), vec3(1.2f, 1.2f, 1.2f), vec4(0, 1, 0, 0), "");
  //   wo = makeObj("chest_0", vec3(10, 0, -10), vec3(0.8f, 0.8f, 0.8f), vec4(0, 1, 0, 0), "");
  //   //*Chest "open" animation.
  //   wo->playAction("chest_0.bar_bot.open");
  //   wo->playAction("chest_0.bar_top.open");
  //   wo->playAction("chest_0.lock.open");
  //   wo->playAction("chest_0.top.open");
  //   wo->playAction("chest_0.bottom.open");
  //   wo = makeObj("gem_0", vec3(4, 0, 4), vec3(0.8f, 0.8f, 0.8f), vec4(0, 1, 0, 0), "gem_0.rotate");
  //
  //  wo = makeObj("coin_0", vec3(4, 1, 12), vec3(0.5, 0.5, 0.5), vec4(0, 1, 0, 0), "coin_0.Rotate");
  //  wo = makeObj("coin_1", vec3(7, 1, 9), vec3(0.5, 0.5, 0.5), vec4(0, 1, 0, 0), "coin_1.rotate");
  wo = nullptr;

  //    wo = makeObj("Tall_Lamp", vec3(0, 0, 0), vec3(0.5, 0.5, 0.5), vec4(0, 1, 0, 0), "");
   // if (wo != nullptr) {
  std::shared_ptr<LightNodePoint> lp = LightNodePoint::create("light",vec3(0, 35.0, 0), 50.0f, vec4(1, 1, 1, 1), "", true);
  getScene()->attachChild(lp);
  //     wo->attachChild(lp);
      // turnOffLamp();//Default turn if otf
//   }

   //DebugHelper::debugHeapBegin(true);

 //  makeDirLight(wo->getPos() + vec3(5, 5, 5), std::move(wo->getPos()), 25.0f, vec4(1, 1, 1, 1), "", true);

   // makeObj("bush_0", vec3(0, 0, -8), vec3(1, 1, 1), vec4(0, 1, 0, 0), "");
   // makeObj("appletree", vec3(15, 0, 15), vec3(0.7, 0.7, 0.7), vec4(0, 1, 0, 0), "appletree.Idle");
   // makeObj("appletree", vec3(-15, 0, 15), vec3(0.7, 0.7, 0.7), vec4(0, 1, 0, 0), "appletree.Idle");
   // makeObj("appletree", vec3(-15, 0, -15), vec3(0.7, 0.7, 0.7), vec4(0, 1, 0, 0), "appletree.Idle");
   // makeObj("appletree", vec3(15, 0, -15), vec3(0.7, 0.7, 0.7), vec4(0, 1, 0, 0), "appletree.Idle");
  // makeObj("box_norm", vec3(3, 1, -3), vec3(0.7, 0.7, 0.7), vec4(0, 1, 0, 0), "box_norm.Rotate");
  auto ob = makeObj("box_glass", vec3(0, BottleUtils::getCellHeight() * 8, 0));
  getScene()->attachChild(ob);
}

std::shared_ptr<ModelNode> World25::makeObj(string_t mobName, vec3& vR3PosToSnap) {
  //TODO: hash + LUT
  std::shared_ptr<ModelNode> wo = nullptr;
  std::shared_ptr<WorldObj> pFound = nullptr;
  for (std::shared_ptr<WorldObj> s : _pGameFile->getMobSpecs()) {
    if (StringUtil::equals(s->getMobName(), mobName)) {
      pFound = s;
      break;
    }
  }

  if (pFound) {
    wo = pFound->createInstance(getThis<World25>(), vR3PosToSnap);
  }

  return wo;
}
void World25::makeSky() {
  BRLogInfo("Making sky.");
  ivec2 gsiz;
  gsiz.construct(2, 2);
  _pSkyBox = std::make_shared<HappySky>();
  _pSkyAtlas = std::make_shared<Atlas>(Gu::getCoreContext(), "SkyBoxAtlas", gsiz);
  _pSkyAtlas->addImage(0, Gu::getPackage()->makeAssetPath("textures", "tx-sb-top.png"));  //top
  _pSkyAtlas->addImage(1, Gu::getPackage()->makeAssetPath("textures", "tx-sb-side.png")); //-s0
  _pSkyAtlas->addImage(2, Gu::getPackage()->makeAssetPath("textures", "tx-sb-side.png")); //-s0
  _pSkyAtlas->addImage(3, Gu::getPackage()->makeAssetPath("textures", "tx-sb-bot.png"));  //bot
  _pSkyAtlas->compileFiles(true, true);
  _pSkyAtlas->setFilter(TexFilter::e::Linear);
  _pSkyBox->init(_pSkyAtlas, 400, true);
}
void World25::makeShaders() {
  _pTileShader = Gu::getShaderMaker()->makeShader(
    std::vector<string_t>{"d_v3i2n3_tileshader.vs", "d_v3i2n3_tileshader.ps"}
  );
  _pGridShader = Gu::getShaderMaker()->makeShader(
    std::vector<string_t>{"d_v3n3_grid.vs", "d_v3n3_grid.gs", "d_v3n3_grid.ps"}
  );
}

void World25::makeAtlas() {
#define MA_P(x) getRoom()->makeAssetPath(x)
  //Init Particles
  BRLogInfo("Making Woorld tex.");

  _pGameFile->getBucket()->loadMotions(_pGameFile);
  int nFrames = _pGameFile->getBucket()->getNumUniqueFrames();
  float fsq = sqrtf((float)nFrames);
  int iSize = (int)ceilf(fsq);

  BRLogInfo("Sprite Map Size = " + iSize + "x" + iSize);

  ivec2 gsiz(iSize, iSize);
  _pWorldAtlas = std::make_shared<Atlas>(Gu::getCoreContext(), "W25Atlas", gsiz);

  //add sprite animations to the atlas.
  _pGameFile->getBucket()->addToAtlas(_pWorldAtlas);
  _pWorldAtlas->compileFiles(false);

  //Make material
  _pWorldMaterial = std::make_shared<Material>();
  _pWorldMaterial->setSpecHardness(190);
  _pWorldMaterial->setDiffuse(vec4(1, 1, 1, 1));
  _pWorldMaterial->setSpec(vec4(1, 1, 1, 0.1));
  _pWorldMaterial->addTextureBinding(_pWorldAtlas, TextureChannel::e::Channel0, TextureType::e::Color, 1.0f);
}

void World25::update(float delta) {
  //Now that physicsworld is not subclassed, this MUST be called AFTER physicsworld::update
  Perf::pushPerf();


  //Objs, Meshes
  _nMeshTrisFrame = 0;
  _nQuadTrisFrame = 0;

  updateTopology();

  //Update Motions
  _pGameFile->getBucket()->update(delta);

  //Copy / UPdat objs
  std::multimap<float, std::shared_ptr<ModelNode>> vecCollected;
  collectObjects(vecCollected);
  updateAndCopyVisibleObjects(delta, vecCollected);

  Perf::popPerf();
}
void World25::updateTopology() {
  for (std::pair<ivec3*, std::shared_ptr<PhysicsGrid>> p : _pPhysics->getGrids()) {
    std::shared_ptr<WorldGrid> pg = std::dynamic_pointer_cast<WorldGrid>(p.second);

    //Topology gets updated on the grid's next "topo update" step.
    pg->updateTopology();
  }
}
void World25::settleLiquids(bool bForce) {
  static t_timeval lastSettle = 0;

  //This is now considered offline processing, but it's forced to be caleld
  if (bForce || getScene()->getWindow()->getFpsMeter()->deltaMs(lastSettle, 250 * 1000)) {
    vec3 pos = _pPhysics->getAwareness()->getAwarenessPos();
    float r2XZ = BottleUtils::getBvhExpensiveUpdateRadiusXZ();
    r2XZ *= r2XZ; // r^2
    float r2Y = BottleUtils::getBvhExpensiveUpdateRadiusY();
    r2Y *= r2Y; // r^2

    for (std::pair<ivec3*, std::shared_ptr<PhysicsGrid>> p : _pPhysics->getGrids()) {
      std::shared_ptr<WorldGrid> pg = std::dynamic_pointer_cast<WorldGrid>(p.second);
      if (pg->getGenerated() == true) {
        vec3 cc = pg->getNodeCenterR3();
        float s2xz = (cc.x - pos.x + cc.y - pos.y) * (cc.x - pos.x + cc.y - pos.y);
        float s2y = (cc.y - pos.y) * (cc.y - pos.y);
        if (s2xz <= r2XZ && s2y < r2Y) {
          //Topology gets updated on the grid's next "topo update" step.
          pg->settleLiquids();
        }
      }
    }
  }
}

void World25::collectObjects(std::multimap<float, std::shared_ptr<ModelNode>>& vecCollected) {
}

//*This method is old, busted
void World25::updateAndCopyVisibleObjects(float delta, std::multimap<float, std::shared_ptr<ModelNode>> vecCollected) {
  updateHandCursorAndAddToRenderList(delta);//Make sure this comes after clearing
}
void World25::updateHandCursorAndAddToRenderList(float delta) {
  std::shared_ptr<CameraNode> bc = getScene()->getActiveCamera();
  vec2 v = this->getScene()->getInput()->getMousePos_Relative();
  Ray_t pr = bc->projectPoint2(v);

  ////if (_pHandCursor) {
  ////    _pHandCursor->update(delta);
  ////    _pHandCursor->getLocal().setIdentity();
  ////    _pHandCursor->getLocal() = mat4::getTranslation(std::move(vec3(-3, 0, 0))) * _pHandCursor->getLocal();

  ////}

  //_pHandCursor->setPos(std::move(vec3(-10,5,-10)));
  //_pHandCursor->compileMatrix();

  //Set pos / scale.
  vec3 pos = pr.getOrigin() + pr.getDir().normalized() * 0.02f;
  pos += bc->getRightNormal() * 0.007f;
  pos += bc->getUpNormal() * -0.007f;
  //  _pHandCursor->setPos(std::move(pos));
  //  _pHandCursor->setScale(std::move(vec3(0.007f, 0.007f, 0.007f)));
  //  _pHandCursor->compileWorldMatrix();

    //Rotate
  Vec3Basis basis;
  basis._z = bc->getViewNormal();
  basis._y = bc->getUpNormal();
  basis._x = bc->getRightNormal();

  //Surprisingly this works to rotate an object into a local basis, just swap the matrix with the other object's basis
  mat4 m;
  m._m11 = basis._y.x; m._m12 = basis._y.y; m._m13 = basis._y.z; m._m14 = 0;
  m._m21 = basis._x.x; m._m22 = basis._x.y; m._m23 = basis._x.z; m._m24 = 0;
  m._m31 = basis._z.x; m._m32 = basis._z.y; m._m33 = basis._z.z; m._m34 = 0;
  m._m41 = 0; m._m42 = 0; m._m43 = 0; m._m44 = 1;

  // *  these confusing because we apply the transforms in reverse so we have to be in the other basis before multiplying
  mat4 mrot = mat4::getRotationRad(M_PI_2, 1, 0, 0);
  mat4 mrot_tilt = mat4::getRotationRad(-M_PI / 4.0, 0, 1, 0);
}

void World25::updateTouch(std::shared_ptr<InputManager> pFingers) {
  if (pFingers->keyPress(SDL_SCANCODE_U)) {
    std::shared_ptr<LightNodeDir> ld = nullptr;
    if (_pPhysics->findNode<LightNodeDir>(ld)) {
      ld->setPos(ld->getPos() + vec3(0.2, 0, 0));
    }
  }
  if (pFingers->keyPress(SDL_SCANCODE_J)) {
    std::shared_ptr<LightNodeDir> ld = nullptr;
    if (_pPhysics->findNode<LightNodeDir>(ld)) {
      ld->setPos(ld->getPos() + vec3(-0.2, 0, 0));
    }
  }
  if (pFingers->keyPress(SDL_SCANCODE_H)) {
    std::shared_ptr<LightNodeDir> ld = nullptr;
    if (_pPhysics->findNode<LightNodeDir>(ld)) {
      ld->setPos(ld->getPos() + vec3(0, 0, -0.2));
    }
  }
  if (pFingers->keyPress(SDL_SCANCODE_K)) {
    std::shared_ptr<LightNodeDir> ld = nullptr;
    if (_pPhysics->findNode<LightNodeDir>(ld)) {
      ld->setPos(ld->getPos() + vec3(0, 0, 0.2));
    }
  }
  if (pFingers->keyPress(SDL_SCANCODE_F7)) {
    _bDisableLighting = !_bDisableLighting;
  }
  if (pFingers->keyPress(SDL_SCANCODE_F2, KeyMod::Shift)) {
    turnOffLamp();
  }
  //if (pFingers->keyPress(SDL_SCANCODE_F3, KeyMod::Shift)) {
  //  turnOffSun();
  //}
  //if (pFingers->keyPress(SDL_SCANCODE_F4, KeyMod::Shift)) {
  //    vec3 v= Random::nextVec3(vec3(-10,0,-10), vec3(10,0,10));
  //    float fRot = Random::nextFloat(0.0f,(float)M_2PI);
  //    makeObj("grave_0", std::move(v), vec3(1,1,1), std::move(vec4(0,1,0, fRot)), "");
  //    v = Random::nextVec3(vec3(-10, 1, -10), vec3(10, 3, 10));
  //     fRot = Random::nextFloat(0.0f, (float)M_2PI);
  //   makeObj("coin_1", std::move(v), vec3(0.5, 0.5, 0.5), std::move(vec4(0, 1, 0, fRot)), "coin_1.rotate");

  //}
  //if (pFingers->keyPress(SDL_SCANCODE_F5, KeyMod::Shift)) {
  //    vec3 v = Random::nextVec3(vec3(-10, 0, -10), vec3(10, 0, 10));
  //    float fRot = Random::nextFloat(0.0f, (float)M_2PI);
  //    makeObj("skeledug", std::move(v), vec3(0.6, 0.6, 0.6), std::move(vec4(0, 1, 0, fRot)), "skeledug.Idle");
  //}
}
void World25::turnOffSun() {
  std::shared_ptr<LightNodeDir> light = nullptr;
  if (_pPhysics->findNode<LightNodeDir>(light)) {
    if (light->isHidden()) { light->show(); }
    else { light->hide(); }
  }
}
void World25::turnOffLamp() {
  std::shared_ptr<PhysicsNode> mn = _pPhysics->findNode("Tall_Lamp");
  if (mn != nullptr) {
    std::shared_ptr<LightNodePoint> light = nullptr;
    if (mn->findNode<LightNodePoint>(light)) {
      if (light->isHidden()) { light->show(); }
      else { light->hide(); }
    }
  }
}
void World25::generateStage1(std::vector<std::shared_ptr<WorldGrid>>& vecGen, bool synchronous) {
  //POST make mesh for the gnerated + linked cells
  for (std::shared_ptr<WorldGrid> pGrid : vecGen) {
    _setGenStage1.insert(pGrid);
    pGrid->generateStage1();
  }
}
void World25::generateStage2() {
  t_timeval idleTime = 0;
  std::set<std::shared_ptr<WorldGrid>> toRemove;

  for (std::shared_ptr<WorldGrid> pg : _setGenStage1) {
    if (pg->getIsGenerating() == false) {
      //Instead of iterating through _setGenStage1 a million times, we'll just use a locale-based
      //gen stage 2.  meaning whenever a grid is generated, we loop through it's neighbors and notify them that a
      //neighbor is done.
      //This prevents the set from keeping old nodes in it
      pg->postGenerateStage1();

      //Once all 6 neighbors are in stage 2, then generate the mesh for teh given grid.
      notifyStage1Complete(pg);
      toRemove.insert(pg);
    }
  }

  for (std::shared_ptr<WorldGrid> pg : toRemove) {
    _setGenStage1.erase(_setGenStage1.find(pg));
  }
  toRemove.clear();
}
void World25::notifyStage1Complete(std::shared_ptr<WorldGrid> pg) {
  //Note - attemptstage2 MUST come for this node BEFORE all others. Why? Because we set = stage2 in them
  attemptStage2(pg);
  for (int i = 0; i < 6; ++i) {
    std::shared_ptr<WorldGrid> pn = std::dynamic_pointer_cast<WorldGrid>(pg->getNeighbor(i));
    if (pn != nullptr) {
      attemptStage2(pn);
    }
  }
}
void World25::attemptStage2(std::shared_ptr<WorldGrid> pg) {
  //*This check was because we needed to generate neighbor grids due to the 'infinite world' paradigm.
  //now that our world is generated beforehand we do't really need this.
  //bool bGood = true;
  //for (int i = 0; i < 6; ++i) {
  //    World25Grid* pn = pg->getNeighbor(i);
  //    if (pn == nullptr) {
  //        bGood = false;
  //    }
  //    else if ((int)pn->getGenState() < (int)GridGenState::e::Stage1Complete) {
  //        bGood = false;
  //    }
  //}
  //if (bGood == true) {
  pg->generateStage2();
  _setGenStage2.insert(pg);
  //}
}
void World25::postGenerateNodes() {
  // t_timeval beg;
  string_t st;
  std::set<std::shared_ptr<WorldGrid>> toRemove;

  for (std::shared_ptr<WorldGrid> pg : _setGenStage2) {
    if (pg->getIsGenerating() == false) {
      pg->postGenerateStage2();
      toRemove.insert(pg);

#ifdef _DEBUG
      st += pg->getGenProfileString();
#endif
    }
  }
  if (st.length() > 0) {
    BRLogInfo(st);
  }

  for (std::shared_ptr<WorldGrid> pg : toRemove) {
    _setGenStage2.erase(_setGenStage2.find(pg));
  }
  toRemove.clear();
}
void World25::drawDeferred(RenderParams& rp) {
  Perf::pushPerf();
  //drawSky(rp);

  drawWorldDeferred(rp);
  Perf::popPerf();
}
void World25::drawForward(RenderParams& rp) {
  if (_eShowGrid != GridShow::e::None) {
    //draw grid liens
    Gu::getCoreContext()->pushBlend();
    Gu::getCoreContext()->pushDepthTest();
    {
      glLineWidth(2.0f);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_DEPTH_TEST);
      RenderParams rp(_pGridShader);
      _pGridShader->setCameraUf(rp.getCamera());
      uint32_t uiShowSide = (_eShowGrid == GridShow::e::TopSideBot) ? 1 : 0;
      _pGridShader->setUf("_ufShowSide", (void*)&uiShowSide);

      int nSeconds = 10;
      float t = (float)(((Gu::getMicroSeconds() / 1000) % (nSeconds * 1000)) * 0.001f) / ((float)nSeconds);
      _pGridShader->setUf("_t01", (void*)&t);
      for (std::pair<float, std::shared_ptr<PhysicsGrid>> p : _pPhysics->getVisibleGrids()) {
        std::shared_ptr<PhysicsGrid> pGrid = p.second;
        std::shared_ptr<WorldGrid> wg = std::dynamic_pointer_cast<WorldGrid>(pGrid);

        wg->drawGrid(rp, _nMeshTrisFrame);
      }
    }
    Gu::getCoreContext()->popDepthTest();
    Gu::getCoreContext()->popBlend();
  }
}
void World25::drawSky(RenderParams& rp) {
  //Sky
  vec3 vCamPos = rp.getCamera()->getPos();
  std::shared_ptr<CameraNode> bc = rp.getCamera();

  //Translate the sky up
  mat4 model = mat4::getTranslation(vCamPos.x, vCamPos.y, vCamPos.z);

  //Gu::getShaderMaker()->getDiffuseShader(v_v3n3x2::getVertexFormat())->setCameraUf(bc, &model);

  Gu::getCoreContext()->pushDepthTest();
  {
    glDisable(GL_DEPTH_TEST);
    RenderParams rp2(Gu::getShaderMaker()->getDiffuseShader(v_v3n3x2::getVertexFormat()));
    rp2.setCamera(rp.getCamera());
    _pSkyBox->draw(rp2);
  }
  Gu::getCoreContext()->popDepthTest();
}
void World25::drawWorldDeferred(RenderParams& rp) {
  static GpuTile gpuTiles[W25_MAX_GPU_SPRITES];
  _pGameFile->getBucket()->getGpuTiles(_pWorldAtlas, gpuTiles);

  // _pTileShader->setFreebieDirLightUf();
  _pTileShader->setCameraUf(rp.getCamera());
  _pTileShader->setAtlasUf(_pWorldAtlas);

  Gu::getShaderMaker()->setUfBlock("GpuSprites", gpuTiles, W25_MAX_GPU_SPRITES * sizeof(GpuTile));

  Gu::getCoreContext()->pushCullFace();
  Gu::getCoreContext()->pushBlend();

  RenderParams rp2(_pTileShader);
  rp2.setCamera(rp.getCamera());
  _pWorldMaterial->bind(_pTileShader);

  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  for (std::pair<float, std::shared_ptr<PhysicsGrid>> p : _pPhysics->getVisibleGrids()) {
    std::shared_ptr<PhysicsGrid> pGrid = p.second;
    std::shared_ptr<WorldGrid> wg = std::dynamic_pointer_cast<WorldGrid>(pGrid);
    wg->drawOpaque(rp2, _nMeshTrisFrame);
  }

  Gu::getCoreContext()->popBlend();
  Gu::getCoreContext()->popCullFace();
}
void World25::drawTransparent(RenderParams& rp) {
  static GpuTile gpuTiles[W25_MAX_GPU_SPRITES];
  _pGameFile->getBucket()->getGpuTiles(_pWorldAtlas, gpuTiles);


  // _pTileShader->setFreebieDirLightUf();
  _pTileShader->setCameraUf(rp.getCamera());
  _pTileShader->setAtlasUf(_pWorldAtlas);

  Gu::getShaderMaker()->setUfBlock("GpuSprites", gpuTiles, W25_MAX_GPU_SPRITES * sizeof(GpuTile));

  Gu::getCoreContext()->pushCullFace();
  Gu::getCoreContext()->pushBlend();

  RenderParams rp2(_pTileShader);
  rp2.setCamera(rp.getCamera());
  _pWorldMaterial->bind(_pTileShader);

  //*NOTE this should be moved to the transparent drawing of teh deferred.
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  {
    for (std::pair<float, std::shared_ptr<PhysicsGrid>> p : _pPhysics->getVisibleGrids()) {
      std::shared_ptr<PhysicsGrid> pGrid = p.second;
      std::shared_ptr<WorldGrid> wg = std::dynamic_pointer_cast<WorldGrid>(pGrid);
      wg->drawTransparent(rp2, _nMeshTrisFrame);
    }
  }

  Gu::getCoreContext()->popBlend();
  Gu::getCoreContext()->popCullFace();
}
WorldCell* World25::getGlobalCellForPoint(vec3& pt) {
  ivec3 iv;
  WorldCell* pc = nullptr;
  std::shared_ptr<WorldGrid> gn = std::dynamic_pointer_cast<WorldGrid>(_pPhysics->getNodeForPoint(pt));

  if (gn != nullptr) {
    pc = gn->getCellForPoint(pt);
  }

  return pc;
}
WorldCell* World25::getBaseCellAtRay(Ray_t* pr) {
  //Gets the cell at the bottom of the grid
  WorldCell* pc = nullptr;
  Plane3f pf;

  vec3 p(0, (float)BottleUtils::getNodesY().getMin() * BottleUtils::getNodeHeight(), 0);
  vec3 n(0, 1, 0);
  pf.construct(n, p);
  if (pf.intersectRay(pr)) {
    vec3 pt = pr->getHitPoint();
    pc = getGlobalCellForPoint(pt);
  }
  return pc;
}
WorldCell* World25::getMidCellAtRay(Ray_t* pr) {
  //Gets teh cell at the mid plane of the grid
  WorldCell* pc = nullptr;
  if (getWorld25Plane()->intersectRay(pr)) {
    vec3 pt = pr->getHitPoint();
    pc = getGlobalCellForPoint(pt);
  }
  return pc;
}
WorldCell* World25::hitCellGroundUnderRay(Ray_t* __inout_ inout_pr, bool bHitPlaneIfNoneFound) {
  WorldCell* pRet = nullptr;
  std::multimap<float, WorldCell*> vecCells;
  getRaycastCells(inout_pr, vecCells);

  RaycastHit rh;
  float minT = FLT_MAX;
  vec3 vNormal(0, 0, 0);
  float cw = BottleUtils::getCellWidth();
  float ch = BottleUtils::getCellHeight();
  for (std::pair<float, WorldCell*> pp : vecCells) {
    WorldCell* pc = pp.second;
    if (pc != nullptr) {
      //Realistically we shouldn't need this check if the geoms are working..
      if (pc->getGeom(GridMeshLayer::e::Opaque) != W25GEOM_EMPTY) {
        if (_pConfig->getRenderBlocks() == true) {
          //Set us to be the box collision
          if (pp.first < minT) {
            pRet = pc;
            minT = pp.first;
          }
        }
        else {
          std::unique_ptr<W25MeshConf>& confSolid = _pMeshMaker->getW25Config(pc->getGeom(GridMeshLayer::e::Opaque));
          confSolid->raycastTris(inout_pr, pc->getOriginR3(), cw, ch);
          if (inout_pr->getTime() < minT) {
            pRet = pc;
            minT = inout_pr->getTime();
            vNormal = inout_pr->getNormal();
          }
          std::unique_ptr<W25MeshConf>& confLiquid = _pMeshMaker->getW25Config(pc->getGeom(GridMeshLayer::e::Transparent));
          confLiquid->raycastTris(inout_pr, pc->getOriginR3(), cw, ch);
          if (inout_pr->getTime() < minT) {
            pRet = pc;
            minT = inout_pr->getTime();
            vNormal = inout_pr->getNormal();
          }
        }
      }
    }
  }

  inout_pr->setTime(minT);
  inout_pr->setNormal(vNormal);

  if (pRet == nullptr && bHitPlaneIfNoneFound == true) {
    //Return the cell at y=0
    return getBaseCellAtRay(inout_pr);
  }

  return pRet;
}
std::shared_ptr<ModelNode> World25::getClosestObjectUnderRay(Ray_t* pr) {
  //**Note this doesn't use the object's logical cell, but only
  //the physical one so it may return nullptr sometimes.
 // Ray_t pr = getMouseRay(vMouse);

  std::multimap<float, std::shared_ptr<ModelNode>> objs;
  getRaycastObjects(pr, objs);

  if (objs.size() > 0) {
    return objs.begin()->second;
  }
  return nullptr;
}
void World25::getRaycastObjects(Ray_t* pr, std::multimap<float, std::shared_ptr<ModelNode>>& outObjs) {
  //*20170503 removed cell manifolds
  //TODO: fix this
  RaycastHit bh;
  for (std::pair<float, std::shared_ptr<SceneNode>> p : _pPhysics->getVisibleNodes()) {
    std::shared_ptr<SceneNode> pn = p.second;
    std::shared_ptr<ModelNode> ob = std::dynamic_pointer_cast<ModelNode>(pn);
    if (ob && pn->getBoundBoxObject()->RayIntersect(pr, &bh)) {
      outObjs.insert(std::make_pair(bh._t, ob));
    }
  }
}
void World25::getRaycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& __out_ vecCells) {
  std::multimap<float, std::shared_ptr<WorldGrid>> grids;

  getRaycastGrids2(pr, grids);

  std::set<std::shared_ptr<ModelNode>> setObjs;
  RaycastHit bh;

  std::shared_ptr<ModelNode> pFound = nullptr;
  for (std::pair<float, std::shared_ptr<WorldGrid>> itG : grids) {
    std::shared_ptr<WorldGrid> g = itG.second;

    std::multimap<float, WorldCell*> cells;
    g->raycastCells(pr, cells);

    for (std::pair<float, WorldCell*> itc : cells) {
      vecCells.insert(std::make_pair(itc.first, itc.second));
    }
  }
}

void World25::getRaycastGrids2(Ray_t* pr, std::multimap<float, std::shared_ptr<WorldGrid>>& grids, std::shared_ptr<WorldGrid> pg, int64_t iMarchStamp) {
  RaycastHit rh;
  for (auto p : _pPhysics->getVisibleGrids()) {
    std::shared_ptr<PhysicsGrid> g = p.second;
    if (g->getBoundBox()->RayIntersect(pr, &rh)) {
      std::shared_ptr<WorldGrid> g2 = std::dynamic_pointer_cast<WorldGrid>(g);
      if (g2 != nullptr) {
        grids.insert(std::make_pair(rh._t, g2));
      }
    }
  }
}

void World25::createObjFromFile(World25ObjectData* obData)// WorldCell* pDestCell, int32_t iStackIndex)
{
  //Todo fix all this 1/11/18
  BRThrowNotImplementedException();
  //Create the obj from the data
  //Stack index is the index of the obj when we saved it - i.e. its "stacked" position.

  if (obData->_iType == PIX_OBJ_TYPE_INVALID) {
    //Technically this should never happen because we don't save invalid objects
    Gu::debugBreak();
    return;
  }
}
bool World25::getCellOrObjectUnderRay(Ray_t* out_pr, std::shared_ptr<ModelNode>& out_ob, WorldCell*& out_cell) {
  //Raycast hit the grid.

  out_cell = hitCellGroundUnderRay(out_pr, true);
  //Save the time
  float saved_t = out_pr->getTime();

  std::multimap<float, std::shared_ptr<ModelNode>> objs;
  getRaycastObjects(out_pr, objs);

  //set time if we are closer
  if (out_pr->getTime() > saved_t) {
    out_pr->setTime(saved_t);
  }

  out_ob = nullptr;
  if (objs.size() > 0) {
    if (objs.begin()->first < out_pr->getTime()) {
      out_ob = objs.begin()->second;
      out_cell = nullptr;
    }
  }

  bool bRet = (out_ob != nullptr || out_cell != nullptr);
  return bRet;
}

Ray_t World25::getMouseRay(vec2& vMouse) {
  Ray_t r = getScene()->getActiveCamera()->projectPoint2(vMouse);
  return r;
}

void World25::getRayHoverBlock(Ray_t* pr, WorldCell*& out_hit, WorldCell*& out_hover, vec3& out_n, vec3& out_pt) {
  //Returns both cell under mouse, and cell above it

  //TODO: Fix all this
  //we need a tri normal AND a block normal.
  //we need to also add colliding with the ground plane and point/normal on that.
  // we need to remove colliding with arbitrary empty blocks (or no material)
  //we need to set material = null when geom = empty and geom = empty wehn material == null

  out_hit = nullptr;
  out_hover = nullptr;

  out_n.construct(0, 0, 0);
  out_pt.construct(0, 0, 0);

  //Get the hit point where we hit the block
  //Then get the side that point is on.
  out_hit = hitCellGroundUnderRay(pr, true);

  if (out_hit != nullptr) {
    //out_tri_n = pr->getNormal();
    //out_tri_pt = pr->getHitPoint();

    //Compute the block surface
    Box3f bb;
    out_hit->getBoundBoxBvh(bb);
    out_pt = pr->getHitPoint();
    vec3 bc = bb.center();
    Ceq::aa_point_contact_plane(out_pt, bc, out_n);
    vec3 dp = out_pt + (out_n * BottleUtils::getCellHeight() * 0.5);//push resulting point into block.
    out_hover = getGlobalCellForPoint(dp);

    //*Nah
    //Instead return ivec3 and say
    //Ok _world25->editCell(ivec3 grid, ivec3 cell, block value, tile)
    //if(out_hover == nullptr && bCreateGridIfNotFound){
    //    createGrid()
    //}
  }
}
void World25::getRayHoverVertex(Ray_t* pr, WorldCell*& out_hit, int& out_vert, vec3& out_n, vec3& out_pt) {
  //Out cells should be in order: bl br tl tr
  out_n.construct(0, 0, 0);
  out_pt.construct(0, 0, 0);
  out_vert = -1;
  out_hit = nullptr;

  //Get the hit point where we hit the block
  //Then get the side that point is on.
  out_hit = hitCellGroundUnderRay(pr, true);

  if (out_hit != nullptr) {
    Box3f b;
    out_hit->getBoundBoxBvh(b);
    vec3 cxyz = b.center();

    out_pt = pr->getHitPoint();
    out_n = pr->getNormal();

    //Snap the pt to the side of the cube

    int dx, dy, dz;

    //determine quadrant
    vec3 dp = out_pt - cxyz;
    dx = (int)(dp.x >= 0);
    dy = (int)(dp.y >= 0);
    dz = (int)(dp.z >= 0);

    out_vert = dz * 2 * 2 + dy * 2 + dx;
  }
}

void World25::remakeGridMeshes() {
  int nMesh = 0;
  for (PhysicsWorld::GridMap::iterator it = _pPhysics->getGrids().begin(); it != _pPhysics->getGrids().end(); it++) {
    std::shared_ptr<WorldGrid> wg = std::dynamic_pointer_cast<WorldGrid>(it->second);
    if (wg->getIsGenerating() == false) {
      for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
        wg->updateRedoMesh((GridMeshLayer::e)iMatter);
      }
    }
    nMesh++;
    BRLogInfo("" + nMesh + " / " + _pPhysics->getGrids().size());
  }
}

void World25::idleProcessing(t_timeval us) {
  //t_timeval idle = generateStage2(us);
  //t_timeval idle = postGenerateNodes(us);

  //Called every x frames **SLOW!

  //2018/3/2 - having issues with settle liquids, we copy the vdCopy array then for some reason it's got bad data ?
  //Just edit the world to reproduce this bug
  //settleLiquids(true);

  //if(idle < us) {
  //    //Possible to do other stuff here.
  //}

  _tvAverageOffline = (_tvAverageOffline + us) / 2;
}
std::shared_ptr<PhysicsGrid> World25::loadGrid(const ivec3& pos) {
  return std::dynamic_pointer_cast<PhysicsGrid>(_pWorldMaker->loadGrid(pos));
}
vec3 World25::i3tov3Cell(const ivec3& iNode, const ivec3& iCell) {
  vec3 vOut = _pPhysics->i3tov3Node(iNode);
  vOut.x += BottleUtils::getCellWidth() * (float)iCell.x;
  vOut.y += BottleUtils::getCellHeight() * (float)iCell.y;
  vOut.z += BottleUtils::getCellWidth() * (float)iCell.z;

#ifdef _DEBUG
  //This is called a lot so we're ifdefing it
  vec3 vloc = vOut - _pPhysics->i3tov3Node(iNode);
  ivec3 v2 = v3Toi3CellLocal(vloc);
  AssertOrThrow2(v2 == iCell);
#endif
  return vOut;
}
ivec3 World25::v3Toi3CellLocal(vec3& v) {
  return PhysicsWorld::v3Toi3Any(v, BottleUtils::getCellWidth_1(), BottleUtils::getCellHeight_1());
}

GridShow::e World25::toggleShowGrid() {
  if (_eShowGrid == GridShow::e::None) {
    _eShowGrid = GridShow::e::Top;
  }
  else if (_eShowGrid == GridShow::e::Top) {
    _eShowGrid = GridShow::e::TopSideBot;
  }
  else if (_eShowGrid == GridShow::e::TopSideBot) {
    _eShowGrid = GridShow::e::None;
  }
  else {
    Gu::debugBreak();
  }
  return _eShowGrid;
}
}//ns Game