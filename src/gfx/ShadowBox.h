/**
*  @file ShadowMapFrustum.h
*  @date April 25, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADOWMAPFRUSTUM_14616351763757858195_H__
#define __SHADOWMAPFRUSTUM_14616351763757858195_H__


#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*    @class PointLightShadowBox
*    @brief Shadow map box for a point light.
*
*/
class ShadowBox : public VirtualMemoryShared<ShadowBox> {
private:
    int64_t _iShadowBoxId;
    std::shared_ptr<ShadowBoxSide> _pShadowBoxSide[6] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    std::shared_ptr<LightNodePoint> _pLightSource = nullptr;
    vec3 _vCachedLastPos;
    float _fCachedLastRadius;

    float _fSmallBoxSize = 0.2f;
    GLuint _glFrameBufferId = 0;
    GLuint _glDepthTextureId;
    GLuint _glShadowCubeMapId = 0;
    uint32_t _iFboWidthPixels;
    uint32_t _iFboHeightPixels;
    Box2f _screenQuadPos;
    Box2f _screenQuadTCoords;
    std::shared_ptr<MeshNode> _pScreenQuadMesh = nullptr;
    bool _bMustUpdate = false;
    bool _bForceUpdate = false;
    bool _bShadowMapEnabled = false;
    void deleteFbo();
    void createFbo();
    void updateLightMatrix();
    void smoothShadows();
public:
    ShadowBox(std::shared_ptr<LightNodePoint> refLightSource, int32_t iWidth, int32_t iHeight, bool bShadowMapEnabled);
    OVERRIDES ~ShadowBox() OVERRIDE;
    void init();
    void update();
    void renderShadows(std::shared_ptr<ShadowBox> pMaster);
    void debugRender();
    std::shared_ptr<LightNodePoint> getLightSource() { return _pLightSource; }

    float& getSmallBoxSize() { return _fSmallBoxSize; }
    void copyAndBlendToShadowMap(std::shared_ptr<ShadowBox> pBox);
    void beginRenderShadowBox();
    void beginRenderSide(BoxSide::e side);
    void endRenderSide();
    void endRenderShadowBox();
    uint32_t getFboWidth() { return _iFboWidthPixels; }
    uint32_t getFboHeight() { return _iFboHeightPixels; }
    GLuint getGlTexId() { return _glShadowCubeMapId; }
    //  void getTexturePixels(t_string& __out_ pixels, CubeSide eCubeSide);
    void updateScreenQuad();
};

}//ns Game



#endif
