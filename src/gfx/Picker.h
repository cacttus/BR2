/**
*  @file Picker.h
*  @date December 27, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __PICKER_15143971382984040801_H__
#define __PICKER_15143971382984040801_H__

#include "../gfx/GfxHeader.h"

namespace Game {
/**
*    @class Picker
*    @brief
*
*/
class Picker : public VirtualMemory {
public:
    //**Note do not set this to be anything but full alpha, if blending is enabled teh blend will screw up this value.
    static const uint32_t c_iInvalidPickId = 0;//0xFFFFFFFF;
private:
    std::shared_ptr<GLContext> _pContext = nullptr;
    //static const VertexFormatType _cVertexFormat = VertexFormatType::V_V2X2;
    uint32_t _iid = 0;
    //SelectionManager* _pSelectionManager;
    //BvhManager* _pBvhManager;
    //GLRenderSystem* _pRenderSystem; z
    //GLShaderManager* _pShaderManager;
    //TRef(GlslShaderBase) _pShader;
    //GLFramebufferManager* _pFramebufferManager;

    //GLuint _glFramebufferId;
    //GLuint _glRenderTextureId;
    //GLuint _glDepthRenderBufferId;

    //int32_t _iCachedHeight;

    //ProjectedRay _projectedRay;
    uint32_t _uiLastSelectedPixelId;//Note: This is relative to the last UserSelectionSet - the Id here is not fixed.
    void updatePickedPixel(int32_t x, int32_t y);
    void samplePixelId(int32_t x, int32_t y, uint32_t& __out_ selectedId);

public:
    Picker(std::shared_ptr<GLContext> pc);
    virtual ~Picker() override;
    uint32_t getSelectedPixelId() { return _uiLastSelectedPixelId; }
    void update(std::shared_ptr<InputManager> pInput);
    uint32_t genPickId();
};

}//ns Game



#endif
