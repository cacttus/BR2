/**
*  @file ShadowFrustum.h
*  @date January 25, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADOWFRUSTUM_15169135621311229167_H__
#define __SHADOWFRUSTUM_15169135621311229167_H__

#include "../math/MathHeader.h"
#include "../gfx/GfxHeader.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*    @class ShadowFrustum
*    @brief
*
*/
class ShadowFrustum : public VirtualMemoryShared<ShadowFrustum> {
    std::shared_ptr<LightNodeDir> _pLightSource = nullptr;
    vec3 _vCachedLastPos;
    float _fCachedLastRadius;

    float _fSmallBoxSize = 0.2f;
    GLuint _glFrameBufferId = 0;
    GLuint _glDepthTextureId;
    GLuint _glShadowMapId = 0;
    uint32_t _iFboWidthPixels;
    uint32_t _iFboHeightPixels;
    std::shared_ptr<MeshNode> _pScreenQuadMesh = nullptr;

    int32_t _iShadowFrustumId;
    std::shared_ptr<FrustumBase> _pFrustum = nullptr;
    std::shared_ptr<RenderViewport> _pViewport = nullptr;
    mat4 _projMatrix;    // Frustum, Proj, View - basic camera parameters
    mat4 _viewMatrix;    //
    mat4 _PVB;
    std::shared_ptr<RenderBucket> _pVisibleSet;
    std::shared_ptr<RenderBucket> _pLastSet;//TODO: check vs current set _bShadowDirty in BvhNOde
    bool _bMustUpdate;
    bool _bShadowMapEnabled = false;
    void deleteFbo();
    void createFbo();
    void collect();
    void updateView();
    void copyAndBlendToShadowMap(std::shared_ptr<ShadowFrustum> pBox);
public:
    ShadowFrustum(std::shared_ptr<LightNodeDir> pLightSource, int32_t iFboWidth, int32_t iFboHeight, bool bShadows);
    virtual ~ShadowFrustum() override;
    std::shared_ptr<FrustumBase> getFrustum() { return _pFrustum; }
    GLint getGlTexId() {return _glShadowMapId;}
    void init();
    void update();
    void endRenderShadowFrustum();
    void beginRenderShadowFrustum();
    void debugRender();
    void renderShadows(std::shared_ptr<ShadowFrustum> pShadowFrustumMaster);
    mat4* getViewMatrixPtr() { return &_viewMatrix; }
    mat4* getProjMatrixPtr() { return &_projMatrix; }
    mat4* getPVBPtr() { return &_PVB;}
    void setChanged() {
        _vCachedLastPos.construct(FLT_MAX,FLT_MAX,FLT_MAX);
    }
};

}//ns Game



#endif
