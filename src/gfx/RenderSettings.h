/**
*  @file RenderSettings.h
*  @date February 23, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERSETTINGS_1519414886985932822_H__
#define __RENDERSETTINGS_1519414886985932822_H__

#include "../gfx/GfxHeader.h"


namespace BR2 {
class RenderDebug : public VirtualMemoryShared<RenderDebug> {
    bool _bShadowHelpVisible = false;
    bool _bShowBoneBoxes = false;
    bool _bShowMeshBoxes = false;
    bool _bShowNormals = false;
    bool _bShowArmatures = false;
    bool _bShowBoneBindBoxes = false;
    bool _bShowModelBoxes = false;
    bool _bShowModelBoundBox = false;
    bool _bShowShadowBox = false;
    bool _bShowGuiBoxes = false;
    bool _bPickGui = false;
    
public:
    bool& getShadowHelpVisible() { return _bShadowHelpVisible; }
    bool& getShowBoneBoxes() { return _bShowBoneBoxes; }
    bool& getShowMeshBoxes() { return _bShowMeshBoxes; }
    bool& getShowNormals() { return _bShowNormals; }
    bool& getShowArmatures() { return _bShowArmatures; }
    bool& getShowBoneBindBoxes() { return _bShowBoneBindBoxes; }
    bool& getShowModelBoxes() { return _bShowModelBoxes; }
    bool& getShowModelBoundBox() { return _bShowModelBoundBox; }
    bool& getShowShadowBox() { return _bShowShadowBox; }
    bool& getShowGuiBoxesAndDisableClipping() { return _bShowGuiBoxes; }
    bool& getPickGui() { return _bPickGui; }
};
/**
*    @class RenderSettings
*    @brief
*
*/
class RenderSettings : public VirtualMemoryShared<RenderSettings> {
    std::shared_ptr<RenderDebug> _pRenderDebug = nullptr;
    bool _bDOF = true;
    bool _bSmoothShadows = false;//2/24 doesn't work
    bool _bEnableTransparency = false;
    float _fDrawDistance = 12;//Number of nodes draw (bvh collection)
    bool _bAmbientOcclusion = false;
    int  _iAmbientOcclusionSamples = 20;
    bool _bBloom = false;
public:
    static std::shared_ptr<RenderSettings> create() { std::shared_ptr<RenderSettings> sp = std::make_shared<RenderSettings>(); sp->init(); return sp; }
    void init();
    bool& getDOF() { return _bDOF; }
    std::shared_ptr<RenderDebug> getDebug() { return _pRenderDebug; }

    bool& getSmoothShadows() { return _bSmoothShadows; }
    bool& enableTransparency() { return _bEnableTransparency; }
    float& drawDistance() {return _fDrawDistance;}
    
    //In the future
    bool& getAmbientOcclusion() { return _bAmbientOcclusion; }
    int & getAmbientOcclusionSamples() { return _iAmbientOcclusionSamples; }
    bool& getBloom() { return _bBloom; }
};


}//ns Game



#endif
