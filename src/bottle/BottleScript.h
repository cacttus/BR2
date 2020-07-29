/**
*  @file BottleScript.h
*  @date January 9, 2017
*  @author Derek Page
*/
#pragma once
#ifndef __DDROOM_1483943709643826476_H__
#define __DDROOM_1483943709643826476_H__

#include "../bottle/BottleHeader.h"
#include "../world/Script.h"

namespace BR2 {
/**
*  @class BottleScript
*  @brief This is the main Scene script that runs the bottle voxel engine.
*/
class BottleScript : public Script {
public:
  BottleScript();
  virtual ~BottleScript() override;

  //this should be on worldeditstate and the name changed to "WorldInfo" or "Scene"
  GameMode getGameMode() { return _eGameMode; }
  std::shared_ptr<World25> getWorld25() { return _pWorld25; }
  NodeId getFocusChar() { return _iFocusCharacter; }
  string_t getGameSaveDir() { return _strGameSaveDir; }
  string_t getGameDirectory(string_t gameName);
  string_t getWorldDirectory(string_t gameName, string_t worldName);
  string_t getGameName() { return _strGameName; }

  //This is useuless, should be in 
  //virtual void userZoom(int amount);
  virtual void updateTouch(std::shared_ptr<InputManager>, float dt);

  //These should be in Scene
  virtual void onStart() override;
  virtual void onUpdate(float dt) override;
  virtual void onExit() override;
  virtual void onIdle(t_timeval ms) override;

  virtual void onDrawShadow(RenderParams& rp) override;
  virtual void onDrawDeferred(RenderParams& rp) override;
  virtual void onDrawForward(RenderParams& rp) override;
  virtual void onDrawDebug(RenderParams& rp) override;
  virtual void onDrawNonDepth(RenderParams& rp) override;
  virtual void onDrawTransparent(RenderParams& rp) override;

  //virtual void onDrawUi(RenderParams& rp) override;
  virtual void onScreenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) override;
 /// virtual void onServerPacketReceived(std::shared_ptr<Packet> pack) override;

  bool loadOrCreateGame(string_t);
  virtual string_t getEnvTexturePath() { return "tex/env1_huge.png"; }
  std::shared_ptr<ObFile> getGameFile() { return _pGameFile; }
  std::shared_ptr<CameraNode> getRTSCam() { return _pRTSCam; }
  GridShow::e toggleShowGrid();
  void placeObject(std::shared_ptr<ModelSpec> ms);

private:
  std::shared_ptr<World25> _pWorld25 = nullptr;
  std::shared_ptr<WorldSelect> _pWorldSelect = nullptr;
  std::shared_ptr<GameUi> _pGameUi = nullptr;
  std::shared_ptr<ObFile> _pGameFile = nullptr;
  std::shared_ptr<WorldEditor> _pWorldEditor = nullptr;
  std::shared_ptr<CameraNode> _pRTSCam = nullptr;
  std::shared_ptr<TouchInfo> _curTouchRmb = nullptr;
  std::shared_ptr<TouchInfo> _lastTouchRmb = nullptr;
  std::shared_ptr<TouchInfo> _curTouchLmb = nullptr;
  std::shared_ptr<TouchInfo> _lastTouchLmb = nullptr;

  NodeId _iFocusCharacter = NO_NODE_ID;
  GameScreen::e _eGameScreen = GameScreen::e::Title;
  GameMode _eGameMode = GameMode::Play;

  t_timeval _iF10Pressed = 0;
  string_t _strGameName;   // 
  string_t _strGameSaveDir;// 

  void createGameSaveDir();
  void drawDebugText();
  void draw2d();
  void testDraw2d();
  void drawHover();
  void toggleDebug(std::shared_ptr<InputManager> pFingers);
  void loadGameFile();
  void handleGameModeControls(std::shared_ptr<InputManager>);
  void unloadGame();
  string_t getNewWorldName(string_t gameName);
  void constructWorld();
};

}//ns Game



#endif
