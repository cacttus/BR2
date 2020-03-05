#include "../base/Perf.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Delta.h"
#include "../base/GraphicsWindow.h"
#include "../base/ApplicationPackage.h"
#include "../base/FileSystem.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/InputManager.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/Atlas.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../base/GraphicsWindow.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/UiControls.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/ParticleManager.h"
#include "../gfx/FlyingCameraControls.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/MegaTex.h"
#include "../model/MeshUtils.h"
#include "../model/Model.h"
#include "../model/MeshNode.h"
#include "../world/RenderBucket.h"
#include "../world/PhysicsWorld.h"
#include "../world/Scene.h"
#include "../bottle/BottleUtils.h"

namespace BR2 {
std::shared_ptr<Scene> Scene::create() {
  std::shared_ptr<Scene> s = std::make_shared<Scene>();
  s->init();
  return s;
}
Scene::Scene() : SceneNode(nullptr) {
}
Scene::~Scene() {
  //_pScreen = nullptr;
  _pLightManager = nullptr;
}
void Scene::init() {
  BRLogInfo("Making PhysicsWorld");
  _pPhysicsWorld = PhysicsWorld::create(getThis<Scene>(),
    BottleUtils::getNodeWidth(), BottleUtils::getNodeHeight(), std::move(vec3(0, 1, 0)),
    BottleUtils::getAwarenessRadiusXZ(), BottleUtils::getAwarenessIncrementXZ(),
    BottleUtils::getAwarenessRadiusY(), BottleUtils::getAwarenessIncrementY(),
    BottleUtils::getNodesY(), BottleUtils::getMaxGridCount());

  BRLogInfo("..LightManager");
  _pLightManager = std::make_shared<LightManager>(getThis<Scene>());

  BRLogInfo("..Render Bucket");
  _pRenderBucket = std::make_shared<RenderBucket>();

  SceneNode::init();
}

void Scene::afterAttachedToWindow() {
  //Lazy init, requires our window to be set before creating.
  //**NOTE** we may not need this in the future if we're going turn flyihg camera to script.
  //***POSSIBLE DISCARD***
  BRLogInfo("Making Flying Camera");
  createFlyingCamera();
  Gu::checkErrorsDbg();

  BRLogInfo("Making UI");
  createUi();
  Gu::checkErrorsDbg();

  makeParticles();

  //Activate all nodes by traversing this parent root.
  std::shared_ptr<Scene> scene = getThis<Scene>();
  scene->iterateBreadthFirst<SceneNode>([scene](std::shared_ptr<SceneNode> node) {
    if (node != scene) {
      node->afterAddedToScene(scene);
    }
    return true;
    });
}
void Scene::createUi() {
  string_t DEBUG_FONT = "Lato-Regular.ttf";

  AssertOrThrow2(getWindow() != nullptr);

  _pUiScreen = UiScreen::create(getWindow());
  Gu::checkErrorsDbg();

  //Removing the 'built-in' UI due to the compile and load() conflicts due to GameUiaxis
  //skins first
  std::shared_ptr<UiLabelSkin> debugTextSkin = UiLabelSkin::create(_pUiScreen, FileSystem::combinePath(Gu::getPackage()->getFontsFolder(), DEBUG_FONT), "20px");
  std::shared_ptr<UiCursorSkin> pCursorSkin = std::make_shared<UiCursorSkin>();
  pCursorSkin->_pTex = UiTex::create(_pUiScreen, Gu::getPackage()->makeAssetPath("ui", "wings-cursor.png"));

  //debug label
  _pDebugLabel = UiLabel::create("", debugTextSkin);
  _pDebugLabel->position() = UiPositionMode::e::Relative;
  _pDebugLabel->left() = "20px";
  _pDebugLabel->top() = "100px";
  _pDebugLabel->width() = "200px";
  _pDebugLabel->height() = "1800px";
  _pDebugLabel->enableWordWrap();
  _pUiScreen->addChild(_pDebugLabel);
  Gu::checkErrorsDbg();

  //Cursor
  std::shared_ptr<UiCursor> cs = UiCursor::create(pCursorSkin);
  cs->width() = "32px";
  cs->height() = "auto"; // Auto?
  _pUiScreen->setCursor(cs);
  Gu::checkErrorsDbg();


  //Removing loadImages and compile fom MegaTex
  //_pUiScreen->getTex()->loadImages();
//Gu::checkErrorsDbg();
  //_pUiScreen->getTex()->compile();
  //Gu::checkErrorsDbg();
}
void Scene::update(float delta) {
  if (_pPhysicsWorld != nullptr) {
    _pPhysicsWorld->update(getWindow()->getDelta()->get());
  }
  if (_pParticleManager != nullptr) {
    _pParticleManager->update(getWindow()->getDelta()->get());
  }
  //if (_pLightManager != nullptr) {
  //  _pLightManager->update(getWindow()->getDelta()->get());
  //}

  SceneNode::update(delta, std::map<Hash32, std::shared_ptr<Animator>>());
}
void Scene::idle(int64_t us) {
}
std::shared_ptr<GLContext> Scene::getContext() {
  std::shared_ptr<GraphicsWindow> w = getWindow();
  if (w) {
    std::shared_ptr<GLContext> c = w->getContext();
    return c;
  }
  else {
    BRLogErrorOnce("Window was null when getting graphics context.");
    Gu::debugBreak();
  }
  return nullptr;
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
  BRLogInfo("Creating Flying Camera");

#define NOSCRIPT

#ifdef NOSCRIPT
  BRLogInfo("Creating Fly Camera.");
  _pDefaultCamera = CameraNode::create(_pGraphicsWindow->getViewport());
  std::shared_ptr<FlyingCameraControls> css = std::make_shared<FlyingCameraControls>();
  _pDefaultCamera->addComponent(css);
  setActiveCamera(_pDefaultCamera);
  attachChild(_pDefaultCamera);
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
  //TODO: dont use scriptmanager, just use like ScriptCompilkr and make it a static method.
  std::shared_ptr<CSharpScript> getScriptManager()->loadScript(FileSystem::combinePath(Gu::getAppPackage()->getScriptsFolder(), "FlyCamera.cs"));
  cam->addComponent(script);
#endif
}
void Scene::debugChangeRenderState() {
  std::shared_ptr<InputManager> inp = getInput();

  if (inp->keyPress(SDL_SCANCODE_F1)) {
    if (inp->shiftHeld()) {
      _bDebugDisableShadows = !_bDebugDisableShadows;
    }
    else {
      _bShowDebugText = !_bShowDebugText;
    }
  }
  if (inp->keyPress(SDL_SCANCODE_F2)) {
    _bDrawDebug = !_bDrawDebug;
  }
  if (inp->keyPress(SDL_SCANCODE_F3)) {
    _bDebugDisableCull = !_bDebugDisableCull;
  }
  if (inp->keyPress(SDL_SCANCODE_F4)) {
    _bDebugShowWireframe = !_bDebugShowWireframe;
  }
  if (inp->keyPress(SDL_SCANCODE_F5)) {
    _bDebugClearWhite = !_bDebugClearWhite;
  }
  if (inp->keyPress(SDL_SCANCODE_F6)) {
    _bDebugDisableDepthTest = !_bDebugDisableDepthTest;
  }
  if (inp->keyPress(SDL_SCANCODE_F7)) {
    if (getWindow()->getFrameSync()->isEnabled()) {
      getWindow()->getFrameSync()->disable();
    }
    else {
      getWindow()->getFrameSync()->enable();
    }
  }

  //#ifdef _DEBUG
  //These must be #ifdef out because glPolygonMOde is not present in gl330 core
  if (_bDebugShowWireframe == true) {
    Gu::getCoreContext()->setPolygonMode(PolygonMode::Line);
  }
  else {
    Gu::getCoreContext()->setPolygonMode(PolygonMode::Fill);
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
  //TODO:
  //Deferred rendering goes here.
  debugChangeRenderState();

  Perf::pushPerf();

  getContext()->pushDepthTest();
  getContext()->pushCullFace();
  getContext()->pushBlend();
  {
    getContext()->enableBlend(false);
    getContext()->enableCullFace(true);
    getContext()->enableDepthTest(true);

    //**
    //2/24 in order to set up the rendering system to be instanced we gotta change a lot around, like merge all the uniform buffers, skin joint buffers, and stuff.  Then reference by gl_InstanceID
    //so -- this is a huge task and it's nto really needed yet.
    //**

    //If we don't have skin, draw instanced,
    //if we have skin draw normally.
    //_pRenderBucket->sortAndDrawMeshes(
    //    [](std::shared_ptr<VertexFormat> vf) {
    //        return getContext()->getShaderManager()->getDiffuseShader(vf);
    //    },
    //    [&](std::shared_ptr<ShaderBase> sb) {
    //        sb->bind();
    //        sb->setUf("_ufProj", (void*)&_projMatrix, 1, false);
    //        sb->setUf("_ufView", (void*)&_viewMatrix, 1, false);
    //        sb->setUf("_ufShadowLightPos", (void*)&vFinal, 1, false);
    //    },
    //    [](std::shared_ptr<ShaderBase> sb, std::shared_ptr<MeshNode> bn) {
    //        RenderParams rp;
    //        rp.setShader(sb);
    //        bn->drawDeferred(std::move(rp));
    //    }
    //);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    ////3D Models
    ////  glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //for (std::pair<float, std::shared_ptr<BaseNode>> p : getVisibleNodes()) {
    //    std::shared_ptr<BaseNode> pm = p.second;
    //    std::shared_ptr<WorldObject> wo = std::dynamic_pointer_cast<WorldObject>(pm);
    //    if (wo != nullptr) {
    //        wo->drawDeferred(std::move(rp)); //Don't pass a matrix in, the model is the same now
    //    }
    //}
    //3D Models
    //  glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshes()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawDeferred(rp);
    }
  }
  getContext()->popBlend();
  getContext()->popCullFace();
  getContext()->popDepthTest();

  Perf::popPerf();
}
void Scene::drawForward(RenderParams& rp) {
  //Meshes
  for (std::pair<float, std::shared_ptr<SceneNode>> p : _pPhysicsWorld->getVisibleNodes()) {
    std::shared_ptr<SceneNode> pm = p.second;
    pm->drawForward(rp);
  }

  drawBackgroundImage();

  RenderUtils::drawAxisShader(getActiveCamera());
  RenderUtils::drawGridShader(getActiveCamera());
}
void Scene::drawBackgroundImage() {
  if (_pQuadMeshBackground == nullptr) {
    _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
      getActiveCamera()->getViewport()->getWidth(), getActiveCamera()->getViewport()->getHeight());
    _pTex = Gu::getTexCache()->getOrLoad(TexFile("test_bk",Gu::getPackage()->makeAssetPath("textures", "test_tex3.png")));
  }
  std::shared_ptr<CameraNode> bc = getActiveCamera();
  Gu::getShaderMaker()->getImageShader_F()->setCameraUf(bc);
  Gu::getShaderMaker()->getImageShader_F()->beginRaster(bc->getViewport()->getWidth(), bc->getViewport()->getHeight());
  {
    //We want depth test so we can see what's in front.
    //glEnable(GL_DEPTH_TEST);
    _pTex->bind(TextureChannel::e::Channel0, Gu::getShaderMaker()->getImageShader_F());

    Gu::getShaderMaker()->getImageShader_F()->draw(_pQuadMeshBackground);
  }
  Gu::getShaderMaker()->getImageShader_F()->endRaster();
}
void Scene::drawShadow(RenderParams& rp) {
}
void Scene::drawForwardDebug(RenderParams& rp) {
}
void Scene::drawNonDepth(RenderParams& rp) {
  draw2d();
}
void Scene::drawTransparent(RenderParams& rp) {
  Perf::pushPerf();

  getContext()->pushDepthTest();
  getContext()->pushCullFace();
  getContext()->pushBlend();
  {
    getContext()->enableDepthTest(true);
    getContext()->enableCullFace(true);
    getContext()->enableBlend(false);

    RenderParams rp;
    for (std::pair<float, std::shared_ptr<MeshNode>> p : _pRenderBucket->getMeshesTransparent()) {
      std::shared_ptr<MeshNode> pm = p.second;
      pm->drawTransparent(rp);
    }
  }
  getContext()->popBlend();
  getContext()->popCullFace();
  getContext()->popDepthTest();

  Perf::popPerf();
}
void Scene::drawUI(RenderParams& rp) {
  _pUiScreen->update(getInput());
  _pUiScreen->drawForward();
}
void Scene::draw2d() {
  //  Gu::getGui()->debugForceLayoutChanged();
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
  }
}
void Scene::drawDebugText() {
  int bx = 5;
  int by = 32 + 34;// 0 is the height of the font
  int dy = 16;//Also the font size
  int cy = 0;

  //Commented out in favor of GameUi

//  if (_pDebugLabel != nullptr) {
//    _pDebugLabel->setText("Debug\n");
//  }
//  if (_bShowDebugText) {
//    size_t iVisibleLights = 0;//_pWorld25->getFrameVisibleLights().size();
//
//#define DBGL(...) if (_pDebugLabel != nullptr) { \
//      std::string dtxt = _pDebugLabel->getText(); \
//      dtxt += (__VA_ARGS__);\
//      dtxt += "\r\n"; \
//      _pDebugLabel->setText(dtxt); \
//      }
//
//      //DBGL("%.2f", Gu::getFpsMeter()->getFps());
//
//      //    _pContext->getTextBoss()->setColor(vec4(0, 0, b, 1));
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
  //}
}
void Scene::updateWidthHeight(int32_t w, int32_t h, bool bForce) {
  _pUiScreen->screenChanged(w, h);
}
//std::shared_ptr<ModelNode> Scene::createObj(std::shared_ptr<ModelData> ms) {
//  std::shared_ptr<ModelNode> mn = std::make_shared<ModelNode>(ms);
//  mn->update(0.0, std::map<Hash32, std::shared_ptr<Animator>>());
//
//  attachChild(mn);
//
//  return mn;
//}
//std::shared_ptr<ModelNode> Scene::createObj(std::shared_ptr<ModelData> ms, vec3& pos, vec4& rot, vec3& scale, std::string action) {
//  std::shared_ptr<ModelNode> mn = createObj(ms);
//
//  mn->stopAllActions();
//  if (StringUtil::isNotEmpty(action)) {
//    mn->playAction(action);
//  }
//
//  mn->setScale(std::move(scale));
//  mn->setRot(std::move(rot));
//  mn->setPos(std::move(pos));
//
//  return mn;
//}
std::shared_ptr<LightNodePoint> Scene::createPointLight(vec3&& pos, float radius, vec4&& color, string_t action, bool bShadowsEnabled) {
  std::shared_ptr<LightNodePoint> lp = LightNodePoint::create(bShadowsEnabled);
  lp->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());
  lp->setPos(std::move(pos));
  lp->setLightRadius(radius);
  lp->setLightColor(std::move(color));

  lp->getShadowBox()->getSmallBoxSize() = 0.6f;
  attachChild(lp);

  return lp;
}
std::shared_ptr<LightNodeDir> Scene::createDirLight(const vec3&& pos, const vec3&& lookAt, float fDist, const vec4&& color, const string_t action, bool bShadowsEnabled) {
  std::shared_ptr<LightNodeDir> dir = LightNodeDir::create(bShadowsEnabled);
  dir->setMaxDistance(fDist);
  dir->setPos(std::move(pos));
  dir->setLookAt(std::move(lookAt));
  dir->setLightColor(std::move(color));
  //dir->getSpecExp() = 50.109f;
  dir->update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());

  attachChild(dir);

  return dir;
}
std::shared_ptr<TreeNode> Scene::attachChild(std::shared_ptr<TreeNode> pChild) {
  std::shared_ptr<PhysicsNode> casted = std::dynamic_pointer_cast<PhysicsNode>(pChild);
  if (casted != nullptr) {
    _pPhysicsWorld->addObj(casted, false, false);
  }

  std::shared_ptr<TreeNode> ret = TreeNode::attachChild(pChild);

  if (getWindow()) {
    std::shared_ptr<Scene> scene = getThis<Scene>();
    pChild->iterateBreadthFirst<SceneNode>([scene](std::shared_ptr<SceneNode> node) {
      node->afterAddedToScene(scene);
      return true;
      });
  }

  return ret;
}
bool Scene::detachChild(std::shared_ptr<TreeNode> pChild) {
  std::shared_ptr<PhysicsNode> casted = std::dynamic_pointer_cast<PhysicsNode>(pChild);
  if (casted != nullptr) {
    _pPhysicsWorld->tryRemoveObj(casted);
  }
  bool ret = TreeNode::detachChild(pChild);

  std::shared_ptr<Scene> scene = getThis<Scene>();
  pChild->iterateBreadthFirst<SceneNode>([scene](std::shared_ptr<SceneNode> node) {
    node->afterRemovedFromScene(scene);
    return true;
    });

  return ret;
}

uint64_t Scene::getFrameNumber() {
  return getWindow()->getFpsMeter()->getFrameNumber();
}
void Scene::makeParticles() {
  ivec2 gsiz(4, 4);
  std::shared_ptr<Atlas> _pParticlesAtlas = std::make_shared<Atlas>(Gu::getCoreContext(), "W25Particles", gsiz);
  _pParticlesAtlas->addImage(0, Gu::getPackage()->makeAssetPath("sprites", "sp-particle.png"));
  _pParticlesAtlas->addImage(1, Gu::getPackage()->makeAssetPath("sprites", "sp-particle.png"));
  _pParticlesAtlas->addImage(2, Gu::getPackage()->makeAssetPath("sprites", "sp-particle.png"));
  _pParticlesAtlas->addImage(3, Gu::getPackage()->makeAssetPath("sprites", "sp-particle.png"));
  _pParticlesAtlas->compileFiles();

  BRLogInfo("..ParticleManager");
  _pParticleManager = std::make_shared<ParticleManager>(getWindow()->getContext());
  _pParticleManager->init(_pParticlesAtlas, BottleUtils::c_iMaxParticles);
  Gu::checkErrorsDbg();
}
}//ns BR2