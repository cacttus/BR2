/**
*  @file VaoDataGeneric.h
*  @date March 24, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __VAODATAGENERIC_14904114892307368716_H__
#define __VAODATAGENERIC_14904114892307368716_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../model/VertexFormat.h"

namespace BR2 {
//This is a VAO for each shader.  It matches the Vertex Format to the
//given shader's inputs.
class VaoShader : public VirtualMemory {
public:
  VaoShader(std::shared_ptr<GLContext> ctx, std::shared_ptr<ShaderBase> pShader, std::shared_ptr<VertexFormat> fmt);
  virtual ~VaoShader() override;
  void constructVao(std::shared_ptr<VboData> pVboData, std::shared_ptr<IboData> pIboData);
  void bind();
  void unbind();
  std::shared_ptr<IboData> getIbo() { return _pIboData; }
  std::shared_ptr<VboData> getVbo() { return _pVboData; }
private:
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<ShaderBase> _pShader = nullptr;
  std::shared_ptr<VertexFormat> _pVertexFormat = nullptr;
  std::shared_ptr<IboData> _pIboData = nullptr;
  std::shared_ptr<VboData> _pVboData = nullptr;
  GLuint _iGlVaoId = 0;

  void deleteVao();
  void enableAttributesForShader(std::shared_ptr<ShaderBase> pShader, std::shared_ptr<VboData> pVboData);
};

/**
*    @class VaoDataGeneric
*    @brief Stores Mesh Buffers and maps them to Vertex Arrays for each Shader that renders
*    the mesh.
*/
class VaoDataGeneric : public VirtualMemory {
public:
  VaoDataGeneric(std::shared_ptr<GLContext> ctx, std::shared_ptr<VertexFormat> vf);
  virtual ~VaoDataGeneric() override;

  void copyFrom(std::shared_ptr<VaoDataGeneric> v);
  std::shared_ptr<VboData> getVbo() { return _pVboData; }
  std::shared_ptr<IboData> getIbo() { return _pIboData; }
  const std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexInfo; }
  bool hasDrawableData();
  void allocate(size_t vcount, size_t icount); //Update the mesh data
  void fillData(const void* verts, size_t vcount, const  void* inds, size_t icount); //Update the mesh data
  std::shared_ptr<VaoShader> getOrCreateVaoForShader(std::shared_ptr<ShaderBase> pShader);

private:
  std::map<std::shared_ptr<ShaderBase>, std::shared_ptr<VaoShader>> _mapVaoShaders;
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<VertexFormat> _pVertexInfo = nullptr;
  std::shared_ptr<VboData> _pVboData = nullptr;
  std::shared_ptr<IboData> _pIboData = nullptr;
};


}//ns Game



#endif
