#include "../base/GLContext.h"
#include "../base/SoundCache.h"
#include "../base/Logger.h"
#include "../base/TouchInfo.h"
#include "../base/Perf.h"
#include "../base/FrameSync.h"
#include "../base/EngineConfig.h"
#include "../base/InputManager.h"
#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../base/FpsMeter.h"
#include "../gfx/Atlas.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/ParticleMaker.h"
#include "../gfx/CameraNode.h"
#include "../gfx/FrustumBase.h"
#include "../base/GraphicsWindow.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/TexCache.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/ShadowFrustum.h"
#include "../gfx/LightManager.h"
#include "../gfx/MegaTex.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderSettings.h"
#include "../model/UtilMeshSphere.h"
#include "../model/Model.h"
#include "../model/ModelCache.h"
#include "../model/MobFile.h"
#include "../model/MeshUtils.h"
#include "../model/MeshNode.h"
#include "../world/PhysicsWorld.h"
#include "../world/PhysicsNode.h"
#include "../world/Manifold.h"
#include "../world/Scene.h"
#include "../bottle/WorldObj.h"
#include "../bottle/World25.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/WorldEditState.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/RTSCamScript.h"
#include "../bottle/BottleScript.h"
#include "../bottle/W25Config.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/W25Config.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/WorldEditor.h"
#include "../bottle/GameFile.h"
#include "../bottle/ObFile.h"
#include "../bottle/GameUi.h"
#include "../bottle/WorldSelect.h"

namespace BR2 {
BottleScript::BottleScript() {
}
BottleScript::~BottleScript() {
  (_pWorldSelect) = nullptr;
  (_pWorldEditor) = nullptr;
  (_pWorld25) = nullptr;

  (_curTouchRmb) = nullptr;
  (_lastTouchRmb) = nullptr;
  (_curTouchLmb) = nullptr;
  (_lastTouchLmb) = nullptr;
}
void BottleScript::onStart() {
  t_timeval t0;

  _curTouchRmb = std::make_shared<TouchInfo>();
  _lastTouchRmb = std::make_shared<TouchInfo>();
  _curTouchLmb = std::make_shared<TouchInfo>();
  _lastTouchLmb = std::make_shared<TouchInfo>();
  _pWorldSelect = std::make_shared<WorldSelect>(getThis<BottleScript>());

  createGameSaveDir();

  //Set game mode to world select
  _eGameMode = GameMode::WorldSelect;

  //Must come before UI (assets)
  loadGameFile();


  //Must come after camera created
  constructWorld();
  //_pGameUi = std::make_shared<GameUi>();
  //_pGameUi->constructUI(getThis<BottleScript>());

  
  //*Camera.  This must come first before world - because we use it
  BRLogInfo("Creating the RTS camera.");
  _pRTSCam = CameraNode::create("RTS cam", getScene()->getWindow()->getViewport());
  std::shared_ptr<RTSCamScript> css = std::make_shared<RTSCamScript>(_pWorld25);
  _pRTSCam->addComponent(css);
 // getScene()->setActiveCamera(_pRTSCam);
  getScene()->attachChild(_pRTSCam);


  //Must come last.
 // setDebugMode();

  _pWorldEditor = std::make_shared<WorldEditor>(_pWorld25);
  _pWorldEditor->init();

  _pWorldSelect->gatherWorlds();
}
void BottleScript::onUpdate(float dt) {
  if (_eGameMode == GameMode::WorldSelect) {
    _pWorldSelect->update();
  }
  else {
    _pWorld25->update(dt);
  }

  updateTouch(getScene()->getInput(), dt);
}
void BottleScript::onIdle(t_timeval us) {
  _pWorld25->idleProcessing(us);
}
void BottleScript::onExit() {
}
void BottleScript::loadGameFile() {
  //Called when we change to edit mode.
  string_t strObFileName = Gu::getPackage()->makeAssetPath("game.dat");
  BRLogInfo("World25 - Parsing Spec File '" + strObFileName + "'");

  _pGameFile = std::make_shared<ObFile>();
  _pGameFile->loadAndParse(strObFileName);
}
void BottleScript::constructWorld() {
  BRLogInfo("Making world.");
  {
    t_timeval t0 = Gu::getMicroSeconds();
    _pWorld25 = std::make_shared<World25>(getScene(), getThis<BottleScript>());
    _pWorld25->init(_pGameFile);
    BRLogInfo("Finished.." + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");
  }
}
string_t BottleScript::getGameDirectory(string_t gameName) {
  return FileSystem::combinePath(_strGameSaveDir, gameName);
}
string_t BottleScript::getWorldDirectory(string_t gameName, string_t worldName) {
  return FileSystem::combinePath(getGameDirectory(gameName), worldName);
}
string_t BottleScript::getNewWorldName(string_t gameName) {
  //Get highest world number
  string_t gameDir = getGameDirectory(gameName);
  std::vector<string_t> dirs;
  FileSystem::getAllDirs(gameDir, dirs);
  int32_t iHigh = 0;
  for (string_t dirPath : dirs) {
    int32_t i;
    string_t dir = FileSystem::getDirectoryFromPath(dirPath);
    if (dir.length() > 1) {
      dir = dir.substr(1);
      if (TypeConv::strToInt(dir, i)) {
        iHigh = MathUtils::brMax(i, iHigh);
      }
      else {
        BRLogWarn("Dir '" + dir + "' is not a valid world name");
      }
    }
    else {
      BRLogWarn("Dir '" + dir + "' is not a valid world name");
    }
  }

  iHigh++;//increment for next world.

  char buf[256];
  memset(buf, 0, 256);

  const int ciWorldDigits = 6;
  string_t fmt = Stz"w%0" + ciWorldDigits + "d";
  snprintf(buf, 256, fmt.c_str(), iHigh);

  string_t wn(buf);
  return wn;
}
bool BottleScript::loadOrCreateGame(string_t gameName) {
  unloadGame();

  _strGameName = gameName;

  string_t gameDir = getGameDirectory(gameName);
  string_t gameFileDir = FileSystem::combinePath(gameDir, "game.dat");

  if (FileSystem::directoryExists(gameDir)) {
    //later
    BRLogInfo("Loading game " + gameName);

    if (FileSystem::fileExists(gameFileDir)) {
      GameFile gf;
      gf.loadAndParse(gameFileDir);
      if (StringUtil::isEmpty(gf.getWorldName())) {
        BRLogError("World name was empty.");
        Gu::debugBreak();
        return false;
      }
      else {
        _pWorld25->initWorld(gf.getWorldName());
      }
    }
    else {
      BRLogError("Game file '" + gameFileDir + "' not found.");
      Gu::debugBreak();
      return false;
    }
  }
  else {
    FileSystem::createDirectoryRecursive(gameDir);

    //Create initial world
    string_t worldName = getNewWorldName(gameName);
    string_t worldPath = getWorldDirectory(gameName, worldName);
    FileSystem::createDirectoryRecursive(worldPath);

    //create game & save file
    _pWorld25->initWorld(worldName);
    _pWorld25->createNewWorld();
    GameFile gf;
    gf.save(gameFileDir, _pWorld25);
  }

  _pWorld25->loadWorld();
  _eGameMode = GameMode::Play;

  // updateGameModeChanged();
  return true;
}
void BottleScript::unloadGame() {
  _pWorld25->unloadWorld();
}
void BottleScript::createGameSaveDir() {
  _strGameSaveDir = Gu::getPackage()->makeAssetPath("games");
  // string_t ddir = "";
  if (!FileSystem::createDirectoryRecursive(_strGameSaveDir)) {
    BRLogError("Failed to create game save directory " + _strGameSaveDir);
  }
}
void BottleScript::onDrawForward(RenderParams& rp) {
  //Since the on.. can only be called when attached to a node, we MUST be attached to a node, AND the scene.

  if (_eGameMode == GameMode::WorldSelect) {
    //This gets called before the debug.  really, we should draw this before everything.
    //You won't see any models! note this.
    _pWorldSelect->drawBackgroundImage();
  }
  else {
    _pWorld25->drawForward(rp);
    _pWorldEditor->drawForward();
  }

  if (Gu::getRenderSettings()->getDebug()->getShowShadowBox()) {
    for (std::shared_ptr<ShadowBox> sb : getScene()->getLightManager()->getAllShadowBoxes()) {
      sb->debugRender(rp);
    }
    for (std::shared_ptr<ShadowFrustum> sb : getScene()->getLightManager()->getAllShadowFrustums()) {
      sb->debugRender(rp);
    }
  }
}
void BottleScript::onDrawDeferred(RenderParams& rp) {
  if (_eGameMode == GameMode::WorldSelect) {
  }
  else {
    _pWorld25->drawDeferred(rp);
  }
}
void BottleScript::onDrawTransparent(RenderParams& rp) {
  if (_eGameMode == GameMode::WorldSelect) {
  }
  else {
    _pWorld25->drawTransparent(rp);
  }
}
void BottleScript::onDrawShadow(RenderParams& rp) {
  //2/16/18 this will be needed if we wnt to render individual object shadows in the future.
}
void BottleScript::onDrawNonDepth(RenderParams& rp) {
  draw2d();
}
void BottleScript::onDrawDebug(RenderParams& rp) {

}
void BottleScript::onScreenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) {
}
void BottleScript::draw2d() {
  Gu::getCoreContext()->pushDepthTest();
  Gu::getCoreContext()->pushBlend();
  {
    //F3 = disable culling
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (_eGameMode == GameMode::WorldSelect) {
      _pWorldSelect->draw2d();
    }
    else {
    }
    drawDebugText();
  }
  Gu::getCoreContext()->popDepthTest();
  Gu::getCoreContext()->popBlend();
}
void BottleScript::drawDebugText() {
  int bx = 5;
  int by = 32 + 34;// 0 is the height of the font
  int dy = 16;//Also the font size
  int cy = 0;
//see cut code
}
void BottleScript::testDraw2d() {
  /*  vec3 vv = vec3(100, 100, 0);
    getContext()->getParty()->make(
        vv,
        100,
        0, //**PARTICLE HYASH NAME
        40.f, 80.f,
        0.3f, 3,//life
        30.0f, 40.0f,//scale
        0.f, 0.f
    );*/
}
void BottleScript::toggleDebug(std::shared_ptr<InputManager> pFingers) {
#define TOGGLECD(y, x) if(pFingers->keyPress(y)) { ((x) = (!x)); }
  if (pFingers->modsHeld(KeyMod::Ctrl | KeyMod::AltDontCare | KeyMod::ShiftDontCare)) {
    //CTRL + x
    //This is not recommended for the future.
    TOGGLECD(SDL_SCANCODE_KP_0, Gu::getEngineConfig()->getEnableDebugErrorChecking());
    if (pFingers->keyPress(SDL_SCANCODE_F1)) {
      //Force layout update
      getScene()->getUiScreen()->debugForceLayoutChanged();
    }
  }
  else if (pFingers->modsHeld(KeyMod::None)) {
    //NONE
    TOGGLECD(SDL_SCANCODE_KP_0, Gu::getRenderSettings()->getDebug()->getShadowHelpVisible());
    TOGGLECD(SDL_SCANCODE_KP_1, Gu::getRenderSettings()->getDebug()->getShowBoneBoxes());
    TOGGLECD(SDL_SCANCODE_KP_2, Gu::getRenderSettings()->getDebug()->getShowMeshBoxes());
    TOGGLECD(SDL_SCANCODE_KP_3, Gu::getRenderSettings()->getDebug()->getShowNormals());
    TOGGLECD(SDL_SCANCODE_KP_4, Gu::getRenderSettings()->getDebug()->getShowArmatures());
    //TOGGLECD(SDL_SCANCODE_KP_5, Gu::getRenderSettings()->getShowBoneBindBoxes());
    TOGGLECD(SDL_SCANCODE_KP_5, Gu::getRenderSettings()->getDebug()->getPickGui());
    TOGGLECD(SDL_SCANCODE_KP_6, Gu::getRenderSettings()->getDebug()->getShowModelBoxes());
    TOGGLECD(SDL_SCANCODE_KP_7, Gu::getRenderSettings()->getDebug()->getShowModelBoundBox());
    TOGGLECD(SDL_SCANCODE_KP_8, Gu::getRenderSettings()->getDebug()->getShowShadowBox());
    TOGGLECD(SDL_SCANCODE_KP_9, Gu::getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping());

    //World Debug

    if (pFingers->keyPress(SDL_SCANCODE_F3)) {
      loadOrCreateGame("TestGame");
    }

    if (pFingers->keyPressOrDown(SDL_SCANCODE_F10)) {
      if (_iF10Pressed == 0) {
        _iF10Pressed = Gu::getMicroSeconds();
      }
      if (Gu::getMicroSeconds() - _iF10Pressed > 1000000) {
        _pWorld25->getConfig()->setRenderBlocks(!_pWorld25->getConfig()->getRenderBlocks());
        BRLogInfo("Block Mode turned " + (_pWorld25->getConfig()->getRenderBlocks() ? "On" : "Off"));
        BRLogInfo("Remaking all grid meshes... be patient.");
        _pWorld25->remakeGridMeshes();
        _iF10Pressed = 0;
      }
    }
    else {
      _iF10Pressed = 0;
    }

  }
}
void BottleScript::updateTouch(std::shared_ptr<InputManager> pFingers, float delta) {
  Perf::pushPerf();
  toggleDebug(pFingers);

  //Touch World
  if (_eGameMode == GameMode::WorldSelect) {
    _pWorldSelect->updateTouch(pFingers, delta);
  }
  else {
    _pWorld25->updateTouch(pFingers);

    handleGameModeControls(pFingers);

    if (!getScene()->getUiScreen()->getIsPicked()) {
      _pWorldEditor->editWorld(pFingers);
    }
  }

  Perf::popPerf();
}
GridShow::e BottleScript::toggleShowGrid() {
  if (_pWorld25 != nullptr) {
    return _pWorld25->toggleShowGrid();
  }
  return GridShow::e::None;
}
void BottleScript::placeObject(std::shared_ptr<ModelSpec> ms) {
  _pWorldEditor->placeObject(ms);
}
void BottleScript::handleGameModeControls(std::shared_ptr<InputManager> pFingers) {
  if (_eGameMode != GameMode::WorldSelect) {
    if (pFingers->keyPress(SDL_Scancode::SDL_SCANCODE_E)) {
      if (getScene()->getActiveCamera() == getRTSCam()) {
        getScene()->setActiveCamera(getScene()->getDefaultCamera());
      }
      else {
        getScene()->setActiveCamera(getRTSCam());
      }
    }
    if (pFingers->keyPress(SDL_Scancode::SDL_SCANCODE_Z)) {
      if (pFingers->controlHeld()) {
        _pWorldEditor->performUndo();
      }
    }
  }
}
void BottleScript::drawHover() {
}

}//ns Game