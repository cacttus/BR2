#include "../base/GLContext.h"
#include "../display/Picker.h"
#include "../display/Viewport.h"
#include "../display/RenderUtils.h"
#include "../display/DeferredFramebuffer.h"
#include "../display/RenderPipe.h"
#include "../display/Viewport.h"
#include "../display/CameraNode.h"


namespace Game {
///////////////////////////////////////////////////////////////////
Picker::Picker(std::shared_ptr<GLContext> pc) : _pContext(pc), _uiLastSelectedPixelId(0) {
}
Picker::~Picker() {
}
///////////////////////////////////////////////////////////////////
uint32_t Picker::genPickId() {

           //DEBUG pick ID that shows the color of the picked object.
    _iid++;
    if (_iid == Picker::c_iInvalidPickId) {
        _iid = 0;
    }

    return _iid;
}
void Picker::update(std::shared_ptr<Fingers> pFingers) {
    updatePickedPixel((int32_t)pFingers->getMousePos().x, (int32_t)pFingers->getMousePos().y);
}
void Picker::updatePickedPixel(int32_t x, int32_t y) {
    if(Gu::getContext()->getRenderPipe()==nullptr){
        return;
    }
    //vec2 mp = Gu::GetMousePosInWindow();
    //if (!Gu::GetRenderManager()->getViewport()->containsPointRelativeToWindow(mp)){
    //    return;
    //}
    
    RenderUtils::debugGetRenderState();

    _pContext->glBindFramebuffer(GL_READ_FRAMEBUFFER, Gu::getContext()->getRenderPipe()->getBlittedDeferred()->getFramebufferId() );
    Gu::checkErrorsDbg();

    glReadBuffer(GL_COLOR_ATTACHMENT4);

    Gu::checkErrorsDbg();

    RenderUtils::debugGetRenderState();

    samplePixelId(x, y, _uiLastSelectedPixelId);

    Gu::checkErrorsDbg();

    #ifdef _DEBUG
    if (_uiLastSelectedPixelId > 0)
    {
        if (_pContext->getFpsMeter()->frameMod(20)) {
            BroLogDebug("(", x, ",", y, "), picked ", _uiLastSelectedPixelId);
        }
    }

    Gu::checkErrorsDbg();
    #endif

    glReadBuffer(GL_NONE);
    Gu::checkErrorsDbg();

}
void Picker::samplePixelId(int32_t x, int32_t y, t_uint32& __out_ selectedId)
{
    GLuint pixel = 0;

    //https://www.khronos.org/opengles/sdk/docs/man/xhtml/glReadPixels.xml
    //If the currently bound framebuffer is not the default framebuffer object, color components 
    // are read from the color image attached to the GL_COLOR_ATTACHMENT0 attachment point.
    RenderUtils::debugGetRenderState();

    int32_t iHeight = _pContext->getCamera()->getViewport()->getHeight();

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
