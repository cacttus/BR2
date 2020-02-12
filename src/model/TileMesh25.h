/**
*
*    @file TileBuffer.h
*    @date March 24, 2017
*    @author MetalMario971
*
*    © 2017
*
*
*/
#pragma once
#ifndef __TILEBUFFER_14904095883666013969_H__
#define __TILEBUFFER_14904095883666013969_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"

namespace Game {
/**
*  @class TileBuffer
*  @brief This is an old system, and likely, isn't needed anymore.
*/
class TileMesh25 : public VirtualMemory {
public:
  typedef v_v3c3x2n3 TileMeshVert;
protected:
  std::vector<TileMeshVert> _verts;
  std::vector<v_index32> _indexes;

  int _iCurrentQuadIndex = 0;
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoData = nullptr;
  void freeData();
  void allocateData();    //Allocates count quads
  int32_t _nQuads = 0;
public:
  TileMesh25(std::shared_ptr<GLContext> pContext, int32_t nQuads);
  virtual ~TileMesh25() override;

  void resetCopy() {
    _iCurrentQuadIndex = 0;
  }

  void nextQuad() {
    _iCurrentQuadIndex++;
    AssertOrThrow2(_iCurrentQuadIndex < _nQuads);
  }
  bool isFull() {
    return _iCurrentQuadIndex >= _nQuads;
  }
  int32_t getQuadCount() { return _iCurrentQuadIndex; }
  int32_t getMaxQuads() { return _nQuads; }
  void getQuad(TileMeshVert*& p0, TileMeshVert*& p1, TileMeshVert*& p2, TileMeshVert*& p3);    // Gets the next client side GPU quad.  After setting the quad call nextQuad().  then copyToGpu() then draw() then resetCopy()
  void copyToGpu(bool bInit = false);// don't unnecessarily copy the indexes.  leave count -1 to copy all quads.
  void draw(RenderParams& rp);

};
}//ns Game



#endif
 //   void backupQuad() {
 //        AssertOrThrow2(_iCurrentQuadIndex>0);
 //       _iCurrentQuadIndex--;
 //   }

   // std::vector<TileBufferVert>& getVerts() { return _verts; }

    //Indexes into the vertex buffer.
  //  static int offBL(int x, int y);
  //  static int offBR(int x, int y);
  //  static int offTL(int x, int y);
  //  static int offTR(int x, int y);

  //  static size_t vtxOff(int x, int y, int ind);
  //  int32_t getQuadCount() { return _iCurrentQuadIndex; }