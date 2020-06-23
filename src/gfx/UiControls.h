/**
*  @file UiControls.h
*  @date February 17, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __UICONTROLS_15188874023404093923_H__
#define __UICONTROLS_15188874023404093923_H__

#include "../gfx/UiSkin.h"

namespace BR2 {
class UiEventFunc : public VirtualMemoryShared<UiEventFunc> {
  std::function<void(UiEventId::e eventId, void*)> _func;
public:
  //This class is needed since there's no way to compare a std::function (reliably)
  //uint64_t _id = 0 ;
  //std::function<void(UiEventId::e eventId, void*)> func() { return _func; }
  void call(UiEventId::e e, void* v) {
    _func(e, v);
  }
  static std::shared_ptr<UiEventFunc> create(std::function<void(UiEventId::e eventId, void*)> f);
};
struct UiLine {
  float _top = 0;
  float _left = 0;
  float _height = 0;
  float _width = 0;
  std::vector<std::shared_ptr<UiElement>> _eles;
};
class UiElement : public VirtualMemoryShared<UiElement> {
public:
  typedef std::vector<std::shared_ptr<UiEventFunc>> EventFuncList;
  typedef std::map<UiEventId::e, EventFuncList> EventMap;
  static const uint32_t c_uiBaseLayer0SortId = 1000;
  typedef std::function<void()> MouseFunc;
  typedef std::multimap<uint32_t, std::shared_ptr<UiElement>> UISortedMap; //children sorted by sort order.
public:
  static std::shared_ptr<UiElement> create();
  UiElement();
  virtual ~UiElement() override;

  virtual void init();
  virtual void update(std::shared_ptr<InputManager> pFingers);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce);

  std::shared_ptr<UiScreen> getScreen();
  std::string getName() { return _strName; }
  void setName(std::string n) { _strName = n; }
  std::shared_ptr<UiElement> getParent() { return _pParent; }

  virtual void drawForward(RenderParams& rp, Box2f& b2ClipRect);
  void drawDebug(RenderParams& rp, std::shared_ptr<UiScreen> pscreen);
  virtual std::shared_ptr<UiElement> addChild(std::shared_ptr<UiElement> c, uint32_t uiSort = UiElement::c_uiBaseLayer0SortId, bool bUpdateLayout = true, bool bCheckDupes = true, bool bCheckCycles = true);
  virtual void setLayoutChanged(bool bChildren = false);//Don't use bChildren unless we're Gui2D

  void enableDrag(UiDragInfo::DragFunc func);

  uDim& top() { return _top; }          //**These are only ever pixels
  uDim& right() { return _right; }      //**These are only ever pixels
  uDim& bottom() { return _bottom; }    //**These are only ever pixels
  uDim& left() { return _left; }        //**These are only ever pixels
  uDim& width() { return _width; }
  uDim& height() { return _height; }
  uDim& minWidth() { return _minWidth; }
  uDim& minHeight() { return _minHeight; }
  uDim& maxWidth() { return _maxWidth; }
  uDim& maxHeight() { return _maxHeight; }
  vec4& getColor() { return _vColor; }

  std::shared_ptr<UiElement> cloneSelfOnly();
  const Box2f& getComputedQuad() { return _b2ComputedQuad; }//In design space
  const Box2f& getLayoutQuad() { return _b2LayoutQuad; }//In sceen space
  const Box2f& getGLRasterQuad() { return _b2RasterQuad; }//Finally, in OpenGL space
  const Box2f& getContentQuad() { return _b2ContentQuad; }//Finally, in OpenGL space

  virtual bool getPickable() { return false; }

  UiPositionMode::e& position() { return _ePosition; }
  UiOverflowMode::e& overflow() { return _eOverflowMode; }
  UiDisplayMode::e& display() { return _eDisplayMode; }

  std::multimap<uint32_t, std::shared_ptr<UiElement>>& getChildren() { return _mapChildren; }
  void clearChildren();
  bool removeChild(std::shared_ptr<UiElement> ele, bool bUpdateLayout = true);
  bool hasChild(std::shared_ptr<UiElement> ele);

  virtual bool pick(std::shared_ptr<InputManager> fingers, std::shared_ptr<UiScreen> pscreen);
  void addEvent(UiEventId::e ev, std::shared_ptr<UiEventFunc> f);
  bool removeEvent(UiEventId::e evId, std::shared_ptr<UiEventFunc> f);

  void doMouseEvents(std::shared_ptr<InputManager> pFingers);
  void getTreeAsList(std::vector<std::shared_ptr<UiElement>>& __out_ children);
  void setSort(uint32_t uisort);
  bool getLayoutVisible() { return _bVisible; };//Remove elem from layout + hide it
  virtual bool setLayoutVisible(bool b, bool bLayoutChanged = true);                //Remove elem from layout + hide it
  bool hide();//Removes element from the parent layout, and also hides it.
  bool show();//Removes element from the parent layout, and also hides it.
  bool getRenderVisible() { return _bRenderVisible; };   //Simply hide element, but still compute it in the layout (take up space)
  virtual void setRenderVisible(bool b) { _bRenderVisible = b; }//Simply hide element, but still compute it in the layout (take up space)
  void hideRender() { setRenderVisible(false); }//Simply doesn't draw element, but it still computes the layout
  void showRender() { setRenderVisible(true); } //Simply doesn't draw element, but it still computes the layout
  void hideLayerRender(uint32_t iLayer);
  void showLayerRender(uint32_t iLayer);
  void disablePick() { _bPickEnabled = false; }

  void setClick(MouseFunc cc);//release
  void setPress(MouseFunc cc);
  void setDown(MouseFunc cc);
  void setHover(MouseFunc cc);//mouse hover

  ButtonState::e getMouseState() { return _eMouseState; }

  void setPickRoot() { _bPickRoot = true; } //Prevents the pick event from flowing downwards.
  bool getPickRoot() { return _bPickRoot; }
  void bringToFront(std::shared_ptr<UiElement> child, bool bCreateNewLayer);
  uDim& padBottom() { return _padBottom; }
  uDim& padTop() { return _padTop; }
  uDim& padRight() { return _padRight; }
  uDim& padLeft() { return _padLeft; }

protected:
  uint64_t _iPickedFrameId = 0; //DEBUG ONLY - this is used for debug only.

  void computePositionalElement(std::shared_ptr<UiElement> ele);
  void layoutEleQuad(std::shared_ptr<UiScreen> s, std::shared_ptr<UiElement> ele);
  void computeQuads(std::shared_ptr<UiScreen> s, float final_r, float final_l, float final_t, float final_b);
  virtual float getAutoWidth();
  virtual float getAutoHeight();
  virtual bool getShouldScalePositionToDesign() { return true; }
  virtual bool getIsPickEnabled() { return _bPickEnabled; }
  vec4 makeClipRectForRender(const Box2f& b2ClipRect);
  bool getLayoutChanged() { return _bLayoutChanged; }
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect);
  bool getPickedLastFrame() { return _bPickedLastFrame; }

  //Override these to implement after add/remove logi
  virtual void afterAdded();
  virtual void afterRemoved();

private:
  std::string _strName = "";
  std::shared_ptr<UiElement> _pParent = nullptr;
  UISortedMap _mapChildren; //Sorted Children

  void checkCycle(std::shared_ptr<UiElement> e, bool& bResult);

  int32_t _iTreeLevel = -1; //This is used to prevent cycles.
  uDim _left = "0px";   //**These may only ever be pixel units.
  uDim _top = "0px";    //**These may only ever be pixel units.
  uDim _right = "0px";  //**These may only ever be pixel units.
  uDim _bottom = "0px"; //**These may only ever be pixel units.

  Box2f _b2ComputedQuadLast;//Last Computed Quad in design space - used to skip unimportant updates.
  Box2f _b2ComputedQuad;//Computed Quad in design space (use this for most stuff)
  Box2f _b2LayoutQuad;//Compuetd quad scaled to the screen
  Box2f _b2RasterQuad;//Computed quad scaled, then transformed into OpenGL raster space
  Box2f _b2ContentQuad; //Quad containing all of the child elements without any clipping, in DESIGN space (_b2ComputedQuad)
  uDim _width = "auto"; //auto - size automatically to parent container (this is how css/html works)
  uDim _height = "auto";//auto - size automatically to parent container (this is how css/html works)

  float _fLastWidth = -99999, _fLastHeight = -99999;
  float _fLastTop = -99999, _fLastLeft = -99999;

  uDim _minWidth = "0px";      //no min/max w/h by default
  uDim _minHeight = "0px";     //no min/max w/h by default
  uDim _maxWidth = "999999px"; //no min/max w/h by default
  uDim _maxHeight = "999999px";//no min/max w/h by default

  uDim _padTop = "0px";//Bottom Padding     static elements only*
  uDim _padRight = "0px";//Bottom Padding   static elements only*
  uDim _padBottom = "0px";//Bottom Padding  static elements only*
  uDim _padLeft = "0px";//Bottom Padding    static elements only*

  UiDisplayMode::e _eDisplayMode = UiDisplayMode::e::InlineWrap;// All elements default to inline, block elements (like whitespace characters) will wrap to new lines.
  UiOverflowMode::e _eOverflowMode = UiOverflowMode::e::Show;//Really only windows should hide stuff
  UiPositionMode::e _ePosition = UiPositionMode::e::Static;
  //UiAutoSizeMode::e _eAutoSizeModeX = UiAutoSizeMode::e::Fixed;//We could also set this to be per axis, like Grow/Shrink downwards!
  //UiAutoSizeMode::e _eAutoSizeModeY = UiAutoSizeMode::e::Fixed;//We could also set this to be per axis, like Grow/Shrink downwards!
  bool _bVisible = true;
  bool _bRenderVisible = true;
  bool _bLayoutChanged = true;
  bool _bPickRoot = false;
  bool _bPickEnabled = true;
  bool _bPickedLastFrame = false;
  std::shared_ptr<UiDragInfo> _pDragInfo = nullptr;
  EventMap _events;
  vec4 _vColor = vec4(1, 1, 1, 1);

  //Mouse
  MouseFunc _clickFunc = nullptr;//Release
  MouseFunc _pressFunc = nullptr;
  MouseFunc _downFunc = nullptr;
  MouseFunc _hoverFunc = nullptr; //Hover
  std::shared_ptr<UiEventFunc> _downEvt = nullptr;
  std::shared_ptr<UiEventFunc> _pressEvt = nullptr;
  std::shared_ptr<UiEventFunc> _releaseEvt = nullptr;
  std::shared_ptr<UiEventFunc> _upEvt = nullptr;
  ButtonState::e _eMouseState = ButtonState::e::Up;

  float _fLineHeight = 0;

  void validateDims();
  void computewh(float& __out_ w, float& __out_ h);
  void computepad(float& pt, float& pr, float& pb, float& pl);
  void computepad_unit(float& pt, float dim, uDim& ud);
  void validateQuad();
  void calcStaticElement(std::shared_ptr<UiElement> ele, std::vector<UiLine>& vecLines, float fAutoWidth, float fAutoHeight);
  void getRootParent(std::shared_ptr<UiElement>& rp);
  void applyMinMax(float& wpx, float& hpx);
  void drawBoundBox(std::shared_ptr<UtilMeshInline2d> pt, vec4& color, bool bPickedOnly, std::shared_ptr<UiScreen> pscreen);
  bool checkDupes(std::shared_ptr<UiElement> c);
  void findElement(std::shared_ptr<UiElement> ele, bool& b);
  void layoutLayer(std::vector<std::shared_ptr<UiElement>> bucket);
  bool isFullyClipped(const Box2f& b2ClipRect);
  Box2f shrinkClipRect(const Box2f& b2ClipRect);
  void performLayoutChildren(std::shared_ptr<UiScreen> s, bool bForce);
  void positionChildren(bool bForce);
  void computeContentQuad();
};
class UiImage : public UiElement {
public:
  UiImage();
  virtual ~UiImage() override;
  virtual void init() override;

  // static std::shared_ptr<UiImage> create(t_string path, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  void getQuadVerts(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect);
  //  std::shared_ptr<MeshNode> getMesh() { return _pMesh; }
  void setTexture(std::shared_ptr<UiTex> ps) { _pTexture = ps; }
  virtual bool getPickable() override { return true; }
  void setSizeMode(UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  void setWrapMode(TexWrap::e u, TexWrap::e v);
  virtual bool pick(std::shared_ptr<InputManager> fingers, std::shared_ptr<UiScreen> pscreen) override;
  Box2f& getTexs() { return  _q2Tex; }
  void regenMeshExposed(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) { regenMesh(verts, inds, b2ClipRect); }

protected:
  virtual float getAutoWidth()  override;
  virtual float getAutoHeight() override;
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
  virtual void afterAdded() override;

private:
  // std::shared_ptr<MeshNode> _pMesh = nullptr;
  std::shared_ptr<UiTex> _pTexture = nullptr;
  UiImageSizeMode::e _eSizeModeX = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
  UiImageSizeMode::e _eSizeModeY = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
  float _fTileWidthPx = 32;//Determines image size  for tiling
  float _fTileHeightPx = 32; //Determines image size for tiling
  Box2f _q2Tex = Box2f(vec2(0, 0), vec2(1, 1));
  uint32_t _iPickId = 0;
};
class UiGlyph : public UiImage {
public:
  static std::shared_ptr<UiGlyph> UiGlyph::create(uint32_t iChar);
  UiGlyph() {}
  virtual ~UiGlyph() override {}
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;

private:
  uint32_t _iChar = 0x20;
};
class UiLabel : public UiElement {
public:
  static std::shared_ptr<UiLabel> create(std::string text, std::shared_ptr<UiLabelSkin> inf);
  UiLabel() {}
  virtual ~UiLabel() override {}
  virtual void init() override;
  std::string getText() { return _strText; }
  void setText(std::string s);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  void enableWordWrap() { _bWordWrap = true; }
  virtual void update(std::shared_ptr<InputManager> pFingers) override;

protected:
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;

private:
  std::vector<std::shared_ptr<UiGlyph>> _vecGlyphs;
  std::string _strText = "";
  std::string _strTextLast = "";
  std::shared_ptr<MeshNode> _pMesh = nullptr;
  std::shared_ptr<UiLabelSkin> _pFontInfo = nullptr;
  bool _bWordWrap = false;
  void createGlyphs();
};
namespace ImageState { typedef enum { Up, Down, Hover } e; }
class UiButtonBase : public UiElement {
public:
  void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void showHover() = 0;
  virtual void showUp() = 0;
  virtual void showDown() = 0;
protected:
  static int layerIdUp();
  static int layerIdHover();
  static int layerIdDown();
  ImageState::e _eImageState = ImageState::e::Up;

private:
  //      ButtonState::e _eButtonState = ButtonState::e::Up;
    //bool _bHoverdLastFrame = false;


};
class UiImageButton : public UiButtonBase {
public:
  //create the same 9 grid
  static std::shared_ptr<UiImageButton> create(std::shared_ptr<UiButtonSkin> pSkin);
  void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void showHover() override;
  virtual void showUp() override;
  virtual void showDown() override;

private:
  std::shared_ptr<UiImage> _pUp = nullptr;
  std::shared_ptr<UiImage> _pHover = nullptr;
  std::shared_ptr<UiImage> _pDown = nullptr;
  std::shared_ptr<UiButtonSkin> _pSkin = nullptr;

  void setHover(std::shared_ptr<UiImage> img);
  void setUp(std::shared_ptr<UiImage> img);
  void setDown(std::shared_ptr<UiImage> img);

};
class UiGridRow : public UiElement {
public:
  UiGridRow() {}
  virtual ~UiGridRow() override {}
  virtual void init() override;
  std::shared_ptr<UiElement> addCol(int nCount = 1, bool bAutoSizeCols = false, uint32_t iSort = UiElement::c_uiBaseLayer0SortId);
  std::shared_ptr<UiElement> getCol(size_t iCol);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;

private:
  std::vector<std::shared_ptr<UiElement>> _cols;
  void resizeCols();

};
class UiGrid : public UiElement {
public:
  static std::shared_ptr<UiGrid> create(int nRow, int nCol);
  static std::shared_ptr<UiGrid> createImageStack(std::shared_ptr<Ui3Tex> tex, uDim uWorHPx, Orientation::e eOr);
  static std::shared_ptr<UiGrid> createPadGrid(std::shared_ptr<UiElement> contents, Orientation::e eOr, uDim worhpx, float torlpct, float midpct, float rorbpct);
  UiGrid() {}
  virtual ~UiGrid() override {}
  virtual void init() override { UiElement::init(); }
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  std::shared_ptr<UiGridRow> addRow(int nr = 1, int nc = 0, bool bAutoSizeRows = false, uint32_t iSort = UiElement::c_uiBaseLayer0SortId);
  std::shared_ptr<UiElement> getCell(size_t iRow, size_t iCol);

protected:
  std::vector<std::shared_ptr<UiGridRow>> _rows;
  void resizeRows();

};
class Ui9Grid : public UiGrid {
public:
  void setBorder(std::shared_ptr<UiBorderDim> d) { _pBorder = d; }
  static std::shared_ptr<Ui9Grid> create(std::shared_ptr<UiBorderDim> dims);
  Ui9Grid() {}
  virtual ~Ui9Grid() override {}
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  void set9GridImages(std::shared_ptr<Ui9Tex> texs, uint32_t iLayer = UiElement::c_uiBaseLayer0SortId);
  void setBorderWidth(std::shared_ptr<UiBorderDim> dims);
  static size_t cellOff(int i, int j) { return (size_t)(j * 3 + i); }
  void iterateCells(std::function<void(std::shared_ptr<UiElement>)>);
  std::shared_ptr<UiElement> getContentContainer() { return _pContentContainer; }//If adding stuff, only add to content containers.

protected:
  //uDim _borderWidth[4];//Top, Right, bottom, left
  std::shared_ptr<UiBorderDim> _pBorder;
  //std::vector<std::shared_ptr<Texture2DSpec>> _vecImages;

private:
  std::shared_ptr<UiElement> _pContentContainer = nullptr;
  uint32_t getContentSortLayer() { return 10; }

};

class UiFlexButton : public UiButtonBase {
public:
  static std::shared_ptr<UiFlexButton> create(std::shared_ptr<UiFlexButtonSkin> skin);
  UiFlexButton() {}
  virtual ~UiFlexButton() override {}
  virtual void init() override;
  virtual void showHover() override;
  virtual void showUp() override;
  virtual void showDown() override;
  void setLabel(std::string strText, std::shared_ptr<UiLabelSkin> labelFont = nullptr);
  std::shared_ptr<UiElement> getContentContainer();//PassThru for PadGrid->getContet

protected:
  std::shared_ptr<UiLabelSkin> _pLabelSkin = nullptr;
  std::shared_ptr<Ui9Grid> _pGrid = nullptr;
  //std::shared_ptr<UiFlexGrid> _pPadGrid = nullptr;
  std::shared_ptr<UiLabel> _pLabel = nullptr;
  void setButtonImages(std::shared_ptr<Ui9Tex> vecUp, std::shared_ptr<Ui9Tex> vecHv, std::shared_ptr<Ui9Tex> vecDn);
  //void setContent(std::shared_ptr<UiElement> cont);
};
class UiScrubGenThumb : public UiElement {
public:
  bool scrubChanged() { return _bPlayerDrag; }
  void resetScrubChanged() { _bPlayerDrag = false; }
  void setScrubChanged() { _bPlayerDrag = true; }
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  Orientation::e getOrientation() { return _eOrientation; }
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  void setBarSizePct(float pos01);
  float getScrollPct();

protected:
  Orientation::e _eOrientation = Orientation::e::Horizontal;

private:
  //***Generic scroll thumb
  float _fBarLeftOrTopY = 0;
  bool _bPlayerDrag = false;
  float _fBarSizePct = 0.0f;  //the ratio of width to height -- Hidden for trackbars
  float _fScrollPct = 0.0f;  //the ratio of width to height
};
class UiScrubGen : public UiElement {
public:
  typedef std::function<float(std::shared_ptr<UiScrubGen> pScrollbar)> BarWidthFunc;
  typedef std::function<void(std::shared_ptr<UiScrubGen> pScrollbar, float scroll)> ScrollFunc;
public:
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void init() override;
  static uint32_t getThumbSortLayer();
  static uint32_t getBackgroundSortLayer();
  Orientation::e getOrientation() { return _eOrientation; }
  bool getCanScrollPastEof() { return _bCanScrollPastEof; }
  float& minVal() { return _fMin; }
  float& maxVal() { return _fMax; }
  float& val() { return _fVal; }
  float& increment() { return _fIncrement; }

protected:
  std::shared_ptr<UiScrubGenThumb> getThumb() { return _pThumb; }
  Orientation::e _eOrientation = Orientation::e::Horizontal;
  bool _bAutoHideWhenMaxed = true;
  // virtual std::shared_ptr<UiElement> getContainerObject() = 0; // This should be set by the Scrollbar implementation, return Null for a Trackbar
  virtual std::shared_ptr<UiScrubGenThumb> createThumb() = 0; // return a thumb to the fiv
  void setBarSizePct(float pos01);
  void setBarWidthFunc(BarWidthFunc b) { _barWidthFunc = b; }
  virtual void setScrollFunc(ScrollFunc b) { _scrollFunc = b; }

private:
  //***Generic scroll
  std::shared_ptr<UiScrubGenThumb> _pThumb = nullptr;
  std::shared_ptr<UiElement> _pContainerObject = nullptr;
  bool _bCanScrollPastEof = false;
  BarWidthFunc _barWidthFunc = nullptr;
  ScrollFunc _scrollFunc = nullptr;
  float _fMin = 0.0f;//should be 0,1 for scrollbars
  float _fMax = 1.0f;//should be 0,1 for scrollbars
  float _fVal = 0.0f;//should be 0,1 for scrollbars
  float _fIncrement = 0.1f;
};
class UiScrollbarThumb : public UiScrubGenThumb {
  std::shared_ptr<UiGrid> _pImage = nullptr;
public:
  static std::shared_ptr<UiScrollbarThumb> create(std::shared_ptr<UiScrollbarSkin> pSkin);//std::vector<std::shared_ptr<Texture2DSpec>> imgs, uDim widthPx, Orientation::e eOr);
  UiScrollbarThumb() {}
  virtual ~UiScrollbarThumb() override {}
  virtual void init() override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
};
class UiScrollbar : public UiScrubGen {
  std::shared_ptr<UiGrid> _pImage = nullptr;
  std::shared_ptr<UiScrollbarSkin> _pSkin = nullptr;
protected:
  virtual std::shared_ptr<UiScrubGenThumb> createThumb() override;
public:
  UiScrollbar() {}
  virtual ~UiScrollbar() override {}
  static std::shared_ptr<UiScrollbar> create(std::shared_ptr<UiScrollbarSkin> pSkin, UiScrubGen::BarWidthFunc bwf, UiScrubGen::ScrollFunc sf);
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
};
class UiCheckbox : public UiElement {
public:
  UiCheckbox() {}
  virtual ~UiCheckbox() override {}
  static std::shared_ptr<UiCheckbox> create(std::shared_ptr<UiCheckboxSkin> pSkin, std::string label);
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  void setCheckFunc(std::function<void(bool bNewValue)> func) { _checkFunc = func; }
  void setChecked(bool b);

private:
  bool _bChecked = false;
  std::shared_ptr<UiLabel> _pLabel = nullptr;
  std::shared_ptr<UiFlexButton> _pButton = nullptr;
  std::shared_ptr<UiCheckboxSkin> _pSkin = nullptr;
  std::shared_ptr<UiImage> _pCheckImg = nullptr;
  std::function<void(bool bNewValue)> _checkFunc = nullptr;
  void doCheck();
};
class UiSliderThumb : public UiScrubGenThumb {
public:
  static std::shared_ptr<UiSliderThumb> create(std::shared_ptr<UiSliderSkin> pSkin);
  UiSliderThumb() {}
  virtual ~UiSliderThumb() override {}
  virtual void init() override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;

private:
  std::shared_ptr<UiGrid> _pImage = nullptr;
};
class UiSlider : public UiScrubGen {
public:
  UiSlider() {}
  virtual ~UiSlider() override {}
  static std::shared_ptr<UiSlider> create(std::shared_ptr<UiSliderSkin> pSkin, UiScrubGen::ScrollFunc sf);
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  std::shared_ptr<UiLabel> getValueLabel() { return _pValueLabel; }
  void setValueLabel(std::shared_ptr<UiLabel> v) { _pValueLabel = v; }

protected:
  virtual std::shared_ptr<UiScrubGenThumb> createThumb() override;

private:
  std::shared_ptr<UiGrid> _pImage = nullptr;
  std::shared_ptr<UiSliderSkin> _pSkin = nullptr;
  uDim _uWidthOrHeightPx;
  std::shared_ptr<UiLabel> _pValueLabel = nullptr;
};
class UiContainer : public UiGrid {
public:
  std::shared_ptr<UiElement> getContentContainer();
  static std::shared_ptr<UiContainer> create();
  void enableScrollbar(std::shared_ptr<UiScrollbarSkin> pSkin);
  void enableResize();
  virtual void init() override;

private:
  float _fBorderWidthPx = 32;
  std::shared_ptr<UiGrid> _pGrid = nullptr;
  std::shared_ptr<UiScrollbar> _pVScrollBar = nullptr;
  std::shared_ptr<UiScrollbar> _pHScrollBar = nullptr;
};
class UiDropdown : public Ui9Grid {
public:
  static std::shared_ptr<UiDropdown> create(std::shared_ptr<UiDropdwonSkin> pSkin);
  virtual void init() override;
  void addItem(std::shared_ptr<UiElement> item, bool bFitHeight = true);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  void setSelectedItem(std::shared_ptr<UiElement> e);
  //virtual std::shared_ptr<UiElement> addChild(std::shared_ptr<UiElement> c, uint32_t uiSort = UiElement::c_uiBaseLayer0SortId, bool bUpdateLayout = true, bool bCheckDupes = true) override;
  void setContainerWidth(uDim w) { _uListContainerWidth = w; }

private:
  std::shared_ptr<UiDropdwonSkin> _pSkin = nullptr;
  std::shared_ptr<Ui9Grid> _pListContainer = nullptr;
  std::shared_ptr<UiElement> _pSelectedContainer = nullptr;
  std::shared_ptr<UiFlexButton> _pArrowButton = nullptr;
  std::shared_ptr<UiElement> _pSelected = nullptr;
  uDim _uListContainerWidth = "auto";//sets it to be auto size


};
class UiWindow : public UiElement {
public:
  UiWindow() {}
  virtual ~UiWindow() override {}
  static std::shared_ptr<UiWindow> create(std::shared_ptr<UiWindowSkin> pSkin);
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers);
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
  virtual bool pick(std::shared_ptr<InputManager> fingers, std::shared_ptr<UiScreen> pscreen) override;
  void enableVScrollbar();//Call this to scroll contents
  void enableHScrollbar();//Call this to scroll contents
  void enableResize();
  void enableDrag();
  void setTitleLabel(std::string lbl);
  std::shared_ptr<UiElement> getContentContainer();

protected:
  std::shared_ptr<UiWindowSkin> _pWindowSkin = nullptr;
  std::shared_ptr<UiElement> _pTitleBar = nullptr;

private:
  std::shared_ptr<Ui9Grid> _pBackground = nullptr;
  std::shared_ptr<UiElement> _pContentArea = nullptr;
  std::shared_ptr<UiContainer> _pContainer = nullptr;
  std::shared_ptr<UiLabel> _lblTitle = nullptr;
  std::shared_ptr<UiImageButton> _pCloseButton = nullptr;
};
class UiToolbar : public UiWindow {
public:
  static std::shared_ptr<UiToolbar> create(std::shared_ptr<UiToolbarSkin> skin);
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pFingers) override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;

private:
  std::shared_ptr<UiToolbarSkin> _pToolbarSkin = nullptr;
};
class UiCursor : public UiImage {
protected:
  virtual bool getShouldScalePositionToDesign() override { return false; }
  virtual bool getIsPickEnabled() override { return false; }
  std::shared_ptr<UiCursorSkin> _pSkin = nullptr;

public:
  static std::shared_ptr<UiCursor> create(std::shared_ptr<UiCursorSkin> tex);
  UiCursor() {}
  virtual ~UiCursor() override {}
  virtual void init() override;
  virtual void performLayout(std::shared_ptr<UiScreen> s, bool bForce) override;
};
/**
* @class UiScreen
* @brief A screen, the root of a UI hierarchy, for a window.
* Only one UiScreen per system Window.
*/
class UiScreen_Internal;
class UiScreen : public UiElement {
public:
  UiScreen(std::shared_ptr<GraphicsWindow> pw);
  virtual ~UiScreen() override;

  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
  static std::shared_ptr<UiScreen> create(std::shared_ptr<GraphicsWindow> g);

  bool getIsPicked();
  const UiDesignSize& getDesignSize();
  std::shared_ptr<UiCursor> getCursor();
  std::shared_ptr<MegaTex> getTex();
  std::shared_ptr<Picker> getPicker();

  void drawForward();
  virtual void drawForward(RenderParams& rp, Box2f& b2ClipRect) override;
  void setCursor(std::shared_ptr<UiCursor> c);
  void hideCursor();
  void showCursor();
  void screenChanged(uint32_t uiWidth, uint32_t uiHeight);
  static void error(std::string errMsg);
  static uint32_t sortLayer(uint32_t n) {
    //Mnemonic wich gves you the base sort layer, provided n will return additional layers.
    return UiElement::c_uiBaseLayer0SortId + n;
  }
  static float getDesignMultiplierW();
  static float getDesignMultiplierH();

  void debugForceLayoutChanged();
  void performForcedLayout();
  uint64_t getFrameNumber();
  std::shared_ptr<GraphicsWindow> getWindow();

private:
  std::unique_ptr<UiScreen_Internal> _pint = nullptr;
  void updateMesh();
  void updateLayout(std::shared_ptr<InputManager> pInputManager);
};

}//ns Game



#endif
