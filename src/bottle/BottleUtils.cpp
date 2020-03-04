#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../gfx/CameraNode.h"
#include "../gfx/GpuQuad3.h"
#include "../gfx/QuadBufferMesh.h"
#include "../gfx/Atlas.h"
#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/VaoDataGeneric.h"
#include "../model/TileMesh25.h"
#include "../world/PhysicsGrid.h"
#include "../world/PhysicsWorld.h"
#include "../bottle/BottleUtils.h"

#include "../bottle/SpriteBucket.h"
#include "../bottle/BottleUtils.h"

namespace BR2 {
void BottleUtils::addTileQuad2(World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
  TileMesh25* qbm, Sprite* pMot,
  vec2& min, vec2& max, float midz, vec3& color0, vec3& color1, vec3& color2, vec3& color3) {
  if (pMot == nullptr) {
    return;
  }
  Hash32 frameId = pMot->getFrameAtlasIndex();

  addTileQuad2(pPlane, pAtlas, qbm, frameId, min, max, midz, color0, color1, color2, color3);
}
void BottleUtils::addTileQuad2(World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
  TileMesh25* qbm, Hash32 frameId,
  vec2& min, vec2& max, float midz, vec3& color0, vec3& color1, vec3& color2, vec3& color3) {
  Quad3f pos;
  v_v3c3x2n3* vertex[4];

  vec2 bl, br, tl, tr;
  // vec3 norm;
  static bool bLoggedFull = false;

  if (qbm->isFull()) {
    if (bLoggedFull == false) {
      BRLogWarn("Quad buffer is full, some items will not be drawn.");
      bLoggedFull = true;
    }
    return;
  }
  else {
    bLoggedFull = true;
  }

  if (frameId != MOTION25_INVALID_FRAME) {
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);

    //TLTRBLBR => BLBRTLTR
    vertex[0]->v = vec3(min.x, midz, min.y);
    vertex[1]->v = vec3(max.x, midz, min.y);
    vertex[2]->v = vec3(min.x, midz, max.y);
    vertex[3]->v = vec3(max.x, midz, max.y);

    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    //*OPTIMIZE:
    //This performs a sprite lookup.
    // We can speed up this by instead passing the ivec2 of the tile quad.
    //*NO: we're going to store the GPU quads on the tils in a new "TilePool" strcuture derived from QBM
    pAtlas->getTCoords(frameId, &bl, &br, &tl, &tr, false);
    Box2f tex(bl, tr);

    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;

    vertex[0]->n = vec3(0, 1, 0);
    vertex[1]->n = vec3(0, 1, 0);
    vertex[2]->n = vec3(0, 1, 0);
    vertex[3]->n = vec3(0, 1, 0);

    qbm->nextQuad();
  }
}
void BottleUtils::addBoxQuads5(World25Plane* pPlane, std::shared_ptr<Atlas> pAtlas,
  TileMesh25* qbm, Sprite* pMot, Box3f* pBox,
  vec3& color0, vec3& color1, vec3& color2, vec3& color3) {
  v_v3c3x2n3* vertex[4];

  //Get Motion
  if (pMot == nullptr) {
    return;
  }
  Hash32 frameId = pMot->getFrameAtlasIndex();

  //Check if full
  static bool bLoggedFull = false;
  if (qbm->getQuadCount() + 6 >= qbm->getMaxQuads()) {
    if (bLoggedFull == false) {
      BRLogWarn("Quad buffer is full, some items will not be drawn.");
      bLoggedFull = true;
    }
    return;
  }
  else {
    bLoggedFull = true;
  }

  if (frameId != MOTION25_INVALID_FRAME) {
    vec2 bl, br, tl, tr;

    vec3& bi = pBox->_min;
    vec3& ba = pBox->_max;

    //Same coords for each quad
    pAtlas->getTCoords(frameId, &bl, &br, &tl, &tr, false);
    Box2f tex(bl, tr);

    //Somewhat unorthodox, because we have the Z axis in the up direction... technically that's "up"
    //but we're using axis enumeration correctly here.
    vec3 p0(bi.x, bi.y, bi.z);
    vec3 p1(ba.x, bi.y, bi.z);
    vec3 p2(bi.x, ba.y, bi.z);
    vec3 p3(ba.x, ba.y, bi.z);
    vec3 p4(bi.x, bi.y, ba.z);
    vec3 p5(ba.x, bi.y, ba.z);
    vec3 p6(bi.x, ba.y, ba.z);
    vec3 p7(ba.x, ba.y, ba.z);

    //All of 0,1,2,3 is BL, BR, TL, TR of the quad.  Top is -Z
    //L
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;
    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    vertex[0]->v = p4;
    vertex[1]->v = p0;
    vertex[2]->v = p6;
    vertex[3]->v = p2;

    vertex[0]->n.construct(-1, 0, 0);
    vertex[1]->n.construct(-1, 0, 0);
    vertex[2]->n.construct(-1, 0, 0);
    vertex[3]->n.construct(-1, 0, 0);
    //R
    qbm->nextQuad();
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;
    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    vertex[0]->v = p1;
    vertex[1]->v = p5;
    vertex[2]->v = p3;
    vertex[3]->v = p7;

    vertex[0]->n.construct(1, 0, 0);
    vertex[1]->n.construct(1, 0, 0);
    vertex[2]->n.construct(1, 0, 0);
    vertex[3]->n.construct(1, 0, 0);
    ////B
    //b is facing down the Z so don't show it
    //qbm->nextQuad();
    //qbm->getGpuQuad(pQuad);
    //pQuad._vertex[0]->x = bl;
    //pQuad._vertex[1]->x = br;
    //pQuad._vertex[2]->x = tl;
    //pQuad._vertex[3]->x = tr;
    //pQuad._vertex[0]->n = color0;
    //pQuad._vertex[1]->n = color1;
    //pQuad._vertex[2]->n = color2;
    //pQuad._vertex[3]->n = color3;

    //pQuad._vertex[0]->v = p5;
    //pQuad._vertex[1]->v = p4;
    //pQuad._vertex[2]->v = p7;
    //pQuad._vertex[3]->v = p6;
    //T
    qbm->nextQuad();
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;
    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    vertex[0]->v = p2;
    vertex[1]->v = p3;
    vertex[2]->v = p6;
    vertex[3]->v = p7;
    vertex[0]->n.construct(0, 1, 0);
    vertex[1]->n.construct(0, 1, 0);
    vertex[2]->n.construct(0, 1, 0);
    vertex[3]->n.construct(0, 1, 0);
    //A
    qbm->nextQuad();
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;
    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    vertex[0]->v = p0;
    vertex[1]->v = p1;
    vertex[2]->v = p2;
    vertex[3]->v = p3;
    vertex[0]->n.construct(0, 0, -1);
    vertex[1]->n.construct(0, 0, -1);
    vertex[2]->n.construct(0, 0, -1);
    vertex[3]->n.construct(0, 0, -1);
    //F
    qbm->nextQuad();
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;
    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    vertex[0]->v = p5;
    vertex[1]->v = p4;
    vertex[2]->v = p7;
    vertex[3]->v = p6;
    vertex[0]->n.construct(0, 0, 1);
    vertex[1]->n.construct(0, 0, 1);
    vertex[2]->n.construct(0, 0, 1);
    vertex[3]->n.construct(0, 0, 1);
    qbm->nextQuad();
  }
}
void BottleUtils::addTileQuad3D(std::shared_ptr<Atlas> pAtlas,
  TileMesh25* qbm, Hash32 frameId,
  vec3& vCamRt, vec3& vCamUp, vec3& vCenter, float sprW2, float sprH2,
  vec3& color0, vec3& color1, vec3& color2, vec3& color3) {
  Quad3f pos;
  v_v3c3x2n3* vertex[4];

  vec2 bl, br, tl, tr;
  // vec3 norm;
  static bool bLoggedFull = false;

  if (qbm->isFull()) {
    if (bLoggedFull == false) {
      BRLogWarn("Quad buffer is full, some items will not be drawn.");
      bLoggedFull = true;
    }
    return;
  }
  else {
    bLoggedFull = true;
  }

  if (frameId != MOTION25_INVALID_FRAME) {
    qbm->getQuad(vertex[0], vertex[1], vertex[2], vertex[3]);

    //TLTRBLBR => BLBRTLTR
    vertex[0]->v = vCenter + vCamRt * -sprW2 + vCamUp * -sprH2;
    vertex[1]->v = vCenter + vCamRt * sprW2 + vCamUp * -sprH2;
    vertex[2]->v = vCenter + vCamRt * -sprW2 + vCamUp * sprH2;
    vertex[3]->v = vCenter + vCamRt * sprW2 + vCamUp * sprH2;

    vertex[0]->c = color0;
    vertex[1]->c = color1;
    vertex[2]->c = color2;
    vertex[3]->c = color3;

    //*OPTIMIZE:
    //This performs a sprite lookup.
    // We can speed up this by instead passing the ivec2 of the tile quad.
    //*NO: we're going to store the GPU quads on the tils in a new "TilePool" strcuture derived from QBM
    pAtlas->getTCoords(frameId, &bl, &br, &tl, &tr, false);
    Box2f tex(bl, tr);

    vertex[0]->x = bl;
    vertex[1]->x = br;
    vertex[2]->x = tl;
    vertex[3]->x = tr;

    vertex[0]->n = vec3(0, 1, 0);
    vertex[1]->n = vec3(0, 1, 0);
    vertex[2]->n = vec3(0, 1, 0);
    vertex[3]->n = vec3(0, 1, 0);

    qbm->nextQuad();
  }
}

void BottleUtils::makeWorldMesh(std::shared_ptr<MeshNode>& __out_ pMesh,
  std::vector<W25MeshVert>* _verts, std::vector<v_index32 >* _indexes) {
  pMesh = nullptr;

  if (_verts->size() > 0 && _indexes->size() > 0) {
    pMesh = MeshNode::create(std::make_shared<MeshSpec>(_verts->data(), _verts->size(),
      _indexes->data(), _indexes->size(), W25MeshVert::getVertexFormat(), nullptr)
    );
    pMesh->getBoundBoxObject()->genResetLimits();

    //Calculate bound box for mesh
    for (size_t iVert = 0; iVert < _verts->size(); ++iVert) {
      pMesh->getBoundBoxObject()->genExpandByPoint(_verts->at(iVert).v);
    }
  }
}

int BottleUtils::getNumCellsPerNode() {
  int w = (int)BottleUtils::getNumCellsWidth();
  int h = (int)BottleUtils::getNumCellsWidth();
  int max = w * h * w;
  return max;
}
int BottleUtils::getNumValsPerNode() {
  int w = (int)BottleUtils::getNumCellsWidth() + 1;
  int h = (int)BottleUtils::getNumCellsWidth() + 1;
  int max = w * h * w;
  return max;
}
int BottleUtils::getCellIndexFori38(int ix, int iy, int iz, int ci) {
  int w = (int)BottleUtils::getNumCellsWidth() * 2;
  int h = (int)BottleUtils::getNumCellsWidth() * 2;
  ivec3 d;
  if (ci == 0) { d.construct(1, 1, 1); } // Top right
  if (ci == 1) { d.construct(0, 1, 1); }
  if (ci == 2) { d.construct(1, 0, 1); }
  if (ci == 3) { d.construct(0, 0, 1); }
  if (ci == 4) { d.construct(1, 1, 0); }
  if (ci == 5) { d.construct(0, 1, 0); }
  if (ci == 6) { d.construct(1, 0, 0); }
  if (ci == 7) { d.construct(0, 0, 0); }

  int ind = (iz + d.z) * (h * w) + (iy + d.y) * w + (ix + d.x);

  //int max = getNumCellsPerNode();
  //
  //AssertOrThrow2(ind < max);

  return ind;
}
int BottleUtils::getCellIndexFori3(int ix, int iy, int iz) {
  int w = (int)BottleUtils::getNumCellsWidth();
  int h = (int)BottleUtils::getNumCellsWidth();

  return getIndexFori3(ix, iy, iz, w, h);
}
int BottleUtils::getVertIndexFori3(int ix, int iy, int iz) {
  //The vertex in the noise field.
  int w = (int)BottleUtils::getNumCellsWidth() + 1;
  int h = (int)BottleUtils::getNumCellsWidth() + 1;

  return getIndexFori3(ix, iy, iz, w, h);
}
int BottleUtils::getIndexFori3(int ix, int iy, int iz, int w, int h) {
  int ind = iz * h * w + iy * w + ix;
  int max = w * h * w;
  AssertOrThrow2(ind >= 0 && ind < max);
  AssertOrThrow2((ix >= 0 && ix < w));
  AssertOrThrow2((iy >= 0 && iy < h));
  AssertOrThrow2((iz >= 0 && iz < w));

  return ind;
}

BlockPos8::e BottleUtils::oppBlockPosX(BlockPos8::e pos) {
  if (pos == BlockPos8::e::NBP3_ABL) return BlockPos8::e::NBP3_ABR;
  else if (pos == BlockPos8::e::NBP3_ATL) return BlockPos8::e::NBP3_ATR;
  else if (pos == BlockPos8::e::NBP3_FBL) return BlockPos8::e::NBP3_FBR;
  else if (pos == BlockPos8::e::NBP3_FTL) return BlockPos8::e::NBP3_FTR;

  else if (pos == BlockPos8::e::NBP3_ABR) return BlockPos8::e::NBP3_ABL;
  else if (pos == BlockPos8::e::NBP3_ATR) return BlockPos8::e::NBP3_ATL;
  else if (pos == BlockPos8::e::NBP3_FBR) return BlockPos8::e::NBP3_FBL;
  else if (pos == BlockPos8::e::NBP3_FTR) return BlockPos8::e::NBP3_FTL;
  BRThrowNotImplementedException();
}
BlockPos8::e BottleUtils::oppBlockPosY(BlockPos8::e pos) {
  if (pos == BlockPos8::e::NBP3_ABL) return BlockPos8::e::NBP3_ATL;
  else if (pos == BlockPos8::e::NBP3_ABR) return BlockPos8::e::NBP3_ATR;
  else if (pos == BlockPos8::e::NBP3_FBL) return BlockPos8::e::NBP3_FTL;
  else if (pos == BlockPos8::e::NBP3_FBR) return BlockPos8::e::NBP3_FTR;

  else if (pos == BlockPos8::e::NBP3_ATL) return BlockPos8::e::NBP3_ABL;
  else if (pos == BlockPos8::e::NBP3_ATR) return BlockPos8::e::NBP3_ABR;
  else if (pos == BlockPos8::e::NBP3_FTL) return BlockPos8::e::NBP3_FBL;
  else if (pos == BlockPos8::e::NBP3_FTR) return BlockPos8::e::NBP3_FBR;

  BRThrowNotImplementedException();
}
BlockPos8::e BottleUtils::oppBlockPosZ(BlockPos8::e pos) {
  if (pos == BlockPos8::e::NBP3_ABL) return BlockPos8::e::NBP3_FBL;
  else if (pos == BlockPos8::e::NBP3_ABR) return BlockPos8::e::NBP3_FBR;
  else if (pos == BlockPos8::e::NBP3_ATL) return BlockPos8::e::NBP3_FTL;
  else if (pos == BlockPos8::e::NBP3_ATR) return BlockPos8::e::NBP3_FTR;

  else if (pos == BlockPos8::e::NBP3_FBL) return BlockPos8::e::NBP3_ABL;
  else if (pos == BlockPos8::e::NBP3_FBR) return BlockPos8::e::NBP3_ABR;
  else if (pos == BlockPos8::e::NBP3_FTL) return BlockPos8::e::NBP3_ATL;
  else if (pos == BlockPos8::e::NBP3_FTR) return BlockPos8::e::NBP3_ATR;

  BRThrowNotImplementedException();
}
}//ns Game