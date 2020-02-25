/**
*  @file AppMain.h
*  @date August 9, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __APPMAIN_1565368851810156743_H__
#define __APPMAIN_1565368851810156743_H__

#include "../gfx/RenderIncludes.h"
#include "../base/AppBase.h"

namespace BR2 {
class AppUi : public VirtualMemoryShared<AppUi> {
public:
  AppUi();
  virtual ~AppUi() override {}
  void clearDebugText();
  void dbgLine(std::string txt);
  void endDebugText();
private:
  std::shared_ptr<UiLabel> _pDebugLabel = nullptr;
};
/**
*  @class AppMain
*  @brief Application entry point.
*/
class AppMain : public AppBase {
public:
  AppMain();
  virtual ~AppMain() override;

  virtual void init() override;
  std::shared_ptr<ProjectFile> getProjectFile() { return _pProjectFile; }

  // virtual void step(float dt) override {}
 //  virtual void teardown() override {}
  virtual void updateTouch(std::shared_ptr<InputManager>, float dt) override {}
  virtual void userZoom(int amount) override {}

  virtual string_t getIconFullPath() { return makeAssetPath("tex", "icon.png"); }
  virtual string_t getConfigPath() override { return makeAssetPath("", "config.xml"); }
  virtual string_t getAssetsDir() override { return "./data/"; }

  virtual string_t getShadersDir() override { return "shr"; }
  virtual string_t getModelsTextDir() override { return "mob"; }
  virtual string_t getModelsBinDir() override { return "mbi"; }
  virtual string_t getCacheDir() override { return "cache"; }

  virtual string_t getEnvTexturePath() override { return "tex/env1_huge.png"; }

  virtual void serverPacketReceived(std::shared_ptr<Packet> temp) override {}
  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp) override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override;
  virtual void drawForwardDebug(RenderParams& rp) override;
  virtual void drawUI(RenderParams& rp) override;

  virtual void drawTransparent(RenderParams& rp) override;
private:
  bool _bDebugDisableCull = false;
  bool _bDrawDebug = false;
  bool _bShowDebugText = false;
  bool _bDebugShowWireframe = false;
  bool _bDebugClearWhite = false;
  bool _bDebugDisableShadows = false;
  bool _bDebugDisableDepthTest = false;

  void debugChangeRenderState();
  void drawBackgroundImage();
  std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;

  std::shared_ptr<ProjectFile> _pProjectFile = nullptr;

  void drawDebugText();
  void draw2d();

  std::shared_ptr<AppUi> _pAppUi = nullptr;
  void setDebugMode();


};

}//ns Game



#endif
