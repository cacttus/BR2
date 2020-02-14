/**
*
*  @file Scene.h
*  @date February 9, 2020
*  @author MetalMario971
*
*/
#pragma once
#ifndef __SCENE_1581228482668944733_H__
#define __SCENE_1581228482668944733_H__

#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class Scene
*  @brief The scenegraph holding all relevant objects.
*/
class Scene : public WorldObject {
public:
  Scene();
  virtual ~Scene() override;

  void idle(int64_t us);
  void updateWidthHeight(int32_t w, int32_t h, bool bForce);

  std::shared_ptr<UiScreen> getUiScreen() { return _pScreen; }
  std::shared_ptr<PhysicsWorld> getPhysicsWorld() { return _pPhysicsWorld; }
  void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { _pPhysicsWorld = p; }
  std::shared_ptr<CameraNode> getActiveCamera() { return _pActiveCamera; }
  std::shared_ptr<LightManager> getLightManager() { return _pLightManager; }
  std::shared_ptr<ScriptManager> getScriptManager() { return _pScriptManager; }
  std::shared_ptr<GraphicsWindow> getWindow() { return _pGraphicsWindow; }
  void setWindow(std::shared_ptr<GraphicsWindow> x) { _pGraphicsWindow=x; }

  void update(float delta);

  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp)override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override; // draw the non-depth test items (last)
  virtual void drawTransparent(RenderParams& rp) override; //These come after the way after, the very end
  virtual void drawUI(RenderParams& rp) override;


private:
  bool _bDebugDisableCull = false;
  bool _bDrawDebug = false;
  bool _bShowDebugText = false;
  bool _bDebugShowWireframe = false;
  bool _bDebugClearWhite = false;
  bool _bDebugDisableShadows = false;
  bool _bDebugDisableDepthTest = false;

  std::shared_ptr<LightManager> _pLightManager;
  std::shared_ptr<UiScreen> _pScreen = nullptr;
  std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;
  std::shared_ptr<ProjectFile> _pProjectFile = nullptr;
  std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
  std::shared_ptr<UiScreen> _pUiScreen = nullptr;
  std::shared_ptr<CameraNode> _pActiveCamera = nullptr;
  //The default fly camera must always be available.
  //std::shared_ptr<FlyingCameraControls> _pFlyCam = nullptr;
  std::shared_ptr<ScriptManager> _pScriptManager = nullptr;
  std::shared_ptr<GraphicsWindow> _pGraphicsWindow = nullptr;

  void setDebugMode();
  void draw2d();
  void drawDebugText();
  void debugChangeRenderState();
  void drawBackgroundImage();
  void createFlyingCamera();

};

}//ns Game



#endif
