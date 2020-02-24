#include "../base/AppMain.h"
#include "../base/AppBase.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../model/MeshUtils.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/UiControls.h"
#include "../gfx/GraphicsApi.h"
#include "../base/GraphicsWindow.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/Fingers.h"
#include "../gfx/MegaTex.h"

namespace BR2 {
AppUi::AppUi() {

  string_t DEBUG_FONT = "Lato-Regular.ttf";

  //skins first
  std::shared_ptr<UiLabelSkin> debugTextSkin = UiLabelSkin::create(Gu::getGui(), Gu::getApp()->makeAssetPath("fnt", DEBUG_FONT), "20px");
  std::shared_ptr<UiCursorSkin> pCursorSkin = std::make_shared<UiCursorSkin>();
  pCursorSkin->_pTex = UiTex::create(Gu::getGui(), Gu::getApp()->makeAssetPath("ui", "wings-cursor.png"));

  Gu::getGui()->getTex()->loadImages();

  //debug label
  _pDebugLabel = UiLabel::create("", debugTextSkin);
  _pDebugLabel->position() = UiPositionMode::e::Relative;
  _pDebugLabel->left() = "20px";
  _pDebugLabel->top() = "100px";
  _pDebugLabel->width() = "200px";
  _pDebugLabel->height() = "1800px";
  _pDebugLabel->enableWordWrap();
  Gu::getGui()->addChild(_pDebugLabel);

  //Cursor 
  std::shared_ptr<UiCursor> cs = UiCursor::create(pCursorSkin);
  cs->width() = "32px";
  cs->height() = "auto"; // Auto?
  Gu::getGui()->setCursor(cs);

  Gu::getGui()->getTex()->compile();

}
void AppUi::clearDebugText() {
  if (_pDebugLabel != nullptr) {
    _pDebugLabel->setText("Debug\n");
  }
}
void AppUi::dbgLine(std::string txt) {
  if (_pDebugLabel != nullptr) {
    std::string dtxt = _pDebugLabel->getText();
    dtxt += txt;
    dtxt += "\r\n";
    _pDebugLabel->setText(dtxt);
  }
}
void AppUi::endDebugText() {
  if (_pDebugLabel != nullptr) {
    int nn = 0;
    nn++;
  }
}
//#######
AppMain::AppMain() {
}
AppMain::~AppMain() {
}
void AppMain::init() {
  _pAppUi = std::make_shared<AppUi>();
  setDebugMode();
}
void AppMain::drawBackgroundImage() {
  if (_pQuadMeshBackground == nullptr) {
    _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
      Gu::getCamera()->getViewport()->getWidth(), Gu::getCamera()->getViewport()->getHeight());
    _pTex = Gu::getTexCache()->getOrLoad(makeAssetPath("tex", "test_tex3.png"));
  }
  std::shared_ptr<CameraNode> bc = Gu::getCamera();
  Gu::getShaderMaker()->getImageShader_F()->setCameraUf(bc);
  Gu::getShaderMaker()->getImageShader_F()->beginRaster();
  {
    //We want depth test so we can see what's in front.
    //glEnable(GL_DEPTH_TEST);
    _pTex->bind(TextureChannel::e::Channel0, Gu::getShaderMaker()->getImageShader_F());

    Gu::getShaderMaker()->getImageShader_F()->draw(_pQuadMeshBackground);
  }
  Gu::getShaderMaker()->getImageShader_F()->endRaster();

}
void AppMain::debugChangeRenderState() {
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F1)) {
    if (Gu::getFingers()->shiftHeld()) {
      _bDebugDisableShadows = !_bDebugDisableShadows;
    }
    else {
      _bShowDebugText = !_bShowDebugText;
    }
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F2)) {
    _bDrawDebug = !_bDrawDebug;
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F3)) {
    _bDebugDisableCull = !_bDebugDisableCull;
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F4)) {
    _bDebugShowWireframe = !_bDebugShowWireframe;
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F5)) {
    _bDebugClearWhite = !_bDebugClearWhite;
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F6)) {
    _bDebugDisableDepthTest = !_bDebugDisableDepthTest;
  }
  if (Gu::getFingers()->keyPress(SDL_SCANCODE_F7)) {
    if (Gu::getFrameSync()->isEnabled()) {
      Gu::getFrameSync()->disable();
    }
    else {
      Gu::getFrameSync()->enable();
    }
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



#ifdef BR2_OS_WINDOWS
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
    Graphics->getClearR() = 1.f;
    Graphics->getClearG() = 1.f;
    Graphics->getClearB() = 1.f;
    Graphics->getClearA() = 1.f;
  }
  else {
    Graphics->getClearR() = 0.01f;
    Graphics->getClearG() = 0.01f;
    Graphics->getClearB() = 0.01f;
    Graphics->getClearA() = 1.0f;
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
void AppMain::drawDeferred(RenderParams& rp) {
  debugChangeRenderState();
}
void AppMain::drawForward(RenderParams& rp) {
  debugChangeRenderState();

  if (_pQuadMeshBackground == nullptr) {
    _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
      Gu::getCamera()->getViewport()->getWidth(), Gu::getCamera()->getViewport()->getHeight());
    _pTex = Gu::getTexCache()->getOrLoad(makeAssetPath("tex", "test_tex3.png"));
  }

  RenderUtils::drawAxisShader();
}
void AppMain::drawShadow(RenderParams& rp) {
}
void AppMain::drawDebug(RenderParams& rp) {
}
void AppMain::drawNonDepth(RenderParams& rp) {
  draw2d();
}
void AppMain::drawTransparent(RenderParams& rp) {
}
void AppMain::draw2d() {
  drawDebugText();

  //  Gu::getGui()->debugForceLayoutChanged();
  Gu::getGui()->update(Gu::getFingers());
  Gu::getGui()->drawForward();
}
void AppMain::setDebugMode() {
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
void AppMain::drawDebugText() {
  int bx = 5;
  int by = 32 + 34;// 0 is the height of the font
  int dy = 16;//Also the font size
  int cy = 0;

  _pAppUi->clearDebugText();
  if (_bShowDebugText) {
    size_t iVisibleLights = 0;//_pWorld25->getFrameVisibleLights().size();

    //////////////////////////////////////////////////////////////////////////
#define DBGL(...) _pAppUi->dbgLine(StringUtil::format(__VA_ARGS__))
//////////////////////////////////////////////////////////////////////////
    DBGL("%.2f", Gu::getFpsMeter()->getFps());

    //    _pContext->getTextBoss()->setColor(vec4(0, 0, b, 1));
    DBGL("Global");
    DBGL("  Debug: %s", _bDrawDebug ? "Enabled" : "Disabled");
    DBGL("  Culling: %s", _bDebugDisableCull ? "Disabled" : "Enabled");
    DBGL("  Depth Test: %s", _bDebugDisableDepthTest ? "Disabled" : "Enabled");
    DBGL("  Render: %s", _bDebugShowWireframe ? "Wire" : "Solid");
    DBGL("  Clear: %s", _bDebugClearWhite ? "White" : "Black-ish");
    DBGL("  Vsync: %s", (Gu::getFrameSync()->isEnabled()) ? "Enabled" : "Disabled");
    DBGL("  Shadows: %s", (_bDebugDisableShadows) ? "Enabled" : "Disabled");

    DBGL("  Camera: %s", Gu::getCamera()->getPos().toString(5).c_str());

  }
  _pAppUi->endDebugText();
}
void AppMain::drawForwardDebug(RenderParams& rp){

}
void AppMain::drawUI(RenderParams& rp) {

}







}//ns Game
