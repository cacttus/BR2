/**
*
*    @file Utils.h
*    @date January 19, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __UTILS_14848788182400156280_H__
#define __UTILS_14848788182400156280_H__

#include "../bottle/BottleHeader.h"

namespace Game {

/**
*    @class Utils
*    @brief
*
*/
class BottleUtils : public VirtualMemory {
public:

    static t_string c_sMainDiffuseVs;
    static t_string c_sMainDiffusePs;
    static const int c_iMaxParticles = 2000;
    
    static float getGlobWidth() { return getNodeWidth() * 4; } // 4x4x4 nodes  node 8x8x8 cells so 4 * 8 = 32 cells width
    static float getGlobHeight() { return getNodeHeight() * (float)(getNodesY().getMax() - getNodesY().getMin() + 1); }

    static MpInt getNodesY() { return MpInt(-2, 2); }

    static uint32_t getMaxGridCount() { return 200; }

    static float getCellWidth() { return 2.0f; /*Does this need to be 2^x?  I think it does because we use Binary division*/ }
    static float getCellHeight() {  return getCellWidth() *1.0f; } // ** full cell widths seem to look spikey
    static float getNumCellsWidth() { return  8; /* must be 2^x*/ }//8 Significantly reduces memory footprint compared to 16, but it's slower to generate.  Possibly just because the topology is simpler.
    static float getCellWidth_1() { return 1.0f / getCellWidth(); }
    static float getCellHeight_1() { return 1.0f / getCellHeight();}
    static float getNodeWidth() {return getNumCellsWidth() * getCellWidth(); }
    static float getNodeWidth_1() { return 1.0f / getNodeWidth(); }
    static float getNodeHeight() { return getNumCellsWidth() * getCellHeight(); }
    static float getNodeHeight_1() {   return 1.0f / getNodeHeight(); }
    static int32_t getNumTilesPerNode() {  return (int32_t)(getNumCellsWidth() * getNumCellsWidth());}

    static MpFloat getAwarenessRadiusY() {
        return MpFloat(getNodeWidth() * 0.5f, getNodeWidth() * 1.0f);
    }
    static float getAwarenessIncrementY(){
        return getNodeWidth() * 0.5f;
    }
    static MpFloat getAwarenessRadiusXZ(){
        return MpFloat(getNodeWidth() * 1.5f, getNodeWidth() * 4.0f);
    }
    static float getAwarenessIncrementXZ(){
        return getNodeWidth() * 0.5f;
    }

    //static float getBvhAwarenessMinRadiusY() { return ; }  //**For the 8x8 nodes we doubled this.
    //static float getBvhAwarenessIncrementY() { return ; }  //**For the 8x8 nodes we doubled this.
    //static float getBvhAwarenessMaxRadiusY() { return getNodeWidth() * 1.0f; }  //**For the 8x8 nodes we doubled this.
    static float getBvhExpensiveUpdateRadiusY() { return getNodeWidth() * 1.0f; }  //**For the 8x8 nodes we doubled this.

    //static float getBvhAwarenessMinRadiusXZ() { return getNodeWidth() * 1.5f; }  //**For the 8x8 nodes we doubled this.
    //static float getBvhAwarenessMaxRadiusXZ() { return getNodeWidth() * 4.0f; }  //**For the 8x8 nodes we doubled this.
    //static float getBvhAwarenessIncrementXZ() { return ; }  //**For the 8x8 nodes we doubled this.
    static float getBvhExpensiveUpdateRadiusXZ() { return getNodeWidth() * 5.0f; }  //**For the 8x8 nodes we doubled this.

  //  static float getBvhAwarenessDeleteRadiusXZ() { return getBvhAwarenessMaxRadiusXZ(); }  //**For the 8x8 nodes we doubled this.
  //  static float getBvhAwarenessDeleteRadiusY() { return getBvhAwarenessMaxRadiusY(); }  //**For the 8x8 nodes we doubled this.
    static float getObjDefaultBoxHeight2() { return getCellWidth() * 0.5f;}
    static float getObjDefaultBoxWidth2() {
        return getCellWidth() * 0.4999f;//If we use 0.5 the boxes will stick & break physics
    }

    //static ivec3 v3Toi3Node(vec3& v);
   // static ivec2 v2Toi3Node(vec2& v);
   // static ivec3 v3Toi3CellLocal(vec3& v); //Note *Global* not local.
    //static ivec3 v3Toi3Any(vec3& v, float w1, float h1);
   // static vec3 i3tov3Node(const ivec3& iNode);
 //   static vec3 i3tov3Cell(const ivec3& iNode, const ivec3& iCell);
    static uint32_t getMaxVisibleObjects() { return 1024; }

    static void addTileQuad2(World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
        TileMesh25* qbm, Sprite* pMot,
        vec2& min, vec2& max, float midz, vec3& color0, vec3& color1, vec3& color2, vec3& color3);
    static void addTileQuad2(World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
        TileMesh25* qbm, Hash32 frameId,
        vec2& min, vec2& max, float midz, vec3& color0, vec3& color1, vec3& color2, vec3& color3);
   
    static void addTileQuad3D(std::shared_ptr<Atlas> pAtlas,
        TileMesh25* qbm, Hash32 frameId,
        vec3& vCamRt, vec3& vCamUp, vec3& vCenter, float sprW, float sprH,
        vec3& color0, vec3& color1, vec3& color2, vec3& color3);

    static void addBoxQuads5(
        World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
        TileMesh25* qbm, Sprite* pMot, Box3f* pBox,
        vec3& color0, vec3& color1, vec3& color2, vec3& color3);

    static void makeWorldMesh( std::shared_ptr<MeshNode>& __out_ pMesh,
        std::vector<W25MeshVert>* _verts, std::vector<v_index32>* _indexes);

    static int getNumCellsPerNode();
    static int getNumValsPerNode();
    static int getCellIndexFori38(int ix, int iy, int iz, int ci);
    static int getCellIndexFori3(int ix, int iy, int iz);
    static int getVertIndexFori3(int ix, int iy, int iz) ;
    static int getIndexFori3(int ix, int iy, int iz, int w, int h) ;
    //static World25Side::e getOppNeighborIndex(World25Side::e iNeighbor);
    static BlockPos8::e oppBlockPosX(BlockPos8::e pos);
    static BlockPos8::e oppBlockPosY(BlockPos8::e pos) ;
    static BlockPos8::e oppBlockPosZ(BlockPos8::e pos);
};

}//ns Game



#endif
