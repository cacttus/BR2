#include "../base/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/FpsMeter.h"

#include "../gfx/Picker.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/DeferredFramebuffer.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/CameraNode.h"

namespace BR2 {
Picker::Picker(std::shared_ptr<GLContext> pc) : _pContext(pc), _uiLastSelectedPixelId(0) {
}
Picker::~Picker() {
}
uint32_t Picker::genPickId() {

  //DEBUG pick ID that shows the color of the picked object.
  _iid++;
  if (_iid == Picker::c_iInvalidPickId) {
    _iid = 0;
  }

  return _iid;
}
void Picker::update(std::shared_ptr<InputManager> pFingers) {
  updatePickedPixel((int32_t)pFingers->getMousePos().x, (int32_t)pFingers->getMousePos().y);
}
void Picker::updatePickedPixel(int32_t x, int32_t y) {
  if (Gu::getRenderPipe() == nullptr) {
    return;
  }
  //vec2 mp = Gu::GetMousePosInWindow();
  //if (!Gu::GetRenderManager()->getViewport()->containsPointRelativeToWindow(mp)){
  //    return;
  //}

  RenderUtils::debugGetRenderState();

  _pContext->glBindFramebuffer(GL_READ_FRAMEBUFFER, Gu::getRenderPipe()->getBlittedDeferred()->getFramebufferId());
  Gu::checkErrorsDbg();

  glReadBuffer(GL_COLOR_ATTACHMENT4);

  Gu::checkErrorsDbg();

  RenderUtils::debugGetRenderState();

  samplePixelId(x, y, _uiLastSelectedPixelId);

  Gu::checkErrorsDbg();

#ifdef _DEBUG
  if (_uiLastSelectedPixelId > 0) {
    if (Gu::getFpsMeter()->frameMod(20)) {
      BRLogDebug("(" + x + "," + y + "), picked " + _uiLastSelectedPixelId);
    }
  }

  Gu::checkErrorsDbg();
#endif

  glReadBuffer(GL_NONE);
  Gu::checkErrorsDbg();

}
void Picker::samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId) {
  GLuint pixel = 0;

  //https://www.khronos.org/opengles/sdk/docs/man/xhtml/glReadPixels.xml
  //If the currently bound framebuffer is not the default framebuffer object, color components 
  // are read from the color image attached to the GL_COLOR_ATTACHMENT0 attachment point.
  RenderUtils::debugGetRenderState();

  int32_t iHeight = Gu::getCamera()->getViewport()->getHeight();

  glReadPixels(x - 1,
    iHeight - y + 1,
    1, 1,
    GL_RED_INTEGER,
    GL_UNSIGNED_INT,
    (GLvoid*)&pixel
  );

  Gu::checkErrorsDbg();

  selectedId = pixel;
}

}//ns Game
