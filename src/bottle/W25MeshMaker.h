/**
*
*    @file W25MeshConf.h
*    @date April 3, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __W25MESHCONF_1491276249331978058_H__
#define __W25MESHCONF_1491276249331978058_H__

#include "../bottle/BottleHeader.h"

namespace Game {

/** Holds configs for making meshes */
class W25MeshMaker : public VirtualMemoryShared<W25MeshMaker> { 
    friend class W25MeshConf;
    std::unique_ptr<W25MeshConf> _pConfigs[256];
    vec3 _vBlock[8];

    std::shared_ptr<Atlas> _pWorldAtlas = nullptr;

    enum { Gpu_BL = 0, Gpu_BR = 1, Gpu_TL = 2, Gpu_TR = 3 };

    vec3& getUnitBlockVertex(int i);
    //void makeFace(vec3& bl, vec3& br, vec3& tl, vec3& tr,
    //    std::vector<v_v3c3x2n3>& verts, std::vector<v_index>& inds, vec3& vOffset, std::shared_ptr<AtlasSprite> ps);
    void makeIdxTri(vec3& ta, vec3& tb, vec3& tc,//ta/tb are coplanar
        std::vector<W25MeshVert>& verts, std::vector<v_index32>& inds);
    void constructUnitBlock();
    void constructTileConfigs();
    static W25Sc::e getOppSideConf(W25Sc::e c);
    void addBlockQuad(vec3& v0, vec3& v1, vec3& v2, vec3& v3,
        std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level,
        std::shared_ptr<Atlas> pa, int32_t iGpuIndex, vec3& base, float fBlockWidth, float fBlockHeight);
    static W25Plane::e getOppPlaneIndex(W25Plane::e iNeighbor);
    bool checkHiddenSide(WorldCell* gc, int iSide, int itri, GridMeshLayer::e eMatterMode);

    void addAsTriangles(WorldCell*, vec3& vBase, W25Geom c_geom, int eSide, GridMeshLayer::e eMatterMode, int32_t iGpuTileId,
        std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level,
        float fBlockWidth, float fBlockHeight);
    int32_t addAsTrianglesGetGpuTile(WorldCell* gc, int iSide, GridMeshLayer::e eMatterMode, std::shared_ptr<SpriteBucket> pBucket);

    bool addAsBlockIsEmptyNeighbor(PhysicsGridSide::e eSide, WorldCell* pc, GridMeshLayer::e eMatterMode);
    void addAsBlock(std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level,
        std::shared_ptr<SpriteBucket> pBucket,
        std::shared_ptr<Atlas> pa, WorldCell* gc, GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight, vec3* v3CustomBase);
    void addConfigTriangles(std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level, std::shared_ptr<SpriteBucket> pBucket,
        std::shared_ptr<Atlas> pa, WorldCell* gc, GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight, vec3* v3CustomBase);
public:
    W25MeshMaker(std::shared_ptr<Atlas> pAtlas);
    virtual ~W25MeshMaker();

    void init();
    std::unique_ptr<W25MeshConf>& getW25Config(W25Geom i) {
        AssertOrThrow2(i < 256);
        return _pConfigs[i];
    }
    static W25Geom confBit3(int iBlockVert, int s1, int s2, int s3, float(&kernel)[27]);
    static W25Geom confBit9(int iBlockVert,
        int s1, int s2, int s3,
        int s4, int s5, int s6,
        int s7, int s8, int s9,
        float(&kernel)[27]);
    static W25Geom confBit6m(int iBlockVert,
        int s1, int s2, int s3,
        int s4, int s5, int s6,
        float(&kernel)[27]);
    void fillMeshByConfig(std::vector<W25MeshVert>& vecVertexes, std::vector<v_index32>& vecIndexes,
        std::shared_ptr<SpriteBucket> pBucket,
        std::shared_ptr<Atlas> pAtlas, WorldCell* pCell, GridMeshLayer::e eMatterMode, float fBlockWidth, 
        float fBlockHeight, std::shared_ptr<W25Config> pConf, vec3* v3CustomBase);

    void redoMeshForCellNoGpuCopy(std::shared_ptr<W25GridMesh> pMesh, std::shared_ptr<Atlas> pAtlas, WorldCell* pCell, std::shared_ptr<SpriteBucket> pBucket,
        GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight,
        std::shared_ptr<W25Config> pConf, std::shared_ptr<WorldGrid> pGrid, bool bAsync);

    size_t getNumVertsForGeom(W25Geom g);

};
/**
*    @class
*    @brief This is the configuration for all of the cell meshes in the world
*/
class W25MeshConf : public VirtualMemory {
public:
    static const int c_nTris = 20;
    //L0, L1, R0, R1, B0, B1, T0, T1, A0, A1, F0, F1, X0, X1, Y0, Y1, Z0, Z1, W0, W1.
    static const int c_nSides = 10;//L, R, B, T, A, F, X Y Z W
private:
    static std::map<int32_t, vec3> _posMap;
    static void buildPosMap();
    void addConfigTriangle(int side, int i0, int i1, int i2, W25Sc::e eTcConf, bool bPlanar);
    bool getCornerVal(int iCorner);
    void addCapTris(int (&t0)[3], W25Sc::e t0tc, bool (&vt)[4], bool (&vb)[4], int (&ti)[4], int (&bi)[4]);
    void doSideTris(int t00, int t01, bool (&vt)[4], bool (&vb)[4], int (&ti)[4], int (&bi)[4], bool in_plane);
    void makeMesh();
    void makePlaces();
    W25Geom _iGeomId;
    std::vector<W25MeshVert> _verts[c_nSides];
    std::vector<v_index32> _indexes[c_nSides];
    std::vector<vec3> _planeNormals[c_nSides];
    std::vector<W25Sc::e> _eTcConf[c_nSides]; //Triangle tcoord conf
    std::vector<bool> _bPlanar[c_nSides]; //If the triangle is planar, hence it can be culled
    std::map<int32_t, vec3> _places;//vtx placement

    std::shared_ptr<W25MeshMaker> _pMeshMaker = nullptr;

    size_t _nTotalVerts = 0;
public:
    W25MeshConf(std::shared_ptr<W25MeshMaker> pm);
    virtual ~W25MeshConf();

    int numTrisSide(int side){
        AssertOrThrow2(side >= 0 && side<c_nSides);
        return _verts[side].size()/3;
    }
    W25Geom getGeomId() {
        return _iGeomId;
    }
   W25Sc::e getTcConf(int side, int tri) {
       AssertOrThrow2(side >= 0 && side<c_nSides);
       AssertOrThrow2(tri >= 0 && tri<2);
        return _eTcConf[side][tri];
    }
   bool getPConf(int side, int tri) {
       AssertOrThrow2(side >= 0 && side<c_nSides);
       AssertOrThrow2(tri >= 0 && tri<2);
       return _bPlanar[side][tri];
   }
   void construct(W25Geom iConfig);

    std::vector<W25MeshVert>& getVerts(int i) {
        AssertOrThrow2(i>=0 && i<c_nSides);
        return _verts[i];
    }
    std::vector<v_index32>& getIndexes(int i) {
        AssertOrThrow2(i >= 0 && i<c_nSides);
        return _indexes[i];
    }
    std::vector<vec3>& getPlaneNormals(int i) {
        AssertOrThrow2(i >= 0 && i<c_nSides);
        return _planeNormals[i];
    }
    void raycastTris(Ray_t* ra, vec3& vOrigin, float fCellWidth, float fCellHeight);
    size_t getNumTotalVerts() { return _nTotalVerts; }
};


}//ns Game



#endif
