/**
*
*    @file Scene.h
*    @date February 9, 2020
*    @author MetalMario971
*
*/
#pragma once
#ifndef __SCENE_1581228482668944733_H__
#define __SCENE_1581228482668944733_H__

#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class Scene
*    @brief The scenegraph holding all relevant objects.
*/
class Scene : public Drawable {
public:
  Scene();
  virtual ~Scene() override;

  std::shared_ptr<UiScreen> getUiScreen() { return _pScreen; }
  std::shared_ptr<PhysicsWorld> getPhysicsWorld() { return _pPhysicsWorld; }
  void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { _pPhysicsWorld = p; }
  //Returns the camera to render to.
  std::shared_ptr<UiScreen> getUiScreen() { return _pUiScreen; }
  std::shared_ptr<CameraNode> getActiveCamera() { return _pActiveCamera; }
  std::shared_ptr<LightManager> getLightManager() { return _pLightManager; }

  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp)override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override; // draw the non-depth test items (last)
  virtual void drawTransparent(RenderParams& rp) override; //These come after the way after, the very end

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
  std::shared_ptr<FlyCam> _pFlyCam = nullptr;

  void setDebugMode();
  void draw2d();
  void drawDebugText();
  void debugChangeRenderState();
  void drawBackgroundImage();

};

}//ns Game



#endif
