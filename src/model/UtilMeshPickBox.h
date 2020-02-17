///**
//*
//*  @file UtilMeshPickBox.h
//*  @date March 22, 2016
//*  @author MetalMario971
//*/
//#pragma once
//#ifndef __UTILMESHPICKBOX_315421104614287930312130_H__
//#define __UTILMESHPICKBOX_315421104614287930312130_H__
//
//#include "../gfx/UtilMesh.h"
//
//namespace Game {
///**
//*  @class UtilMeshPickBox
//*  @brief
//*
//*/
//class UtilMeshPickBox : public UtilMesh {
//private:
//    float _fSize;
//    vec3 _vOffset;
//    Color4f _vColor;
//    bool _blnWireFrame;
//    TRef(MeshSpec) _pSpec; 
//    Cube3f* _pCube;
//    uint32_t _uiColorId;
//public:
//    UtilMeshPickBox( Cube3f* pCube, uint32_t uiColorId);
//    virtual ~UtilMeshPickBox() override ;
//    
//    void setWireFrame(bool blnWireFrame) { _blnWireFrame = blnWireFrame; }
//    OVERRIDES TRef(GlslShaderBase) getShader();
//    OVERRIDES void generate();
//    OVERRIDES void preDraw();
//    OVERRIDES void postDraw();
//};
//}//ns BR2
//
//
//
//#endif
