/**
*  @file UtilMesh.h
*  @date November 21, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __UTILMESH_12100900221286185268356_H__
#define __UTILMESH_12100900221286185268356_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class UtilMesh
*  @brief Quick utility mesh for debug drawing.
*/
class UtilMesh : public GLFramework {
public:
  UtilMesh(std::shared_ptr<GLContext> ctx, std::shared_ptr<VertexFormat>, std::shared_ptr<ShaderBase> pShader, GLenum drawMode = GL_LINES);
  virtual ~UtilMesh() override;

  void init();
  void draw(std::shared_ptr<CameraNode> cam);

  void setModelMatrix(mat4& m) { _m4ModelMatrix = m; }

  virtual void generate() = 0;
  virtual void preDraw() = 0;
  virtual void postDraw() = 0;
  virtual std::shared_ptr<ShaderBase> getShader();

protected:
  void copyBuffersToVao();
  void copyFromSpec(std::shared_ptr<MeshSpec> sp);
  std::shared_ptr<IndexBufferData> getIndexes() { return _pIndexes; }
  std::shared_ptr<FragmentBufferData> getVerts() { return _pVerts; }
  void setDrawMode(GLenum e) { _eDrawMode = e; }
  void allocData(int nVerts, int nIndexes, std::shared_ptr<VertexFormat> fmt);
  void cleanup();

private:
  std::shared_ptr<VertexFormat> _pVertexFormat = nullptr;
  GLuint bdVerts, bdIndexes, vaoIndexes;
  bool _bInitialized = false;

  GLenum _eDrawMode = GL_LINES;

  mat4 _m4ModelMatrix;
  std::shared_ptr<ShaderBase> _pShader = nullptr;
  std::shared_ptr<FragmentBufferData> _pVerts = nullptr; //Note if using a spec make sure to set vertexes and indexes to NULL in post draw so you don't delete the spec data.
  std::shared_ptr<IndexBufferData> _pIndexes = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoData = nullptr;
  std::shared_ptr<VaoDataGeneric> getVaoData() { return _pVaoData; }
};

}//ns game



#endif
