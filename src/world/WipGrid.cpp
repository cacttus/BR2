#include "../world/WipGrid.h"
#include "../world/BottleUtils.h"
#include "../world/WorldGrid.h"
#include "../world/W25Config.h"
#include "../world/Lair.h"
#include "../world/SpriteBucket.h"
#include "../world/Tile25Spec.h"
#include "../world/WorldCell.h"
#include "../world/W25MeshMaker.h"
/*
gen all base land (no caves)
2 gen "sea" in land where y < 0 - don't gen water where there is solid land
3 - then make caves - this will prevent water from filling caves
4 - Then Iterate tiles Top - Down
    calc sunlight for each.
*/
namespace BR2 {
WipGrid::WipGrid(ivec3& viPos, LairSpec* pLair, int iSeed) :
  EasyNoise(iSeed), _pLair(pLair), _viPos(viPos) {
  // _iNoiseArrSize = CongaUtils::getNumValsPerNode();
  _iGDataSize = BottleUtils::getNumCellsPerNode();
  _pGData = new GData[_iGDataSize];
  //  _pNoiseValues = new NoiseData[_iNoiseArrSize];

}
WipGrid::~WipGrid() {
  delete[] _pGData;
  //  DEL_MEM(_pNoiseValues);
}
void WipGrid::clear(Tile25Spec* pt) {
  AssertOrThrow2(pt != nullptr);
  //memset(_pGData, 0, sizeof(GData) * _iGDataSize);
  //_pGData = new GData[_iGDataSize];
  int ix = (int)BottleUtils::getNumCellsWidth();
  int iy = (int)BottleUtils::getNumCellsWidth();
  int iz = (int)BottleUtils::getNumCellsWidth();

  //Create both the sea and the land.
  for (int zz = 0; zz < iz; ++zz) {
    for (int yy = 0; yy < iy; ++yy) {
      for (int xx = 0; xx < ix; ++xx) {
        int ind = BottleUtils::getCellIndexFori3(xx, yy, zz);

        _pGData[ind]._tile[GridMeshLayer::e::Opaque] = pt->getTileIndex();
        _pGData[ind]._geom[GridMeshLayer::e::Opaque] = W25GEOM_SOLID;
        _pGData[ind]._tile[GridMeshLayer::e::Transparent] = pt->getTileIndex();
        _pGData[ind]._geom[GridMeshLayer::e::Transparent] = W25GEOM_EMPTY;
        //  for (int mat = 0; mat < MaxMatters; ++mat) {
              //W25Geom g = genGeom(xx, yy, zz, (MatterMode::e)mat);

         //     if(yy < -powf((float)xx, 2) + (float)iy  ){
          //         = W25GEOM_SOLID;
       //       }
       //       else {
       //           _pGData[ind]._geom[mat] = 0;
        //      }

        //  }
          //float f = Random::nextFloat(-1,1);
          //float f = 0.0f;// = genPointLocal(xx, yy, zz, (MatterMode::e)mat);
          //setDv(xx, yy, zz, f, (MatterMode::e)mat);
      }
    }
  }
}
//void WipGrid::generate() {
//
//    makeLandAndSea();
//   // walkers();
//    compile();
//}
void WipGrid::makeCell(int ix, int iy, int iz, WorldCell* __in_ pc) {
  int ind = BottleUtils::getCellIndexFori3(ix, iy, iz);
  GData gd = _pGData[ind];

  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    pc->setGeom((GridMeshLayer::e)iMatter, gd._geom[iMatter]);
  }
  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    pc->setTile((GridMeshLayer::e)iMatter, gd._tile[iMatter]);
  }

}
//void WipGrid::makeLandAndSea() {
//    int ix = (int)CongaUtils::getNumCellsWidth() + 1;
//    int iy = (int)CongaUtils::getNumCellsWidth() + 1;
//    int iz = (int)CongaUtils::getNumCellsWidth() + 1;
//
//    //Create both the sea and the land.
//    for (int zz = 0; zz < iz; ++zz) {
//    for (int yy = 0; yy < iy; ++yy) {
//    for (int xx = 0; xx < ix; ++xx) {
//    for(int mat=0; mat<MatterMode::e::MaxMatters; ++mat){
//        //float f = Random::nextFloat(-1,1);
//        float f = genPointLocal(xx, yy, zz, (MatterMode::e)mat);
//        setDv(xx, yy, zz, f, (MatterMode::e)mat);
//    }
//    }
//    }
//    }
//}

//void WipGrid::compile() { 
//    //Compiles the noise and features into the GData values.
//    int cx = (int)CongaUtils::getNumCellsWidth();
//    int cy = (int)CongaUtils::getNumCellsWidth();
//    int cz = (int)CongaUtils::getNumCellsWidth();
//
//    //DEBUG / TEST 
//    Tile25Spec* tileWater = _pWorld25->getMotionBucket()->getTileByName("t-water");
//    Tile25Spec* tileGrass= _pWorld25->getMotionBucket()->getTileByName("t-grass-swamp");
//    Tile25Spec* tileMud= _pWorld25->getMotionBucket()->getTileByName("t-mud");
//    AssertOrThrow2(tileWater != nullptr);
//    AssertOrThrow2(tileGrass != nullptr);
//    AssertOrThrow2(tileMud != nullptr);
//
//    for (int zz = 0; zz < cz; ++zz) {
//    for (int yy = 0; yy < cy; ++yy) {
//    for (int xx = 0; xx < cx; ++xx) {
//
//        int ind = CongaUtils::getCellIndexFori3(xx, yy, zz);
//
//        for (int mat = 0; mat<MatterMode::e::MaxMatters; ++mat) {
//            W25Geom g = genGeom(xx,yy,zz, (MatterMode::e)mat);
//        
//            _pGData[ind]._geom[mat] = g;
//
//        }
//
//        //Here we calculate "grass tops"
//        W25Geom g2, g3;
//        if (yy + 2 < CongaUtils::getNumCellsWidth()) {
//            g2 = genGeom(xx, yy + 2, zz, MatterMode::e::Solid);
//        }
//        else {
//            g2 = W25GEOM_SOLID;
//        }
//        if( yy + 1 < CongaUtils::getNumCellsWidth()){
//            g3 = genGeom(xx, yy+1, zz, MatterMode::e::Solid);
//        }
//        else { 
//            g3 = W25GEOM_SOLID; 
//        }
//
//        if(g3 == W25GEOM_EMPTY) {
//            _pGData[ind]._tile[MatterMode::e::Solid] = tileGrass->getTileIndex();
//        }
//        else if(g2 == W25GEOM_EMPTY && g3 != W25GEOM_EMPTY) {
//            _pGData[ind]._tile[MatterMode::e::Solid] = tileGrass->getTileIndex();
//        }
//        else {
//            _pGData[ind]._tile[MatterMode::e::Solid] = tileMud->getTileIndex();
//        }
//        _pGData[ind]._tile[MatterMode::e::Liquid] = tileWater->getTileIndex();
//
//    }
//    }
//    }
//
//}

//void WipGrid::setDv(int ix, int iy, int iz, float v, MatterMode::e eMatter) {
//    int ind = CongaUtils::getVertIndexFori3(ix, iy, iz);
//    AssertOrThrow2(ind >= 0 && ind < (int)_iNoiseArrSize);
//    _pNoiseValues[ind]._fValue[eMatter] = v;
//}
//float WipGrid::getDv(int ix, int iy, int iz, MatterMode::e eMatter) {
//    int ind = CongaUtils::getVertIndexFori3(ix, iy, iz);
//    AssertOrThrow2(ind >= 0 && ind < (int)_iNoiseArrSize);
//    return _pNoiseValues[ind]._fValue[eMatter];
//}

//float WipGrid::genPointLocal(int c_x, int c_y, int c_z, MatterMode::e eMatterMode){
//    float fRet;
//
//    DebugGridMode::e em = DebugGridMode::e::Disable;//_pWorld25->getConfig()->getDebugGridMode();
//    if (em == DebugGridMode::e::Disable) {
//        if(eMatterMode == MatterMode::e::Solid){
//            fRet = makeNoisePointSolid(c_x, c_y, c_z);
//        }
//        else if (eMatterMode == MatterMode::e::Liquid) {
//            fRet = makeNoisePointLiquid(c_x, c_y, c_z);
//        }
//        //else if (eMatterMode == MatterMode::e::Gas) {
//        //    fRet = makeNoisePointGas(c_x, c_y, c_z);
//        //}
//    }
//    else {
//        //if (eMatterMode == MatterMode::e::Liquid) {
//        //    return 1; // No water for debug mode
//        //}
//        //else {
//        //    //Debug Modes
//        //    if (em == DebugGridMode::e::Flat) {
//        //        //**Debug
//        //        vec3 tileOffset;
//        //        ivec3 ivCell(c_x, c_y, c_z);
//        //        tileOffset = CongaUtils::i3tov3Cell(_pGrid->getGridPos(), ivCell);
//        //        fRet = tileOffset.y;
//        //    }
//        //    else if (em == DebugGridMode::e::Random) {
//        //        fRet = ((float)(-10000 + rand() % 20000) * 0.0001f);
//        //    }
//        //    else if (em == DebugGridMode::e::Hill) {
//        //        //fRet = ((float)(-10000 + rand() % 20000) * 0.0001);
//        //        float a = (float)c_x / (float)CongaUtils::getNumCellsWidth();
//        //        float b = (float)c_y / (float)CongaUtils::getNumCellsWidth();
//        //        float c = (float)c_z / (float)CongaUtils::getNumCellsWidth();
//        //        fRet = ((c)-(b * a));
//        //        int nnn = 0;
//        //        nnn++;
//        //    }
//        //}
//        
//    }
//
//    return fRet;
//}
//float WipGrid::makeNoisePointLiquid(int xx, int yy, int zz) {
//    //Make the "sea"
//    float fy = _pGrid->getOriginR3().y + (float)yy * CongaUtils::getCellHeight();
//
//    if(fy < 0) { 
//        return -1;
//    }
//    else { 
//        return 1;
//    }
//}
//float World25Generator::makeNoisePointGas(int xx, int yy, int zz) {
//    //This is an odd function. It needs to be created by the generator.
//    return 1;
//}
//float WipGrid::makeNoisePointSolid(int xx, int yy, int zz) {
//
//    float f, pr;
//    //float factor;
//    vec3 vd;
//    float baseLine = 0;
//    float full = (float)CongaUtils::getNumCellsWidth() / 2 * CongaUtils::getCellHeight();
//
//    int octaves;
//    float persistence;
//    float offset;
//    float scale;
//
//    //**Topo
//  //  factor = 3.030141651f;//Offset from a perfect division
//  //  octaves = 4;
//  //  persistence = 1.67;
//  //  scale = = 0.002;
//    octaves = _pLair->getNoiseFunc()->_nOctaves;
//    offset = _pLair->getNoiseFunc()->_fOffset;
//    persistence = _pLair->getNoiseFunc()->_fPersistence;
//    scale = _pLair->getNoiseFunc()->_fScale;
//
//    vec3 tileOffset;
//    ivec3 ivCell(xx, yy, zz);
//    tileOffset = CongaUtils::i3tov3Cell(_viPos, ivCell);
//
//    vd = (tileOffset)* offset;
//
//    pr = terrain(vd.x, vd.y, vd.z, octaves, persistence, scale) ;
//
//    f = tileOffset.y - baseLine;
//    f += pr * full;
//
//
//    return f;
//}

//float WipGrid::getOrGenPoint(int g_ix, int g_iy, int g_iz, int c_ix, int c_iy, int c_iz, MatterMode::e eMatterMode) {
//    float fRet = 1;//Empty
//
//    if (c_ix < 0) g_ix -= 1;
//    if (c_iy < 0) g_iy -= 1;
//    if (c_iz < 0) g_iz -= 1;
//
//    if (c_ix >= CongaUtils::getNumCellsWidth()) g_ix += 1;
//    if (c_iy >= CongaUtils::getNumCellsWidth()) g_iy += 1;
//    if (c_iz >= CongaUtils::getNumCellsWidth()) g_iz += 1;
//
//    if ((c_ix < 0) || (c_ix >= CongaUtils::getNumCellsWidth()) ||
//        (c_iy < 0) || (c_iy >= CongaUtils::getNumCellsWidth()) ||
//        (c_iz < 0) || (c_iz >= CongaUtils::getNumCellsWidth()))
//    {
//        fRet = genPointLocal(c_ix, c_iy, c_iz, eMatterMode);
//    }
//    else
//    {
//        fRet = getDv(c_ix, c_iy, c_iz, eMatterMode);
//    }
//
//    return fRet;
//}
//W25Geom WipGrid::genGeom(int c_ix, int c_iy, int c_iz, MatterMode::e em) {
//    //Generate the topology for the given block
//    W25Geom geom = 0;
//    float val[8];
//    ivec3 gp = _viPos;
//    val[0] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix    , c_iy    , c_iz    , em);
//    val[1] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix + 1, c_iy    , c_iz    , em);
//    val[2] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix    , c_iy + 1, c_iz    , em);
//    val[3] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix + 1, c_iy + 1, c_iz    , em);
//    val[4] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix    , c_iy    , c_iz + 1, em);
//    val[5] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix + 1, c_iy    , c_iz + 1, em);
//    val[6] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix    , c_iy + 1, c_iz + 1, em);
//    val[7] = getOrGenPoint(gp.x, gp.y, gp.z, c_ix + 1, c_iy + 1, c_iz + 1, em);
//
//    for(int i=0; i<8; ++i){
//        if((val[i]<0)){
//        geom |= (1 << i);
//        }
//    }
//
//    return geom;
//}
//
//





}//ns Game
