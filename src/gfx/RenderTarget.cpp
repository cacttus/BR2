#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../gfx/RenderTarget.h"

namespace Game {
RenderTarget::~RenderTarget() {
  glDeleteTextures(1, &_iGlTexId);
}
bool RenderTarget::getMsaaEnabled() {
  if (_eTextureTarget == GL_TEXTURE_2D_MULTISAMPLE) {
    return true;
  }
  else if (_eTextureTarget == GL_TEXTURE_2D) {
    return false;
  }
  else {
    BroThrowNotImplementedException();
  }
}

void RenderTarget::bind(GLenum eAttachment) {
  if (_eTargetType == RenderTargetType::e::Depth) {
    if (getMsaaEnabled()) {
      std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, _iGlTexId, 0);
    }
    else {
      std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _iGlTexId, 0);
    }
  }
  else {
    if (eAttachment == 0) {
      eAttachment = _eAttachment;
    }
    if (getMsaaEnabled()) {
      std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, eAttachment, GL_TEXTURE_2D_MULTISAMPLE, _iGlTexId, 0);
    }
    else {
      std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, eAttachment, GL_TEXTURE_2D, _iGlTexId, 0);
    }
  }
  Gu::checkErrorsDbg();
}






}//ns Game
