/**
*
*    @file W25GridMesh.h
*    @date May 3, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __W25GRIDMESH_14937925164018290644_H__
#define __W25GRIDMESH_14937925164018290644_H__

#include "../bottle/BottleHeader.h"

namespace Game {
/**
*    @class W25GridMesh
*    @brief
*
*/
class W25GridMesh : public VirtualMemoryShared<W25GridMesh> {
private:
    std::shared_ptr<WorldGrid> _pGrid = nullptr;
    std::vector<W25MeshVert> _verts;
    std::vector<v_index32> _indexes;
    size_t _iMaxVertexCount = 0;
    std::shared_ptr<MeshNode> _pMesh = nullptr;
    GridMeshLayer::e _eMatterMode;
    std::mutex _mtTopoMtx;
    std::set<WorldCell*> _setTopoMod; //topo was modified, nullptr in here means we redo whole mesh

    void makeMeshImmediately_r(BlockNode* pParent);
   //void propogateMeshData();
   //void propogateMeshData_r(BlockNode* pParent, int32_t& dbg_tot);
    bool getMustRedoWholeMesh();
    void makeMeshForCell(WorldCell* pc);
public:
    W25GridMesh(std::shared_ptr<WorldGrid> pGrid, GridMeshLayer::e eMatter);
    virtual ~W25GridMesh() override;
    std::shared_ptr<MeshNode> getMesh() { return _pMesh; }
    void setVertexPreallocCount(size_t s) { _iMaxVertexCount = s; }

    std::vector<W25MeshVert>& getVerts() { return _verts; }
    std::vector<v_index32 >& getIndexes() { return _indexes; }
    void sendMeshToGpu();
    void draw(RenderParams& rp, int& iDbgNumTrisDrawn);

    void updateTopology();
    void makeMeshImmediately(bool bAsync);
    void updateRedoMeshForCell(WorldCell* pc, bool bNeighbors);
    void updateRedoMesh();
};



}//ns Game



#endif
