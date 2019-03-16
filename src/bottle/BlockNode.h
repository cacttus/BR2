/**
*
*    @file BlockNode.h
*    @date May 2, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __BLOCKNODE_14937714702398736072_H__
#define __BLOCKNODE_14937714702398736072_H__


#include "../bottle/BottleHeader.h"
namespace Game {
/**
*    @class BlockNode
*    @brief
*   Global Node structure for grids.
*
*/
class BlockNode : public VirtualMemory {
public:
    //Keep this structure lightweight
    static const int c_nCells = 8;
private:    
    std::shared_ptr<WorldGrid> _pGrid = nullptr;
    int32_t _iIndexCount[GridMeshLayer::e::MaxMatters];//This is an optimization to tell us how many indexes all the children have
    Box3f _box; //24
    std::vector<BlockNode*>* _vecChildren = nullptr; // 8 * 4 = 32
    WorldCell** _pCells = nullptr;
    bool _bIsLeaf = false;

    void createChildren();
    WorldCell* makeCell(int xx, int yy, int zz, WorldCellFile* pFile, int32_t& iCell, World25GridGen::e eg,
        WipGrid* nf, std::shared_ptr<WorldGrid> pGrid, size_t(&nVerts)[GridMeshLayer::e::MaxMatters]);
    vec3 getOriginR3() { return _box._min; }
    void setIsLeaf() { _bIsLeaf = true; }
    void deleteCells();
    uint32_t bitOff(uint32_t x, uint32_t y, uint32_t z);
public:
    BlockNode(std::shared_ptr<WorldGrid> pg);
    virtual ~BlockNode() override;

    std::shared_ptr<WorldGrid> getGrid() { return _pGrid; }

    bool getIsLeaf() { return _bIsLeaf; }
    Box3f* getBox() { return &_box; }
    WorldCell** getCells() { return _pCells; }

    std::vector<BlockNode*>* getChildren() { return _vecChildren; }
    WorldCell* getCell(size_t i) { return _pCells[i]; }

    int32_t getIndexCount(GridMeshLayer::e eMatterMode) { return _iIndexCount[eMatterMode]; }
    void setIndexCount(GridMeshLayer::e eMatterMode, int32_t ind) { _iIndexCount[eMatterMode] = ind;}

    void divide(t_timeval startTime, std::shared_ptr<WorldGrid> _pMyGlob, int32_t& nNodes, int32_t& nLeaves, int32_t treeLevel,
        Vec3f* ptPointToDivideTo, BlockNode** ptFoundNode, bool bCreate);
    void createCells(WorldCellFile* pFile, int32_t& iCell, World25GridGen::e eg,
        WipGrid* nf, std::shared_ptr<WorldGrid> pGrid, size_t(&nVerts)[GridMeshLayer::e::MaxMatters], 
        int32_t& nCells);
    BlockNode* getChild(BlockPos8::e pos) { 
        AssertOrThrow2(_vecChildren != nullptr);
        AssertOrThrow2((size_t)pos < _vecChildren->size());
        return _vecChildren->at((size_t)pos);
    }
    void getCellForPoint(vec3& pt, WorldCell*& __out_ ret);
    void consolidate(bool& bEmpty, int32_t& nCells, int32_t& nDeleted, int32_t& nNodes);
    void raycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& outCells);


};

}//ns Game



#endif
