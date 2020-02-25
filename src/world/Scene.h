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
#include "../model/SceneNode.h"

namespace BR2 {
/**
*  @class Scene
*  @brief The scenegraph holding all relevant objects.
*/
class Scene : public SceneNode {
public:
  Scene();
  virtual ~Scene() override;

  void idle(int64_t us);
  void update(float delta);
  void updateWidthHeight(int32_t w, int32_t h, bool bForce);

  //std::shared_ptr<UiScreen> getUiScreen() { return _pScreen; }
  std::shared_ptr<PhysicsWorld> getPhysicsManager() { return _pPhysicsWorld; }

  std::shared_ptr<CameraNode> getActiveCamera() { return _pActiveCamera; }
  //**TODO: we need to differentiate the different cameras by keeping exactly one camera defined as the "main player camera"
  std::shared_ptr<CameraNode> getPlayerCamera() { return _pActiveCamera; }

  std::shared_ptr<LightManager> getLightManager() { return _pLightManager; }
  std::shared_ptr<ScriptManager> getScriptManager() { return _pScriptManager; }
  std::shared_ptr<GraphicsWindow> getWindow() { return _pGraphicsWindow; }
  std::shared_ptr<GLContext> getContext();
  std::shared_ptr<RenderBucket> getRenderBucket() { return _pRenderBucket; }
  std::vector<std::shared_ptr<CameraNode>> getAllCameras();
  void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { _pPhysicsWorld = p; }
  void setWindow(std::shared_ptr<GraphicsWindow> x) { _pGraphicsWindow = x; }

  virtual void drawDeferred(RenderParams& rp) override;
  virtual void drawForward(RenderParams& rp)override;
  virtual void drawShadow(RenderParams& rp) override;
  virtual void drawForwardDebug(RenderParams& rp) override;
  virtual void drawNonDepth(RenderParams& rp) override; // draw the non-depth test items (last)
  virtual void drawTransparent(RenderParams& rp) override; //These come after the way after, the very end
  virtual void drawUI(RenderParams& rp) override;

  virtual std::shared_ptr<TreeNode> attachChild(std::shared_ptr<TreeNode> pChild) override;
  virtual bool detachChild(std::shared_ptr<TreeNode> pChild) override;

  //std::shared_ptr<ModelNode> createObj(std::shared_ptr<ModelData> ms);
  //std::shared_ptr<ModelNode> createObj(std::shared_ptr<ModelData> ms, vec3& pos, vec4& rot, vec3& scale, std::string action);
  std::shared_ptr<LightNodePoint> createPointLight(vec3&& pos, float radius, vec4&& color, string_t action, bool bShadowsEnabled);
  std::shared_ptr<LightNodeDir> createDirLight(const vec3&& pos, const vec3&& lookAt, float fDist, const vec4&& color, const string_t action, bool bShadowsEnabled);

private:
  bool _bDebugDisableCull = false;
  bool _bDrawDebug = false;
  bool _bShowDebugText = false;
  bool _bDebugShowWireframe = false;
  bool _bDebugClearWhite = false;
  bool _bDebugDisableShadows = false;
  bool _bDebugDisableDepthTest = false;

  std::shared_ptr<LightManager> _pLightManager;
  std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;
  std::shared_ptr<ProjectFile> _pProjectFile = nullptr;
  std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
  //std::shared_ptr<UiScreen> _pUiScreen = nullptr;
  std::shared_ptr<CameraNode> _pActiveCamera = nullptr;
  //The default fly camera must always be available.
  //std::shared_ptr<FlyingCameraControls> _pFlyCam = nullptr;
  std::shared_ptr<ScriptManager> _pScriptManager = nullptr;
  std::shared_ptr<GraphicsWindow> _pGraphicsWindow = nullptr;
  std::shared_ptr<RenderBucket> _pRenderBucket = nullptr;

  void setDebugMode();
  void draw2d();
  void drawDebugText();
  void debugChangeRenderState();
  void drawBackgroundImage();
  void createFlyingCamera();

};

}//ns BR2



#endif
