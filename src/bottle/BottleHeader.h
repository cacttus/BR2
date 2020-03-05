/**
*  @file Conga Header.h
*  @date 3/1 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __DARTHEADER_14848788473239398051_H__
#define __DARTHEADER_14848788473239398051_H__

#include "../base/BaseHeader.h"
#include "../model/ModelHeader.h"
#include "../world/WorldHeader.h"
#include "../math/MathAll.h"
#include "../gfx/GfxHeader.h"
#include "../model/VertexTypes.h"

namespace BR2 {
//typedef uint32_t World25GridId;
//We store materials in the hash32
//typedef Hash32 WorldCellSize;
//typedef uint64_t PixObjId;
//#define W25_NO_OBJECT_ID 0
//Serialized Types.
typedef uint8_t TileId8;
typedef v_v3i2n3 W25MeshVert;

//*Note; if updating this make sure to update the vertex shader v3c3x2_tileshader uniform buffer _ufGpuSprites.
#define W25_MAX_GPU_SPRITES 128

#define NO_CELL_SPRITE 0
//Conga File version
//0x50534404 - positions were grid based
//0x50534405 - Added vec3 positions, changed Object eType from uint8 to Int16
#define CONGA_GRID_FILE_VERSION 0x5044000A   /*5044xxxx  xxxx = 0000,0001,.. = 0.0, 0.1, etc*/
//version of the sprites.dat file for holding sprite 
#define CONGA_SPRITE_FILE_VERSION "0.02"

#define PIX_OBJ_TYPE_INVALID 0
enum class GameMode { Play, WorldSelect };
namespace GridShow { typedef enum { None, Top, TopSideBot } e; }
namespace BR2Mode { typedef enum { Play, WorldSelect } e; }
namespace DrawType { typedef enum { Sprite, Box5, Mesh, Model, MaxDrawTypes } e; }
namespace EditMode { typedef enum { Invalid, Tile, Block, Vertex, VertexGroup, HillTile, PlaceObject, NineTile, Select,  Proc, MaxEditModes} e; }
namespace EditTool { typedef enum { Invalid, Pencil, Eraser, MaxEditTools } e; }
namespace BlockMode { typedef enum { Single, Paint, MaxBlockModes } e; }
namespace DebugGridMode { typedef enum { Disable, Flat, Random, Hill, MaxDebugGridModes } e; }
namespace WorldObjState { typedef enum { Idle, Moving, Destroying, Dying }e; }
namespace RegionSize { typedef enum { Region_1x1, Region_2x2, Region_4x4, Region_8x8 } e; }
namespace ProcMode { typedef enum { Block, Hills, Slab, Random, Clear } e; }
namespace GridMeshLayer { typedef enum { Opaque, Transparent, MaxMatters, Invalid } e; }
namespace GridPhysicsStep { typedef enum { Unstick, Collide, Invalid } e; }
namespace World25GridGen { typedef enum { Load, GenAndSave } e; }

namespace LairGenLocale { typedef enum { Glob, Lair, World, MaxGenLocales } e; }
namespace WalkerType { typedef enum { Cave, Tile, /*World, Chute, Material, Ghost, Reference, RoomHs*/ } e; }
namespace WalkerVolumeType { typedef enum { Box, Ellipsoid } e; }
namespace GameScreen { typedef enum { Title, World/*, MultiplayerSelect, CharMaker*/} e; }
namespace BlockPos8 { typedef enum { NBP3_ABL, NBP3_ABR, NBP3_ATL, NBP3_ATR, NBP3_FBL, NBP3_FBR, NBP3_FTL, NBP3_FTR} e; }
namespace GridGenState { typedef enum { PreGen, Queued, Stage1, Stage1Complete, Stage2, Generated } e; }

typedef uint32_t PlaceIndex; //0-26 are the vertexes in LHS axis-order, 27 - 50 - are the 24 sub-quad indexes in AA LHS (4 per side)
namespace PlaceMode { typedef enum { None, Orient } e; }

//CrossA  |\+   + is normal
//CrossB  +/|
//CrossC  |/+
//CrossD  +\|
namespace W25Plane {
typedef enum { Left, Right, Bottom, Top, Back, Front, CrossA, CrossB, CrossC, CrossD, None } e;
};
namespace W25SidePlace {
typedef enum { Top, Bot, Side, Cross, Count } e;
};
//W25Geom - The geom is 8 bits corresponding to 8 corners of a cube.
//These determine the configuration.  All corners that are '1' are solid, inside the world.
//Each World Block has 2 geoms, 1 for opaque, 1 for transparent
//Corners that are '0' are empty or reside on the surface.
//Ordering (LHS): 0 = x=0y=0z=0 
//                1 = x=1y=0z=0
//                2 = x=0y=1z=0
//                3 = x=1y=1z=0
//                4 = x=0y=0z=1
//                5 = x=1y=0z=1
//                6 = x=0y=1z=1
//                7 = x=1y=1z=1
//Cube Side Order: left, Right, bottom, top, back front (-x, +x, -y +y, -z +z)
typedef uint8_t W25Geom;
typedef uint8_t W25Tile;
#define W25GEOM_EMPTY 0
#define W25TILE_EMPTY 0
#define W25GEOM_SOLID 0xFF

static bool w25_get(W25Geom g, int i){
    return (g & (1<<i)) != 0;
}
static void w25_set(W25Geom& g, int i) {
  //  if(b) {
        g = g | (1 << i);
  //  }
  //  else {
  //      g = g & ~(1 << i);
  //  }
}
static void w25_clr(W25Geom& g, int i) {
    g = g & ~(1 << i);
}
// Texture configuration
namespace W25Sc {
typedef enum {
     A
    //+
    //| \
    //+---+
    , B
    //    +
    //  / |
    //+---+
    , C
    //+---+
    //| /
    //+
    , D
    //+---+
    //  \ |
    //    +
    ,Empty

}e;
}

typedef Hash32 PixObjType;

#define PHY_STATIC_MASS (-1.0f);

class World25;
class WorldGrid;
class WorldCell;
class CongaGui;
//class WorldObj;
class BottleScript;
//class WorldObjSpec;
class World25ObjectData;
class WorldCellData;
class WorldCellFile;
class TileButton;
class EditButton;
class EditButtonGroup;
class TileShader;
class SpriteShader;
class Sprite;
class SpriteSpec;
//class World25Light    ;
//class World25LightData;
class GodCamScript;
//class WorldObject;
class Goal25;
class Brain25;
class Goal25MoveTo;
class Goal25Think;
class BlockNode;
//class W25CellManifold   ;
//class W25ObjManfiold    ;
//class W25ObjCellManfiold;
class TileMesh25;
class v_v3c3i4i4;
class VaoDataGeneric;
class ObFile;
class W25MeshMaker;
class W25MeshConf;
class WipGrid;
typedef v_v3c3i4i4 TileBufferVert;
class FlatShader;
class Tile25Spec;
class WorldMaker;
class W25Config;
class LairSpec;
class WalkerSpec;
class LairTileSpec;
class MorphTile;
class ClimateParams;
class LairTile;
class LairWalker;
class World25;
class SnakeMaker;
class Walker;
class WorldEditor;
class W25GridMesh;
//typedef Hash32 LairId;
class WorldObj;
class WorldEditState;
class GameUi;


class World25Plane : public Plane3f {
    vec3 _u, _v;
public:
    vec3& getBasisU() { return _u; }
    vec3& getBasisV() { return _v; }
};
class CellVisibility {
public:
    static float getMaxVis() {return 4.0f; }

    float _fVisCount = 0; // 1-4 the number of corners which are visible determines the % of light this cell gets.
};
//Vertexes, cells or etc
class W25SelectData : public VirtualMemory {
public:
    //All selection items get filled regardless of mode.
    //Cell
    // cell vertex
    // ** If cluster mode  then we just edit all neighbors - 
    //  we don't need a separate selection
    vec3 _vTriPoint; //Triangle point, inside block
    vec3 _vTriNormal; // normal on tri
    WorldCell* _pTriCell = nullptr;

    vec3 _vBlockPoint; //Point at edge of block
    vec3 _vBlockNormal; //normal on blcok
    WorldCell* _pBlockCell = nullptr;


    // int _iCellVtx; //vertex 0-7 *Not needed - this can be calculated
    int getCellVtx() { return 0; } //TODO - calc cell vtx.

                                   //we need FOUR modes
                                   // cell block
                                   // group block - raise block and all neighbor cells to make hill
                                   // Optional - 2 cell / 3 cell
                                   // cell vertex - single corenr of cell
                                   // group vertex - 4 cells at corner
    W25SelectData() { 
    _vTriNormal = _vTriPoint = _vBlockPoint = _vBlockNormal = vec3(0,0,0);
    }
    virtual ~W25SelectData() override { }
};
struct GpuTile {
    int32_t _iOff;
    int32_t pad[3];
};
class ClimateSpec : public VirtualMemory{
    MpFloat _fLight;
    MpFloat _fTemp;
    MpFloat _fPrecip;
    MpFloat _fDepth;
public:
    MpFloat& getLight() { return _fLight; }
    MpFloat& getTemp() { return _fTemp; }
    MpFloat& getPrecip() { return _fPrecip; }
    MpFloat& getDepth() { return _fDepth; }

    ClimateSpec() { }
    virtual ~ClimateSpec() override { }
};
class ClimateInstance : public VirtualMemory {
    ClimateSpec* _pSpec = nullptr;
    float _fLight;
    float _fTemp;
    float _fPrecip;
    float _fDepth;
public:
    float& getLight() { return _fLight; }
    float& getTemp() { return _fTemp; }
    float& getPrecip() { return _fPrecip; }
    float& getDepth() { return _fDepth; }

    ClimateInstance(ClimateSpec* s) : _pSpec(s) { }
    virtual ~ClimateInstance() override { }
};
//Params for the perlin terrain gen function
class W25NoiseFunc : public VirtualMemory {
public:
    int _nOctaves = 4;
    float _fOffset = 3.03f;
    float _fPersistence = 1.67f;
    float _fScale = 0.002f;
    W25NoiseFunc() { }
    virtual ~W25NoiseFunc() override { }
};
class VisibleCell {

    int16_t _iMeshIndexOffset[GridMeshLayer::e::MaxMatters];//Offset of this Cell's triangle indexes in the mesh.
    int16_t _iMeshIndexCount[GridMeshLayer::e::MaxMatters];//Count of this Cell's triangle indexes in the mesh.
    bool _bFrameVisible = true;
public:
    void setFrameVisible(bool b) { _bFrameVisible = b; }
    bool getFrameVisible() { return _bFrameVisible; }

    int32_t getMeshIndexOffset(GridMeshLayer::e eMatter) { return _iMeshIndexOffset[eMatter]; }
    void setMeshIndexOffset(GridMeshLayer::e eMatter, int32_t off) { _iMeshIndexOffset[eMatter] = off; }
    int32_t getMeshIndexCount(GridMeshLayer::e eMatter) { return _iMeshIndexCount[eMatter]; }
    void setMeshIndexCount(GridMeshLayer::e eMatter, int32_t a) { _iMeshIndexCount[eMatter] = a; }
};

class BlockInfo {
    Ray_t _ray; // Set before calling
    WorldCell* _pCell;
    vec3 _pick_n;//Normal to picked surface
    vec3 _pick_pt;//origin
};



}//ns Game



#endif
