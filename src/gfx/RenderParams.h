/**
*  @file RenderParams.h
*  @date May 18, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERPARAMS_14951219313732568565_H__
#define __RENDERPARAMS_14951219313732568565_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*  @class RenderParams
*  @brief Pass this around to control rendering.
*/
class RenderParams : public VirtualMemory {
public:
  RenderParams() {}
  RenderParams(std::shared_ptr<ShaderBase> pShader);

  virtual ~RenderParams() override;
  // void clearTextures() { _setTextures.clear(); }
  int32_t getCount() { return _iCount; }
  void setCount(int32_t i) { _iCount = i; }
  GLenum getDrawMode() { return _eDrawMode; }
  void setDrawMode(GLenum e) { _eDrawMode = e; }
  std::shared_ptr<ShaderBase> getShader() { return _pShader; }
  void setShader(std::shared_ptr<ShaderBase> sb) { _pShader = sb; }
  void setMesh(std::shared_ptr<MeshNode> x) { _pMeshNode = x; }
  void setVaoGeneric(std::shared_ptr<VaoDataGeneric> x) { _pVaoDataGeneric = x; }
  void setVaoShader(std::shared_ptr<VaoShader> x) { _pVaoShader = x; }
  void draw();

private:
  std::shared_ptr<ShaderBase> _pShader = nullptr;
  int32_t _iCount = -1; //Number of indexes
  GLenum _eDrawMode = GL_TRIANGLES;
  //Only one of these must be set.
  std::shared_ptr<MeshNode> _pMeshNode = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoDataGeneric = nullptr;
  std::shared_ptr<VaoShader> _pVaoShader = nullptr;
};


}//ns Game



#endif
