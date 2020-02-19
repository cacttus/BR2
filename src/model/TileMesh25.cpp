#include "../base/GLContext.h"
#include "../base/OglErr.h"

#include "../gfx/GpuQuad3.h"
#include "../gfx/ShaderBase.h"

#include "../model/MeshNode.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"
#include "../model/IboData.h"

#include "../model/TileMesh25.h"
//#include "../bottle/World25Grid.h"
//#include "../bottle/CongaUtils.h"

namespace BR2 {
TileMesh25::TileMesh25(std::shared_ptr<GLContext> c, int32_t nQuads) : _pContext(c), _nQuads(nQuads) {
  _pVaoData = std::make_shared<VaoDataGeneric>(c, TileMeshVert::getVertexFormat());

  allocateData();
  copyToGpu(true);
}
TileMesh25::~TileMesh25() {
  _pVaoData = nullptr;
  //DEL_MEM(_pVaoData);
  freeData();
}
void TileMesh25::draw(RenderParams& rp) {
  rp.setCount(_iCurrentQuadIndex * 6);
  rp.setVaoGeneric(_pVaoData);
  rp.draw();//(getShader()->draw(, _iCurrentQuadIndex * 6);
}
void TileMesh25::freeData() {
  _verts.resize(0);
  _indexes.resize(0);
}
void TileMesh25::allocateData() {
  freeData();

  int nVerts = _nQuads * 4;
  int nIndexes = _nQuads * 6;

  _indexes.reserve(nIndexes);
  _verts.reserve(nVerts);

  for (int i = 0; i < nIndexes; ++i) {
    _indexes.push_back(0);
  }
  for (int i = 0; i < nVerts; i++) {
    _verts.push_back(TileMeshVert());
  }

  size_t iInd = 0;
  size_t vind = 0;
  for (int i = 0; i < _nQuads; ++i) {
    _indexes[iInd++] = (v_index32)vind + 0; //Tri1 CCW
    _indexes[iInd++] = (v_index32)vind + 1;
    _indexes[iInd++] = (v_index32)vind + 3;
    _indexes[iInd++] = (v_index32)vind + 0; //Tri2 CCW
    _indexes[iInd++] = (v_index32)vind + 3;
    _indexes[iInd++] = (v_index32)vind + 2;
    vind += 4;
  }

}
void TileMesh25::copyToGpu(bool bInit) {
  if (bInit) {
    _pVaoData->fillData(_verts.data(), _verts.size(), _indexes.data(), _indexes.size());
  }
  else {
    _pVaoData->fillData(_verts.data(), _iCurrentQuadIndex * 4, nullptr, 0);
  }
}
void TileMesh25::getQuad(TileMeshVert*& p0, TileMeshVert*& p1, TileMeshVert*& p2, TileMeshVert*& p3) {
  p0 = &_verts[_iCurrentQuadIndex * 4 + 0];
  p1 = &_verts[_iCurrentQuadIndex * 4 + 1];
  p2 = &_verts[_iCurrentQuadIndex * 4 + 2];
  p3 = &_verts[_iCurrentQuadIndex * 4 + 3];
}



}//ns BR2


 //int TileMesh25::offBL(int x, int y) {
 //    return vtxOff(x, y, 0);
 //}
 //int TileMesh25::offBR(int x, int y) {
 //    return vtxOff(x, y, 1);
 //}
 //int TileMesh25::offTL(int x, int y) {
 //    return vtxOff(x, y, 2);
 //}
 //int TileMesh25::offTR(int x, int y) {
 //    return vtxOff(x, y, 3);
 //}
 //size_t TileMesh25::vtxOff(int x, int y, int ind) {
 //    size_t iw = CongaUtils::getNumCellsWidth() * 4;
 //    size_t off = y * iw + x * 4 + ind;
 //    return off;
 //};
 //  AssertOrThrow2(_indexes.size() % 6 == 0);
 //Set verts
 // 4     5     6  ...    ^
 // 0     1     2   3 ... | Y
 // ---> 
 // size_t vind = 0;

 //vec3 origin = _pGrid->getOriginR3();
 //for (int j = 0; j < CongaUtils::getNumCellsWidth(); ++j) {
 //    for (int i = 0; i < CongaUtils::getNumCellsWidth(); ++i) {

 //        AssertOrThrow2((vind+4) <= _verts.size());

 //        _verts[vind + 0].v = origin + vec3( i      * CongaUtils::getCellWidth(),  j     * CongaUtils::getCellWidth(), 0);
 //        _verts[vind + 1].v = origin + vec3((i + 1) * CongaUtils::getCellWidth(),  j     * CongaUtils::getCellWidth(), 0);
 //        _verts[vind + 2].v = origin + vec3( i      * CongaUtils::getCellWidth(), (j + 1)* CongaUtils::getCellWidth(), 0);
 //        _verts[vind + 3].v = origin + vec3((i + 1) * CongaUtils::getCellWidth(), (j + 1)* CongaUtils::getCellWidth(), 0);
 //        _verts[vind + 0].c = vec3(0, 0, 0);
 //        _verts[vind + 1].c = vec3(0, 0, 0);
 //        _verts[vind + 2].c = vec3(0, 0, 0);
 //        _verts[vind + 3].c = vec3(0, 0, 0);
 //        _verts[vind + 0].viTile = ivec4(0, 0, 0, 0);
 //        _verts[vind + 1].viTile = ivec4(0, 0, 0, 0);
 //        _verts[vind + 2].viTile = ivec4(0, 0, 0, 0);
 //        _verts[vind + 3].viTile = ivec4(0, 0, 0, 0);

 //        vind +=4;
 //    }
 //}

 ////set indexes

 //for (int j = 0; j < CongaUtils::getNumCellsWidth(); ++j) {
 //    for (int i = 0; i < CongaUtils::getNumCellsWidth(); ++i) {

 //        AssertOrThrow2((iInd + 6) <= _indexes.size());



 //        vind +=4;
 //    }
 //}