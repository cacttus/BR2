
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/Stopwatch.h"

#include "../bottle/W25MeshMaker.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../display/Atlas.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/W25Config.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/W25GridMesh.h"

namespace Game {

W25MeshMaker::W25MeshMaker(std::shared_ptr<GLContext> pContext, std::shared_ptr<Atlas> pAtlas) : 
    _pWorldAtlas(pAtlas), _pContext(pContext) 
{


}
W25MeshMaker::~W25MeshMaker() {
    /*for (int i = 0; i <= 0xFF; ++i) {
        DEL_MEM(_pConfigs[i]);
    }*/
}
void W25MeshMaker::init(){
    constructUnitBlock();
    constructTileConfigs();
}
void W25MeshMaker::constructUnitBlock() {


    //float g = w * 0.5f; //wall width;
    int bbl = 0, bbr = 1, btl = 2, btr = 3, tbl = 4, tbr = 5, ttl = 6, ttr = 7;
    
    //V8 indexes in LHS coordinates
    _vBlock[0] = vec3(0, 0, 0); //abl
    _vBlock[1] = vec3(1, 0, 0); //abr
    _vBlock[2] = vec3(0, 1, 0); //atl
    _vBlock[3] = vec3(1, 1, 0); //atr

    _vBlock[4] = vec3(0, 0, 1); //fbl
    _vBlock[5] = vec3(1, 0, 1); //fbr
    _vBlock[6] = vec3(0, 1, 1); //ftl
    _vBlock[7] = vec3(1, 1, 1); //ftr
}
void W25MeshMaker::constructTileConfigs() {
    //Initialize CConfigs
    // Each config is from 0 to 255.  Each of the bits in the
    //config corersponds to a solid corner.
    for (int i = 0; i <= 0xFF; ++i) {
        _pConfigs[i] = std::make_unique<W25MeshConf>(shared_from_this());
        _pConfigs[i]->construct(i);
    }
}
void W25MeshMaker::makeIdxTri(vec3& ta, vec3& tb, vec3& tc,//ta/tb are coplanar
    std::vector<W25MeshVert>& verts, std::vector<v_index32>& inds) {

    vec3 d1 = tc - ta;
    vec3 d2 = tb - ta;
    vec3 n = (d1.cross(d2)).normalize();

    int ic = verts.size();

    W25MeshVert vv[3];

    vv[0].n = n;
    vv[0].v = ta;


    vv[1].n = n;
    vv[1].v = tb;


    vv[2].n = n;
    vv[2].v = tc;

    verts.push_back(vv[0]);
    verts.push_back(vv[1]);
    verts.push_back(vv[2]);

    inds.push_back(ic + 0);
    inds.push_back(ic + 1);
    inds.push_back(ic + 2);
}
vec3& W25MeshMaker::getUnitBlockVertex(int i) {
    AssertOrThrow2(i >= 0 && i < 8);
    return _vBlock[i];
}
void W25MeshMaker::fillMeshByConfig(std::vector<W25MeshVert>& vecVertexes, std::vector<v_index32>& vecIndexes,
    std::shared_ptr<SpriteBucket> pBucket,
    std::shared_ptr<Atlas> pAtlas, WorldCell* pCell, GridMeshLayer::e eMatterMode, float fBlockWidth,
    float fBlockHeight, std::shared_ptr<W25Config> pConf, vec3* v3CustomBase) {


    if (pCell->getGeom(eMatterMode) != W25GEOM_EMPTY) {
        if (pConf->getRenderBlocks()) {
            addAsBlock(
                vecVertexes,
                vecIndexes,
                pBucket,
                pAtlas, pCell, eMatterMode,
                fBlockWidth, fBlockHeight, v3CustomBase);
        }
        else {
            addConfigTriangles(
                vecVertexes,
                vecIndexes,
                pBucket,
                pAtlas, pCell, eMatterMode,
                fBlockWidth, fBlockHeight, v3CustomBase);
        }
    }
}
void W25MeshMaker::addConfigTriangles(std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level,
    std::shared_ptr<SpriteBucket> pBucket,
    std::shared_ptr<Atlas> pa, WorldCell* gc, GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight, vec3* v3CustomBase)
{

    if (gc->getGeom(eMatterMode) == W25GEOM_EMPTY) {
        return;
    }
    vec3 vBase;
    if(v3CustomBase != nullptr) { 
        vBase = *v3CustomBase;
    }
    else {
        vBase = gc->getOriginR3();
    }
    vec2 xbl, xbr, xtl, xtr;
    W25Geom c_geom = gc->getGeom(eMatterMode);
    std::unique_ptr<W25MeshConf>& cc = getW25Config(c_geom);


    //Reset indexes
    gc->tryCreateVisibleCell();
    gc->getVisibleCell()->setMeshIndexCount(eMatterMode, 0);
    gc->getVisibleCell()->setMeshIndexOffset(eMatterMode, inds_level.size());

    //Loop over the 10 sides.  Remember, we have 4 extra sides for the crosses
    for(int iSide=0; iSide<W25MeshConf::c_nSides; ++iSide) {
        //TCoords is wrong! it is expecting a side.
        int32_t gpuTile = addAsTrianglesGetGpuTile(gc, iSide, eMatterMode, pBucket);

        if(eMatterMode == GridMeshLayer::e::Transparent){
            //Make sure transparent + opaque don't overlap
            W25Geom c_g_oapque = gc->getGeom(GridMeshLayer::e::Opaque);
            if(W25Config::hideTransparentSide(c_geom, c_g_oapque, iSide)){
                continue;
            }
        }

        addAsTriangles(gc,
            vBase, c_geom, iSide, eMatterMode, gpuTile,
            verts_level, inds_level, 
            fBlockWidth, fBlockHeight);
    }

}
void W25MeshMaker::addAsBlock(std::vector<W25MeshVert>& vecVertexes, std::vector<v_index32>& vecIndexes,
    std::shared_ptr<SpriteBucket> pBucket,
    std::shared_ptr<Atlas> pAtlas, WorldCell* pCell, GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight, vec3* v3CustomBase) {

    if(pCell->getGeom(eMatterMode) == W25GEOM_EMPTY) {
        return;
    }

    vec3 origin;
    if(v3CustomBase != nullptr) {
        origin = *v3CustomBase;
    }
    else {
        origin = pCell->getOriginR3();//don't add pCell->getOriginR3();
    }
    vec3 v[8];
    vec3 vwh(fBlockWidth, fBlockHeight, fBlockWidth);
    Box3f bb;
    pCell->getBoundBoxBvh(bb);
    vec3 bi = bb._min;
    vec3 ba = bb._max;

    pCell->tryCreateVisibleCell();
    pCell->getVisibleCell()->setMeshIndexOffset(eMatterMode, vecIndexes.size());
    

    v[0] = _vBlock[0] * vwh; //vec3(bi.x, bi.y, bi.z);
    v[1] = _vBlock[1] * vwh; //vec3(ba.x, bi.y, bi.z);
    v[2] = _vBlock[2] * vwh; //vec3(bi.x, ba.y, bi.z);
    v[3] = _vBlock[3] * vwh; //vec3(ba.x, ba.y, bi.z);
    v[4] = _vBlock[4] * vwh; //vec3(bi.x, bi.y, ba.z);
    v[5] = _vBlock[5] * vwh; //vec3(ba.x, bi.y, ba.z);
    v[6] = _vBlock[6] * vwh; //vec3(bi.x, ba.y, ba.z);
    v[7] = _vBlock[7] * vwh; //vec3(ba.x, ba.y, ba.z);
    
    //Hash32 imTop, imSide, imBot;

    //20170418 All cells must have a tile.
   // AssertOrThrow2(pCell->getTile(eMatterMode) != nullptr);

    Tile25Spec* ps = pBucket->getTileById(pCell->getTile(eMatterMode));
    int32_t idTop = ps->getTop()->getGpuIndex();
    int32_t idSide =ps->getSide()->getGpuIndex();
    int32_t idBot = ps->getBot()->getGpuIndex();

    if(addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gL, pCell, eMatterMode)) {
      addBlockQuad(v[4], v[0], v[6], v[2], vecVertexes, vecIndexes, pAtlas, idSide, origin, fBlockWidth, fBlockHeight);
    }
    if (addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gR, pCell, eMatterMode)) {
      addBlockQuad(v[1], v[5], v[3], v[7], vecVertexes, vecIndexes, pAtlas, idSide, origin, fBlockWidth, fBlockHeight);
    }
    if (addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gB, pCell, eMatterMode)) {
      addBlockQuad(v[4], v[5], v[0], v[1], vecVertexes, vecIndexes, pAtlas, idBot, origin, fBlockWidth, fBlockHeight);
    }
    if (addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gT, pCell, eMatterMode)) {
      addBlockQuad(v[2], v[3], v[6], v[7], vecVertexes, vecIndexes, pAtlas, idTop, origin, fBlockWidth, fBlockHeight);
    }
    if (addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gA, pCell, eMatterMode)) {
      addBlockQuad(v[0], v[1], v[2], v[3], vecVertexes, vecIndexes, pAtlas, idSide, origin, fBlockWidth, fBlockHeight);
    }
    if (addAsBlockIsEmptyNeighbor(PhysicsGridSide::e::gF, pCell, eMatterMode)) {
      addBlockQuad(v[5], v[4], v[7], v[6], vecVertexes, vecIndexes, pAtlas, idSide, origin, fBlockWidth, fBlockHeight);
    }

    int32_t iCount = vecIndexes.size() - pCell->getVisibleCell()->getMeshIndexOffset(eMatterMode);
    pCell->getVisibleCell()->setMeshIndexCount(eMatterMode, iCount);

}
bool W25MeshMaker::addAsBlockIsEmptyNeighbor(PhysicsGridSide::e eSide, WorldCell* pCell, GridMeshLayer::e eMatterMode) {
    bool b = false;
    b = b || (pCell->getNeighbor(eSide) == nullptr);
  //  b = b || (pCell->getNeighbor(eSide)->getTile(eMatterMode) == nullptr) ;
    b = b || (pCell->getNeighbor(eSide)->getGeom(eMatterMode) == W25GEOM_EMPTY);
    return b;
}
void W25MeshMaker::addBlockQuad(vec3& v0, vec3& v1, vec3& v2, vec3& v3,
    std::vector<W25MeshVert>& vecVertexes, std::vector<v_index32>& vecIndexes,
    std::shared_ptr<Atlas> pAtlas, int32_t iGpuIndex, vec3& origin, float fBlockWidth, float fBlockHeight) {

  //  std::shared_ptr<AtlasSprite> pSprite = pAtlas->getSprite(matId);
 //   vec2 xbl, xbr, xtl, xtr;
   // pSprite->getAtlas()->getTCoords(&(pSprite->_viGridPos), &xbl, &xbr, &xtl, &xtr);

    int32_t ivBase = (int32_t)vecVertexes.size();

    vec3 vn = ((v2 - v0).cross(v1 - v0)).normalize();

    W25MeshVert tv[4];


    tv[0].v = origin + v0;
    tv[0].n = vn;
    tv[0].ix.x = Gpu_BL;// = xbl;
    tv[0].ix.y = iGpuIndex;

    tv[1].v = origin + v1;
    tv[1].n = vn;
    tv[1].ix.x = Gpu_BR; //tv[1].x = xbr;
    tv[1].ix.y = iGpuIndex;

    tv[2].v = origin + v2;
    tv[2].n = vn;
    tv[2].ix.x = Gpu_TL; //xtl;
    tv[2].ix.y = iGpuIndex;

    tv[3].v = origin + v3;
    tv[3].n = vn;
    tv[3].ix.x = Gpu_TR; //;
    tv[3].ix.y = iGpuIndex;

    for(int i=0; i<4; ++i) {
        vecVertexes.push_back(tv[i]);
    }
    vecIndexes.push_back(ivBase + 0);
    vecIndexes.push_back(ivBase + 1);
    vecIndexes.push_back(ivBase + 3);

    vecIndexes.push_back(ivBase + 0);
    vecIndexes.push_back(ivBase + 3);
    vecIndexes.push_back(ivBase + 2);
}
W25Plane::e W25MeshMaker::getOppPlaneIndex(W25Plane::e iNeighbor) {
    if (iNeighbor == W25Plane::e::Left) { return  W25Plane::e::Right; }
    else if (iNeighbor == W25Plane::e::Right) { return  W25Plane::e::Left; }
    else if (iNeighbor == W25Plane::e::Top) { return  W25Plane::e::Bottom; }
    else if (iNeighbor == W25Plane::e::Bottom) { return  W25Plane::e::Top; }
    else if (iNeighbor == W25Plane::e::Back) { return  W25Plane::e::Front; }
    else if (iNeighbor == W25Plane::e::Front) { return  W25Plane::e::Back; }
    else if (iNeighbor == W25Plane::e::CrossA) { return  W25Plane::e::CrossD; }
    else if (iNeighbor == W25Plane::e::CrossB) { return  W25Plane::e::CrossC; }
    else if (iNeighbor == W25Plane::e::CrossC) { return  W25Plane::e::CrossB; }
    else if (iNeighbor == W25Plane::e::CrossD) { return  W25Plane::e::CrossA; }
    BroThrowNotImplementedException();
}
void W25MeshMaker::addAsTriangles(WorldCell* gc,
    vec3& vBase, W25Geom c_geom, int eSide, GridMeshLayer::e eMatterMode, int32_t iGpuTileId,
    std::vector<W25MeshVert>& verts_level, std::vector<v_index32>& inds_level,
    float fBlockWidth, float fBlockHeight
) {
    W25MeshVert vert[3];
    std::unique_ptr<W25MeshConf>&  cc = getW25Config(c_geom);
    vec3 vwh(fBlockWidth, fBlockHeight, fBlockWidth);

    if(cc->getVerts(eSide).size() == 0) {
        return;
    }

    int32_t iIndexCountForSide = 0;
    size_t vsiz = cc->getVerts(eSide).size();
    int vtxBase;
    int itri;
    bool bIsHidden;
    W25Sc::e sc;

    for(size_t vi=0; vi<vsiz; vi+=3) {
        vtxBase = verts_level.size();
        itri = vi / 3;
        bIsHidden = checkHiddenSide(gc, eSide, itri, eMatterMode);
   
        if(bIsHidden) {
            continue;
        }
   
        vert[0] = cc->getVerts(eSide)[vi + 0];
        vert[1] = cc->getVerts(eSide)[vi + 1];
        vert[2] = cc->getVerts(eSide)[vi + 2];
   
        vert[0].v *= vwh;
        vert[1].v *= vwh;
        vert[2].v *= vwh;
   
        vert[0].v += vBase;
        vert[1].v += vBase;
        vert[2].v += vBase;
   
        vert[0].ix.y = iGpuTileId;
        vert[1].ix.y = iGpuTileId;
        vert[2].ix.y = iGpuTileId;
   
   
        sc = cc->getTcConf(eSide, itri);
        if (sc == W25Sc::e::A) {
            vert[0].ix.x = Gpu_BL;
            vert[1].ix.x = Gpu_BR;
            vert[2].ix.x = Gpu_TL;
        }
        else if (sc == W25Sc::e::B) {
            vert[0].ix.x = Gpu_BL;
            vert[1].ix.x = Gpu_BR;
            vert[2].ix.x = Gpu_TR;
        }
        else if (sc == W25Sc::e::C) {
            vert[0].ix.x = Gpu_BL;
            vert[1].ix.x = Gpu_TR;
            vert[2].ix.x = Gpu_TL;
        }
        else if (sc == W25Sc::e::D) {
            vert[0].ix.x = Gpu_BR;
            vert[1].ix.x = Gpu_TR;
            vert[2].ix.x = Gpu_TL;
        }
        else if (sc == W25Sc::e::Empty) {
            //empty
        }
   
        verts_level.push_back(vert[0]);
        verts_level.push_back(vert[1]);
        verts_level.push_back(vert[2]);
   
        //Indexes are off.  If we skip tris, etc.  For now just add them as "triangles"
        inds_level.push_back(vtxBase + 0);//cc->getIndexes(eSide)[itri * 3 + 0]);
        inds_level.push_back(vtxBase + 1);//cc->getIndexes(eSide)[itri * 3 + 1]);
        inds_level.push_back(vtxBase + 2);//cc->getIndexes(eSide)[itri * 3 + 2]);
   
        iIndexCountForSide += 3;
   
    }
    //Note:prevIndCount because we're topologizing each *side* so we need to addt he index count
    int32_t prevIndCount = gc->getVisibleCell()->getMeshIndexCount(eMatterMode);
    gc->getVisibleCell()->setMeshIndexCount(eMatterMode, iIndexCountForSide + prevIndCount);
    
}
bool W25MeshMaker::checkHiddenSide(WorldCell* gc, int iSide, int iTri, GridMeshLayer::e eMatterMode) {
    AssertOrThrow2(gc!=nullptr);
    bool bIsHidden = false;
      
    //Get neighbor
    //W25MeshConf* cc_n = nullptr;
    WorldCell* gn = nullptr;
    //W25Sc::e nsc = W25Sc::e::Empty;
    W25Plane::e opp = getOppPlaneIndex((W25Plane::e)iSide);
    std::shared_ptr<WorldGrid> gridSide;

    if (iSide < 6) { // sides, no crosses
        gn = gc->getNeighbor((PhysicsGridSide::e)iSide);

        //*Check for border cell.  If the neighbor glob isn't generated don't create a side.
        if (gn == nullptr) {
            //20170602 - With 2-stage grid generation we now don't have to worry about "seams" Yay.
            //The only way we will get here is if the neighbor node was consolidated, in which case the neighbor is empty. Thus we show this side.
            return false;
        }
    }
    else {
        //we are doing crosses in the same cell
        gn = gc;
    }

    if(gn == nullptr) {
        //This shouldn't happen.  Inter-cells should all be linked.
        //We take care of edge cells (neighbor grids) above.
        //error in linking somewhere
        //**Unless we implement consolidation.  Then fix this
        //int nnn = 0; 
        //nnn++;
        //Gu::debugBreak();

        //*This is a consolidated node, ignore.
        return false;
    }

    std::unique_ptr<W25MeshConf>&  gn_c = getW25Config(gn->getGeom(eMatterMode));
    std::unique_ptr<W25MeshConf>&  gc_c = getW25Config(gc->getGeom(eMatterMode));

    int c_nt = gc_c->numTrisSide(iSide);
    int n_nt = gn_c->numTrisSide(opp);

    //Plane check
    if(c_nt==2 && n_nt==2) {
        if(
            gc_c->getPConf(iSide, 0) == true &&
            gc_c->getPConf(iSide, 1) == true &&
            gn_c->getPConf(opp, 0) == true &&
            gn_c->getPConf(opp, 1) == true 
            )
        {
            return true;
        }
    }

    //Only planar sides can be culled.
    bool c_planar = gc_c->getPConf(iSide, iTri);
    if(c_planar == false){
        return false;
    }

    W25Sc::e c_conf = gc_c->getTcConf(iSide, iTri);
    W25Sc::e n_c[2];
    bool n_p[2];
    n_c[0] = n_c[1] = W25Sc::e::Empty;
    n_p[0] = n_p[1] = false;

    if (n_nt == 1) {
        n_c[0] = gn_c->getTcConf(opp, 0);
        n_p[0] = gn_c->getPConf(opp, 0);
    }
    else if (n_nt == 2) {
        n_c[0] = gn_c->getTcConf(opp, 0);
        n_p[0] = gn_c->getPConf(opp, 0);
        n_c[1] = gn_c->getTcConf(opp, 1);
        n_p[1] = gn_c->getPConf(opp, 1);
    }

    for(int i=0; i<n_nt; ++i) {
        W25Sc::e oppConf = getOppSideConf(n_c[i]);
        bool oppPlan = n_p[i];
        if(oppConf == c_conf && n_p[i] == true) {
            if(iSide == (W25Plane::e::Top) && gc_c->getGeomId()==0xff){
                int nnn=0;
                nnn++;
            }
            bIsHidden = true;
            break;
        }
    }

    return bIsHidden;
}
int32_t W25MeshMaker::addAsTrianglesGetGpuTile(WorldCell* gc, int iSide, GridMeshLayer::e eMatterMode, std::shared_ptr<SpriteBucket> pBucket) {
   // Hash32 imTop, imSide, imBot;
   // std::shared_ptr<AtlasSprite> ps = nullptr;
    Tile25Spec* pTile = pBucket->getTileById(gc->getTile(eMatterMode));
    
    //Blocks must have tiles.
    AssertOrThrow2(pTile != nullptr);

    //Get sprtie for side
    if ((PhysicsGridSide::e)iSide == PhysicsGridSide::e::gT) {
        return pTile->getTop()->getGpuIndex();
    }
    else if ((PhysicsGridSide::e)iSide == PhysicsGridSide::e::gB) {
        return pTile->getBot()->getGpuIndex();
    }
    else {
        return pTile->getSide()->getGpuIndex();
    }
}
W25Geom W25MeshMaker::confBit3(int iBlockVert, int s1, int s2, int s3, float(&kernel)[27]) {
    //rETURNS A CONFIGURATION FOR THE GIVEN SET BITS.
    AssertOrThrow2(iBlockVert >= 0 && iBlockVert < 8);

    W25Geom ret;
    bool b = true;

    b = b && (kernel[s1] < 0);
    b = b && (kernel[s2] < 0);
    b = b && (kernel[s3] < 0);

    ret = (W25Geom)((int)b << iBlockVert);

    return ret;
}
W25Geom W25MeshMaker::confBit9(int iBlockVert,
    int s1, int s2, int s3,
    int s4, int s5, int s6,
    int s7, int s8, int s9,
    float(&kernel)[27]) {
    //rETURNS A CONFIGURATION FOR THE GIVEN SET BITS.
    AssertOrThrow2(iBlockVert >= 0 && iBlockVert < 8);

    W25Geom ret;
    bool b0 = true, b1 = true, b2 = true;

    b0 = b0 && (kernel[s1] < 0);
    b0 = b0 && (kernel[s2] < 0);
    b0 = b0 && (kernel[s3] < 0);

    b1 = b1 && (kernel[s4] < 0);
    b1 = b1 && (kernel[s5] < 0);
    b1 = b1 && (kernel[s6] < 0);

    b2 = b2 && (kernel[s7] < 0);
    b2 = b2 && (kernel[s8] < 0);
    b2 = b2 && (kernel[s9] < 0);

    ret = (W25Geom)((int)(b0 || b1 || b2) << iBlockVert);

    return ret;
}
W25Geom W25MeshMaker::confBit6m(int iBlockVert,
    int s1, int s2, int s3,
    int s4, int s5, int s6,
    float(&kernel)[27]) {
    //rETURNS A CONFIGURATION FOR THE GIVEN SET BITS.
    AssertOrThrow2(iBlockVert >= 0 && iBlockVert < 8);

    W25Geom ret;
    bool b0 = true, b1 = true;

    b0 = b0 && (kernel[s1] < 0);
    b0 = b0 && (kernel[s2] < 0);
    b0 = b0 && (kernel[s3] < 0);

    b1 = b1 && (kernel[s4] < 0);
    b1 = b1 && (kernel[s5] < 0);
    b1 = b1 && (kernel[s6] < 0);

    ret = (W25Geom)((int)(b0 || b1) << iBlockVert);

    return ret;
}
//////////////////////////////////////////////////////////////////////////
std::map<int32_t, vec3> W25MeshConf::_posMap;

W25MeshConf::W25MeshConf(std::shared_ptr<W25MeshMaker> pm) : _pMeshMaker(pm) {
}
W25MeshConf::~W25MeshConf(){
}
void W25MeshConf::raycastTris(Ray_t* ra, vec3& vOrigin, float fCellWidth, float fCellHeight) {
    PlaneHit ph;
    RaycastHit rhTemp;

    ra->setTime(FLT_MAX);
    vec3 twh(fCellWidth, fCellHeight, fCellWidth);

    for (int iSide = 0; iSide < c_nSides; ++iSide) {
        for (size_t iInd = 0; iInd < _indexes[iSide].size(); iInd += 3) {
            vec3 nn = _verts[iSide][_indexes[iSide][iInd]].n;
            vec3 v0 = vOrigin + _verts[iSide][_indexes[iSide][iInd + 0]].v * twh;
            vec3 v1 = vOrigin + _verts[iSide][_indexes[iSide][iInd + 1]].v * twh;
            vec3 v2 = vOrigin + _verts[iSide][_indexes[iSide][iInd + 2]].v * twh;
            float d = -nn.dot(v0);

            TriPlane tp(v0, v1, v2, nn, d);

            ph = tp.hitTest(ra->getBegin(), ra->getEnd());
            if (ph.getIsContained()) {
                if (ph.getHitTime() < ra->getTime()) {
                    ra->setTime(ph.getHitTime());
                    ra->setNormal(nn);
                }
            }

        }
    }
}
bool W25MeshConf::getCornerVal(int iCorner) {
    AssertOrThrow2(iCorner < 8);
    int c = ((int)_iGeomId & (1 << iCorner));
    return  c > 0;
}
void W25MeshConf::construct(W25Geom iConfig) {
    _iGeomId = iConfig;

    makeMesh();
    makePlaces();
}
void W25MeshConf::makePlaces(){
    if (_posMap.size() == 0) {
        buildPosMap();
    }



}
void W25MeshConf::buildPosMap() {
    int n = 0;
    vec3 v;
    float cw = BottleUtils::getCellWidth();
    float ch = BottleUtils::getCellHeight();

    //Do the vertex grid 0-26
    for (int zi = 0; zi < 3; ++zi) {
    for (int yi = 0; yi < 3; ++yi) {
    for (int xi = 0; xi < 3; ++xi) {
        v.z = (float)zi * cw * 0.5f;
        v.y = (float)yi * ch * 0.5f;
        v.x = (float)xi * cw * 0.5f;
        _posMap.insert(std::make_pair(n, v));
        n++;
    }
    }
    }

    //Do the quad gri
    /*
    24 of these
    for each side -x, +x, -y +y, -z +z
    +---------+
    | .2 | .3 |
    +---------+
    | .0 | .1 |
    +---------+
    */
    vec3 b(cw / 3.0f, ch / 3.0f, cw / 3.0f);
    _posMap.insert(std::make_pair(n++, vec3( 0, b.y * 1, b.z * 2))); // -x
    _posMap.insert(std::make_pair(n++, vec3( 0, b.y * 1, b.z * 1)));
    _posMap.insert(std::make_pair(n++, vec3( 0, b.y * 2, b.z * 2)));
    _posMap.insert(std::make_pair(n++, vec3( 0, b.y * 2, b.z * 1)));

    _posMap.insert(std::make_pair(n++, vec3(cw, b.y * 1, b.z * 1))); // +x
    _posMap.insert(std::make_pair(n++, vec3(cw, b.y * 1, b.z * 2)));
    _posMap.insert(std::make_pair(n++, vec3(cw, b.y * 2, b.z * 1)));
    _posMap.insert(std::make_pair(n++, vec3(cw, b.y * 2, b.z * 2)));

    _posMap.insert(std::make_pair(n++, vec3(b.x * 1,  0, b.z * 2))); // -y
    _posMap.insert(std::make_pair(n++, vec3(b.x * 1,  0, b.z * 1)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2,  0, b.z * 2)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2,  0, b.z * 1)));

    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, ch, b.z * 1))); // +y
    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, ch, b.z * 2)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, ch, b.z * 1)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, ch, b.z * 2)));

    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, b.y * 1,  0))); // -z
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, b.y * 1,  0)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, b.y * 2,  0)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, b.y * 2,  0)));

    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, b.y * 1, cw))); // +z
    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, b.y * 1, cw)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 2, b.y * 2, cw)));
    _posMap.insert(std::make_pair(n++, vec3(b.x * 1, b.y * 2, cw)));

}
void W25MeshConf::makeMesh() {
    bool hill = true; //Hill or Valley determines how the triangle backs Pop up or Sinkd own
    int ti[4];  // v8 indexes of the Top Vertesess
    int bi[4];  // v8 Indexes of the Bottom vertexes
    bool vt[4]; // Values of the top
    bool vb[4]; // Values of bottm
    bool s[4];  // Supports 1 = value, 0 = nothing
    bool h[4];  // Heights, 1 = top, 0 = bottom
    int t0[3];  // Triangle indexes in v4, later converted to v8
    int t1[3];
    int cx[4]; //Cross v indexes
    W25Sc::e t0tc; //triangle configuration
    W25Sc::e t1tc;
    W25Sc::e t0vc = W25Sc::e::A;
    W25Sc::e t1vc = W25Sc::e::A;
    /*
    boolean cube indexes
    6  7
    ^  2  3   4  5
    +y|  0  1          +z
    -->+x        /
    */
    bi[0] = 0; bi[1] = 1; bi[2] = 4; bi[3] = 5; // Bottom V8 Indexes
    ti[0] = 2; ti[1] = 3; ti[2] = 6; ti[3] = 7; // Top

                                                //cx[0] = cx[1] = cx[2] = cx[3] = -1;
    t0[0] = t0[1] = t0[2] = -1;
    t1[0] = t1[1] = t1[2] = -1;

    vt[0] = getCornerVal(ti[0]);//t/f values of the 8 point boolean cube
    vt[1] = getCornerVal(ti[1]);
    vt[2] = getCornerVal(ti[2]);
    vt[3] = getCornerVal(ti[3]);

    vb[0] = getCornerVal(bi[0]);
    vb[1] = getCornerVal(bi[1]);
    vb[2] = getCornerVal(bi[2]);
    vb[3] = getCornerVal(bi[3]);

    //Supports
    s[0] = (vt[0] || vb[0]);
    s[1] = (vt[1] || vb[1]);
    s[2] = (vt[2] || vb[2]);
    s[3] = (vt[3] || vb[3]);

    //Heights
    h[0] = (vt[0]);
    h[1] = (vt[1]);
    h[2] = (vt[2]);
    h[3] = (vt[3]);

#define T_0(a,b,c, cf) t0[0] = a; t0[1] = b; t0[2] = c; t0tc = cf;
#define T_1(a,b,c, cf) t1[0] = a; t1[1] = b; t1[2] = c; t1tc = cf;
    //#define CX_0(a,b,c,d) cx[0] = a; cx[1] = b; cx[2] = c; cx[3] = d;

    //cross config( single tri only) + is normal
    //CrossA  |\+   + is normal
    //CrossB  +/|
    //CrossC  |/+
    //CrossD  +\|

    //Check for the "config tris".  we only have a config tri when we have no vertexes on our plane (top/bot) and we have
    //less than 4 "on" verts below (or above).  If we don't have a config tri then the topology is a solid block.
    if (!s[0] && !s[1] && !s[2] && !s[3]) {
        //No value.  Empty
    }
    else if (s[0] && (!s[1] && !s[2] && !s[3])) {
        T_0(0, 1, 2, W25Sc::e::A);
    }
    else if (s[1] && (!s[0] && !s[2] && !s[3])) {
        T_0(1, 3, 0, W25Sc::e::B);
    }
    else if (s[2] && (!s[0] && !s[1] && !s[3])) {
        T_0(2, 0, 3, W25Sc::e::C);
    }
    else if (s[3] && (!s[0] && !s[1] && !s[2])) {
        T_0(3, 2, 1, W25Sc::e::D);
    }
    else {
        // Are both tris present?
        //Both tris present. Determine the backing (hill or valley).
        //THere are only 2 configurations that require us to distinguish backing.  All others are in-plane.
        if (h[0] && h[3]) {
            if (hill) {
                T_0(0, 1, 3, W25Sc::e::B);
                T_1(0, 3, 2, W25Sc::e::C);
            }
            else {
                T_0(0, 1, 2, W25Sc::e::A);
                T_1(1, 3, 2, W25Sc::e::D);
            }
        }
        else if (h[1] && h[2]) {
            if (hill) {
                T_0(0, 1, 2, W25Sc::e::A); // These configs are the above ones swapped for hill/vally
                T_1(1, 3, 2, W25Sc::e::D);
            }
            else {
                T_0(0, 1, 3, W25Sc::e::B);
                T_1(0, 3, 2, W25Sc::e::C);
            }
        }
        else {
            //All are in-plane. Use first config
            if (hill) {
                T_0(0, 1, 3, W25Sc::e::B);
                T_1(0, 3, 2, W25Sc::e::C);
            }
            else {
                T_0(0, 1, 2, W25Sc::e::A);
                T_1(1, 3, 2, W25Sc::e::D);
            }
        }
    }

    //if (
    //    _iGeomId == 0x54 
    //   || _iGeomId == 0x01
    //_iGeomId == 0x40
    //_iGeomId == 0x20
    //if(/*_iGeomId == 0x72 ||*/ _iGeomId == 0x34) {
    //    int nnn = 0;
    //    nnn++;
    //}

    addCapTris(t0, t0tc, vt, vb, ti, bi);
    addCapTris(t1, t1tc, vt, vb, ti, bi);
}
void W25MeshConf::addCapTris(int (&t0)[3], //triangle v4 indexes (top)
    W25Sc::e t0tc, //Texture configuration (top)
    bool (&vt)[4], //On/Off values for the v4 top
    bool (&vb)[4], //on/off for v4 bot
    int (&ti)[4], //v4 index into v8
    int (&bi)[4] //v4 index into v8
){
    int nt[3]; //Final v8 indexes, Stored for debugging
    int nb[3]; //Final v8 indexes, Stored for debugging
    if (t0[0] != -1 && t0[1] != -1 && t0[2] != -1) {
        //Height conversion. 
        //NOTE: the n[x] indexes are the V8 indexes and are of a different from the ti/bi[x] ordering.
        bool in_plane_both = (vt[0] && vb[0]) || (vt[1] && vb[1]) || (vt[2] && vb[2]) || (vt[3] && vb[3]);
        bool in_plane_top = in_plane_both || vt[t0[0]] || vt[t0[1]] || vt[t0[2]];
        bool in_plane_bot = in_plane_both || vb[t0[0]] || vb[t0[1]] || vb[t0[2]];

        //We stick the same plane if any verts are on our side.
        //Top
        if (in_plane_top) {
            //We are in-plane.  Only make the plane on this cap side. Don't use vertexes from the other side.
            nt[0] = ti[t0[0]];
            nt[1] = ti[t0[1]];
            nt[2] = ti[t0[2]];
        }
        else {
            //Make a Ramp, Corner, or fold.
            nt[0] = vb[t0[0]] ? ti[t0[0]] : bi[t0[0]];
            nt[1] = vb[t0[1]] ? ti[t0[1]] : bi[t0[1]];
            nt[2] = vb[t0[2]] ? ti[t0[2]] : bi[t0[2]];
        }
        addConfigTriangle(W25Plane::Top, nt[0], nt[1], nt[2], t0tc, in_plane_top);

        //**Add the opposing bottom triangle.  We always have a bottom triangle with the "blanket blocks "
        if (in_plane_bot) {
            nb[0] = bi[t0[0]];
            nb[1] = bi[t0[1]];
            nb[2] = bi[t0[2]];
        }  
        else {
            nb[0] = vt[t0[0]] ? bi[t0[0]] : ti[t0[0]];
            nb[1] = vt[t0[1]] ? bi[t0[1]] : ti[t0[1]];
            nb[2] = vt[t0[2]] ? bi[t0[2]] : ti[t0[2]];
        }
        //Note!: 0, 2, 1 - we flip these for teh bottom
        addConfigTriangle(W25Plane::Bottom, nb[0], nb[2], nb[1], W25MeshMaker::getOppSideConf(t0tc), in_plane_bot);

        doSideTris(t0[0], t0[1], vt, vb, ti, bi, in_plane_top && in_plane_bot);
        doSideTris(t0[1], t0[2], vt, vb, ti, bi, in_plane_top && in_plane_bot);
        doSideTris(t0[2], t0[0], vt, vb, ti, bi, in_plane_top && in_plane_bot);
    }

}
void W25MeshConf::doSideTris(int t00, int t01, bool (&vt)[4], bool (&vb)[4], int(&ti)[4], int(&bi)[4], bool in_plane) {
    //Sides
    //Since all triangles wind CCW we can be sure this order will work
    //Input: t0, t1, vb, vt
    bool sv[4];//values
    int si[4];// v8 indexes
    W25Sc::e stvc;//tex config AND vis config
    //W25Sc::e svc;//vis config these should equal
    //   A
    //   +
    //   | \
    //   +---+
    //   B
    //       +
    //     / |
    //   +---+
    //   C
    //   +---+
    //   | /
    //   +
    //   D
    //   +---+
    //     \ |
    //       +
    //CrossA  |\+   + is normal
    //CrossB  +/|
    //CrossC  |/+
    //CrossD  +\|
    sv[0] = vb[t00];
    sv[1] = vb[t01];
    sv[2] = vt[t00];
    sv[3] = vt[t01];

    si[0] = bi[t00];
    si[1] = bi[t01];
    si[2] = ti[t00];
    si[3] = ti[t01];

    W25Plane::e ePlane;
    if (si[0] == 0 && si[1] == 1) { ePlane = W25Plane::e::Back; }
    else if (si[0] == 1 && si[1] == 5) { ePlane = W25Plane::e::Right; }
    else if (si[0] == 5 && si[1] == 4) { ePlane = W25Plane::e::Front; }
    else if (si[0] == 4 && si[1] == 0) { ePlane = W25Plane::e::Left; }

    else if (si[0] == 1 && si[1] == 4) { ePlane = W25Plane::e::CrossA; }
    else if (si[0] == 4 && si[1] == 1) { ePlane = W25Plane::e::CrossD; }
    else if (si[0] == 0 && si[1] == 5) { ePlane = W25Plane::e::CrossC; }
    else if (si[0] == 5 && si[1] == 0) { ePlane = W25Plane::e::CrossB; }
    else {
        Gu::debugBreak(); //impossible
    }

    //We should be able to handle just the triangle cases, then the "no edge" cases.
    //There shouldn't be any empty configs unless the b == t
    if(!sv[0] && !sv[1] && !sv[2] && !sv[3] && in_plane == false){
        //empty/
    }
    else if (sv[0] && !sv[1] && !sv[2] && !sv[3] && in_plane == false) {
        addConfigTriangle(ePlane, si[2], si[0], si[1], W25Sc::e::A, true);
    }
    else if (!sv[0] && sv[1] && !sv[2] && !sv[3] && in_plane == false) {
        addConfigTriangle(ePlane, si[0], si[1], si[3], W25Sc::e::B, true);
    }
    else if (!sv[0] && !sv[1] && sv[2] && !sv[3] && in_plane == false) {
        //Changed from 1,2,3 for pattern 0x40
        addConfigTriangle(ePlane, si[3], si[2], si[0], W25Sc::e::C, true);
    }
    else if (!sv[0] && !sv[1] && !sv[2] && sv[3] && in_plane == false) {
        //Changed from 0,3,2 for pattern 0x40
        addConfigTriangle(ePlane, si[1], si[3], si[2], W25Sc::e::D, true);
    }
    else {
        //Quad
        //TODO: we need to check for cross visibility here.
        addConfigTriangle(ePlane, si[0], si[1], si[3], W25Sc::e::B, true);
        addConfigTriangle(ePlane, si[0], si[3], si[2], W25Sc::e::C, true);
    }
}
void W25MeshConf::addConfigTriangle(int side, int i0, int i1, int i2, W25Sc::e eTcConf, bool bPlanar) {
    _eTcConf[side].push_back(eTcConf);
    _bPlanar[side].push_back(bPlanar);


    //2/27/18
    //In order to do the grid shader GS, vertexes need to be wound in the correct order with the first 3 vertexes creating "legs"
    //Config is HARD So i'm not going to mess with that!! 
    //So here - find hypotenuse, if hypotenuse is greater than legs, then swap the indexes\
        //This messes up textures, TODO: fix textures.
    int tmp;
    vec3 v0 = _pMeshMaker->getUnitBlockVertex(i0);
    vec3 v1 = _pMeshMaker->getUnitBlockVertex(i1);
    vec3 v2 = _pMeshMaker->getUnitBlockVertex(i2);

    //we want i0, i2 to be the hypotenuse
    float d0 = (v1 - v0).length2();
    float d1 = (v2 - v0).length2();
    float d2 = (v1 - v2).length2();

    if (d2 >= d1 && d2 >= d0) {
        //i1, i2 is hypotenuse, shift back
        tmp = i2;
        i2 = i1;
        i1 = i0;
        i0 = tmp;
    }
    else if (d1 >= d2 && d1 >= d0) {
        //i0, i2 is hypotenuse, i0,i1, and i1, i2 are legs
        //all good
    }
    else if (d0 >= d1 && d0 >= d2) {
        //i1 i0 is hypotenuse, shift forward
        tmp = i0;
        i0 = i1;
        i1 = i2;
        i2 = tmp;
    }
    //


    _pMeshMaker->makeIdxTri(
        _pMeshMaker->getUnitBlockVertex(i0),
        _pMeshMaker->getUnitBlockVertex(i1),
        _pMeshMaker->getUnitBlockVertex(i2),
        _verts[side],
        _indexes[side]
    );

    _nTotalVerts += 3;
}
W25Sc::e W25MeshMaker::getOppSideConf(W25Sc::e c) {
    if (c == W25Sc::e::A) { return W25Sc::e::B; }
    else if (c == W25Sc::e::B) { return W25Sc::e::A; }
    else if (c == W25Sc::e::C) { return W25Sc::e::D; }
    else if (c == W25Sc::e::D) { return W25Sc::e::C; }
    return c;

}
size_t W25MeshMaker::getNumVertsForGeom(W25Geom g){
    std::unique_ptr<W25MeshConf>&  conf = getW25Config(g);
    if(conf != nullptr) { 
        return conf->getNumTotalVerts();
    }

    return 0;
}
void W25MeshMaker::redoMeshForCellNoGpuCopy(
    std::shared_ptr<W25GridMesh> pMesh, std::shared_ptr<Atlas> pAtlas, WorldCell* pCell, std::shared_ptr<SpriteBucket> pBucket,
    GridMeshLayer::e eMatterMode, float fBlockWidth, float fBlockHeight,
    std::shared_ptr<W25Config> pConf, std::shared_ptr<WorldGrid> pGrid, bool bAsync)
{
    //optimization for meshes to redo just one cell's mesh.
    //we will have to update all the indexes for all the subsequent cells.
    
    int32_t iOldCount = -1;
    int32_t iOldOff = -1;

    //Erase the current cell from the mesh.
    if(pCell->getVisibleCell() != nullptr) {
        //Slight optimization.  A lot of cells have no old count.
        //We won't need to update any indexes if this is null
        iOldCount = pCell->getVisibleCell()->getMeshIndexCount(eMatterMode);
        iOldOff  = pCell->getVisibleCell()->getMeshIndexOffset(eMatterMode);

        if (iOldCount > 0) {

            //Update indices
            for (size_t iInd = iOldOff; iInd < pMesh->getIndexes().size(); ++iInd) {
                pMesh->getIndexes()[iInd] -= iOldCount;
                if (pMesh->getIndexes()[iInd] < 0) {
                    Gu::debugBreak();//Bad boy
                    pMesh->getIndexes()[iInd] = 0;
                }
            }

            //Just remove them.  Then add new verts/inds to the end. 
            pMesh->getVerts().erase(pMesh->getVerts().begin() + iOldOff, pMesh->getVerts().begin() + iOldOff + iOldCount);
            pMesh->getIndexes().erase(pMesh->getIndexes().begin() + iOldOff, pMesh->getIndexes().begin() + iOldOff + iOldCount);
            
  
            //update the indexes per cell, wht this does, is it is really an optimization for single-cell edit so we don't have to
            //redo the whole mesh
            pGrid->deflateCellIndexes(eMatterMode, iOldOff, iOldCount, pCell);


        }
    }

    //Add new verts to the cell. 
   fillMeshByConfig(pMesh->getVerts(), pMesh->getIndexes(), pBucket, pAtlas, pCell, eMatterMode, 
       fBlockWidth, fBlockHeight, pConf, nullptr);

}




}//ns Game
