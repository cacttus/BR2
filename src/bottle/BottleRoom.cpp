#include "../base/GLContext.h"
#include "../base/SoundCache.h"
#include "../base/TouchInfo.h"
#include "../base/FrameSync.h"
#include "../base/EngineConfig.h"
#include "../base/FileSystem.h"

#include "../display/Atlas.h"
#include "../display/CameraNode.h"
#include "../display/Viewport.h"
#include "../display/Party.h"
#include "../display/CameraNode.h"
#include "../display/FrustumBase.h"
#include "../display/ShaderMaker.h"
#include "../display/ShaderBase.h"
#include "../display/RenderUtils.h"
#include "../display/TexCache.h"
#include "../display/ShadowBox.h"
#include "../display/ShadowFrustum.h"
#include "../display/LightManager.h"
#include "../display/MegaTex.h"
#include "../display/RenderPipe.h"
#include "../display/LightNode.h"
#include "../display/RenderSettings.h"

#include "../model/UtilMeshSphere.h"
#include "../model/Model.h"
#include "../model/ModelCache.h"
#include "../model/MobFile.h"
#include "../model/MeshUtils.h"
#include "../model/MeshNode.h"

#include "../world/PhysicsWorld.h"
#include "../world/PhysicsNode.h"
#include "../world/Manifold.h"

//#include "../net/NetHeader.h"
//#include "../net/Packet.h"

#include "../bottle/WorldObj.h"
#include "../bottle/BottleRoom.h"
#include "../bottle/World25.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/WorldEditState.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"

#include "../bottle/GodCam.h"
#include "../display/FlyCam.h"
#include "../bottle/W25Config.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/W25Config.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/WorldEditor.h"
#include "../bottle/GameFile.h"
#include "../bottle/ObFile.h"
#include "../bottle/GameUi.h"

namespace Game {

///////////////////////////////////////////////////////////////////
BottleRoom::BottleRoom()
{
    _curTouchRmb = std::make_shared<TouchInfo>();
    _lastTouchRmb = std::make_shared<TouchInfo>();
    _curTouchLmb = std::make_shared<TouchInfo>();
    _lastTouchLmb = std::make_shared<TouchInfo>();
    _pWorldSelect = std::make_shared<WorldSelect>(this);
}
BottleRoom::~BottleRoom()
{
    (_pWorldSelect) = nullptr;
    (_pWorldEditor) = nullptr;
    (_pWorld25) = nullptr;

    (_curTouchRmb) = nullptr;
    (_lastTouchRmb) = nullptr;
    (_curTouchLmb) = nullptr;
    (_lastTouchLmb) = nullptr;
}
///////////////////////////////////////////////////////////////////

void BottleRoom::setup(std::shared_ptr<Viewport> pv, std::shared_ptr<GLContext> ct) {
    _pContext = ct;
    t_timeval t0;

    createGameSaveDir();

    vec3 baseN = vec3(0, 1, 0);

    //Set game mode to world select
    _eGameMode = GameMode::e::WorldSelect;

    //*Camera.  This must come first before world - because we use it
    BroLogInfo("Making Cam/Input.");
    _pSnapRooter = std::make_shared<GodCam>(ct, pv);
    _pFlyCam = std::make_shared<FlyCam>(ct, pv);

    //Must come before UI (assets)
    loadGameFile();

    //Must come after camera created
    constructWorld();
    _pGameUi = std::make_shared<GameUi>();
    _pGameUi->constructUI(getThis<BottleRoom>());

    //Must come last.
    setDebugMode();

    _pWorldEditor = std::make_shared<WorldEditor>(_pWorld25);
    _pWorldEditor->init();

    _pWorldSelect->gatherWorlds();
}
void BottleRoom::loadGameFile() {
    //Called when we change to edit mode.
    t_string strObFileName = makeAssetPath("game.dat");
    BroLogInfo("World25 - Parsing Spec File '" + strObFileName + "'");

    _pGameFile = std::make_shared<ObFile>(getThis<AppBase>());
    _pGameFile->loadAndParse(strObFileName);

}
void BottleRoom::constructWorld() {
    BroLogInfo("Making world.");
    {
        t_timeval t0 = Gu::getMicroSeconds();
        _pWorld25 = std::make_shared<World25>();
        Gu::getContext()->setPhysicsWorld(_pWorld25);
        _pWorld25->init(_pGameFile);
        BroLogInfo("Finished.." + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");
    }
}

t_string BottleRoom::getGameDirectory(t_string gameName) {
    return FileSystem::combinePath(_strGameSaveDir, gameName);
}
t_string BottleRoom::getWorldDirectory(t_string gameName, t_string worldName) {
    return FileSystem::combinePath(getGameDirectory(gameName), worldName);
}
t_string BottleRoom::getNewWorldName(t_string gameName) {
    //Get highest world number
    t_string gameDir = getGameDirectory(gameName);
    std::vector<t_string> dirs;
    FileSystem::getAllDirs(gameDir, dirs);
    int32_t iHigh = 0;
    for (t_string dirPath : dirs) {
        int32_t i;
        t_string dir = FileSystem::getDirectoryFromPath(dirPath);
        if (dir.length() > 1) {
            dir = dir.substr(1);
            if (TypeConv::strToInt(dir, i)) {
                iHigh = MathUtils::broMax(i, iHigh);
            }
            else {
                BroLogWarn("Dir '" + dir + "' is not a valid world name");
            }
        }
        else {
            BroLogWarn("Dir '" + dir + "' is not a valid world name");

        }

    }

    iHigh++;//increment for next world.

    char buf[256];
    memset(buf, 0, 256);

    const int ciWorldDigits = 6;
    t_string fmt = Stz"w%0" + ciWorldDigits + "d";
    snprintf(buf, 256, fmt.c_str(), iHigh);

    t_string wn(buf);
    return wn;
}
bool BottleRoom::loadOrCreateGame(t_string gameName) {
    unloadGame();

    _strGameName = gameName;

    t_string gameDir = getGameDirectory(gameName);
    t_string gameFileDir = FileSystem::combinePath(gameDir, "game.dat");

    if (FileSystem::directoryExists(gameDir)) {
        //later
        BroLogInfo("Loading game " + gameName);

        if (FileSystem::fileExists(gameFileDir)) {

            GameFile gf;
            gf.loadAndParse(gameFileDir);
            if (StringUtil::isEmpty(gf.getWorldName())) {
                BroLogError("World name was empty.");
                Gu::debugBreak();
                return false;
            }
            else {
                _pWorld25->initWorld(gf.getWorldName());
            }
        }
        else {
            BroLogError("Game file '" + gameFileDir + "' not found.");
            Gu::debugBreak();
            return false;
        }
    }
    else {
        FileSystem::createDirectoryRecursive(gameDir);

        //Create initial world
        t_string worldName = getNewWorldName(gameName);
        t_string worldPath = getWorldDirectory(gameName, worldName);
        FileSystem::createDirectoryRecursive(worldPath);

        //create game & save file
        _pWorld25->initWorld(worldName);
        _pWorld25->createNewWorld();
        GameFile gf;
        gf.save(gameFileDir, _pWorld25);
    }

    _pWorld25->loadWorld();
    _eGameMode = GameMode::e::Play;
   // updateGameModeChanged();
    return true;
}
void BottleRoom::unloadGame() {
    _pWorld25->unloadWorld();
}
void BottleRoom::createGameSaveDir() {
    _strGameSaveDir = makeAssetPath("games");
    // t_string ddir = "";
    if (!FileSystem::createDirectoryRecursive(_strGameSaveDir)) {
        BroLogError("Failed to create game save directory " + _strGameSaveDir);
    }
}
void BottleRoom::setDebugMode() {
    //set some debug vars to make ikte easier
    if (Gu::isDebug()) {
        _bShowDebugText = true;
        _bDrawDebug = false;
        //Keep this legit so we can test x , y
        _pFlyCam->setPosAndLookAt(vec3(0, 5, -20), vec3(0, 0, 0));
    }
}
void BottleRoom::step(float dt) {
    update(dt);
}
void BottleRoom::update(float dt) {
    if (_eGameMode == GameMode::e::WorldSelect) {
        _pWorldSelect->update();
    }
    else {
        _pWorld25->update(dt);
    }
}

void BottleRoom::drawForward(RenderParams& rp) {
    debugChangeRenderState();

    if (_eGameMode == GameMode::e::WorldSelect) {
        //This gets called before the debug.  really, we should draw this before everything.
        //You won't see any models! note this.
        _pWorldSelect->drawBackgroundImage();
    }
    else {
        _pWorld25->drawForward();
        _pWorldEditor->drawForward();
    }

    if (Gu::getContext()->getRenderSettings()->getDebug()->getShowShadowBox()) {
        for (std::shared_ptr<ShadowBox> sb : Gu::getContext()->getLightManager()->getAllShadowBoxes()) {
            sb->debugRender();
        }
        for (std::shared_ptr<ShadowFrustum> sb : Gu::getContext()->getLightManager()->getAllShadowFrustums()) {
            sb->debugRender();
        }
    }


}
void BottleRoom::drawDeferred(RenderParams& rp) {
    debugChangeRenderState();

    if (_eGameMode == GameMode::e::WorldSelect) {
    }
    else {
        _pWorld25->drawDeferred();
    }

    //sb->endRaster();
}
void BottleRoom::drawTransparent(RenderParams& rp){
    //test
    _pWorld25->drawTransparent();
}
void BottleRoom::drawShadow(RenderParams& rp) {
    //2/16/18 this will be needed if we wnt to render individual object shadows in the future.

}
void BottleRoom::drawNonDepth(RenderParams& rp) {
    debugChangeRenderState();
    draw2d();
}
void BottleRoom::drawDebug(RenderParams& rp) {
    if (_bDrawDebug == true) {
        AppBase::drawDebug();
    }

}
void BottleRoom::debugChangeRenderState() {
#ifdef BRO_OS_WINDOWS
    //#ifdef _DEBUG
    //These must be #ifdef out because glPolygonMOde is not present in gl330 core 
    if (_bDebugShowWireframe == true) {
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }
    if (_bDebugClearWhite == true) {
        _pContext->getClearR() = 1.f;
        _pContext->getClearG() = 1.f;
        _pContext->getClearB() = 1.f;
        _pContext->getClearA() = 1.f;
    }
    else {
        _pContext->getClearR() = 0.01f;
        _pContext->getClearG() = 0.01f;
        _pContext->getClearB() = 0.01f;
        _pContext->getClearA() = 1.0f;
    }
    //#endif
#endif
    if (_bDebugDisableCull == true) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
    }
    if (_bDebugDisableDepthTest == true) {
        glDisable(GL_DEPTH_TEST);
    }
    else {
        glEnable(GL_DEPTH_TEST);
    }
}

void BottleRoom::draw2d() {
    Gu::pushDepthTest();
    Gu::pushBlend();
    {
        //F3 = disable culling
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (_eGameMode == GameMode::e::WorldSelect) {
            _pWorldSelect->draw2d();
        }
        else {
            Gu::getContext()->getGui()->drawForward();
        }
        drawDebugText();

    }
    Gu::popDepthTest();
    Gu::popBlend();
}
void BottleRoom::drawDebugText() {
    int bx = 5;
    int by = 32 + 34;// 0 is the height of the font
    int dy = 16;//Also the font size
    int cy = 0;

    _pGameUi->clearDebugText();
    if (_bShowDebugText) {
        size_t nGrids = _pWorld25->getNumGrids();
        size_t nObjs = _pWorld25->numObjects();

        size_t iVisibleGrids = _pWorld25->getVisibleGrids().size();
        size_t iVisibleNodes = _pWorld25->getVisibleNodes().size();
        size_t iVisibleLights = 0;//_pWorld25->getFrameVisibleLights().size();
        int32_t nMeshTris = _pWorld25->getMeshTrisFrame();
        int32_t nQuadTris = _pWorld25->getQuadTrisFrame();
        size_t nGenGrids = _pWorld25->getNumGridsGenerating();
        t_timeval iAvgOffline = _pWorld25->getAverageOfflineDuration();
        std::shared_ptr<SpriteBucket> mbt = _pWorld25->getSpriteBucket();

//////////////////////////////////////////////////////////////////////////
#define DBGL(...) _pGameUi->dbgLine(StringUtil::format(__VA_ARGS__))
//////////////////////////////////////////////////////////////////////////
        DBGL("%.2f", getContext()->getFpsMeter()->getFps());

        //    _pContext->getTextBoss()->setColor(vec4(0, 0, b, 1));
        DBGL("Global");
        DBGL("  Debug: %s", _bDrawDebug ? "Enabled" : "Disabled");
        DBGL("  Culling: %s", _bDebugDisableCull ? "Disabled" : "Enabled");
        DBGL("  Depth Test: %s", _bDebugDisableDepthTest ? "Disabled" : "Enabled");
        DBGL("  Render: %s", _bDebugShowWireframe ? "Wire" : "Solid");
        DBGL("  Clear: %s", _bDebugClearWhite ? "White" : "Black-ish");
        DBGL("  Vsync: %s", (_pContext->getFrameSync()->isEnabled()) ? "Enabled" : "Disabled");
        DBGL("  Shadows: %s", (_bDebugDisableShadows) ? "Enabled" : "Disabled");

        DBGL("  Camera: %s", getContext()->getCamera()->getPos().toString(5).c_str());
        DBGL("  Avg. Offline: %i", (iAvgOffline), "us");


        DBGL("  Num Grids: %i", nGrids);
        DBGL("  Generating: %i", nGenGrids);
        DBGL("  Num Objects: %i", nObjs);

        //    _pContext->getTextBoss()->setColor(vec4(0, b, 0, 1));
        //    _pContext->getTextBoss()->pstrb(TStr(""), bx, by + cy); cy += dy;
        DBGL("Frame Info ");
        DBGL("  Vis. Objects: %i", iVisibleNodes);
        DBGL("  Vis. Grids: %i", iVisibleGrids);
        DBGL("  Vis. Lights: %i", iVisibleLights);
        DBGL("  Terrain Tris: %i", nMeshTris);
        DBGL("  Object Quad Tris: %i", nQuadTris);

        //    _pContext->getTextBoss()->setColor(vec4(0, b, b, 1));
        //    _pContext->getTextBoss()->pstrb(TStr(""), bx, by + cy); cy += dy;
        DBGL("Physics ");
        DBGL("  Active Objs: %i", _pWorld25->numActiveObjects());

        //    //**Focus Object
        //    std::shared_ptr<WorldObject> ob = std::dynamic_pointer_cast<WorldObject>(_pWorld25->getObj(_iFocusCharacter));
        //    if (ob != nullptr) {
        //        vec3 cv = ob->getVelocity();
        //        vec3 cp = ob->getPos();
        //        //vec3 ca = ob->getTempAccelleration();

        //        _pContext->getTextBoss()->pstrb(TStr("  Char Pos: ",
        //            cp.toString()), bx, by + cy); cy += dy;

        //        _pContext->getTextBoss()->pstrb(TStr("  Char Vel: ",
        //            cv.toString(5)), bx, by + cy); cy += dy;
        //        _pContext->getTextBoss()->pstrb(TStr("  Char Vel Len: ",
        //            cv.length()), bx, by + cy); cy += dy;
        //        //_pContext->getTextBoss()->pstrb(TStr("  Char Acc: ",
        //        //    ca.toString(5)), bx, by + cy); cy += dy;
        //        //_pContext->getTextBoss()->pstrb(TStr("  Char Acc Len: ",
        //        //    ca.length()), bx, by + cy); cy += dy;
        //    }
        //    _pContext->getTextBoss()->setColor(vec4(b, b, b, 1));
        //    _pContext->getTextBoss()->pstrb(TStr(""), bx, by + cy); cy += dy;
        //    //**Hovered tile
        //    WorldCell* pht = _pWorldEditState->getHoveredTile();
        //    if (pht != nullptr) {

        //        int32_t pt_t_s;
        //        t_string pt_t_n_s;
        //        int32_t pt_t_l;
        //        t_string pt_t_n_l;
        //        if (mbt->getTileById(pht->getTile(MatterMode::e::Solid)) != nullptr) {
        //            pt_t_s = mbt->getTileById(pht->getTile(MatterMode::e::Solid))->getTileIndex();
        //            pt_t_n_s = mbt->getTileById(pht->getTile(MatterMode::e::Solid))->getName();
        //        }
        //        else {
        //            pt_t_s = 0;
        //            pt_t_n_s = "";
        //        }
        //        if (mbt->getTileById(pht->getTile(MatterMode::e::Liquid)) != nullptr) {
        //            pt_t_l = mbt->getTileById(pht->getTile(MatterMode::e::Liquid))->getTileIndex();
        //            pt_t_n_l = mbt->getTileById(pht->getTile(MatterMode::e::Liquid))->getName();
        //        }
        //        else {
        //            pt_t_l = 0;
        //            pt_t_n_l = "";
        //        }
        //        W25Geom pt_s = pht->getGeom(MatterMode::e::Solid);
        //        W25Geom pt_l = pht->getGeom(MatterMode::e::Liquid);

        //        _pContext->getTextBoss()->pstrb(TStr("Tile Info "), bx, by + cy); cy += dy;
        //        _pContext->getTextBoss()->pstrb(TStr("  Solid: ", pt_s, " -> ", StringUtil::uppercase(StringUtil::toHex(pt_s, true))), bx, by + cy); cy += dy;
        //        _pContext->getTextBoss()->pstrb(TStr("  Solid Tile: ", pt_t_s, " -> '", pt_t_n_s, "'"), bx, by + cy); cy += dy;
        //        _pContext->getTextBoss()->pstrb(TStr("  Liquid: ", pt_l, " -> ", StringUtil::uppercase(StringUtil::toHex(pt_l, true))), bx, by + cy); cy += dy;
        //        _pContext->getTextBoss()->pstrb(TStr("  Liquid Tile: ", pt_t_l, " -> '", pt_t_n_l, "'"), bx, by + cy); cy += dy;
        //    }

    }
    _pGameUi->endDebugText();
}
void BottleRoom::testDraw2d() {
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

void BottleRoom::makeParticles() {
    ivec2 gsiz(4, 4);
    std::shared_ptr<Atlas> partyAtlas = std::make_shared<Atlas>(_pContext, "W25Particles", gsiz);
    partyAtlas->addImage(0, makeAssetPath("spr", "sp-particle.png"));
    partyAtlas->addImage(1, makeAssetPath("spr", "sp-particle.png"));
    partyAtlas->addImage(2, makeAssetPath("spr", "sp-particle.png"));
    partyAtlas->addImage(3, makeAssetPath("spr", "sp-particle.png"));
    partyAtlas->compileFiles();
    _pContext->getParty()->init(partyAtlas, BottleUtils::c_iMaxParticles);
}

void BottleRoom::teardown() {}
void BottleRoom::userZoom(int amount) {
    if (Gu::getContext()->getCamera() == _pFlyCam->getCam()) {
        _pFlyCam->userZoom((float)amount);
    }
    else {
        _pSnapRooter->userZoom((float)amount);
    }

}
void BottleRoom::toggleDebug(std::shared_ptr<Fingers> pFingers) {
#define TOGGLECD(y, x) if(pFingers->keyPress(y)) { ((x) = (!x)); }
    if (pFingers->modsHeld(KeyMod::Ctrl | KeyMod::AltDontCare | KeyMod::ShiftDontCare)) {
        //CTRL + x
        //This is not recommended for the future.
        TOGGLECD(SDL_SCANCODE_KP_0, Gu::getEngineConfig()->getEnableDebugErrorChecking());
        if (pFingers->keyPress(SDL_SCANCODE_F1)) {
            //Force layout update
            Gu::getContext()->getGui()->debugForceLayoutChanged();
        }
    }
    else if (pFingers->modsHeld(KeyMod::None)) {
        //NONE
        TOGGLECD(SDL_SCANCODE_KP_0, Gu::getContext()->getRenderSettings()->getDebug()->getShadowHelpVisible());
        TOGGLECD(SDL_SCANCODE_KP_1, Gu::getContext()->getRenderSettings()->getDebug()->getShowBoneBoxes());
        TOGGLECD(SDL_SCANCODE_KP_2, Gu::getContext()->getRenderSettings()->getDebug()->getShowMeshBoxes());
        TOGGLECD(SDL_SCANCODE_KP_3, Gu::getContext()->getRenderSettings()->getDebug()->getShowNormals());
        TOGGLECD(SDL_SCANCODE_KP_4, Gu::getContext()->getRenderSettings()->getDebug()->getShowArmatures());
        //TOGGLECD(SDL_SCANCODE_KP_5, Gu::getContext()->getRenderSettings()->getShowBoneBindBoxes());
        TOGGLECD(SDL_SCANCODE_KP_5, Gu::getContext()->getRenderSettings()->getDebug()->getPickGui());
        TOGGLECD(SDL_SCANCODE_KP_6, Gu::getContext()->getRenderSettings()->getDebug()->getShowModelBoxes());
        TOGGLECD(SDL_SCANCODE_KP_7, Gu::getContext()->getRenderSettings()->getDebug()->getShowModelBoundBox());
        TOGGLECD(SDL_SCANCODE_KP_8, Gu::getContext()->getRenderSettings()->getDebug()->getShowShadowBox());
        TOGGLECD(SDL_SCANCODE_KP_9, Gu::getContext()->getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping());

        //Debug info
        //TODO: these should all be in rednersettings 1/22/18
        if (pFingers->keyPress(SDL_SCANCODE_F1)) {
            if (pFingers->shiftHeld()) {
                _bDebugDisableShadows = !_bDebugDisableShadows;
            }
            else {
                _bShowDebugText = !_bShowDebugText;
            }
        }
        if (pFingers->keyPress(SDL_SCANCODE_F2)) {
            _bDrawDebug = !_bDrawDebug;
        }
        if (pFingers->keyPress(SDL_SCANCODE_F3)) {
            _bDebugDisableCull = !_bDebugDisableCull;
        }
        if (pFingers->keyPress(SDL_SCANCODE_F4)) {
            _bDebugShowWireframe = !_bDebugShowWireframe;
        }
        if (pFingers->keyPress(SDL_SCANCODE_F5)) {
            _bDebugClearWhite = !_bDebugClearWhite;
        }
        if (pFingers->keyPress(SDL_SCANCODE_F6)) {
            _bDebugDisableDepthTest = !_bDebugDisableDepthTest;
        }
        if (pFingers->keyPress(SDL_SCANCODE_F7)) {
            if (_pContext->getFrameSync()->isEnabled()) {
                _pContext->getFrameSync()->disable();
            }
            else {
                _pContext->getFrameSync()->enable();
            }
        }
        if (pFingers->keyPressOrDown(SDL_SCANCODE_F10)) {
            if (_iF10Pressed == 0) {
                _iF10Pressed = Gu::getMicroSeconds();
            }
            if (Gu::getMicroSeconds() - _iF10Pressed > 1000000) {
                _pWorld25->getConfig()->setRenderBlocks(!_pWorld25->getConfig()->getRenderBlocks());
                BroLogInfo("Block Mode turned " + (_pWorld25->getConfig()->getRenderBlocks() ? "On" : "Off"));
                BroLogInfo("Remaking all grid meshes... be patient.");
                _pWorld25->remakeGridMeshes();
                _iF10Pressed = 0;
            }
        }
        else {
            _iF10Pressed = 0;
        }
    }
}
void BottleRoom::updateTouch(std::shared_ptr<Fingers> pFingers, float delta) {
    Gu::pushPerf();
    toggleDebug(pFingers);

    //Camera
    if (_eGameMode == GameMode::e::WorldSelect) {
        _pFlyCam->update(pFingers, delta);
    }
    else if (_eGameMode == GameMode::e::Play) {
        if (Gu::getContext()->getCamera() == _pFlyCam->getCam()) {
            _pFlyCam->update(pFingers, delta);
        }
        else {
            _pSnapRooter->update(pFingers, delta);
        }
    }

    //Touch World
    if (_eGameMode == GameMode::e::WorldSelect) {
        _pWorldSelect->updateTouch(pFingers, delta);
    }
    else {

        _pWorld25->updateTouch(pFingers);

        handleGameModeControls(pFingers);

        if (Gu::getContext()->getCamera() == _pFlyCam->getCam()) {
            _pFlyCam->moveCameraWSAD(pFingers, delta);
        }
        else {
            _pSnapRooter->moveCameraWSAD(pFingers, delta);
        }


        if (!Gu::getContext()->getGui()->getIsPicked()) {
            _pWorldEditor->editWorld(pFingers);   
        }
    }

    Gu::popPerf();
}

GridShow::e BottleRoom::toggleShowGrid() {
    if (_pWorld25 != nullptr) {
        return _pWorld25->toggleShowGrid();
    }
    return GridShow::e::None;
}
void BottleRoom::placeObject(std::shared_ptr<ModelSpec> ms) {
    _pWorldEditor->placeObject(ms);
}
void BottleRoom::handleGameModeControls(std::shared_ptr<Fingers> pFingers) {
    if (_eGameMode != GameMode::e::WorldSelect) {

        if (pFingers->keyPress(SDL_Scancode::SDL_SCANCODE_E)) {
            if(Gu::getContext()->getCamera() == _pFlyCam->getCam()){
                _pSnapRooter->setActive();
            }
            else {
                _pFlyCam->setActive();
            }

        }
        if (pFingers->keyPress(SDL_Scancode::SDL_SCANCODE_Z)) {
            if (pFingers->controlHeld()) {
                _pWorldEditor->performUndo();
            }
        }
    }
}
bool BottleRoom::getStartFullscreen() {
    return _pContext->getConfig()->getStartFullscreen();
}
uint32_t BottleRoom::getStartWidth() {
    return _pContext->getConfig()->getDefaultScreenWidth();
}
uint32_t BottleRoom::getStartHeight() {
    return _pContext->getConfig()->getDefaultScreenHeight();
}
bool BottleRoom::getForceAspectRatio() {
    return _pContext->getConfig()->getForceAspectRatio();
}
void BottleRoom::drawHover() {
}
void BottleRoom::idle(t_timeval us) {
    _pWorld25->idleProcessing(us);
}
t_string BottleRoom::getIconFullPath() {
    return makeAssetPath("tex", "icon.png");
}
t_string BottleRoom::getConfigPath() {
    return makeAssetPath("", "config.xml");
}
t_string BottleRoom::getAssetsDir() {
    return "./data/";
}
void BottleRoom::serverPacketReceived(std::shared_ptr<Packet> pack) {
    //todo
}


//////////////////////////////////////////////////////////////////////////
WorldSelect::WorldSelect(BottleRoom* cr) : _pCongaRoom(cr) {

}
WorldSelect::~WorldSelect() {
    _pQuadMeshBackground = nullptr;
}
void WorldSelect::gatherWorlds() {
    if (_pQuadMeshBackground == nullptr) {
        _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
            Gu::getContext()->getCamera()->getViewport()->getWidth(), Gu::getContext()->getCamera()->getViewport()->getHeight());
        _pTex = Gu::getContext()->getTexCache()->getOrLoad(_pCongaRoom->makeAssetPath("tex", "test_tex3.png"));
    }

    t_string gsd = _pCongaRoom->getGameSaveDir();
    FileSystem::getAllDirs(gsd, _vecWorldFolders);
    for (size_t iw = 0; iw < _vecWorldFolders.size(); ++iw) {
        t_string wf = _vecWorldFolders[iw];
        wf = FileSystem::getDirectoryFromPath(FileSystem::getPathFromPath(wf));
        _vecWorldFolders[iw] = wf;
    }
}
void WorldSelect::update() {
    if (StringUtil::isNotEmpty(_strSelectedWorld)) {
        if (_bLoadNextFrame == false) {
            _bLoadNextFrame = true;
        }
        else {
            _pCongaRoom->loadOrCreateGame(_strSelectedWorld);

        }
    }
}
t_string WorldSelect::genNewWorldName(t_string baseName) {
    //Get a new world name
    t_string strNewWorld = "NewWorld";
    int iNewWorldIdx = 0;
    bool bFound = true;
    t_string strNewWorldIdx;
    int nIter = 0;
    while (bFound) {
        bFound = false;
        strNewWorldIdx = strNewWorld + TypeConv::intToStr(iNewWorldIdx);
        for (t_string wf : _vecWorldFolders) {
            if (StringUtil::equalsi(wf, strNewWorldIdx)) {
                bFound = true;
                iNewWorldIdx++;
            }
        }
        nIter++;
    }
    return strNewWorldIdx;
}
void WorldSelect::deleteWorld(t_string worldName) {
    t_string gameDir = _pCongaRoom->getGameDirectory(worldName);
    std::vector<t_string> fileExts{ ".dat", ".g" };
    FileSystem::deleteDirectoryRecursive(gameDir, fileExts);
}
void WorldSelect::updateTouch(std::shared_ptr<Fingers> pFingers, float dt) {
//#ifdef _DEBUG
    //Go Straight to a new world.
    _strSelectedWorld = genNewWorldName("NewWorld");
//#endif

    if (_eWorldSelectState == WorldSelectState::e::ConfirmDeleteAll) {
        if (pFingers->keyPress(SDL_SCANCODE_Y)) {
            for (t_string wf : _vecWorldFolders) {
                deleteWorld(wf);
            }
            gatherWorlds();
            _eWorldSelectState = WorldSelectState::Select;
        }
        else if (pFingers->keyPress(SDL_SCANCODE_N)) {
            _eWorldSelectState = WorldSelectState::Select;
        }
    }
    else if (_eWorldSelectState == WorldSelectState::e::Select) {
        if (pFingers->keyPress(SDL_SCANCODE_0)) {
            _eWorldSelectState = WorldSelectState::e::ConfirmDeleteAll;
        }
        else if (pFingers->keyPress(SDL_SCANCODE_1)) {
            _strSelectedWorld = genNewWorldName("NewWorld");
        }
        else {
            for (int32_t ikey = SDL_SCANCODE_1; ikey <= SDL_SCANCODE_9; ++ikey) {
                if (pFingers->keyPress((SDL_Scancode)ikey)) {
                    size_t ind = ikey - SDL_SCANCODE_1 - 1;
                    if (ind < _vecWorldFolders.size()) {
                        _strSelectedWorld = _vecWorldFolders[ind];
                    }
                }
            }
        }
    }
}
void WorldSelect::draw2d() {
    drawText();
    // _pContext->getTextBoss()->pstrb(TStr("0: New World"), bx, by + cy); cy += 30;
}
void WorldSelect::drawBackgroundImage() {
    std::shared_ptr<CameraNode> bc = Gu::getContext()->getCamera();
    Gu::getContext()->getShaderMaker()->getImageShader_F()->setCameraUf(bc);
    Gu::getContext()->getShaderMaker()->getImageShader_F()->beginRaster();
    {
        //We want depth test so we can see what's in front.
        //glEnable(GL_DEPTH_TEST);
        _pTex->bind(TextureChannel::e::Channel0, Gu::getContext()->getShaderMaker()->getImageShader_F());

        Gu::getContext()->getShaderMaker()->getImageShader_F()->draw(_pQuadMeshBackground);
    }
    Gu::getContext()->getShaderMaker()->getImageShader_F()->endRaster();

}
void WorldSelect::drawText() {

    //int bx = 40;
    //int by = 40;
    //int cx = 0;
    //int cy = 0;
    //if (StringUtil::isNotEmpty(_strSelectedWorld)) {
    //    FontSize iFontHeight = 40;
    //    Box2f quad;

    //    Gu::getContext()->getTextBoss()->setFontSize(iFontHeight);
    //    Gu::getContext()->getTextBoss()->setColor(vec4(0.9877, 0.9778, 0.969, 1));

    //    t_string strLoad = TStr("Loading ", _strSelectedWorld, "..");


    //    float dx = Gu::getContext()->getTextBoss()->getCenteredTextX(strLoad);
    //    float dy = Gu::getContext()->getTextBoss()->getCenteredTextY(strLoad);

    //    Gu::getContext()->getTextBoss()->pstrb(TStr(strLoad), dx, dy);
    //}
    //else {
    //    Gu::getContext()->getTextBoss()->setColor(vec4(0.9877, 0.9778, 0.969, 1));
    //    Gu::getContext()->getTextBoss()->setFont(std::string("Nunito"));

    //    //Title - testing centering
    //    Gu::getContext()->getTextBoss()->setFontSize(40);
    //    t_string strTitle = "Animal Motors v0.01";
    //    float titleX = Gu::getContext()->getTextBoss()->getCenteredTextX(strTitle);
    //    Gu::getContext()->getTextBoss()->pstrb(TStr(strTitle), titleX, by + cy); cy += 40;

    //    Gu::getContext()->getTextBoss()->setFontSize(30);
    //    if (_eWorldSelectState == WorldSelectState::Select) {
    //        Gu::getContext()->getTextBoss()->pstrb(TStr("Select World by typing key"), bx, by + cy); cy += 30;
    //        Gu::getContext()->getTextBoss()->setFontSize(28);
    //        Gu::getContext()->getTextBoss()->pstrb(TStr("1: *New World*"), bx, by + cy); cy += 30;

    //        for (size_t n = 0; n < _vecWorldFolders.size(); ++n) {
    //            Gu::getContext()->getTextBoss()->pstrb(TStr(n + 2, " :", _vecWorldFolders[n], "   ", (int)STRHASH(_vecWorldFolders[n]) % 100, "%%"), bx, by + cy); cy += 30;
    //        }
    //        Gu::getContext()->getTextBoss()->pstrb(TStr(""), bx, by + cy); cy += 30;
    //        Gu::getContext()->getTextBoss()->pstrb(TStr("0: **Delete All Worlds"), bx, by + cy); cy += 30;
    //    }
    //    else  if (_eWorldSelectState == WorldSelectState::ConfirmDeleteAll) {
    //        Gu::getContext()->getTextBoss()->pstrb(TStr("Delete All Worlds ? (Y/N)"), bx, by + cy); cy += 30;
    //        Gu::getContext()->getTextBoss()->setFontSize(28);
    //    }
    //}
}





}//ns Game
