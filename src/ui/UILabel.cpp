//#include "../ui/UILabel.h"
//
//
//namespace Game {
//#pragma region UiGlyph
//std::shared_ptr<UiGlyph> UiGlyph::create(uint32_t iChar) {
//  std::shared_ptr<UiGlyph> g = std::make_shared<UiGlyph>();
//  //Avoid any init crap becuase we're SUPER SLOW
//
//  //g->init();
//  //g->setName("G");//**Save space?
//  return g;
//}
//void UiGlyph::performLayout(bool bForce) {
//  //Skips UiImage PerformLayout
//  UiElement::performLayout(bForce);
//}
//void UiGlyph::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) {
//  getQuadVerts(verts, inds, b2ClipRect);
//}
//#pragma endregion
//
/////////////////////////////////////////////////////////////////////
//std::shared_ptr<UiLabel> UiLabel::create(std::string text, std::shared_ptr<UiLabelSkin> inf) {
//  std::shared_ptr<UiLabel> lbl = std::make_shared<UiLabel>();
//  lbl->_pFontInfo = inf;
//  lbl->_strText = text;
//  // lbl->_strText = "UiLabel";
//  lbl->init();
//
//  return lbl;
//}
//void UiLabel::init() {
//  UiElement::init();
//  setName("UiLabel");
//
//}
//void UiLabel::setText(std::string s) {
//  _strText = s;
//}
//
//void UiLabel::performLayout(bool bForce) {
//  if (_pFontInfo != nullptr) {
//    //glyph creation is in performlayout because the HEIGHT of the label needs to be
//    //calculated first
//#ifdef _DEBUG
//    if (StringUtil::contains(_strText, "Apple")) {
//      int n = 0;
//      n++;
//    }
//    if (StringUtil::contains(_strText, "DOF")) {
//      int n = 0;
//      n++;
//    }
//#endif
//    // This has to come before performlayout in order to get the correct width.
//    createGlyphs();
//  }
//  //Use the system to flow the layout.
//  t_timeval us0 = Gu::getMicroSeconds();
//  UiElement::performLayout(bForce);
//  t_timeval us1 = Gu::getMicroSeconds() - us0;
//  int nn = 0;
//  nn++;
//}
//void UiLabel::update(std::shared_ptr<Fingers> pFingers) {
//
//  if (StringUtil::equals(_strTextLast, _strText) == false) {
//    _strTextLast = _strText;
//    setLayoutChanged();
//  }
//
//  UiElement::update(pFingers);
//}
//void UiLabel::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) {
//  UiElement::regenMesh(verts, inds, b2ClipRect);
//}
//void UiLabel::createGlyphs() {
//  //Easy out - if text is the same.
//  //However, we still need to send the mesh becasue we use absolute coordinates instead of a positional uniform.
//
//  //Hide glyphs - this is an optimization because adding/removing children was causing major slowdown
//  for (auto c : getChildren()) {
//    c.second->setLayoutVisible(false, false);
//  }
//
//  float labelHeight = bottom().px() - top().px();
//
//  //Font height is the height of the label.
//  float fontHeight;
//  uDim& d = _pFontInfo->getFontSize();
//  if (d.getDimUnit() == UiDimUnit::e::Pixel) {
//    fontHeight = d.px();
//  }
//  else if (d.getDimUnit() == UiDimUnit::e::Percent) {
//    fontHeight = d.pctOf(labelHeight);
//  }
//  else if (d.getDimUnit() == UiDimUnit::e::Auto) {
//    fontHeight = labelHeight;
//  }
//  std::shared_ptr<MtFont> ft = _pFontInfo->getFont();
//  /*
//  lineheight - in the layout the height of space between lines
//  padding
//  text-align - top, center, bottom
//  */
//  //Add glyphs
//  float curX = 0, curY = 0;//setting curY = labelHeight SORT OF fixes the issue.  It's not right
//  Box2f outQuad;
//  Box2f outTexs;
//  float outW, outH, outPadT, outPadR, outPadB, outPadL;
//  bool bNextWrap = false;
//  int nCh = 0;
//  float labelWidth = 0;
//  float labelWidthCur = 0;
//  //float labelHeight = 0;
//  //float labelHeightCur = 0;
//  int32_t cCodePrev = -1;//Previous code for kerning
//  for (int32_t c : _strText) {
//    ft->getCharQuad(c, cCodePrev, fontHeight, outW, outH, outTexs, outPadT, outPadR, outPadB, outPadL);
//    cCodePrev = c;
//
//    std::shared_ptr<UiGlyph> g;
//
//    if (nCh < getChildren().size()) {
//      auto it = getChildren().begin();
//      std::advance(it, nCh);
//      g = std::dynamic_pointer_cast<UiGlyph>(it->second);
//      g->setLayoutVisible(true, false);
//    }
//    else {
//      g = std::make_shared<UiGlyph>();//UiGlyph::create(c);
//      addChild(g, 0, false, false);
//      g->setSizeMode(UiImageSizeMode::e::Computed, UiImageSizeMode::e::Computed);
//    }
//
//    g->position() = UiPositionMode::Static;
//    g->width().setDimUnit(UiDimUnit::e::Pixel);
//    g->height().setDimUnit(UiDimUnit::e::Pixel);
//
//    g->width() = outW;
//    g->height() = outH;
//
//    g->getTexs() = outTexs;
//    g->getColor() = getColor(); //Copy color over Note: font color? I do't know I think we should use gui color
//
//  //  g->padRight() = outPadR;
//    //g->padBottom() = outPadB;
//    g->padTop() = outPadT;//fontHeight - outH;    //this should never be greater
//    g->padRight() = outPadR;//fontHeight - outH;    //this should never be greater
//    g->padBottom() = outPadB;//fontHeight - outH;    //this should never be greater
//    g->padLeft() = outPadL;//fontHeight - outH;    //this should never be greater
//
//    labelWidthCur += g->right().px() - g->left().px() + g->padRight().px() + g->padLeft().px();
//    ////Don't alter texcoords
//    if (_bWordWrap) {
//      //This won't work it'll let chars go past their container.
//      //I think the only method to make this work is to encapsulate Words within UiElements (simply)
//      g->display() = UiDisplayMode::e::InlineNoWrap;
//      if (bNextWrap) {
//        labelWidth = MathUtils::broMax(labelWidth, labelWidthCur);
//        labelWidthCur = 0;
//        g->display() = (UiDisplayMode::e::Block);
//        bNextWrap = false;
//      }
//
//      if (c == '\r' || c == '\n') {
//        bNextWrap = true;
//      }
//      else if (StringUtil::isWsExceptNewline(c)) {
//        g->display() = (UiDisplayMode::e::InlineWrap);
//      }
//      else {
//        g->display() = (UiDisplayMode::e::InlineNoWrap);
//      }
//    }
//    else {
//      g->display() = (UiDisplayMode::e::InlineNoWrap);//Default
//    }
//
//    //Top-Left origin conversion (STB uses bottom left of quad, place our quads at the bottom of the labe.
//    //*We could even have centering, and other jargon as to where to place the text.  For now just place it at the bottom.
//   // g->top() = g->top().px() + fontHeight; // add max height to the top element
//   // g->bottom() = g->bottom().px() + fontHeight; // add max height to the top element
//
//
//    nCh++;
//  }
//
//  if (labelWidth < 0.0f) {
//    labelWidth = 0.0f;//hasn't happened yet
//  }
//  minWidth() = uDim(labelWidth, UiDimUnit::e::Pixel);
//
//
//
//}
//
/////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
//
//
//}//ns Game
