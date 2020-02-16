/**
*
*    @file World2dCell.h
*    @date March 2, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __GRID2DCELL_1488475204794252213_H__
#define __GRID2DCELL_1488475204794252213_H__

#include "../bottle/BottleHeader.h"

namespace Game {

/**
*    @class World2dCell
*    @brief
*
*/
class WorldCell {
public:
    typedef int16_t CellSize;
    static const int c_nNeighbors = 6;
private:
    //DO NOT ADD MORE SHIT TO THIS CLASS
    //this = 4
    //12
    ivec3 _viPosLocal;
    //4
    //World25Grid* _pWorld25Grid = nullptr; // we can remove this if we just use parent.  If generating an object this may be nullptr.
    //6 * 4 = 24
    WorldCell* _pNeighbor[c_nNeighbors];//Indexed by World25Side
  //  W25ObjCellManfiold* _pManifold = nullptr;
    //4
    BlockNode* _pParentNode = nullptr;//If we get rid of final nodes we can get rid of this it's only for the bound box.
   // vec3 _vColor[4];
    //4
    W25Geom _iGeom[GridMeshLayer::e::MaxMatters];
    W25Tile _iTile[GridMeshLayer::e::MaxMatters];

    //Tile25Spec* _pTile[MatterMode::e::MaxMatters]; //this can be srhunk to 2 bytes
    
    VisibleCell* _pVisibleCell = nullptr; //Null if the cell is not visible.

    static WorldCellData* serializeData(
        W25Tile tsolid, W25Tile tliquid, W25Tile tgas,
        W25Geom gsolid, W25Geom gliquid, W25Geom ggas
    );
public:
   // t_timeval _iLightMarchStamp = 0;

public:

    WorldCell( int ix, int iy, int iz, BlockNode* pParentNode, W25Geom solid, W25Geom liquid);
    ~WorldCell();

    //Static
    static int getV8IndexForV4Side(PhysicsGridSide::e eSide, int i);
    static int getV8IndexForV4SideOpp(PhysicsGridSide::e eSide, int i);
    static void getRelativeNeighborPos(PhysicsGridSide::e eSide, ivec3& iv);
    static bool isValidCellPos(const ivec3& relPos);
    static bool isBorderCellPos(const ivec3& vi);

    VisibleCell* getVisibleCell() { return _pVisibleCell; }
    void tryCreateVisibleCell() {
        if (_pVisibleCell == nullptr) {
            _pVisibleCell = new VisibleCell();
        }
    }
    void deleteVisibleCell() {
        if(_pVisibleCell != nullptr){
            delete _pVisibleCell;
            _pVisibleCell = nullptr;
        }
    }
    bool isBorderCell();
    bool isBorderSide(PhysicsGridSide::e eSide);

    WorldCellData* serialize();
    static WorldCellData* serializeEmpty();
    void getBoundBoxBvh(Box3f&  __out_ box);
    Box3f getBoundBoxBvh();

    const ivec3& getLocalPos() { return _viPosLocal; }
    vec3 getOriginR3();
    vec3 getCenterR3();

    int32_t getLevel() { return _viPosLocal.y; }

    WorldCell* getNeighbor(PhysicsGridSide::e eNeighbor){ return _pNeighbor[eNeighbor]; }
    void setNeighbor(PhysicsGridSide::e eNeighbor, WorldCell* c) { _pNeighbor[eNeighbor] = c; }
    std::shared_ptr<WorldGrid> getGrid();

    //vec3& getColor(int idx) { 
    //    AssertOrThrow2(idx >= 0 && idx <= 3);
    //    return _vColor[idx]; 
    //}
    //  W25ObjCellManfiold* getCellManifold() { return _pManifold; }

    void setTile(GridMeshLayer::e eMatterMode, W25Tile ps){
     //   AssertOrThrow2(ps != nullptr);
        AssertOrThrow2(eMatterMode < GridMeshLayer::e::MaxMatters);
        _iTile[eMatterMode] = ps;
        //_pTile[eMatterMode] = ps;
    }
    W25Tile getTile(GridMeshLayer::e eMatterMode) {
        AssertOrThrow2(eMatterMode < GridMeshLayer::e::MaxMatters);
        return _iTile[eMatterMode];//_pTile[eMatterMode];
    }
    W25Geom getGeom(GridMeshLayer::e eMatterMode) {
        AssertOrThrow2(eMatterMode < GridMeshLayer::e::MaxMatters);
        return _iGeom[eMatterMode];
    }
    void setGeom(GridMeshLayer::e eMatterMode, W25Geom ge) {
        AssertOrThrow2(eMatterMode < GridMeshLayer::e::MaxMatters);
        _iGeom[eMatterMode] = ge;
    }



};

}//ns Game



#endif

