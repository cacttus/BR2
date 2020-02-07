/**
*
*    @file Window.h
*    @date August 11, 2019
*    @author MetalMario971
*
*    © 2019
*
*
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__


#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class GraphicsWindow
*    @brief Window to draw to.
*/
class GraphicsWindow : public VirtualMemoryShared<GraphicsWindow> {
public:
  GraphicsWindow(bool ismain, t_string title, RenderSystem::e sys);
  virtual ~GraphicsWindow() override;

  t_string _title;

  void init();
  void step();
  SDL_Window* getSDLWindow() { return _pSDLWindow; }
  void printHelpfulDebug();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void checkErrorsDbg();
  void checkErrorsRt();

  std::shared_ptr<ModelCache> _pModelCache = nullptr;
  std::shared_ptr<ModelCache> getModelCache() { return _pModelCache; }

  std::shared_ptr<WindowViewport> _pWindowViewport = nullptr;
  std::shared_ptr<WindowViewport> getWindowViewport() { return _pWindowViewport; }

  std::shared_ptr<LightManager> _pLightManager = nullptr;
  std::shared_ptr<LightManager> getLightManager() { return _pLightManager; }

  std::shared_ptr<RenderPipe> getRenderPipe() { return _pRenderPipe; }

  std::shared_ptr<UiScreen> getScreen() { return _pScreen; }
  std::shared_ptr<FpsMeter> getFpsMeter() { return _pFpsMeter; }
  std::shared_ptr<FrameSync> getFrameSync() { return _pFrameSync; }
  std::shared_ptr<ShaderMaker> getShaderMaker() { return _pShaderMaker; }
  std::shared_ptr<TexCache> getTexCache() { return _pTexCache; }
  std::shared_ptr<CameraNode> getCamera() { return _pCamera; }
  std::shared_ptr<ParticleManager> getParty() { return _pParty; }
  std::shared_ptr<PhysicsWorld> getPhysicsWorld() { return _pPhysicsWorld; }
  std::shared_ptr<Picker> getPicker() { return _pPicker; }
  std::shared_ptr<Delta> getDelta() { return _pDelta; }
  std::shared_ptr<GLContext> getGraphicsContext() { return _pContext; }
  std::shared_ptr<WindowViewport> getViewport() { return _pWindowViewport; }
  void setCamera(std::shared_ptr<CameraNode> pc) { _pCamera = pc; }
  void setPhysicsWorld(std::shared_ptr<PhysicsWorld> p) { _pPhysicsWorld = p; }

protected:
  void makeSDLWindow(t_string title, int rendersystem);
  void initRenderSystem();

  void swapBuffers();
  void makeCurrent();
  void getDrawableSize(int* w, int* h);
private:
  std::shared_ptr<TexCache> _pTexCache = nullptr;
  std::shared_ptr<ParticleManager> _pParty = nullptr;
  std::shared_ptr<CameraNode> _pCamera = nullptr;
  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<ShaderMaker> _pShaderMaker = nullptr;
  std::shared_ptr<Picker> _pPicker = nullptr;
  std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<GLContext> _pContext = nullptr;

  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  bool _bIsMainWindow = false;

  friend class GraphicsApi;
  friend class OpenGLApi;
  friend class VulkanApi;

  std::shared_ptr<UiScreen> _pScreen = nullptr;

  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;
  void createRenderPipe();
  void beginRender();
  void endRender();

  void toggleFullscreen();

  void createManagers();

  void createSDL_OpenGLWindow(t_string title);
};

}//ns Game



#endif
