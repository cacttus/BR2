/**
*
*    @file ShadowFrustumSide.h
*    @date April 28, 2016
*    @author MetalMario971
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __SHADOWFRUSTUMSIDE_14618979642881895404_H__
#define __SHADOWFRUSTUMSIDE_14618979642881895404_H__

#include "../math/BoxUtils.h"
#include "../math/MathHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"
//#include "../material/MaterialHeader.h"
//#include "../scene/BvhHeader.h"

namespace Game {
/**
*    @class ShadowFrustumSide
*    @brief Side of a shadow cube frustum.
*        This is basically a camera with a view frustum.
*        Analogous to BaseCamera3D - without the extra node system overhead.
*/
class ShadowBoxSide : public VirtualMemory { 
private:
    std::shared_ptr<WindowViewport> _pViewport = nullptr;
    BoxSide::e _eSide;
    std::shared_ptr<FrustumBase> _pFrustum = nullptr;
    std::shared_ptr<LightNodePoint> _pLightSource = nullptr;
    mat4 _projMatrix;    // Frustum, Proj, View - basic camera parameters
    mat4 _viewMatrix;    //
    std::shared_ptr<RenderBucket> _pVisibleSet;
    std::shared_ptr<RenderBucket> _pLastSet;//TODO: check vs current set _bShadowDirty in BvhNOde
    std::shared_ptr<ShadowBox> _pParentBox = nullptr;
    bool _bMustUpdate;
    bool _bShadowMapEnabled = false;

public:
    ShadowBoxSide(
        std::shared_ptr<ShadowBox> pParentBox
        , std::shared_ptr<LightNodePoint> rLightSource
        , BoxSide::e eSide
        , bool bShadowMapEnabled
        );
    OVERRIDES ~ShadowBoxSide() OVERRIDE;

    bool getMustUpdate() { return _bMustUpdate; }
    std::shared_ptr<WindowViewport> getViewport() { return _pViewport; }
    std::shared_ptr<FrustumBase> getFrustum() { return _pFrustum; }
    std::shared_ptr<LightNodePoint> getLight() { return _pLightSource; }
    mat4* getProjectionMatrix() { return &_projMatrix; }
    mat4* getViewMatrix() { return &_viewMatrix; }

    void updateView();
    void collect();
    void renderShadows(std::shared_ptr<ShadowBox> pMasterBox, bool);
    bool computeIsVisible(std::shared_ptr<FrustumBase> pCamFrustum);
};
}//ns Game



#endif
