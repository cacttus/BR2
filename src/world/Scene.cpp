#include "../base/AppMain.h"
#include "../base/AppBase.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/GraphicsWindow.h"
#include "../base/WindowManager.h"
#include "../base/ApplicationPackage.h"
#include "../base/FileSystem.h"
#include "../base/ScriptManager.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/InputManager.h"

#include "../model/MeshUtils.h"

#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/UiControls.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/FlyingCameraControls.h"
#include "../gfx/MegaTex.h"

#include "../world/Scene.h"


namespace BR2 {
Scene::Scene() {
  //_pFlyCam = std::make_shared<FlyingCameraControls>(Gu::getViewport());

  //_pFlyCam->getCam()->setLookAt(vec3(0, 0, 0));
  //_pFlyCam->getCam()->setPos(vec3(0, 0, -10));

  BroLogInfo("Creating Window UI");
  _pScreen = std::make_shared<UiScreen>(getThis<GraphicsWindow>());

  BroLogInfo("GLContext -  Lights");
  _pLightManager = std::make_shared<LightManager>();

  BroLogInfo("GLContext -  ScriptManager");
  _pScriptManager = std::make_shared<ScriptManager>();

  createFlyingCamera();

}
Scene::~Scene() {
  _pScreen = nullptr;
  _pLightManager = nullptr;
  _pScriptManager = nullptr;
}
std::vector<std::shared_ptr<CameraNode>> Scene::getAllCameras() {
  std::vector<std::shared_ptr<CameraNode>> ret;
  iterateBreadthFirst<CameraNode>([&ret](std::shared_ptr<CameraNode> cam) {
    ret.push_back(cam);
    return true;
  });
  return ret;
}
void Scene::createFlyingCamera() {
  //In the future we will replace this witht he active object.
  BroLogInfo("Creating Flying Camera");

#define NOSCRIPT

#ifdef NOSCRIPT
  BroLogInfo("Creating Fly Camera.");
  std::shared_ptr<CameraNode> cn = std::make_shared<CameraNode>();
  std::shared_ptr<FlyingCameraControls> css = std::make_shared<FlyingCameraControls>();
  cn->addComponent(css);
  attachChild(cn);
  // cn->init();

  //cn->getFrustum()->setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  
  //updateCameraPosition();
  //_vMoveVel.construct(0, 0, 0);
  //_pCamera->setPos(vec3(30, 30, 30));
  //_pCamera->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());//Make sure to create the frustum.
  //_vCamNormal = _pCamera->getViewNormal();
  //_vCamPos = _pCamera->getPos();
#else
  //Sort of also a scripting test.
  std::shared_ptr<CameraNode> cam = CameraNode::create(getWindow()->getViewport(), getThis<Scene>());
  std::shared_ptr<CSharpScript> getScriptManager()->loadScript(FileSystem::combinePath(Gu::getAppPackage()->getScriptsFolder(), "FlyCamera.cs"));
  cam->addComponent(script);
#endif

}
void Scene::update(float delta) {

}

void Scene::debugChangeRenderState() {
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F1)) {
    if (Gu::getInputManager()->shiftHeld()) {
      _bDebugDisableShadows = !_bDebugDisableShadows;
    }
    else {
      _bShowDebugText = !_bShowDebugText;
    }
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F2)) {
    _bDrawDebug = !_bDrawDebug;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F3)) {
    _bDebugDisableCull = !_bDebugDisableCull;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F4)) {
    _bDebugShowWireframe = !_bDebugShowWireframe;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F5)) {
    _bDebugClearWhite = !_bDebugClearWhite;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F6)) {
    _bDebugDisableDepthTest = !_bDebugDisableDepthTest;
  }
  if (Gu::getInputManager()->keyPress(SDL_SCANCODE_F7)) {
    //if (Gu::getFrameSync()->isEnabled()) {
    //  Gu::getFrameSync()->disable();
    //}
    //else {
    //  Gu::getFrameSync()->enable();
    //}
  }
  //if (Gu::getFingers()->keyPressOrDown(SDL_SCANCODE_F10)) {
  //    if (_iF10Pressed == 0) {
  //        _iF10Pressed = Gu::getMicroSeconds();
  //    }
  //    if (Gu::getMicroSeconds() - _iF10Pressed > 1000000) {
  //        _pWorld25->getConfig()->setRenderBlocks(!_pWorld25->getConfig()->getRenderBlocks());
  //        BroLogInfo("Block Mode turned " + (_pWorld25->getConfig()->getRenderBlocks() ? "On" : "Off"));
  //        BroLogInfo("Remaking all grid meshes... be patient.");
  //        _pWorld25->remakeGridMeshes();
  //        _iF10Pressed = 0;
  //    }
  //}
  //else {
  //    _iF10Pressed = 0;
  //}



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
    //  Graphics->getClearR() = 1.f;
    //  Graphics->getClearG() = 1.f;
    //  Graphics->getClearB() = 1.f;
    //  Graphics->getClearA() = 1.f;
    //}
    //else {
    //  Graphics->getClearR() = 0.01f;
    //  Graphics->getClearG() = 0.01f;
    //  Graphics->getClearB() = 0.01f;
    //  Graphics->getClearA() = 1.0f;
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
void Scene::drawDeferred(RenderParams& rp) {
  debugChangeRenderState();
}
void Scene::drawForward(RenderParams& rp) {
  debugChangeRenderState();

  if (_pQuadMeshBackground == nullptr) {
    _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
      getActiveCamera()->getViewport()->getWidth(), getActiveCamera()->getViewport()->getHeight());
    _pTex = Gu::getContext()->getTexCache()->getOrLoad(Gu::getAppPackage()->makeAssetPath("tex", "test_tex3.png"));
  }

  RenderUtils::drawAxisShader();
}
void Scene::drawShadow(RenderParams& rp) {
}
void Scene::drawDebug(RenderParams& rp) {
}
void Scene::drawNonDepth(RenderParams& rp) {
  draw2d();
}
void Scene::drawTransparent(RenderParams& rp) {
}
void Scene::drawUI(RenderParams& rp) {
  _pUiScreen->update(Gu::getInputManager());
  _pUiScreen->drawForward();
}
void Scene::draw2d() {
  drawDebugText();
}
void Scene::draw2d() {
  drawDebugText();
}
void Scene::setDebugMode() {
  //set some debug vars to make ikte easier
  _bShowDebugText = true;
  _bDrawDebug = true;
  if (Gu::isDebug()) {
    _bShowDebugText = true;
    _bDrawDebug = false;
    //Keep this legit so we can test x , y
 //   _pFlyCam->setPosAndLookAt(vec3(0, 5, -20), vec3(0, 0, 0));
  }
}
void Scene::drawDebugText() {
  int bx = 5;
  int by = 32 + 34;// 0 is the height of the font
  int dy = 16;//Also the font size
  int cy = 0;

  //  _pAppUi->clearDebugText();
  //  if (_bShowDebugText) {
  //    size_t iVisibleLights = 0;//_pWorld25->getFrameVisibleLights().size();
  //
  //    //////////////////////////////////////////////////////////////////////////
  //#define DBGL(...) _pAppUi->dbgLine(StringUtil::format(__VA_ARGS__))
  ////////////////////////////////////////////////////////////////////////////
  //    //DBGL("%.2f", Gu::getFpsMeter()->getFps());
  //
  //    //    _pContext->getTextBoss()->setColor(vec4(0, 0, b, 1));
  //    DBGL("Global");
  //    DBGL("  Debug: %s", _bDrawDebug ? "Enabled" : "Disabled");
  //    DBGL("  Culling: %s", _bDebugDisableCull ? "Disabled" : "Enabled");
  //    DBGL("  Depth Test: %s", _bDebugDisableDepthTest ? "Disabled" : "Enabled");
  //    DBGL("  Render: %s", _bDebugShowWireframe ? "Wire" : "Solid");
  //    DBGL("  Clear: %s", _bDebugClearWhite ? "White" : "Black-ish");
  //    // DBGL("  Vsync: %s", (Gu::getFrameSync()->isEnabled()) ? "Enabled" : "Disabled");
  //    DBGL("  Shadows: %s", (_bDebugDisableShadows) ? "Enabled" : "Disabled");
  //
  //    //DBGL("  Camera: %s", Gu::getCamera()->getPos().toString(5).c_str());
  //
  //  }
  //  _pAppUi->endDebugText();
}

void Scene::updateWidthHeight(int32_t w, int32_t h, bool bForce) {
  _pScreen->screenChanged(w, h);
}
void Scene::idle(int64_t us) {
}




}//ns Game
