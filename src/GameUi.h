/**
*
*    @file GameUi.h
*    @date February 22, 2018
*    @author Derek Page
*
*    © 2018 
*
*
*/
#pragma once
#ifndef __GAMEUI_15193202931878573091_H__
#define __GAMEUI_15193202931878573091_H__

#include "../display/Gui2d.h"
#include "../bottle/BottleHeader.h"

namespace Game {

//TEMP for testing and debug fo the skin load
class SKIN_TEMP {
public:
    std::shared_ptr<UiToolbarSkin> pToolbarSkin = nullptr;
    std::shared_ptr<UiCursorSkin> pCursorSkin = nullptr;
    std::shared_ptr<UiCheckboxSkin> pCheckboxSkin = nullptr;
    std::shared_ptr<UiAssetWindowSkin> pAssetWindowSkin = nullptr;
    std::shared_ptr<UiDropdwonSkin> pDropdownSkin = nullptr;
    std::shared_ptr<UiSliderSkin> pHTrackbarSkin = nullptr;
    std::shared_ptr<UiAssetWindowSkin> pMenuWindowSkin = nullptr;
    std::shared_ptr<UiTex> pTbMenu;
    std::shared_ptr<UiTex> pTbGrid0;
    std::shared_ptr<UiTex> pTbGrid1;
    std::shared_ptr<UiTex> pTbGrid2;
    std::shared_ptr<UiTex> pTbObjects;
    std::shared_ptr<UiTex> pTbSettings;
    std::shared_ptr<UiTex> pCoinImg;
    std::shared_ptr<UiTex> pTbTerrain;
    std::shared_ptr<UiTex> pTbHouse ;
    std::string pButtonClickSound;
};
class UiAssetItem : public UiElement {
    std::shared_ptr<ModelSpec> _pNode = nullptr;
public:
    static std::shared_ptr<UiAssetItem> UiAssetItem::create(
        std::shared_ptr<ModelSpec> ms,
        float contW, float contH,
        std::shared_ptr<UiFlexButtonSkin> flexSkin,
        std::shared_ptr<UiTex> coinImage);
    virtual void UiAssetItem::init() override {
        UiElement::init();
    }
};

class GameUi : public VirtualMemoryShared<GameUi>{
    std::shared_ptr<UiLabel> _pDebugLabel;
public:
    std::shared_ptr<SKIN_TEMP> _pGuiSkin = nullptr;
    std::shared_ptr<BottleRoom> _pApp = nullptr;
    std::string makeAssetPath(std::string dir, std::string p);
   void createToolbar();
   void createCursor();
   void GameUi::createDummySkin();
   std::shared_ptr<UiWindow> createAssetWindow();
   std::shared_ptr<UiWindow> createConfigWindow();
   std::shared_ptr<UiWindow> createTerrainWindow() ;

   std::shared_ptr<UiWindow> createMenuWindow();
   void constructUI(std::shared_ptr<BottleRoom> r);

   void clearDebugText();
   void dbgLine(std::string txt);
   void endDebugText();
   
};

}//ns Game



#endif
