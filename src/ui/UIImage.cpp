//#include "../ui/UIImage.h"
//
//
//namespace Game {
//#pragma region UiImage
//std::shared_ptr<UiImage> UiImage::create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY) {
//  return UiImage::create(tex, eSizeX, eSizeY, (float)tex->getWidth(), (float)tex->getHeight());
//}
//std::shared_ptr<UiImage> UiImage::create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx) {
//  std::shared_ptr<UiImage> pImg = std::make_shared<UiImage>();
//  pImg->init();
//  pImg->width() = "100%";
//  pImg->height() = "100%";
//  pImg->_pTexture = tex;
//  pImg->_fTileWidthPx = fWidthPx;
//  pImg->_fTileHeightPx = fHeightPx;
//  pImg->setSizeMode(eSizeX, eSizeY);
//
//  pImg->_iPickId = Gu::getPicker()->genPickId();
//  return pImg;
//}
//UiImage::UiImage() {
//}
//UiImage::~UiImage() {
//}
//void UiImage::init() {
//  UiElement::init();
//  setName("UiImage");
//}
//void UiImage::getQuadVerts(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) {
//  if (getLayoutVisible() == false) {
//    return;
//  }
//  bool bHasTex = (_pTexture != nullptr && _pTexture->tex() != nullptr);
//  bool bComputedTex = !bHasTex && (_eSizeModeX == UiImageSizeMode::e::Computed && _eSizeModeY == UiImageSizeMode::e::Computed);
//  if (!bHasTex && !bComputedTex) {
//    return;
//  }
//
//
//
//  if (_eSizeModeX == UiImageSizeMode::e::Expand) {
//    _q2Tex._p0.x = _pTexture->tex()->uv0().x;
//    _q2Tex._p1.x = _pTexture->tex()->uv1().x;
//  }
//  else if (_eSizeModeX == UiImageSizeMode::e::Tile) {
//    float wPx = getComputedQuad().right() - getComputedQuad().left();
//    _q2Tex._p0.x = _pTexture->tex()->uv0().x;
//
//    if (_fTileWidthPx > 0) {
//      _q2Tex._p1.x = _pTexture->tex()->uv0().x + wPx / _fTileWidthPx;
//    }
//    else {
//      _q2Tex._p1.x = _pTexture->tex()->uv1().x;
//      static int x = 0;
//      if (x == 0) { Gu::debugBreak(); }
//    }
//  }
//  else if (_eSizeModeX == UiImageSizeMode::e::Computed) {
//    //Tex coords are computed (used for UiGlyph)
//  }
//  else {
//    Gui2d::error("Invalid layout X image size mode.");
//  }
//
//  if (_eSizeModeY == UiImageSizeMode::e::Expand) {
//    _q2Tex._p0.y = _pTexture->tex()->uv1().y;
//    _q2Tex._p1.y = _pTexture->tex()->uv0().y;
//  }
//  else if (_eSizeModeY == UiImageSizeMode::e::Tile) {
//    float hPx = getComputedQuad().bottom() - getComputedQuad().top();
//    _q2Tex._p0.y = _pTexture->tex()->uv1().y;
//
//    if (_fTileHeightPx > 0) {
//      _q2Tex._p1.y = _pTexture->tex()->uv1().y + hPx / _fTileHeightPx;
//    }
//    else {
//      _q2Tex._p1.y = _pTexture->tex()->uv0().y;
//      static int x = 0;
//      if (x == 0) { Gu::debugBreak(); }//Texture height was zero
//    }
//  }
//  else if (_eSizeModeY == UiImageSizeMode::e::Computed) {
//    //Tex coords are computed (used for UiGlyph)
//  }
//  else if (_eSizeModeY == UiImageSizeMode::e::Proportion) {
//    //proportion the Y to the X
//    _q2Tex._p0.y = _pTexture->tex()->uv1().y;
//    float fw = _q2Tex._p1.x - _q2Tex._p0.x;
//    float fr = _pTexture->tex()->getSizeRatio();
//    float fh = fw * fr;
//    _q2Tex._p1.y = _q2Tex._p0.y + fh;
//  }
//  else {
//    Gui2d::error("Invalid layout size mode.");
//  }
//
//  //if(_pQuad != nullptr) {
//  //Add the vertexes of this element tot he given buffers
//  //Creates an image out of this element.
//  const Box2f& cpyPos = getGLRasterQuad();
//  v_GuiVert v;
//  v._rect.x = cpyPos._p0.x;
//  v._rect.y = cpyPos._p0.y;
//  v._rect.z = cpyPos._p1.x;
//  v._rect.w = cpyPos._p1.y;
//
//  //Clip Rect.  For discard
//  v._clip = makeClipRectForRender(b2ClipRect);
//  if (Gu::getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping()) {
//    //Disable clip
//    v._clip.x = -9999;
//    v._clip.y = -9999;
//    v._clip.z = 9999;
//    v._clip.w = 9999;
//  }
//
//  //Texs
//  v._tex.x = _q2Tex._p0.x;//GL - bottom left
//  v._tex.y = _q2Tex._p0.y;
//  v._tex.z = _q2Tex._p1.x;//GL - top right *this essentially flips it upside down
//  v._tex.w = _q2Tex._p1.y;
//
//  if (_pTexture != nullptr) {
//    v._texsiz.x = fabsf(_pTexture->tex()->uv1().x - _pTexture->tex()->uv0().x);
//    v._texsiz.y = fabsf(_pTexture->tex()->uv0().y - _pTexture->tex()->uv1().y);//Uv0 - uv1 - because we flipped coords bove
//  }
//  else {
//    //*this is for "Fixed" textures, e.g. fonts and text.  We compute them in UiLabel first
//    v._texsiz.x = fabsf(v._tex.z - v._tex.x);
//    v._texsiz.y = fabsf(v._tex.w - v._tex.y);//y is flipfloped again
//  }
//
//
//  //**Texture Adjust - modulating repeated textures causes seaming issues, especially with texture filtering
//  //adjust the texture coordinates by some pixels to account for that.  0.5f seems to work well.
//  static float pixAdjust = 0.51f;  // # of pixels to adjust texture by
////#ifdef _DEBUG
////    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
////        pixAdjust -= 0.005;
////    }
////    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
////        pixAdjust += 0.005;
////    }
////#endif
//  float w1px = 0; // 1 pixel subtract from the u/v to prevent creases during texture modulation
//  float h1px = 0;
//  if (_pTexture != nullptr) {
//    if (_pTexture->tex()->getWidth() > 0 && v._texsiz.x > 0) {
//      w1px = 1.0f / _pTexture->tex()->getWidth();
//      w1px *= v._texsiz.x;
//      w1px *= pixAdjust;
//    }
//    if (_pTexture->tex()->getHeight() > 0 && v._texsiz.y > 0) {
//      h1px = 1.0f / _pTexture->tex()->getHeight();
//      h1px *= v._texsiz.y;
//      h1px *= pixAdjust;
//    }
//  }
//  v._texsiz.x -= w1px * 2.0f;
//  v._texsiz.y -= h1px * 2.0f;
//  v._tex.x += w1px;
//  v._tex.y += h1px;
//  v._tex.z -= w1px;
//  v._tex.w -= h1px;
//
//  //**End texture adjust
//
//  //Pick Color
//  v._pick_color.x = _iPickId;
//
//  //Display Color
//  vec4 vc = getColor();
//  v._pick_color.y = //    0xFFFFFFFF; //White
//    ((int)(vc.x * 255.0f) << 24) |
//    ((int)(vc.y * 255.0f) << 16) |
//    ((int)(vc.z * 255.0f) << 8) |
//    ((int)(vc.w * 255.0f) << 8);
//  inds.push_back((v_index32)verts.size());
//  verts.push_back(v);
//
//}
//bool UiImage::pick(std::shared_ptr<Fingers> fingers) {
//  if (getLayoutVisible()) {
//    if (getRenderVisible()) {
//      if (_iPickId > 0) {
//        uint32_t pixid = Gu::getPicker()->getSelectedPixelId();
//        if (pixid != 0) {
//          if (pixid == _iPickId) {
//
//            _iPickedFrameId = Gu::getFrameNumber();
//
//            return true;
//          }
//        }
//        else {
//          int nnn = 0;
//          nnn++;
//        }
//      }
//    }
//  }
//  return false;
//}
//float UiImage::getAutoWidth() {
//  if (_pTexture != nullptr) {
//    //Assuming height is set
//    return height().px() * _pTexture->getSizeRatio();
//  }
//  return 0.0f;
//}
//float UiImage::getAutoHeight() {
//  if (_pTexture != nullptr) {
//    if (_pTexture->getSizeRatio() != 0.0f) {
//      return width().px() * (1.0f / _pTexture->getSizeRatio());
//    }
//  }
//  return 0.0f;
//}
//void UiImage::setSizeMode(UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY) {
//  _eSizeModeY = eSizeY;
//  _eSizeModeX = eSizeX;
//  TexWrap::e wu = TexWrap::Clamp, wv = TexWrap::Clamp;
//  if (eSizeX == UiImageSizeMode::e::Tile) { wu = TexWrap::Repeat; }
//  if (eSizeY == UiImageSizeMode::e::Tile) { wv = TexWrap::Repeat; }
//
//  setWrapMode(wu, wv);
//
//  setLayoutChanged();
//}
//void UiImage::setWrapMode(TexWrap::e u, TexWrap::e v) {
//  if (_pTexture != nullptr) {
//    //Null for UiGlyph
//    _pTexture->setWrapU(u);
//    _pTexture->setWrapV(v);
//    setLayoutChanged();
//  }
//}
//
//void UiImage::performLayout(bool bForce) {
//  UiElement::performLayout(bForce);
//}
//
//void UiImage::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) {
//  getQuadVerts(verts, inds, b2ClipRect);
//  UiElement::regenMesh(verts, inds, b2ClipRect);
//
//}
//
//#pragma endregion
//
//
//}//ns Game
