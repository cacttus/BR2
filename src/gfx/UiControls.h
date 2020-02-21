/**
*  @file UiControls.h
*  @date February 17, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __UICONTROLS_15188874023404093923_H__
#define __UICONTROLS_15188874023404093923_H__

#include "../gfx/UiSkin.h"
#include <future>

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
class UiElement : public VirtualMemoryShared<UiElement> {
public:
  typedef std::vector<std::shared_ptr<UiEventFunc>> EventFuncList;
  typedef std::map<UiEventId::e, EventFuncList> EventMap;

  static const uint32_t c_uiBaseLayer0SortId = 1000;
  typedef std::function<void()> MouseFunc;
public:
  //static std::shared_ptr<UiElement> create();
  UiElement(std::shared_ptr<UiScreen> mgr);
  virtual ~UiElement() override;

  std::string getName() { return _strName; }
  virtual void update(std::shared_ptr<InputManager> pInputManager);
  virtual void performLayout(bool bForce);

  virtual void drawForward(RenderParams& rp, Box2f& b2ClipRect);
  void drawDebug();//ONLY CALL ON Gui2d!!
  virtual std::shared_ptr<UiElement> addChild(std::shared_ptr<UiElement> c, uint32_t uiSort = UiElement::c_uiBaseLayer0SortId, bool bUpdateLayout = true, bool bCheckDupes = true);
  std::shared_ptr<UiElement> getParent() { return _pParent; }
  virtual void setLayoutChanged(bool bChildren = false);//Don't use bChildren unless we're Gui2D

  void enableDrag(UiDragInfo::DragFunc func);

  void setName(std::string n) { _strName = n; }
  virtual void init();
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
  ///void setOverflowMode(UiOverflowMode::e ee) { _eOverflowMode = ee; }
  ///::e getDisplayMode() { return _eDisplayMode; }
  ///void setDisplayMode(UiDisplayMode::e ee) { _eDisplayMode = ee; }
  //UiAutoSizeMode::e getAutoSizeModeX() { return _eAutoSizeModeX; }
  //void setAutoSizeModeX(UiAutoSizeMode::e m) { _eAutoSizeModeX = m; }
  //UiAutoSizeMode::e getAutoSizeModeY() { return _eAutoSizeModeY; }
  //void setAutoSizeModeY(UiAutoSizeMode::e m) { _eAutoSizeModeY = m; }
  std::multimap<uint32_t, std::shared_ptr<UiElement>>& getChildren() { return _mapChildren; }
  void clearChildren();
  bool removeChild(std::shared_ptr<UiElement> ele, bool bUpdateLayout = true);
  bool hasChild(std::shared_ptr<UiElement> ele);

  virtual bool pick(std::shared_ptr<InputManager> InputManager);
  void addEvent(UiEventId::e ev, std::shared_ptr<UiEventFunc> f);
  bool removeEvent(UiEventId::e evId, std::shared_ptr<UiEventFunc> f);

  void doMouseEvents(std::shared_ptr<InputManager> pInputManager);
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

  void setPickRoot() { _bPickRoot = true; }
  bool getPickRoot() { return _bPickRoot; }
  void bringToFront(std::shared_ptr<UiElement> child, bool bCreateNewLayer);
  uDim& padBottom() { return _padBottom; }
  uDim& padTop() { return _padTop; }
  uDim& padRight() { return _padRight; }
  uDim& padLeft() { return _padLeft; }

  virtual std::shared_ptr<GraphicsWindow> getWindow();

  std::shared_ptr<UiScreen> getManager() {
    return _pManager;
  }


protected:
  uint64_t _iPickedFrameId = 0; //DEBUG ONLY - this is used for debug only.

  void computePositionalElement(std::shared_ptr<UiElement> ele);
  void layoutEleQuad(std::shared_ptr<UiElement> ele);
  void computeQuads(float final_r, float final_l, float final_t, float final_b);
  virtual float getAutoWidth();
  virtual float getAutoHeight();
  virtual bool getShouldScalePositionToDesign() { return true; }
  virtual bool getIsPickEnabled() { return _bPickEnabled; }
  vec4 makeClipRectForRender(const Box2f& b2ClipRect);

  bool getLayoutChanged() { return _bLayoutChanged; }
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect);
  //virtual void setQuad() { }
  bool getPickedLastFrame() { return _bPickedLastFrame; }

private:
  struct UiLine {
    float _top = 0;
    float _left = 0;
    float _height = 0;
    float _width = 0;
    std::vector<std::shared_ptr<UiElement>> _eles;
  };
private:
  std::string _strName = "";
  std::shared_ptr<UiElement> _pParent = nullptr;
  std::multimap<uint32_t, std::shared_ptr<UiElement>> _mapChildren; //Sorted Children
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

 // ButtonState::e _eLastLmb = ButtonState::e::Up;
 // ButtonState::e _eLastRmb = ButtonState::e::Up;
 // ButtonState::e _eLastMmb = ButtonState::e::Up;

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
  std::shared_ptr<UiScreen> _pManager = nullptr;

  void validateDims();
  void computewh(float& __out_ w, float& __out_ h);
  void computepad(float& pt, float& pr, float& pb, float& pl);
  void computepad_unit(float& pt, float dim, uDim& ud);
  void validateQuad();
  void calcStaticElement(std::shared_ptr<UiElement> ele, std::vector<UiElement::UiLine>& vecLines, float fAutoWidth, float fAutoHeight);
  void getRootParent(std::shared_ptr<UiElement>& rp);
  void applyMinMax(float& wpx, float& hpx);
  void drawBoundBox(std::shared_ptr<UtilMeshInline2d> pt, vec4& color, bool bPickedOnly);
  bool checkDupes(std::shared_ptr<UiElement> c);
  void findElement(std::shared_ptr<UiElement> ele, bool& b);
  void layoutLayer(std::vector<std::shared_ptr<UiElement>> bucket);
  bool isFullyClipped(const Box2f& b2ClipRect);
  Box2f shrinkClipRect(const Box2f& b2ClipRect);
  void performLayoutChildren(bool bForce);
  void positionChildren(bool bForce);
  void computeContentQuad();


};
class UiImage : public UiElement {
  // std::shared_ptr<MeshNode> _pMesh = nullptr;
  std::shared_ptr<UiTex> _pTexture = nullptr;
  UiImageSizeMode::e _eSizeModeX = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
  UiImageSizeMode::e _eSizeModeY = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
  float _fTileWidthPx = 32;//Determines image size  for tiling
  float _fTileHeightPx = 32; //Determines image size for tiling
  Box2f _q2Tex = Box2f(vec2(0, 0), vec2(1, 1));
  uint32_t _iPickId = 0;

protected:
  virtual float getAutoWidth()  override;
  virtual float getAutoHeight() override;
  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
  //virtual void setQuad() override;

public:
  UiImage(std::shared_ptr<UiScreen> sc, std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx);
  virtual ~UiImage() override;
  virtual void init() override;

  // static std::shared_ptr<UiImage> create(t_string path, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  //static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  //static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx);
  virtual void performLayout(bool bForce) override;
  void getQuadVerts(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect);
  //  std::shared_ptr<MeshNode> getMesh() { return _pMesh; }
  void setTexture(std::shared_ptr<UiTex> ps) { _pTexture = ps; }
  virtual bool getPickable() override { return true; }
  void setSizeMode(UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
  void setWrapMode(TexWrap::e u, TexWrap::e v);
  virtual bool pick(std::shared_ptr<InputManager> InputManager) override;
  Box2f& getTexs() { return  _q2Tex; }
  void regenMeshExposed(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) { regenMesh(verts, inds, b2ClipRect); }

};
//class UiGlyph : public UiImage {
//  uint32_t _iChar = 0x20;
//public:
//  //static std::shared_ptr<UiGlyph> UiGlyph::create(uint32_t iChar);
//  UiGlyph(std::shared_ptr<UiScreen> s) : UiImage(s) {}
//  virtual ~UiGlyph() override {}
//  virtual void performLayout(bool bForce) override;
//  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
//
//};
//class UiLabel : public UiElement {
//  std::vector<std::shared_ptr<UiGlyph>> _vecGlyphs;
//  std::string _strText = "";
//  std::string _strTextLast = "";
//  std::shared_ptr<MeshNode> _pMesh = nullptr;
//  std::shared_ptr<UiLabelSkin> _pFontInfo = nullptr;
//  bool _bWordWrap = false;
//  void createGlyphs();
//protected:
//  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
//public:
// // static std::shared_ptr<UiLabel> create(std::string text, std::shared_ptr<UiLabelSkin> inf);
//  UiLabel(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual ~UiLabel() override {}
//  virtual void init() override;
//  std::string getText() { return _strText; }
//  void setText(std::string s);
//  virtual void performLayout(bool bForce) override;
//  void enableWordWrap() { _bWordWrap = true; }
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//};
//namespace ImageState { typedef enum { Up, Down, Hover } e; }
//class UiButtonBase : public UiElement {
//public:
//  UiButtonBase(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//
//private:
//  //      ButtonState::e _eButtonState = ButtonState::e::Up;
//    //bool _bHoverdLastFrame = false;
//protected:
//  static int layerIdUp();
//  static int layerIdHover();
//  static int layerIdDown();
//  ImageState::e _eImageState = ImageState::e::Up;
//
//public:
//  void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void showHover() = 0;
//  virtual void showUp() = 0;
//  virtual void showDown() = 0;
//};
//class UiImageButton : public UiButtonBase {
//  std::shared_ptr<UiImage> _pUp = nullptr;
//  std::shared_ptr<UiImage> _pHover = nullptr;
//  std::shared_ptr<UiImage> _pDown = nullptr;
//  std::shared_ptr<UiButtonSkin> _pSkin = nullptr;
//
//  void setHover(std::shared_ptr<UiImage> img);
//  void setUp(std::shared_ptr<UiImage> img);
//  void setDown(std::shared_ptr<UiImage> img);
//public:
//  //create the same 9 grid
//  static std::shared_ptr<UiImageButton> create(std::shared_ptr<UiButtonSkin> pSkin);
//  void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void showHover() override;
//  virtual void showUp() override;
//  virtual void showDown() override;
//};
//class UiGridRow : public UiElement {
//  std::vector<std::shared_ptr<UiElement>> _cols;
//  void resizeCols();
//public:
//  UiGridRow(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual ~UiGridRow() override {}
//  virtual void init() override;
//  std::shared_ptr<UiElement> addCol(int nCount = 1, bool bAutoSizeCols = false, uint32_t iSort = UiElement::c_uiBaseLayer0SortId);
//  std::shared_ptr<UiElement> getCol(size_t iCol);
//  virtual void performLayout(bool bForce) override;
//};
//class UiGrid : public UiElement {
//protected:
//  std::vector<std::shared_ptr<UiGridRow>> _rows;
//  void resizeRows();
//public:
//  static std::shared_ptr<UiGrid> create(int nRow, int nCol);
//  static std::shared_ptr<UiGrid> createImageStack(std::shared_ptr<Ui3Tex> tex, uDim uWorHPx, Orientation::e eOr);
//  static std::shared_ptr<UiGrid> createPadGrid(std::shared_ptr<UiElement> contents, Orientation::e eOr, uDim worhpx, float torlpct, float midpct, float rorbpct);
//  UiGrid(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual ~UiGrid() override {}
//  virtual void init() override { UiElement::init(); }
//  virtual void performLayout(bool bForce) override;
//  std::shared_ptr<UiGridRow> addRow(int nr = 1, int nc = 0, bool bAutoSizeRows = false, uint32_t iSort = UiElement::c_uiBaseLayer0SortId);
//  std::shared_ptr<UiElement> getCell(size_t iRow, size_t iCol);
//};
//class Ui9Grid : public UiGrid {
//  std::shared_ptr<UiElement> _pContentContainer = nullptr;
//  uint32_t getContentSortLayer() { return 10; }
//protected:
//  //uDim _borderWidth[4];//Top, Right, bottom, left
//  std::shared_ptr<UiBorderDim> _pBorder;
//  //std::vector<std::shared_ptr<Texture2DSpec>> _vecImages;
//public:
//  void setBorder(std::shared_ptr<UiBorderDim> d) { _pBorder = d; }
//  static std::shared_ptr<Ui9Grid> create(std::shared_ptr<UiBorderDim> dims);
//  Ui9Grid(std::shared_ptr<UiScreen> mg) : UiGrid(mg) {}
//  virtual ~Ui9Grid() override {}
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  void set9GridImages(std::shared_ptr<Ui9Tex> texs, uint32_t iLayer = UiElement::c_uiBaseLayer0SortId);
//  void setBorderWidth(std::shared_ptr<UiBorderDim> dims);
//  static size_t cellOff(int i, int j) { return (size_t)(j * 3 + i); }
//  void iterateCells(std::function<void(std::shared_ptr<UiElement>)>);
//  std::shared_ptr<UiElement> getContentContainer() { return _pContentContainer; }//If adding stuff, only add to content containers.
//};
////class UiFlexGrid : public Ui9Grid {
////public:
////    static std::shared_ptr<UiFlexGrid> create();
////    virtual void init() override;
////    UiFlexGrid() { }
////    virtual ~UiFlexGrid() override { }
////    virtual void performLayout(bool bForce) override;
////};
//
//class UiFlexButton : public UiButtonBase {
//public:
//protected:
//  std::shared_ptr<UiLabelSkin> _pLabelSkin = nullptr;
//  std::shared_ptr<Ui9Grid> _pGrid = nullptr;
//  //std::shared_ptr<UiFlexGrid> _pPadGrid = nullptr;
//  std::shared_ptr<UiLabel> _pLabel = nullptr;
//  void setButtonImages(std::shared_ptr<Ui9Tex> vecUp, std::shared_ptr<Ui9Tex> vecHv, std::shared_ptr<Ui9Tex> vecDn);
//  //void setContent(std::shared_ptr<UiElement> cont);
//public:
//  static std::shared_ptr<UiFlexButton> create(std::shared_ptr<UiFlexButtonSkin> skin);
//  UiFlexButton(std::shared_ptr<UiScreen> mg) : UiButtonBase(mg) {}
//  virtual ~UiFlexButton() override {}
//  virtual void init() override;
//  virtual void showHover() override;
//  virtual void showUp() override;
//  virtual void showDown() override;
//  void setLabel(std::string strText, std::shared_ptr<UiLabelSkin> labelFont = nullptr);
//  std::shared_ptr<UiElement> getContentContainer();//PassThru for PadGrid->getContet
//};
//
//class UiScrubGenThumb : public UiElement {
//  //***Generic scroll thumb
//  float _fBarLeftOrTopY = 0;
//  bool _bPlayerDrag = false;
//  float _fBarSizePct = 0.0f;  //the ratio of width to height -- Hidden for trackbars
//  float _fScrollPct = 0.0f;  //the ratio of width to height
//protected:
//  Orientation::e _eOrientation = Orientation::e::Horizontal;
//public:
//  UiScrubGenThumb(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  bool scrubChanged() { return _bPlayerDrag; }
//  void resetScrubChanged() { _bPlayerDrag = false; }
//  void setScrubChanged() { _bPlayerDrag = true; }
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  Orientation::e getOrientation() { return _eOrientation; }
//  virtual void performLayout(bool bForce) override;
//  void setBarSizePct(float pos01);
//  float getScrollPct();
//};
//
//class UiScrubGen : public UiElement {
//public:
//  typedef std::function<float(std::shared_ptr<UiScrubGen> pScrollbar)> BarWidthFunc;
//  typedef std::function<void(std::shared_ptr<UiScrubGen> pScrollbar, float scroll)> ScrollFunc;
//private:
//  //***Generic scroll
//  std::shared_ptr<UiScrubGenThumb> _pThumb = nullptr;
//  std::shared_ptr<UiElement> _pContainerObject = nullptr;
//  bool _bCanScrollPastEof = false;
//  BarWidthFunc _barWidthFunc = nullptr;
//  ScrollFunc _scrollFunc = nullptr;
//  float _fMin = 0.0f;//should be 0,1 for scrollbars
//  float _fMax = 1.0f;//should be 0,1 for scrollbars
//  float _fVal = 0.0f;//should be 0,1 for scrollbars
//  float _fIncrement = 0.1f;
//protected:
//  std::shared_ptr<UiScrubGenThumb> getThumb() { return _pThumb; }
//  Orientation::e _eOrientation = Orientation::e::Horizontal;
//  bool _bAutoHideWhenMaxed = true;
//  // virtual std::shared_ptr<UiElement> getContainerObject() = 0; // This should be set by the Scrollbar implementation, return Null for a Trackbar
//  virtual std::shared_ptr<UiScrubGenThumb> createThumb() = 0; // return a thumb to the fiv
//  void setBarSizePct(float pos01);
//  void setBarWidthFunc(BarWidthFunc b) { _barWidthFunc = b; }
//  virtual void setScrollFunc(ScrollFunc b) { _scrollFunc = b; }
//public:
//  UiScrubGen(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void init() override;
//  static uint32_t getThumbSortLayer();
//  static uint32_t getBackgroundSortLayer();
//  Orientation::e getOrientation() { return _eOrientation; }
//  bool getCanScrollPastEof() { return _bCanScrollPastEof; }
//  float& minVal() { return _fMin; }
//  float& maxVal() { return _fMax; }
//  float& val() { return _fVal; }
//  float& increment() { return _fIncrement; }
//};
//class UiScrollbarThumb : public UiScrubGenThumb {
//  std::shared_ptr<UiGrid> _pImage = nullptr;
//public:
//  static std::shared_ptr<UiScrollbarThumb> create(std::shared_ptr<UiScrollbarSkin> pSkin);//std::vector<std::shared_ptr<Texture2DSpec>> imgs, uDim widthPx, Orientation::e eOr);
//  UiScrollbarThumb(std::shared_ptr<UiScreen> mg) : UiScrubGenThumb(mg) {}
//  virtual ~UiScrollbarThumb() override {}
//  virtual void init() override;
//  virtual void performLayout(bool bForce) override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//};
//
//class UiScrollbar : public UiScrubGen {
//  std::shared_ptr<UiGrid> _pImage = nullptr;
//  std::shared_ptr<UiScrollbarSkin> _pSkin = nullptr;
//protected:
//  virtual std::shared_ptr<UiScrubGenThumb> createThumb() override;
//public:
//  UiScrollbar(std::shared_ptr<UiScreen> mg) : UiScrubGen(mg) {}
//  virtual ~UiScrollbar() override {}
//  static std::shared_ptr<UiScrollbar> create(std::shared_ptr<UiScrollbarSkin> pSkin, UiScrubGen::BarWidthFunc bwf, UiScrubGen::ScrollFunc sf);
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void performLayout(bool bForce) override;
//};
//
//class UiCheckbox : public UiElement {
//  bool _bChecked = false;
//  std::shared_ptr<UiLabel> _pLabel = nullptr;
//  std::shared_ptr<UiFlexButton> _pButton = nullptr;
//  std::shared_ptr<UiCheckboxSkin> _pSkin = nullptr;
//  std::shared_ptr<UiImage> _pCheckImg = nullptr;
//  std::function<void(bool bNewValue)> _checkFunc = nullptr;
//  void doCheck();
//public:
//  UiCheckbox(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual ~UiCheckbox() override {}
//  static std::shared_ptr<UiCheckbox> create(std::shared_ptr<UiCheckboxSkin> pSkin, std::string label);
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void performLayout(bool bForce) override;
//  void setCheckFunc(std::function<void(bool bNewValue)> func) { _checkFunc = func; }
//  void setChecked(bool b);
//};
//
//class UiSliderThumb : public UiScrubGenThumb {
//  std::shared_ptr<UiGrid> _pImage = nullptr;
//public:
//  static std::shared_ptr<UiSliderThumb> create(std::shared_ptr<UiSliderSkin> pSkin);
//  UiSliderThumb(std::shared_ptr<UiScreen> mg) : UiScrubGenThumb(mg) {}
//  virtual ~UiSliderThumb() override {}
//  virtual void init() override;
//  virtual void performLayout(bool bForce) override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//};
//class UiSlider : public UiScrubGen {
//  std::shared_ptr<UiGrid> _pImage = nullptr;
//  std::shared_ptr<UiSliderSkin> _pSkin = nullptr;
//  uDim _uWidthOrHeightPx;
//  std::shared_ptr<UiLabel> _pValueLabel = nullptr;
//protected:
//  virtual std::shared_ptr<UiScrubGenThumb> createThumb() override;
//public:
//  UiSlider(std::shared_ptr<UiScreen> mg) : UiScrubGen(mg) {}
//  virtual ~UiSlider() override {}
//  static std::shared_ptr<UiSlider> create(std::shared_ptr<UiSliderSkin> pSkin, UiScrubGen::ScrollFunc sf);
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void performLayout(bool bForce) override;
//  std::shared_ptr<UiLabel> getValueLabel() { return _pValueLabel; }
//  void setValueLabel(std::shared_ptr<UiLabel> v) { _pValueLabel = v; }
//
//};
//
//class UiContainer : public UiGrid {
//  float _fBorderWidthPx = 32;
//  std::shared_ptr<UiGrid> _pGrid = nullptr;
//  std::shared_ptr<UiScrollbar> _pVScrollBar = nullptr;
//  std::shared_ptr<UiScrollbar> _pHScrollBar = nullptr;
//  //std::shared_ptr<UiElement> _pCorner = nullptr;
//  //std::shared_ptr<UiElement> _pRight = nullptr;
//  //std::shared_ptr<UiElement> _pBot = nullptr;
//  //std::shared_ptr<UiElement> _pBot = nullptr;
//public:
//  UiContainer(std::shared_ptr<UiScreen> mg) : UiGrid(mg) {}
//  std::shared_ptr<UiElement> getContentContainer();
//  static std::shared_ptr<UiContainer> create();
//  void enableScrollbar(std::shared_ptr<UiScrollbarSkin> pSkin);
//  void enableResize();
//  virtual void init() override;
//};
//class UiDropdown : public Ui9Grid {
//  std::shared_ptr<UiDropdwonSkin> _pSkin = nullptr;
//  std::shared_ptr<Ui9Grid> _pListContainer = nullptr;
//  std::shared_ptr<UiElement> _pSelectedContainer = nullptr;
//  std::shared_ptr<UiFlexButton> _pArrowButton = nullptr;
//  std::shared_ptr<UiElement> _pSelected = nullptr;
//  uDim _uListContainerWidth = "auto";//sets it to be auto size
//public:
//  static std::shared_ptr<UiDropdown> create(std::shared_ptr<UiDropdwonSkin> pSkin);
//  virtual void init() override;
//  void addItem(std::shared_ptr<UiElement> item, bool bFitHeight = true);
//  virtual void performLayout(bool bForce) override;
//  void setSelectedItem(std::shared_ptr<UiElement> e);
//  //virtual std::shared_ptr<UiElement> addChild(std::shared_ptr<UiElement> c, uint32_t uiSort = UiElement::c_uiBaseLayer0SortId, bool bUpdateLayout = true, bool bCheckDupes = true) override;
//  void setContainerWidth(uDim w) { _uListContainerWidth = w; }
//
//};
//
//class UiWindow : public UiElement {
//  std::shared_ptr<Ui9Grid> _pBackground = nullptr;
//  std::shared_ptr<UiElement> _pContentArea = nullptr;
//  std::shared_ptr<UiContainer> _pContainer = nullptr;
//  std::shared_ptr<UiLabel> _lblTitle = nullptr;
//  std::shared_ptr<UiImageButton> _pCloseButton = nullptr;
//protected:
//  std::shared_ptr<UiWindowSkin> _pWindowSkin = nullptr;
//  std::shared_ptr<UiElement> _pTitleBar = nullptr;
//public:
//  UiWindow(std::shared_ptr<UiScreen> mg) : UiElement(mg) {}
//  virtual ~UiWindow() override {}
//  static std::shared_ptr<UiWindow> create(std::shared_ptr<UiWindowSkin> pSkin);
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager);
//  virtual void performLayout(bool bForce) override;
//  virtual bool pick(std::shared_ptr<InputManager> InputManager) override;
//  void enableVScrollbar();//Call this to scroll contents
//  void enableHScrollbar();//Call this to scroll contents
//  void enableResize();
//  void enableDrag();
//  void setTitleLabel(std::string lbl);
//  std::shared_ptr<UiElement> getContentContainer();
//};
//
//class UiToolbar : public UiWindow {
//  std::shared_ptr<UiToolbarSkin> _pToolbarSkin = nullptr;
//public:
//  static std::shared_ptr<UiToolbar> create(std::shared_ptr<UiToolbarSkin> skin);
//  virtual void init() override;
//  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
//  virtual void performLayout(bool bForce) override;
//};


class UiCursor : public UiImage {
protected:
  virtual bool getShouldScalePositionToDesign() override { return false; }
  virtual bool getIsPickEnabled() override { return false; }
//  std::shared_ptr<UiCursorSkin> _pSkin = nullptr;
public:
 // static std::shared_ptr<UiCursor> create(std::shared_ptr<UiCursorSkin> tex);
  UiCursor(std::shared_ptr<UiScreen> s, std::shared_ptr<UiTex> tex);
  virtual ~UiCursor() override {}
  virtual void init() override;
  virtual void performLayout(bool bForce) override;
};

/**
* @class UiScreen
* @brief A screen, the root of a UI hierarchy, for a window.
*/
class UiScreen : public UiElement {
public:
  UiScreen(std::shared_ptr<GraphicsWindow> pw);
  virtual ~UiScreen() override;
  bool getIsPicked() { return _bIsPicked; }
  const UiDesignSize& getDesignSize() { return _designSize; }
  virtual void init() override;
  virtual void update(std::shared_ptr<InputManager> pInputManager) override;
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
  std::shared_ptr<MegaTex> getTex() { return _pTex; }
  virtual std::shared_ptr<GraphicsWindow> getWindow() override;

private:
  //bool _bLayoutChanged = false;
  bool _bIsPicked = false;//capture of the bubbled-up pick
  std::shared_ptr<UiCursor> _pCursor = nullptr;
  UiDesignSize _designSize;//1920 x 1080
  ButtonState::e _eLmb = ButtonState::e::Up;
  ButtonState::e _eRmb = ButtonState::e::Up;
  ButtonState::e _eMmb = ButtonState::e::Up;
  //void updateCursorPos(const vec2& vMouse);
  bool _bDebugForceLayoutChange = false;
  std::future<bool> _updateFuture;
  std::shared_ptr<MegaTex> _pTex = nullptr;
  std::shared_ptr<MeshNode> _pMesh = nullptr;
  std::shared_ptr<GraphicsWindow> _pWindow = nullptr;
  //std::shared_ptr<UiFastQuads> _pFastQuads = nullptr;
  void updateMesh();
  void updateLayout(std::shared_ptr<InputManager> pInputManager);
protected:
  std::shared_ptr<UiCursor> getCursor() { return _pCursor; }
};
//
///**
//* @class Gui2d
//* @brief UI manager. 
//*/
//class UiScreen : public VirtualMemoryShared<UiScreen> {
//  std::map<std::shared_ptr<GraphicsWindow>, std::shared_ptr<UiScreen>> _screens;
//public:
//  void init();
//  std::shared_ptr<UiScreen> getScreenForWindow(std::shared_ptr<GraphicsWindow> w) { return nullptr; }
//  void addScreen(std::shared_ptr<GraphicsWindow> w, std::shared_ptr<UiScreen> s) { 
//    _screens.insert(std::make_pair(w, s));
//  }
//  void update(std::shared_ptr<InputManager> pInputManager);
//  void drawForward(RenderParams& rp, Box2f& b2ClipRect);
//};

}//ns BR2



#endif
