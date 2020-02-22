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

namespace BR2 {
class RenderParamsInternal;
/**
*  @class RenderParams
*  @brief
*/
class RenderParams : public VirtualMemory {
public:
  RenderParams() {}
  RenderParams(std::shared_ptr<ShaderBase> pShader);
  virtual ~RenderParams() override;

  int32_t getCount();
  void setCount(int32_t i);
  std::shared_ptr<ShaderBase> getShader();
  void setShader(std::shared_ptr<ShaderBase> sb);
  void setCamera(std::shared_ptr<CameraNode> c);
  std::shared_ptr<CameraNode> getCamera();
  void setMesh(std::shared_ptr<MeshComponent> x);
  void setVaoGeneric(std::shared_ptr<VaoDataGeneric> x);
  void setVaoShader(std::shared_ptr<VaoShader> x);

  void draw();

  //**PHASE1
//void setDrawMode(GLenum e) { _eDrawMode = e; }
private:
  std::shared_ptr<RenderParamsInternal> _internal=nullptr;

};

}//ns BR2



#endif
