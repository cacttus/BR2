#include "../base/Gu.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/World25.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/WorldCellFile.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/BlockNode.h"

namespace BR2 {
//static int v8side6[6][4];
//static int v8side6opp[6][4];
int WorldCell::getV8IndexForV4Side(PhysicsGridSide::e eSide, int i) {
  static int v8side6[6][4]{
      { 4,0,6,2 }//L
      ,{ 1,5,3,7 }//R
      ,{ 4,5,0,1 }//B
      ,{ 2,3,6,7 }//T
      ,{ 0,1,2,3 }//A
      ,{ 5,4,7,6 }//F
  };
  AssertOrThrow2((int)eSide >= 0 && (int)eSide < 6);
  AssertOrThrow2(i >= 0 && i < 4);

  return v8side6[eSide][i];
}
int WorldCell::getV8IndexForV4SideOpp(PhysicsGridSide::e eSide, int i) {
  static int v8side6opp[6][4]{
      { 5,1,7,3 }//L
      ,{ 0,4,2,6 }//R
      ,{ 6,7,2,3 }//B
      ,{ 0,1,4,5 }//T

      ,{ 4,5,6,7 }//A
      ,{ 1,0,3,2 }//F
  };
  AssertOrThrow2((int)eSide >= 0 && (int)eSide < 6);
  AssertOrThrow2(i >= 0 && i < 4);
  return v8side6opp[eSide][i];
}
void WorldCell::getRelativeNeighborPos(PhysicsGridSide::e eSide, ivec3& iv) {
  static int relativeNeighborPos[6][3]{
       { -1,    0,    0 }//L
      ,{  1,    0,    0 }//R
      ,{     0,-1,    0 }//B
      ,{     0, 1,    0 }//T
      ,{     0,    0,-1 }//A
      ,{     0,    0, 1 }//F
  };

  iv.construct(
    relativeNeighborPos[eSide][0],
    relativeNeighborPos[eSide][1],
    relativeNeighborPos[eSide][2]
  );
}

///////////////////////////////////////////////////////////////////
WorldCell::WorldCell(int ix, int iy, int iz, BlockNode* pParentNode, W25Geom solid, W25Geom liquid) :
  _pParentNode(pParentNode) {
  _viPosLocal.x = ix;
  _viPosLocal.y = iy;
  _viPosLocal.z = iz;

  for (int ni = 0; ni < c_nNeighbors; ++ni) {
    _pNeighbor[ni] = nullptr;
  }
  //  _pManifold = new W25ObjCellManfiold();
    //for (int i = 0; i<4; ++i) {
    //    _vColor[i] = 1.0f;
    //}
  for (int mi = 0; mi < GridMeshLayer::e::MaxMatters; ++mi) {
    //_iMeshIndexOffset[mi] = -1;
    //_iMeshIndexCount[mi] = 0;

    _iGeom[mi] = W25GEOM_EMPTY;
    _iTile[mi] = W25TILE_EMPTY;
  }

  _iGeom[GridMeshLayer::e::Opaque] = solid;
  _iGeom[GridMeshLayer::e::Transparent] = liquid;
}
WorldCell::~WorldCell() {
  //Unset the neighbors
  for (int i = 0; i < c_nNeighbors; ++i) {
    if (_pNeighbor[i] != nullptr) {
      _pNeighbor[i]->_pNeighbor[PhysicsGrid::getOppNeighborIndex((PhysicsGridSide::e)i)] = nullptr;
    }
  }
  DEL_MEM(_pVisibleCell);
  //  DEL_MEM(_pManifold);
}
///////////////////////////////////////////////////////////////////

bool WorldCell::isBorderCell() {
  return isBorderCellPos(_viPosLocal);
}
bool WorldCell::isBorderCellPos(const ivec3& vi) {
  uint32_t gw1 = (uint32_t)BottleUtils::getNumCellsWidth() - 1;
  if (vi.x != 0
    && vi.x != gw1
    && vi.y != 0
    && vi.y != gw1
    && vi.z != 0
    && vi.z != gw1) {
    return false;
  }
  return true;
}
bool WorldCell::isBorderSide(PhysicsGridSide::e eSide) {
  uint32_t gw1 = (uint32_t)BottleUtils::getNumCellsWidth() - 1;
  if (eSide == (PhysicsGridSide::e::gL) && _viPosLocal.x == 0) {
    return true;
  }
  else if (eSide == (PhysicsGridSide::e::gR) && _viPosLocal.x == gw1) {
    return true;
  }
  else if (eSide == (PhysicsGridSide::e::gB) && _viPosLocal.y == 0) {
    return true;
  }
  else if (eSide == (PhysicsGridSide::e::gT) && _viPosLocal.y == gw1) {
    return true;
  }
  else if (eSide == (PhysicsGridSide::e::gA) && _viPosLocal.z == 0) {
    return true;
  }
  else if (eSide == (PhysicsGridSide::e::gF) && _viPosLocal.z == gw1) {
    return true;
  }

  return false;
}
void WorldCell::getBoundBoxBvh(Box3f& __out_ box) {
  box._min = getOriginR3();
  box._max = box._min;
  box._max.x += BottleUtils::getCellWidth();
  box._max.y += BottleUtils::getCellHeight();
  box._max.z += BottleUtils::getCellWidth();
}
Box3f WorldCell::getBoundBoxBvh() {
  Box3f b;
  getBoundBoxBvh(b);
  return b;
}
bool WorldCell::isValidCellPos(const ivec3& vi) {
  int cw = BottleUtils::getNumCellsWidth();
  if (vi.x >= 0 && vi.x < cw &&
    vi.y >= 0 && vi.y < cw &&
    vi.z >= 0 && vi.z < cw) {
    return true;
  }
  else {
    return false;
  }
}

//void WorldCell::update(float del) {
//    int n = 0;
//    n++;
//    //this get called?
//    //if(_pBaseTile){
//    //    _pBaseTile->update(del);
//    //}
//    //if (_pNineTile) {
//    //    _pNineTile->update(del);
//    //}
//}
//GPos WorldCell::getGPos() {
//    AssertOrThrow2(_pWorld25Grid);
//    GPos gp(_pWorld25Grid->getGridPos(), _viPosLocal);
//    gp.validate();
//
//    return gp;
//}
std::shared_ptr<WorldGrid> WorldCell::getGrid() { return _pParentNode->getGrid(); }

vec3 WorldCell::getOriginR3() {
  vec3 r;
  AssertOrThrow2(_pParentNode != nullptr);
  if (_pParentNode->getGrid() != nullptr) {
    r = _pParentNode->getGrid()->getOriginR3();

    r.x += ((float)_viPosLocal.x * BottleUtils::getCellWidth());
    r.y += ((float)_viPosLocal.y * BottleUtils::getCellHeight());
    r.z += ((float)_viPosLocal.z * BottleUtils::getCellWidth());
  }
  //else {
  //    return _v3CustomBase;
  //}

  return r;
}
vec3 WorldCell::getCenterR3() {
  vec3 r = getOriginR3();
  r.x += BottleUtils::getCellWidth() * 0.5f;
  r.y += BottleUtils::getCellHeight() * 0.5f;
  r.z += BottleUtils::getCellWidth() * 0.5f;
  return r;
}

WorldCellData* WorldCell::serialize() {
  WorldCellData* pd = new WorldCellData();

  //    AssertOrThrow2(_pTile[MatterMode::e::Solid] != nullptr && _pTile[MatterMode::e::Liquid] != nullptr);

  return  WorldCell::serializeData(
    _iTile[GridMeshLayer::e::Opaque]
    , _iTile[GridMeshLayer::e::Transparent]
    , 0//_iTile[MatterMode::e::Gas]
    , _iGeom[GridMeshLayer::e::Opaque]
    , _iGeom[GridMeshLayer::e::Transparent]
    , 0//_iGeom[MatterMode::e::Gas]
  );
}
WorldCellData* WorldCell::serializeEmpty() {
  return  WorldCell::serializeData(
    W25TILE_EMPTY
    , W25TILE_EMPTY
    , W25TILE_EMPTY
    , W25GEOM_EMPTY
    , W25GEOM_EMPTY
    , W25GEOM_EMPTY
  );
}
WorldCellData* WorldCell::serializeData(
  W25Tile tsolid, W25Tile tliquid, W25Tile tgas,
  W25Geom gsolid, W25Geom gliquid, W25Geom ggas
) {
  WorldCellData* pd = new WorldCellData();

  pd->_iBaseTileIdx = tsolid;
  pd->_iLiquidTileIdx = tliquid;
  //   pd->_iGasTileIdx = tgas;
  pd->_iSolid = gsolid;
  pd->_iLiquid = gliquid;
  //  pd->_iGas = ggas;

  return pd;
}
}//ns Game