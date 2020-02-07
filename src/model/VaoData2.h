///**
//*
//*    @file VaoData2.h
//*    @date November 25, 2016
//*    @author MetalMario971
//*
//*    © 2016 
//*
//*
//*/
//#pragma once
//#ifndef __VAODATA2_1480133025531171099_H__
//#define __VAODATA2_1480133025531171099_H__
//#include "../model/ModelHeader.h"
//
//namespace Game {
///**
//*    @class VaoData2
//*    @brief
//*
//*/
//class VaoData2 : public VirtualMemory {
//    GLuint _iGlVaoId = 0;
//
//    std::shared_ptr<VboData> _pVboData = nullptr;
//    std::shared_ptr<IboData> _pIboData = nullptr;
//    std::shared_ptr<GLContext> _pContext = nullptr;
//public:
//    VaoData2(std::shared_ptr<GLContext> ctx);
//    virtual ~VaoData2() override;
//
//    bool hasDrawableData();
//
//    void fillDataInit(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds);//Initialize the VAO data
//    void refillData(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds); //Update the mesh data
//    void refillData(std::vector<v_v3n3x2>* verts, int32_t vcount, std::vector<v_index>* inds, int32_t icount); //Update the mesh data
//    void bind();
//    void draw(int32_t iCount = -1);
//};
//
//}//ns Game
//
//
//
//#endif
//
