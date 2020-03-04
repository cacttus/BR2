/**
*  @file W25GridMesh.h
*  @date May 3, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __W25GRIDMESH_14937925164018290644_H__
#define __W25GRIDMESH_14937925164018290644_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class W25GridMesh
*  @brief
*/
class W25GridMesh_Internal;
class W25GridMesh : public VirtualMemoryShared<W25GridMesh> {
public:
  W25GridMesh(std::shared_ptr<WorldGrid> pGrid, GridMeshLayer::e eMatter);
  virtual ~W25GridMesh() override;

  void draw(RenderParams& rp, int& iDbgNumTrisDrawn);

  std::shared_ptr<MeshNode> getMesh();
  void setVertexPreallocCount(size_t s);
  std::vector<W25MeshVert>& getVerts();
  std::vector<v_index32 >& getIndexes();

  void sendMeshToGpu();
  void updateTopology();
  void makeMeshImmediately(bool bAsync);
  void updateRedoMeshForCell(WorldCell* pc, bool bNeighbors);
  void updateRedoMesh();

private:
  std::unique_ptr<W25GridMesh_Internal> _pint = nullptr;
};



}//ns Game



#endif
