#include "../base/GLContext.h"
#include "../base/SoundCache.h"
#include "../base/TouchInfo.h"
#include "../base/FrameSync.h"
#include "../base/EngineConfig.h"
#include "../base/FileSystem.h"

#include "../display/MegaTex.h"
#include "../display/RenderSettings.h"
#include "../display/CameraNode.h"

#include "../model/Model.h"
#include "../model/ModelCache.h"
#include "../world/PhysicsWorld.h"

#include "../bottle/GameUi.h"
#include "../bottle/BottleRoom.h"
#include "../bottle/ObFile.h"
#include "../bottle/WorldObj.h"
#include "../bottle/World25.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/GodCam.h"


namespace Game {
//"Acme-Regular.ttf
#define HEAD_FONT "FreckleFace-Regular.ttf"
#define SUB_FONT "FreckleFace-Regular.ttf"
#define DEBUG_FONT "EmilysCandy-Regular.ttf"
#define CHECKBOX_FONT "AveriaSansLibre-Regular.ttf"
void GameUi::constructUI(std::shared_ptr<BottleRoom> r) {
    _pRoom = r;
    BroLogInfo("Making UI.");
    {
        t_timeval t0 = Gu::getMicroSeconds();

        GameUi::createDummySkin();

        std::shared_ptr<UiLabelSkin> debugTextSkin = UiLabelSkin::create(r->makeAssetPath("fnt", DEBUG_FONT), "20px");

        Gu::getContext()->getGui()->getTex()->loadImages();
        GameUi::createToolbar();
        GameUi::createCursor();

        //textbox test
        _pDebugLabel = UiLabel::create("", debugTextSkin);
        _pDebugLabel->position() = UiPositionMode::e::Relative;
        _pDebugLabel->left() = "20px";
        _pDebugLabel->top() = "100px";
        _pDebugLabel->width() = "200px";
        _pDebugLabel->height() = "1800px";
        _pDebugLabel->enableWordWrap();
        Gu::getContext()->getGui()->addChild(_pDebugLabel);

        Gu::getContext()->getGui()->getTex()->compile();

        //_pWorldEditState->init();
        BroLogInfo("Finished.." + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");
    }
}
void GameUi::clearDebugText(){
    if(_pDebugLabel != nullptr) {
        _pDebugLabel->setText("Debug\n");
    }
}
void GameUi::dbgLine(std::string txt){
    if (_pDebugLabel != nullptr) {
        std::string dtxt = _pDebugLabel->getText();
        dtxt += txt;
        dtxt += "\r\n";
        _pDebugLabel->setText(dtxt);
    }

}
void GameUi::endDebugText() {
    if (_pDebugLabel != nullptr) {
        int nn=0;
        nn++;
    }
}
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiAssetItem> UiAssetItem::create( std::shared_ptr<ModelSpec> ms,float contW, float contH,
    std::shared_ptr<UiFlexButtonSkin> flexSkin, std::shared_ptr<UiTex> coinImage) {

    /*
    -----------------------------
    | --------------------- |   |
    |                       |   |padR0
    |                       |   |
    |        Image          |   |
    |                       |   |
    padC0                     |   |
    | --------------------- |  padC1
    |        Name           |   |
    | --------------------- |   |
    |    Coin | Coin Count  |   |
    | --------------------- |   |
    |---------------------------|
    |             padR1         |
    |---------------------------|

    */

    std::shared_ptr<UiAssetItem> ret = std::make_shared<UiAssetItem>();
    ret->_pNode = ms;
    ret->init();

    //float contentHeight = contH -
    //    flexBorder->get(GridBorder::e::Top).px() -
    //    flexBorder->get(GridBorder::e::Bot).px();

    ret->width() = uDim(contW, UiDimUnit::e::Pixel);
    ret->height() = uDim(contH, UiDimUnit::e::Pixel);

    std::shared_ptr<UiFlexButton> expb = UiFlexButton::create(flexSkin);
    expb->width() = "100%";
    expb->height() = "100%";

    //The asset grid spacing with a "10%" width for each border
    std::shared_ptr<UiGrid> pPadGrid = UiGrid::create(0, 0);
    pPadGrid->setName("WigglyBoo");
    pPadGrid->width() = "100%";
    pPadGrid->height() = "100%";
    std::shared_ptr<UiGridRow> padR0 = pPadGrid->addRow(1);
    padR0->width() = "100%";
    padR0->height() = "90%";
    std::shared_ptr<UiGridRow> padR1 = pPadGrid->addRow(1);
    padR1->width() = "100%";
    padR1->height() = "10%";
    std::shared_ptr<UiElement> padC0 = padR0->addCol(1);
    padC0->width() = "90%";
    padC0->height() = "100%";
    std::shared_ptr<UiElement> padC1 = padR0->addCol(1);
    padC1->width() = "10%";
    padC1->height() = "100%";
    ////

    std::shared_ptr<UiGrid> pContentGrid = UiGrid::create(0, 0);
    pContentGrid->width() = "100%";
    pContentGrid->height() = "100%";
    std::shared_ptr<UiGridRow> r0 = pContentGrid->addRow(1);
    std::shared_ptr<UiGridRow> r1 = pContentGrid->addRow(1);
    std::shared_ptr<UiGridRow> r2 = pContentGrid->addRow(1);

    std::shared_ptr<UiElement> c0 = r0->addCol(1);
    std::shared_ptr<UiElement> c1 = r1->addCol(1);
    //Coin + Text
    std::shared_ptr<UiElement> c2 = r2->addCol(1);
  //  std::shared_ptr<UiElement> c3 = r2->addCol(1);

    r0->height() = "80%";
    r1->height() = "10%";
    r2->height() = "10%";

    c0->height() = "100%";
    c1->height() = "100%";
    c2->height() = "100%";
   // c3->height() = "100%";

    c0->width() = "100%";
    c1->width() = "100%";

    c2->width() = "100%";
 //   c3->width() = "auto";

    std::shared_ptr<UiTex> thumbTex = UiTex::create(ms->getThumb());
    std::shared_ptr<UiImage> childImage = UiImage::create(thumbTex, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    childImage->disablePick();
    c0->addChild(childImage);
    c1->addChild(UiLabel::create(ms->getFriendlyName(), flexSkin->_pLabelFont));
    std::shared_ptr<UiImage> coinImg = UiImage::create(coinImage, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Proportion);
    coinImg->display() = (UiDisplayMode::e::InlineNoWrap);
    coinImg->width() = "20px";
    coinImg->height() = "20px";
    coinImg->padLeft() = "20px";
    coinImg->padRight() = "10px";
    c2->addChild(coinImg);
    std::shared_ptr<UiLabel> coinCost = UiLabel::create("4", UiLabelSkin::create(Gu::getContext()->getRoom()->makeAssetPath("fnt", SUB_FONT)));
    coinCost->display() = (UiDisplayMode::e::InlineNoWrap);
    coinCost->width() = "auto";
    coinCost->height() = "100%";
    c2->addChild(coinCost);

   // std::weak_ptr<BottleRoom> weak_b = _PROC_THREAD_ATTRIBUTE_NUM
    std::weak_ptr<UiAssetItem> weak_a = ret->getThis<UiAssetItem>();
    expb->setClick([weak_a]() {
        if (std::shared_ptr<UiAssetItem> ua = weak_a.lock()) {
            std::shared_ptr<BottleRoom> br = std::dynamic_pointer_cast<BottleRoom>(Gu::getContext()->getRoom());
            if(br) {
                std::shared_ptr<ModelNode> mn;
                br->placeObject(ua->_pNode);
            }
        }
    });


    expb->getContentContainer()->addChild(pContentGrid);
    padC0->addChild(expb);//Pad grid col 0
    ret->addChild(pPadGrid);

    return ret;
}
//////////////////////////////////////////////////////////////////////////
std::string GameUi::makeAssetPath(std::string dir, std::string p){
    return _pRoom->makeAssetPath(dir,p);
}
void GameUi::createCursor() {
    std::shared_ptr<UiCursor> cs = UiCursor::create(_pGuiSkin->pCursorSkin);
    cs->width() = "32px";
    cs->height() = "auto"; // Auto?
    Gu::getContext()->getGui()->setCursor(cs);
}
void GameUi::createDummySkin() {

    std::shared_ptr<UiLabelSkin> lbl = UiLabelSkin::create(makeAssetPath("fnt", SUB_FONT));

    //**Chinese font test.
    //std::shared_ptr<UiLabelSkin> lbl2 = UiLabelSkin::create(makeAssetPath("fnt", "simhei-chinese.ttf"));
    //**Chinese font test.

    std::shared_ptr<UiWindowSkin> ws = std::make_shared<UiWindowSkin>();
    ws->_pBorder = UiBorderDim::create("50px", "50px", "50px", "50px");
    ws->_pBorderImgs = Ui9Tex::create(makeAssetPath("ui", "wings-window-9p.png"));
    ws->_pCloseButton = std::make_shared<UiButtonSkin>();
    ws->_pCloseButton->_pUp = UiTex::create(makeAssetPath("ui", "wings-win-canc-up.png"));
    ws->_pCloseButton->_pHv = UiTex::create(makeAssetPath("ui", "wings-win-canc-hv.png"));
    ws->_pCloseButton->_pDn = UiTex::create(makeAssetPath("ui", "wings-win-canc-dn.png"));
    
    ws->_pHScroll = std::make_shared<UiScrollbarSkin>();
    ws->_pHScroll->_eOrientation = Orientation::e::Horizontal;
    ws->_pHScroll->_pBackImgs = Ui3Tex::create(makeAssetPath("ui", "wings-scroll-h-3p.png"));
    ws->_pHScroll->_pThumbImgs = Ui3Tex::create(makeAssetPath("ui", "wings-scroll-thumb-h-3p.png"));
    ws->_pHScroll->_uWidthOrHeightPx = ws->_pBorder->get(GridBorder::e::Right);

    ws->_pVScroll = std::make_shared<UiScrollbarSkin>();
    ws->_pVScroll->_eOrientation = Orientation::e::Vertical;
    ws->_pVScroll->_pBackImgs = Ui3Tex::create(makeAssetPath("ui", "wings-scroll-v-3p.png"));
    ws->_pVScroll->_pThumbImgs = Ui3Tex::create(makeAssetPath("ui", "wings-scroll-thumb-v-3p.png"));
    ws->_pVScroll->_uWidthOrHeightPx = ws->_pBorder->get(GridBorder::e::Bot);
    
    ws->_pTitleFont = std::make_shared<UiLabelSkin>();
    *ws->_pTitleFont = *lbl;

    //std::shared_ptr<UiTrackbarSkin> pHTrackbarSkin = nullptr;
    std::shared_ptr<UiSliderSkin> tbs = std::make_shared<UiSliderSkin>();
    tbs->_eOrientation = Orientation::e::Horizontal;
    tbs->_pBackImgs = Ui3Tex::create(makeAssetPath("ui", "wings-track-h-3p.png"));
    tbs->_pThumbImgs = Ui3Tex::create(makeAssetPath("ui", "wings-track-thumb-h-3p.png"));
    tbs->_uWidthOrHeightPx = ws->_pBorder->get(GridBorder::e::Right);
    
    std::shared_ptr<UiFlexButtonSkin> fb = std::make_shared<UiFlexButtonSkin>();
    fb->_border = UiBorderDim::create("6px", "6px", "6px", "6px");
    fb->_pUp = Ui9Tex::create(makeAssetPath("ui", "wings-button-9p-up.png"));
    fb->_pHv = Ui9Tex::create(makeAssetPath("ui", "wings-button-9p-hv.png"));
    fb->_pDn = Ui9Tex::create(makeAssetPath("ui", "wings-button-9p-dn.png"));
    fb->_pLabelFont = std::make_shared<UiLabelSkin>();
    *fb->_pLabelFont = *lbl;


    std::shared_ptr<UiDropdwonSkin> dd = std::make_shared<UiDropdwonSkin>();
    dd->_pBorder = UiBorderDim::create("4px", "4px", "4px", "4px");
    dd->_pBorderImgs = Ui9Tex::create(makeAssetPath("ui", "wings-box-9p.png"));
    dd->_pFont = UiLabelSkin::create(makeAssetPath("fnt", SUB_FONT));
    dd->_arrow = UiTex::create(makeAssetPath("ui", "wings-dd-arrow.png"));
    dd->_pArrowButtonSkin = std::make_shared<UiFlexButtonSkin>();
    *dd->_pArrowButtonSkin = *fb;


    std::shared_ptr<UiCheckboxSkin> cbs = std::make_shared<UiCheckboxSkin>();
    cbs->_pCheckImg = UiTex::create(makeAssetPath("ui", "wings-checkbox-check.png"));
    cbs->_pFlexButtonSkin = std::make_shared<UiFlexButtonSkin>();
    *cbs->_pFlexButtonSkin = *fb;
    cbs->_pFlexButtonSkin->_border = UiBorderDim::create("6px", "6px", "6px", "6px");
    cbs->_pLabelFont = UiLabelSkin::create(makeAssetPath("fnt", CHECKBOX_FONT));


    //////////////////////////////////////////////////////////////////////////

    _pGuiSkin = std::make_shared<SKIN_TEMP>();
    _pGuiSkin->pToolbarSkin = std::make_shared<UiToolbarSkin>();
    _pGuiSkin->pToolbarSkin->_pWindowSkin = std::make_shared<UiWindowSkin>();
    *_pGuiSkin->pToolbarSkin->_pWindowSkin = *ws;

    
    _pGuiSkin->pButtonClickSound = Gu::getContext()->getRoom()->makeAssetPath("snd", "click_0.ogg");

    _pGuiSkin->pDropdownSkin = dd;
    _pGuiSkin->pCheckboxSkin = cbs;
    _pGuiSkin->pHTrackbarSkin = tbs;

    _pGuiSkin->pToolbarSkin->_pWindowSkin->_pBorder = UiBorderDim::create("0px", "32px", "32px", "32px");
    _pGuiSkin->pToolbarSkin->_pWindowSkin->_pBorderImgs = Ui9Tex::create(makeAssetPath("ui", "wings-toolbar-9p.png"));
    _pGuiSkin->pToolbarSkin->_pFlexButtonSkin = std::make_shared<UiFlexButtonSkin>();
    *_pGuiSkin->pToolbarSkin->_pFlexButtonSkin = *fb;

    _pGuiSkin->pCursorSkin = std::make_shared<UiCursorSkin>();
    _pGuiSkin->pCursorSkin->_pTex = UiTex::create(makeAssetPath("ui", "wings-cursor.png"));

    //**Asset window
    _pGuiSkin->pAssetWindowSkin = std::make_shared<UiAssetWindowSkin>();
    _pGuiSkin->pAssetWindowSkin->_pWindowSkin = std::make_shared<UiWindowSkin>();
    *_pGuiSkin->pAssetWindowSkin->_pWindowSkin = *ws;
    _pGuiSkin->pAssetWindowSkin->_pFlexButtonSkin = std::make_shared<UiFlexButtonSkin>();
    *_pGuiSkin->pAssetWindowSkin->_pFlexButtonSkin = *fb;

    _pGuiSkin->pTbMenu = UiTex::create(makeAssetPath("ui", "menu-bt-img.png"));
    _pGuiSkin->pTbObjects = UiTex::create(makeAssetPath("ui", "tree-bt-img.png"));
    _pGuiSkin->pTbSettings = UiTex::create(makeAssetPath("ui", "gear-bt-img.png"));
    _pGuiSkin->pCoinImg = UiTex::create(makeAssetPath("ui", "coin-bt-img.png"));
    _pGuiSkin->pTbGrid0 = UiTex::create(makeAssetPath("ui", "grid0-bt-img.png"));
    _pGuiSkin->pTbGrid1 = UiTex::create(makeAssetPath("ui", "grid1-bt-img.png"));
    _pGuiSkin->pTbGrid2 = UiTex::create(makeAssetPath("ui", "grid2-bt-img.png"));
    _pGuiSkin->pTbTerrain = UiTex::create(makeAssetPath("ui", "terrain-bt-img.png"));
    _pGuiSkin->pTbHouse = UiTex::create(makeAssetPath("ui", "house-bt-img.png"));
}
void GameUi::createToolbar() {
    //////////////////////////////////////////////////////////////////////////
    //Expando-Button
    uDim spacing = "10px";
    std::shared_ptr<SKIN_TEMP> sk = _pGuiSkin;

    //OBJECTS BUTTON
    std::shared_ptr<UiFlexButton> tbObjects = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbObjects->position() = UiPositionMode::Static;
    tbObjects->display() = (UiDisplayMode::e::InlineNoWrap);
    tbObjects->width() = "80px";
    tbObjects->height() = "80px";
    tbObjects->padRight() = spacing;
    tbObjects->getContentContainer()->addChild(UiImage::create(_pGuiSkin->pTbObjects, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand));
    std::shared_ptr<UiWindow> winObjects = createAssetWindow();
    std::weak_ptr<UiWindow> winObjects_wekak = winObjects;
    tbObjects->setClick([winObjects_wekak, sk]() {
        if (std::shared_ptr<UiWindow> win = winObjects_wekak.lock()) {
            Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
            win->show();
        }
    });

    //CONFIG BUTTON
    std::shared_ptr<UiFlexButton> tbConfig = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbConfig->position() = UiPositionMode::Static;
    tbConfig->display() = (UiDisplayMode::e::InlineNoWrap);
    tbConfig->width() = "80px";
    tbConfig->height() = "80px";
    tbConfig->padRight() = spacing;
    tbConfig->getContentContainer()->addChild(UiImage::create(_pGuiSkin->pTbSettings, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand));
    std::shared_ptr<UiWindow> winConfig = createConfigWindow();
    std::weak_ptr<UiWindow> winConfig_weak = winConfig;
    tbConfig->setClick([winConfig_weak, sk]() {
        if (std::shared_ptr<UiWindow> win = winConfig_weak.lock()) {
            Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
            win->show();
        }
    });

    //MENU BUTTON
    std::shared_ptr<UiFlexButton> tbMenu = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbMenu->position() = UiPositionMode::Static;
    tbMenu->display() = (UiDisplayMode::e::InlineNoWrap);
    tbMenu->width() = "80px";
    tbMenu->height() = "80px";
    tbMenu->padRight() = spacing;
    tbMenu->getContentContainer()->addChild(UiImage::create(_pGuiSkin->pTbMenu, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand));
    std::shared_ptr<UiWindow> winMenu = createMenuWindow();
    std::weak_ptr<UiWindow> winMenu_wekak = winMenu;
    tbMenu->setClick([winMenu_wekak, sk]() {
        if (std::shared_ptr<UiWindow> win = winMenu_wekak.lock()) {
            Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
            win->show();


        }
    });
    //GRID BUTTON
    std::shared_ptr<UiFlexButton> tbGrid = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbGrid->position() = UiPositionMode::Static;
    tbGrid->display() = (UiDisplayMode::e::InlineNoWrap);
    tbGrid->width() = "80px";
    tbGrid->height() = "80px";
    tbGrid->padRight() = spacing;
    std::shared_ptr<UiImage> buttonImg = UiImage::create(_pGuiSkin->pTbGrid0, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    tbGrid->getContentContainer()->addChild(buttonImg);
    std::shared_ptr<BottleRoom> r = _pRoom ;
    std::weak_ptr<UiImage> img_w = buttonImg;
    tbGrid->setClick([r, img_w, sk]() {
        if(std::shared_ptr<UiImage> img =  img_w.lock()){
            Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
            GridShow::e eShow = r->toggleShowGrid();
            if (eShow == GridShow::e::None) {
                img->setTexture(sk->pTbGrid0);
            }
            else if (eShow == GridShow::e::Top) {
                img->setTexture(sk->pTbGrid1);
            }
            else if (eShow == GridShow::e::TopSideBot) {
                img->setTexture(sk->pTbGrid2);
            }
            else {
                Gu::debugBreak();
            }
        }

    });

    //Terrain BUTTON
    std::shared_ptr<UiFlexButton> tbTerrain = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbTerrain->position() = UiPositionMode::Static;
    tbTerrain->display() = (UiDisplayMode::e::InlineNoWrap);
    tbTerrain->width() = "80px";
    tbTerrain->height() = "80px";
    tbTerrain->padRight() = spacing;
    std::shared_ptr<UiImage> terimg = UiImage::create(_pGuiSkin->pTbTerrain, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    tbTerrain->getContentContainer()->addChild(terimg);
    std::shared_ptr<UiWindow> winTer = createTerrainWindow();
    std::weak_ptr<UiWindow> winTer_wekak = winTer;
    tbTerrain->setClick([winTer_wekak, sk]() {
        if (std::shared_ptr<UiWindow> win = winTer_wekak.lock()) {
            Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
            win->show();
        }
    });

    //House BUTTON
    std::shared_ptr<UiFlexButton> tbHouse = UiFlexButton::create(_pGuiSkin->pToolbarSkin->_pFlexButtonSkin);
    tbHouse->position() = UiPositionMode::Static;
    tbHouse->display() = (UiDisplayMode::e::InlineNoWrap);
    tbHouse->width() = "80px";
    tbHouse->height() = "80px";
    tbHouse->padRight() = spacing;
    std::shared_ptr<UiImage> himg = UiImage::create(_pGuiSkin->pTbHouse, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    tbHouse->getContentContainer()->addChild(himg);
    
    tbHouse->setClick([sk]() {
        Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
        std::shared_ptr<BottleRoom> br = std::dynamic_pointer_cast<BottleRoom>(Gu::getContext()->getRoom());
        if(br){
            br->getGodCam()->setTarget(vec3(0,0,0));
        }

        //if (std::shared_ptr<UiWindow> win = winTer_wekak.lock()) {
        //    Gu::getContext()->getSoundCache()->tryPlay(sk->pButtonClickSound);
        //    win->show();
        //}
    });

    //////////////////////////////////////////////////////////////////////////
    std::shared_ptr<UiToolbar> tb = UiToolbar::create(_pGuiSkin->pToolbarSkin);
   // tb->enableResize();
    tb->getContentContainer()->addChild(tbObjects);
    tb->getContentContainer()->addChild(tbConfig);
    tb->getContentContainer()->addChild(tbMenu);
    tb->getContentContainer()->addChild(tbGrid);
    tb->getContentContainer()->addChild(tbTerrain);
    tb->getContentContainer()->addChild(tbHouse);
    tb->getContentContainer()->overflow() = (UiOverflowMode::e::Hide);
    tb->display() = UiDisplayMode::e::InlineNoWrap;

    //float px = 0;
    //for(auto c : tb->getContentContainer()->getChildren()){
    //    if(c.second->width().getDimUnit() == UiDimUnit::Pixel){
    //        px += c.second->width().px() + c.second->padRight().px();
    //    }
    //}
  //  px+= _pGuiSkin->pToolbarSkin->_pWindowSkin->_pBorder->get(GridBorder::e::Left).px() + _pGuiSkin->pToolbarSkin->_pWindowSkin->_pBorder->get(GridBorder::e::Right).px();
  //  tb->width() = uDim(px, UiDimUnit::e::Pixel);

    Gu::getContext()->getGui()->addChild(tb);


}
std::shared_ptr<UiWindow> GameUi::createAssetWindow() {
    //////////////////////////////////////////////////////////////////////////
    //Window
    std::vector<std::shared_ptr<Texture2DSpec>> texs;
    std::shared_ptr<UiWindow> winObjects = UiWindow::create(_pGuiSkin->pAssetWindowSkin->_pWindowSkin);
    winObjects->position() = UiPositionMode::Relative;
    float dw = Gu::getContext()->getGui()->getDesignSize().getWidth();
    float dh = Gu::getContext()->getGui()->getDesignSize().getHeight();
    winObjects->left() = uDim(dw / 6 * 1, UiDimUnit::e::Pixel);
    winObjects->width() = uDim(dw / 6 * 4, UiDimUnit::e::Pixel);
    winObjects->top() = uDim(dh / 6 * 1, UiDimUnit::e::Pixel);
    winObjects->height() = uDim(dh / 6 * 2, UiDimUnit::e::Pixel);
   // winObjects->enableCloseButton();
    winObjects->setTitleLabel("Objects");
    winObjects->enableVScrollbar();
    winObjects->enableHScrollbar();
    winObjects->enableResize();
    winObjects->enableDrag();
    winObjects->hide();

    //////////////////////////////////////////////////////////////////////////
    //Create the asset pad grid.
    int buttonHeight = 250;
    int buttonWidth = (int)((float)buttonHeight * 0.7);

    //Note moved the grid code to "cutcode"
    //////////////////////////////////////////////////////////////////////////
    //Add Asset item
    std::shared_ptr<UiTex> coinImg = _pGuiSkin->pCoinImg;
    for (auto ws : _pRoom->getGameFile()->getMobSpecs()) {
        std::shared_ptr<ModelSpec> ms = Gu::getContext()->getModelCache()->getOrLoadModel(ws->getMobName());
        if (ms != nullptr) {
            winObjects->getContentContainer()->addChild(UiAssetItem::create(
                ms,
                buttonWidth, buttonHeight,
                _pGuiSkin->pAssetWindowSkin->_pFlexButtonSkin,
                coinImg
            ));
        }
        else {
            BroLogError("Failed to find asset '" + ws->getMobName() + "'");
            Gu::debugBreak();
        }
    }
    Gu::getContext()->getGui()->addChild(winObjects);

    return winObjects;
}
std::shared_ptr<UiWindow> GameUi::createConfigWindow() {
    //////////////////////////////////////////////////////////////////////////
    //Window
    std::vector<std::shared_ptr<Texture2DSpec>> texs;
    std::shared_ptr<UiWindow> winConfig = UiWindow::create(_pGuiSkin->pAssetWindowSkin->_pWindowSkin);
    winConfig->position() = UiPositionMode::Relative;
    float dw = Gu::getContext()->getGui()->getDesignSize().getWidth();
    float dh = Gu::getContext()->getGui()->getDesignSize().getHeight();
    winConfig->left() = uDim(dw / 4 * 1, UiDimUnit::e::Pixel);
    winConfig->width() = uDim(dw / 4 * 2, UiDimUnit::e::Pixel);
    winConfig->top() = uDim(dh / 4 * 1, UiDimUnit::e::Pixel);
    winConfig->height() = uDim(dh / 4 * 2, UiDimUnit::e::Pixel);
   // winConfig->enableCloseButton();
    winConfig->setTitleLabel("Config");
    winConfig->enableVScrollbar();
    //winConfig->enableHScrollbar();
    winConfig->enableResize();
    winConfig->enableDrag();
    winConfig->hide();

    std::shared_ptr<SKIN_TEMP> gg = _pGuiSkin;
    std::shared_ptr<UiLabelSkin> skHead = UiLabelSkin::create(makeAssetPath("fnt", SUB_FONT), "40px");

    //Dropdown
    std::function<void(std::vector<std::string> items)> makeDD =
        [winConfig, gg](std::vector<std::string> items) {

        std::shared_ptr<UiDropdown> dd = UiDropdown::create(gg->pDropdownSkin);
        
        dd->position() = UiPositionMode::e::Static;
        dd->display() = (UiDisplayMode::e::Block);
        dd->width() = "100%";
        dd->height() = "50px";
        dd->padLeft() = "0px";
        dd->padBottom() = "32px";

        for(int i=0; i<items.size(); ++i){
            std::shared_ptr<UiLabel> lbl = UiLabel::create(items[i], gg->pDropdownSkin->_pFont);
            lbl->position() = UiPositionMode::e::Static;
            lbl->display() = (UiDisplayMode::e::Block);
            lbl->height() = "32px";
            lbl->width() = "100%";
            dd->addItem(lbl);
        }

        winConfig->getContentContainer()->addChild(dd);
    };
    //Slider
    std::function<void(std::string, float, float, float, float, UiScrubGen::ScrollFunc)> makeSD =
        [winConfig, gg, skHead](std::string label, float initial, float fmin, float fmax, float fIncrement, UiScrubGen::ScrollFunc sf) {
        std::shared_ptr<UiSlider> tb = UiSlider::create(gg->pHTrackbarSkin, sf);


        std::shared_ptr<UiLabel> lbl = UiLabel::create(label, skHead);
        lbl->position() = UiPositionMode::e::Static;
        lbl->display() = (UiDisplayMode::e::Block);
        lbl->width() = "90px";
        lbl->height() = "50px";
        lbl->padBottom() = "10px";
        winConfig->getContentContainer()->addChild(lbl);

        tb->position() = UiPositionMode::e::Static;
        tb->display() = (UiDisplayMode::e::InlineNoWrap);
        tb->width() = "200px";
        tb->height() = "50px";
        tb->padLeft() = "0px";
        tb->padBottom() = "10px";
        tb->minVal() = fmin;
        tb->maxVal() = fmax;
        tb->val() = initial;
        tb->increment() = fIncrement;
        winConfig->getContentContainer()->addChild(tb);

        std::shared_ptr<UiLabel> lbl2 = UiLabel::create("0", skHead);
        lbl2->position() = UiPositionMode::e::Static;
        lbl2->display() = (UiDisplayMode::e::InlineNoWrap);
        lbl2->width() = "auto";
        lbl2->height() = "50px";
        lbl2->padBottom() = "10px";
        winConfig->getContentContainer()->addChild(lbl2);
        tb->setValueLabel(lbl2);
    };
    //Checkbox
    std::function<void(bool, std::string, std::function<void(bool)>)> makeChk =
        [winConfig, gg](bool bChecked, std::string label, std::function<void(bool)> checkFunc) {
        std::shared_ptr<UiCheckbox> chk = UiCheckbox::create(gg->pCheckboxSkin, label);
        float chkHeight = 40;
        chk->position() = UiPositionMode::e::Static;
        chk->display() = (UiDisplayMode::e::Block);
        chk->width() = "100%";
        chk->height() = "50px";
        chk->padLeft() = "20px";
        chk->padBottom() = "0px";
        chk->setChecked(bChecked);

        chk->setCheckFunc(checkFunc);
        winConfig->getContentContainer()->addChild(chk);
    };
    //Head
    std::function<void(std::string, int)> makeHead =
        [winConfig, skHead](std::string label, int padLevel) {

        std::shared_ptr<UiLabel> lbl = UiLabel::create(label, skHead);
        lbl->position() = UiPositionMode::e::Static;
        lbl->display() = (UiDisplayMode::e::Block);
        lbl->width() = "100%";
        lbl->height() = "50px";
        lbl->padLeft() = uDim(padLevel * 20, UiDimUnit::e::Pixel);
        lbl->padBottom() = "10px";

        winConfig->getContentContainer()->addChild(lbl);
    };

    makeDD(std::vector<std::string>({ "Test0", "test1", "test2..", "Finally, test3" }));
    makeSD( "FOV", Gu::getContext()->getCamera()->getFOV(), 45.0f, 179.0f, 1.0f, [](std::shared_ptr<UiScrubGen> sb, float fScroll) {
        Gu::getContext()->getCamera()->setFOV(fScroll);
    });
    makeSD("Draw Dist", Gu::getContext()->getRenderSettings()->drawDistance(), 2.0f, 60.0f, 1.0f, [](std::shared_ptr<UiScrubGen> sb, float fScroll) {
        Gu::getContext()->getRenderSettings()->drawDistance() = fScroll;
    });

    makeHead("Render", 0);
    makeChk(Gu::getContext()->getRenderSettings()->getDOF(), "DOF", [](bool b) { Gu::getContext()->getRenderSettings()->getDOF() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getSmoothShadows(), "Shadow Smoothing", [](bool b) { Gu::getContext()->getRenderSettings()->getSmoothShadows() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->enableTransparency(), "Enable Transparency", [](bool b) {Gu::getContext()->getRenderSettings()->enableTransparency() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getBloom(), "Bloom", [](bool b) {Gu::getContext()->getRenderSettings()->getBloom() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getAmbientOcclusion(), "SSAO", [](bool b) {Gu::getContext()->getRenderSettings()->getAmbientOcclusion() = b; });
    makeSD(" Samples", (float)Gu::getContext()->getRenderSettings()->getAmbientOcclusionSamples(), 1, 100, 1, [](std::shared_ptr<UiScrubGen> sb, float fScroll) {Gu::getContext()->getRenderSettings()->getAmbientOcclusionSamples() = fScroll; });

    makeChk((Gu::getContext()->getGui()->getTex()->oglGetFilter() == TexFilter::e::Linear), "Toggle UI Texture Filter", [](bool b) {
        if (Gu::getContext()->getGui() && Gu::getContext()->getGui()->getTex()){
            if (b) {

                Gu::getContext()->getGui()->getTex()->oglSetFilter(TexFilter::e::Linear);
            }
            else {
                Gu::getContext()->getGui()->getTex()->oglSetFilter(TexFilter::e::Nearest);
            }
        }else { BroLogError("No GUI tex. ERR!");}
    });

    makeHead("", 0);

    makeHead("Debug", 0);
    makeHead("Boxes", 1);
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowModelBoundBox(), "Model Boundbox", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowModelBoundBox() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowBoneBoxes(), "Bone Boxes", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowBoneBoxes() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowMeshBoxes(), "Mesh Boxes", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowMeshBoxes() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowBoneBindBoxes(), "Bone Bind Boxes", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowBoneBindBoxes() = b; });

    makeHead("Visuals", 1);
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowNormals(), "Normals", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowNormals() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowArmatures(), "Armatures", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowArmatures() = b; });
    makeChk(Gu::getContext()->getRenderSettings()->getDebug()->getShowShadowBox(), "Shadow Boxes", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowShadowBox() = b; });

    makeHead("Gui", 1);
    makeChk(false, "Pick Gui", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getPickGui() = b; });
    makeChk(false, "Gui Boxes & Disable Clip", [](bool b) { Gu::getContext()->getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping() = b; });
   


    Gu::getContext()->getGui()->addChild(winConfig);

    return winConfig;
}
std::shared_ptr<UiWindow> GameUi::createMenuWindow() {
    //////////////////////////////////////////////////////////////////////////
    //Window
    std::vector<std::shared_ptr<Texture2DSpec>> texs;
    std::shared_ptr<UiWindow> win = UiWindow::create(_pGuiSkin->pAssetWindowSkin->_pWindowSkin);
    win->position() = UiPositionMode::Relative;
    float dw = Gu::getContext()->getGui()->getDesignSize().getWidth();
    float dh = Gu::getContext()->getGui()->getDesignSize().getHeight();
    win->left() = uDim(dw / 3 * 1, UiDimUnit::e::Pixel);
    win->width() = uDim(dw / 3 * 1, UiDimUnit::e::Pixel);
    win->top() = uDim(dh / 3 * 1, UiDimUnit::e::Pixel);
    win->height() = uDim(dh / 3 * 1, UiDimUnit::e::Pixel);
   // win->enableCloseButton();
    win->setTitleLabel("Menu");
    win->enableVScrollbar();
    win->enableHScrollbar();
    win->enableResize();
    win->enableDrag();
    win->hide();

    std::shared_ptr<SKIN_TEMP> gg = _pGuiSkin;
    std::function<void(std::string, std::function<void()>)> menuOption =
        [win, gg](std::string label, std::function<void()> clickFunc) {

        std::shared_ptr<UiFlexButton> bt = UiFlexButton::create(gg->pAssetWindowSkin->_pFlexButtonSkin);
        bt->position() = UiPositionMode::e::Static;
        bt->width() = "100%";
        bt->height() = "80px";
        bt->padBottom() = "16px";
        bt->setLabel(label);
        bt->setClick(clickFunc);
        win->getContentContainer()->addChild(bt);
    };
    //

    menuOption("Exit Game", []() {
        exit(0);//LOL!
    });
    std::weak_ptr<UiWindow> win_weak = win;
    menuOption("Close", [win_weak]() {
        if(std::shared_ptr<UiWindow> win = win_weak.lock()){
            win->hide();
        }
    });

    Gu::getContext()->getGui()->addChild(win);

    return win;
}
std::shared_ptr<UiWindow> GameUi::createTerrainWindow(){
    //////////////////////////////////////////////////////////////////////////
    //Window
    std::vector<std::shared_ptr<Texture2DSpec>> texs;
    std::shared_ptr<UiWindow> win = UiWindow::create(_pGuiSkin->pAssetWindowSkin->_pWindowSkin);
    win->position() = UiPositionMode::Relative; 
    win->left() = "10px";
    win->top() = "200px";
    win->width() = "400px";
    win->height() = "700px";
    //win->enableCloseButton();
    win->setTitleLabel("Terrain");
    win->enableVScrollbar();
    win->enableHScrollbar();
    win->enableResize();
    win->enableDrag();
    win->hide();
    //std::shared_ptr<UiFlexButton> bt = UiFlexButton::create(_pGuiSkin->pAssetWindowSkin->_pFlexButtonSkin);
    //bt->position() = UiPositionMode::e::Static;
    //bt->width() = "100%";
    //bt->height() = "80px";
    //bt->padBottom() = "16px";
    //bt->setLabel(label);
    //bt->setClick(clickFunc);
    //win->getContentContainer()->addChild(bt);

    //std::shared_ptr<UiDropdown> dd = UiDropdown::create(_pGuiSkin->pDropdownSkin);

    //dd->position() = UiPositionMode::e::Static;
    //dd->display() = (UiDisplayMode::e::Block);
    //dd->width() = "100%";
    //dd->height() = "50px";
    //dd->padLeft() = "0px";
    //dd->padBottom() = "32px";

    //std::vector<std::string> items({"Block"});
    //for (int i = 0; i < items.size(); ++i) {
    //    std::shared_ptr<UiLabel> lbl = UiLabel::create(items[i], _pGuiSkin->pDropdownSkin->_pFont);
    //    lbl->position() = UiPositionMode::e::Static;
    //    lbl->display() = (UiDisplayMode::e::Block);
    //    lbl->height() = "32px";
    //    lbl->width() = "100%";
    //    dd->addItem(lbl);
    //}

    //win->getContentContainer()->addChild(dd);

    //For the sake of the UI just load all the images a second time

    std::shared_ptr<UiDropdown> ddTiles = UiDropdown::create(_pGuiSkin->pDropdownSkin);

    ddTiles->position() = UiPositionMode::e::Static;
    ddTiles->display() = (UiDisplayMode::e::Block);
    ddTiles->width() = "130px";
    ddTiles->height() = "100px";
    ddTiles->padLeft() = "0px";
    ddTiles->padBottom() = "32px";
    ddTiles->setContainerWidth("200px");
    //for (int i = 0; i < items.size(); ++i) {
    //    std::shared_ptr<UiLabel> lbl = UiLabel::create(items[i], _pGuiSkin->pDropdownSkin->_pFont);
    //    lbl->position() = UiPositionMode::e::Static;
    //    lbl->display() = (UiDisplayMode::e::Block);
    //    lbl->height() = "32px";
    //    lbl->width() = "100%";
    //    ddTiles->addItem(lbl);
    //}


    for (auto p : _pRoom->getWorld25()->getSpriteBucket()->getSpecs()) {
        std::shared_ptr<SpriteSpec> ss = p.second;
        if(ss->getFrames().size()>0){
            std::string imgName = ss->getFrames()[0]->getImageName();
            std::shared_ptr<UiImage> img = UiImage::create(UiTex::create(imgName, true),UiImageSizeMode::e::Expand, UiImageSizeMode::e::Proportion);
            img->display() = (UiDisplayMode::e::InlineWrap);
            img->position() = UiPositionMode::e::Static;
            img->width() = "64px";
            img->height() = "64px";
            
            img->padTop() = "6px";
            img->padLeft() = "6px";
            img->padRight() = "6px";
            img->padBottom() = "6px";
            ddTiles->addItem(img, false);
        }
    }
    win->getContentContainer()->addChild(ddTiles);


    Gu::getContext()->getGui()->addChild(win);

    return win;
}









}//ns Game
