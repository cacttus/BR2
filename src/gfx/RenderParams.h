/**
*
*    @file RenderParams.h
*    @date May 18, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __RENDERPARAMS_14951219313732568565_H__
#define __RENDERPARAMS_14951219313732568565_H__

#include "../gfx/DisplayHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*    @class RenderParams
*    @brief
*
*/
class RenderParams : public VirtualMemory {
    std::shared_ptr<ShaderBase> _pShader = nullptr;
  //  std::vector<std::shared_ptr<Texture2DSpec>> _setTextures; //Essentially we shouldjsut be using the material on MeshSpec
    int32_t _iCount = -1; //Number of indexes
    GLenum _eDrawMode = GL_TRIANGLES;

    //Only one of these must be set.
    std::shared_ptr<MeshNode> _pMeshNode = nullptr;
    std::shared_ptr<VaoDataGeneric> _pVaoDataGeneric = nullptr;
    std::shared_ptr<VaoShader> _pVaoShader = nullptr;
   // std::shared_ptr<Material> _pMaterial = nullptr;

public:
    RenderParams(){}
    RenderParams(std::shared_ptr<ShaderBase> pShader);

    virtual ~RenderParams() override;
   // void clearTextures() { _setTextures.clear(); }
    int32_t getCount() { return _iCount; }
    void setCount(int32_t i) { _iCount = i; }
    GLenum getDrawMode() { return _eDrawMode; }
    void setDrawMode(GLenum e) {  _eDrawMode = e; }
    std::shared_ptr<ShaderBase> getShader() { return _pShader; }
    void setShader(std::shared_ptr<ShaderBase> sb) { _pShader = sb;}
//    void addTexture(std::shared_ptr<Texture2DSpec> ps) { _setTextures.push_back(ps);}

    //void setMaterial(std::shared_ptr<Material> ms) { _pMaterial = ms; }
    void setMesh(std::shared_ptr<MeshNode> x) { _pMeshNode = x; }
    void setVaoGeneric(std::shared_ptr<VaoDataGeneric> x) { _pVaoDataGeneric  = x; }
    void setVaoShader(std::shared_ptr<VaoShader> x) { _pVaoShader  = x; }

    void draw();

};
//class ShadowRenderParams : public VirtualMemory {
//public:
//
//    std::shared_ptr<ShaderBase> _pShader = nullptr;
//    //I feel like we're going to have tons of params soon.
//};


}//ns Game



#endif
