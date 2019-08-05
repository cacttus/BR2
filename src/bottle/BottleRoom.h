/**
*
*    @file DartRoom.h
*    @date January 9, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __DDROOM_1483943709643826476_H__
#define __DDROOM_1483943709643826476_H__

#include "../bottle/BottleHeader.h"
#include "../base/RoomBase.h"

namespace Game {
class SKIN_TEMP;

//////////////////////////////////////////////////////////////////////////
namespace WorldSelectState { typedef enum { Select, ConfirmDeleteAll } e; }
class WorldSelect : public VirtualMemory {
    std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
    BottleRoom* _pCongaRoom = nullptr;
    std::vector<t_string> _vecWorldFolders;
    WorldSelectState::e _eWorldSelectState = WorldSelectState::e::Select;
    std::shared_ptr<Texture2DSpec> _pTex = nullptr;
    t_string _strSelectedWorld = "";
    bool _bLoadNextFrame = false;
    t_string genNewWorldName(t_string baseName);
    void deleteWorld(t_string worldName);
    void drawText();
public:
    WorldSelect(BottleRoom* cr);
    virtual ~WorldSelect();
    void gatherWorlds();
    void drawBackgroundImage();
    void draw2d();
    void update();
    void updateTouch(std::shared_ptr<Fingers>, float dt);

};
//////////////////////////////////////////////////////////////////////////
class BottleRoom : public RoomBase {

    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<World25> _pWorld25 = nullptr;
    GameMode::e _eGameMode = GameMode::e::Play;
    std::shared_ptr<GodCam> _pSnapRooter = nullptr;
    std::shared_ptr<FlyCam> _pFlyCam = nullptr;

    t_string _strGameSaveDir;//Portals 12/22/17

    std::shared_ptr<TouchInfo> _curTouchRmb = nullptr;
    std::shared_ptr<TouchInfo> _lastTouchRmb = nullptr;
    std::shared_ptr<TouchInfo> _curTouchLmb = nullptr;
    std::shared_ptr<TouchInfo> _lastTouchLmb = nullptr;

    bool _bDebugDisableCull = false;
    bool _bDrawDebug = false;
    bool _bShowDebugText = false;
    bool _bDebugShowWireframe = false;
    bool _bDebugClearWhite = false;
    bool _bDebugDisableShadows = false;
    bool _bDebugDisableDepthTest = false;
    std::shared_ptr<WorldEditor> _pWorldEditor = nullptr;
    NodeId _iFocusCharacter = NO_NODE_ID;
    GameScreen::e _eGameScreen = GameScreen::e::Title;

    t_timeval _iF10Pressed = 0;

    t_string _strGameName;
    std::shared_ptr<ObFile> _pGameFile = nullptr;


    //UtilMeshSphere* _pDebugSphere = nullptr;
    std::shared_ptr<WorldSelect> _pWorldSelect = nullptr;
    std::shared_ptr<GameUi> _pGameUi = nullptr;
    //void createDebugSphere();
    void createGameSaveDir();
   

    void update(float dt);
    void drawDebugText();
    void draw2d();
    void makeParticles();
    void testDraw2d();
    void drawHover();
    void toggleDebug(std::shared_ptr<Fingers> pFingers);
    void loadGameFile();

    void handleGameModeControls(std::shared_ptr<Fingers>);
    void setDebugMode();
    void debugChangeRenderState();

    void unloadGame();
    t_string getNewWorldName(t_string gameName) ;
    void constructWorld();
public:
    BottleRoom();
    virtual ~BottleRoom() override;

    //this should be on worldeditstate and the name changed to "WorldInfo" or "Scene"
    GameMode::e getGameMode() { return _eGameMode; }
    std::shared_ptr<World25> getWorld25() { return _pWorld25; }
    NodeId getFocusChar() { return _iFocusCharacter; }
    t_string getGameSaveDir() { return _strGameSaveDir; }
    t_string getGameDirectory(t_string gameName);
    t_string getWorldDirectory(t_string gameName, t_string worldName);
    t_string getGameName() { return _strGameName; }

    virtual void userZoom(int amount) override;
    virtual void updateTouch(std::shared_ptr<Fingers>, float dt) override;
    virtual void setup(std::shared_ptr<Viewport> pv, std::shared_ptr<GLContext> ct) override;
    virtual void step(float dt) override;
    
    virtual void drawShadow(RenderParams& rp) override;
    virtual void drawDeferred(RenderParams& rp) override;
    virtual void drawForward(RenderParams& rp) override;
    virtual void drawDebug(RenderParams& rp) override;
    virtual void drawNonDepth(RenderParams& rp) override;
    virtual void drawTransparent(RenderParams& rp) override;

    virtual void teardown() override;
    virtual t_string getAssetsDir() override;
    virtual t_string getShadersDir() override { return "shr"; }
    virtual t_string getModelsTextDir() override { return "mob"; }
    virtual t_string getModelsBinDir() override { return "mbi"; }
    virtual t_string getCacheDir() override { return "cache"; }
    virtual std::shared_ptr<GLContext> getContext() override { return _pContext; }
    virtual bool getStartFullscreen() override;
    virtual uint32_t getStartWidth() override;
    virtual uint32_t getStartHeight()override;
    virtual bool getForceAspectRatio() override;
    virtual void screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) override {}
    virtual void idle(t_timeval ms) override;
    virtual t_string getIconFullPath() override;
    virtual t_string getConfigPath() override;
    virtual void serverPacketReceived(std::shared_ptr<Packet> pack) override;

    bool loadOrCreateGame(t_string);
    virtual t_string getEnvTexturePath() override { return "tex/env1_huge.png"; }
    std::shared_ptr<ObFile> getGameFile() { return _pGameFile; }

    GridShow::e toggleShowGrid();
    void placeObject(std::shared_ptr<ModelSpec> ms);
    std::shared_ptr<GodCam> getGodCam() { return _pSnapRooter; }
};

}//ns Game



#endif
