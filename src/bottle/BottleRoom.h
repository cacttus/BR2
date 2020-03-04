/**
*  @file BottleRoom.h
*  @date January 9, 2017
*  @author Derek Page
*/
#pragma once
#ifndef __DDROOM_1483943709643826476_H__
#define __DDROOM_1483943709643826476_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
class SKIN_TEMP;

//////////////////////////////////////////////////////////////////////////
namespace WorldSelectState { typedef enum { Select, ConfirmDeleteAll } e; }
class WorldSelect : public VirtualMemory {
  std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
  BottleRoom* _pCongaRoom = nullptr;
  std::vector<string_t> _vecWorldFolders;
  WorldSelectState::e _eWorldSelectState = WorldSelectState::e::Select;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;
  string_t _strSelectedWorld = "";
  bool _bLoadNextFrame = false;
  string_t genNewWorldName(string_t baseName);
  void deleteWorld(string_t worldName);
  void drawText();
public:
  WorldSelect(BottleRoom* cr);
  virtual ~WorldSelect();
  void gatherWorlds();
  void drawBackgroundImage();
  void draw2d();
  void update();
  void updateTouch(std::shared_ptr<InputManager>, float dt);

};
//////////////////////////////////////////////////////////////////////////
class BottleRoom : public VirtualMemory/*: public AppBase*/ {
public:
  BottleRoom();
  virtual ~BottleRoom() override;

  //this should be on worldeditstate and the name changed to "WorldInfo" or "Scene"
  GameMode getGameMode() { return _eGameMode; }
  std::shared_ptr<World25> getWorld25() { return _pWorld25; }
  NodeId getFocusChar() { return _iFocusCharacter; }
  string_t getGameSaveDir() { return _strGameSaveDir; }
  string_t getGameDirectory(string_t gameName);
  string_t getWorldDirectory(string_t gameName, string_t worldName);
  string_t getGameName() { return _strGameName; }

  virtual void userZoom(int amount);
  virtual void updateTouch(std::shared_ptr<InputManager>, float dt);
  virtual void setup(std::shared_ptr<RenderViewport> pv, std::shared_ptr<GLContext> ct);
  virtual void step(float dt);

  virtual void drawShadow(RenderParams& rp);
  virtual void drawDeferred(RenderParams& rp);
  virtual void drawForward(RenderParams& rp);
  virtual void drawDebug(RenderParams& rp);
  virtual void drawNonDepth(RenderParams& rp);
  virtual void drawTransparent(RenderParams& rp);

  virtual void teardown();
  //virtual string_t getAssetsDir();
  //virtual string_t getShadersDir() { return "shr"; }
  //virtual string_t getModelsTextDir() { return "mob"; }
  //virtual string_t getModelsBinDir() { return "mbi"; }
  //virtual string_t getCacheDir() { return "cache"; }
  virtual std::shared_ptr<GLContext> getContext() { return _pContext; }
  virtual bool getStartFullscreen();
  virtual uint32_t getStartWidth();
  virtual uint32_t getStartHeight();
  virtual bool getForceAspectRatio();
  virtual void screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) {}
  virtual void idle(t_timeval ms);
  virtual string_t getIconFullPath();
  virtual string_t getConfigPath();
  virtual void serverPacketReceived(std::shared_ptr<Packet> pack);

  bool loadOrCreateGame(string_t);
  virtual string_t getEnvTexturePath()  { return "tex/env1_huge.png"; }
  std::shared_ptr<ObFile> getGameFile() { return _pGameFile; }

  GridShow::e toggleShowGrid();
  void placeObject(std::shared_ptr<ModelSpec> ms);
  std::shared_ptr<GodCam> getGodCam() { return _pSnapRooter; }

private:
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<World25> _pWorld25 = nullptr;
  GameMode _eGameMode = GameMode::Play;
  std::shared_ptr<GodCam> _pSnapRooter = nullptr;
  //std::shared_ptr<FlyCam> _pFlyCam = nullptr;

  string_t _strGameSaveDir;//Portals 12/22/17

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

  string_t _strGameName;
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
  void toggleDebug(std::shared_ptr<InputManager> pFingers);
  void loadGameFile();

  void handleGameModeControls(std::shared_ptr<InputManager>);
  void setDebugMode();
  void debugChangeRenderState();

  void unloadGame();
  string_t getNewWorldName(string_t gameName);
  void constructWorld();

};

}//ns Game



#endif
