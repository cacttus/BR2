///**
//*
//*    @file UtilMeshPickBox.h
//*    @date March 22, 2016
//*    @author Derek Page
//*
//*    © 2016 
//*
//*
//*/
//#pragma once
//#ifndef __UTILMESHPICKBOX_315421104614287930312130_H__
//#define __UTILMESHPICKBOX_315421104614287930312130_H__
//
//#include "../display/UtilMesh.h"
//
//namespace Game {
///**
//*    @class UtilMeshPickBox
//*    @brief
//*
//*/
//class UtilMeshPickBox : public UtilMesh {
//private:
//    float _fSize;
//    vec3 _vOffset;
//    Color4f _vColor;
//    t_bool _blnWireFrame;
//    TRef(MeshSpec) _pSpec; 
//    Cube3f* _pCube;
//    t_uint32 _uiColorId;
//public:
//    UtilMeshPickBox( Cube3f* pCube, t_uint32 uiColorId);
//    virtual ~UtilMeshPickBox() override ;
//    
//    void setWireFrame(t_bool blnWireFrame) { _blnWireFrame = blnWireFrame; }
//    OVERRIDES TRef(GlslShaderBase) getShader();
//    OVERRIDES void generate();
//    OVERRIDES void preDraw();
//    OVERRIDES void postDraw();
//};
//}//ns game
//
//
//
//#endif
