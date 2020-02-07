///**
//*
//*    @file ModFile.h
//*    @date May 29, 2014
//*    @author MetalMario971
//*
//*    © 2014 
//*
//*
//*/
//#pragma once
//#ifndef __MODFILE_29721394829489844822522_H__
//#define __MODFILE_29721394829489844822522_H__
//
//#include "../base/PoundFile.h"
//
//namespace Game {
//class ModelLoadInfo;
//class ModelLoadInfoPart;
///**
//*    @class ModFile
//*    @brief Model file.  Stores file locations of model asset data.
//*
//*    New command: mpt - defines model part.  All model parts start with mpt
//
//    Note:Given the defined configuration we're limited to just one Geom per .obj file.
//
//#MOD FILE
//mpt T02SDT1U
//tmd T02SDT1U_D.TGA
//tmn T02SDT1U_N.TGA
//skn T02SDT1U.skn
//obj T02SDT1U.obj
//mtl T02SDT1U.mtl
//shr default
//
//
//*/
//class ModFile : public PoundFile {
//protected:
//    void addPart();
//    ModelLoadInfoPart* _pCurPart;    // current part being parsed.  o
//    ModelLoadInfo* _pModelLoadInfo;
//    //ModelLoadInfo* parse(char* buf);
//    //virtual void pkp(t_string tok0, t_string tok1, t_int& texNum);
//    virtual bool tkModelPart(t_string tok0, t_string tok1);
//
//    //Inherited
//    virtual void pkp(std::vector<t_string>& tokens);
//    virtual void preLoad();
//    virtual void postLoad();
//
//public:
//    ModelLoadInfo* getModelLoadInfo();
//
//    //+-- CTOR/DTOR --+
//    ModFile();
//    virtual ~ModFile() override ;
//};
//
//}//ns game
//
//
//
//#endif
