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
#include "../base/SDLGLIncludes.h"

namespace BR2 {
class RenderParams_Internal;
/**
*  @class RenderParams
*  @brief Pass this around to control rendering.
*/
class RenderParams : public VirtualMemory {
public:
  RenderParams();
  RenderParams(std::shared_ptr<ShaderBase> pShader);
  virtual ~RenderParams() override;

  // void clearTextures() { _setTextures.clear(); }
  int32_t getCount();
  void setCount(int32_t i);
  void setDrawMode(GLenum e);
  std::shared_ptr<ShaderBase> getShader();
  void setShader(std::shared_ptr<ShaderBase> sb);
  void setMesh(std::shared_ptr<MeshNode> x);
  void setVaoGeneric(std::shared_ptr<VaoDataGeneric> x);
  void setVaoShader(std::shared_ptr<VaoShader> x);
  void draw();
  std::shared_ptr<CameraNode> getCamera();
  void setCamera(std::shared_ptr<CameraNode> c);

private:
  std::unique_ptr<RenderParams_Internal> _pint = nullptr;
};


}//ns Game



#endif
