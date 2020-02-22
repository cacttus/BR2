#include "../base/InputManager.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/FpsMeter.h"

#include "../gfx/Picker.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/DeferredFramebuffer.h"
#include "../gfx/RenderPipeline.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderPipeline.h"
#include "../gfx/GLContext.h"


namespace BR2 {
Picker::Picker(std::shared_ptr<RenderPipeline> pc) {
  _pRenderPipe = pc;
  _uiLastSelectedPixelId = 0;
}
Picker::~Picker() {
}
uint32_t Picker::genPickId() {

  //DEBUG pick ID that shows the color of the picked object.
  _iid++;
  if (_iid == INVALID_PICK_ID) {
    _iid = 0;
  }

  return _iid;
}
void Picker::update(std::shared_ptr<InputManager> pInput) {
  updatePickedPixel((int32_t)pInput->getMousePos().x, (int32_t)pInput->getMousePos().y);
}
void Picker::updatePickedPixel(int32_t x, int32_t y) {
  //vec2 mp = Gu::GetMousePosInWindow();
  //if (!Gu::GetRenderManager()->getViewport()->containsPointRelativeToWindow(mp)){
  //    return;
  //}

  RenderUtils::debugGetRenderState(_pRenderPipe->getContext());

  _pRenderPipe->getContext()->glBindFramebuffer(GL_READ_FRAMEBUFFER, _pRenderPipe->getBlittedDeferred()->getFramebufferId());
  _pRenderPipe->getContext()->chkErrDbg();

  glReadBuffer(GL_COLOR_ATTACHMENT4);

  _pRenderPipe->getContext()->chkErrDbg();

  RenderUtils::debugGetRenderState(_pRenderPipe->getContext());

  samplePixelId(x, y, _uiLastSelectedPixelId);

  _pRenderPipe->getContext()->chkErrDbg();

#ifdef _DEBUG
  if (_uiLastSelectedPixelId > 0) {
    if (_pRenderPipe->getContext()->getFpsMeter()->frameMod(20)) {
      Br2LogDebug("(" + x + "," + y + "), picked " + _uiLastSelectedPixelId);
    }
  }

  _pRenderPipe->getContext()->chkErrDbg();

#endif

  glReadBuffer(GL_NONE);
  _pRenderPipe->getContext()->chkErrDbg();


}
void Picker::samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId) {
  GLuint pixel = 0;

  //https://www.khronos.org/opengles/sdk/docs/man/xhtml/glReadPixels.xml
  //If the currently bound framebuffer is not the default framebuffer object, color components 
  // are read from the color image attached to the GL_COLOR_ATTACHMENT0 attachment point.
  RenderUtils::debugGetRenderState(_pRenderPipe->getContext());

  int32_t iHeight = _pRenderPipe->getBufferHeight();

  glReadPixels(x - 1,
    iHeight - y + 1,
    1, 1,
    GL_RED_INTEGER,
    GL_UNSIGNED_INT,
    (GLvoid*)&pixel
  );

  _pRenderPipe->getContext()->chkErrDbg();


  selectedId = pixel;
}

}//ns BR2
