/**
*  @file QuadBufferMesh.h
*  @date March 9, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __QUADBUFFERMESH_308121747560101184222802_H__
#define __QUADBUFFERMESH_308121747560101184222802_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../math/MathAll.h"
namespace BR2 {
/**
*   @class QuadBufferMesh
*   @brief Manages an array of quads for modification and reflection back to the GPU.
*   @note The vertex format for this shader is FIXED to v2c4x2 format  see @cGpuQuad2.
*/
class QuadBufferMesh : public GLFramework {
public:
  typedef v_v3n3x2 QuadBufferVert;
public:
  QuadBufferMesh(std::shared_ptr<GLContext> pContext, int32_t count);
  virtual ~QuadBufferMesh() override;

  void setTexture(std::shared_ptr<Texture2DSpec> tex);
  int32_t getMaxQuads() { return _iMaxQuads; }
  int32_t getQuadCount() { return _iCurrentQuadIndex; }
  void resetCopy() { _iCurrentQuadIndex = 0; }
  void backupQuad();
  void nextQuad();
  bool isFull();
  void getGpuQuad(GpuQuad3& q);    // Gets the next client side GPU quad.  After setting the quad call nextQuad().  then copyToGpu() then draw() then resetCopy()
  void allocateQuads(int32_t count);    //Allocates count quads
  void copyToGpu(int32_t count = -1, bool bCopyIndexes = false);// don't unnecessarily copy the indexes.  leave count -1 to copy all quads.
  void draw(RenderParams& rp);
  void getQuad(v_v3n3x2*& v0, v_v3n3x2*& v1, v_v3n3x2*& v2, v_v3n3x2*& v3);

protected:
  std::vector<QuadBufferVert> _verts;
  std::vector<v_index32> _indexes;
  int _iCurrentQuadIndex = 0;
  std::shared_ptr<MeshNode> _pMesh = nullptr;
  int32_t _iMaxQuads = 0;
  std::shared_ptr<Texture2DSpec> _pTexture = nullptr;

  void assignIndexes();
  void freeData();
};

}//ns BR2



#endif
