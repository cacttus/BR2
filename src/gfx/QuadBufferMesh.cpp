#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../gfx/QuadBufferMesh.h"
#include "../gfx/GpuQuad3.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Texture2DSpec.h"
#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/VertexFormat.h"

namespace BR2 {
QuadBufferMesh::QuadBufferMesh(std::shared_ptr<GLContext> c, int32_t count) : _pContext(c) {
  allocateQuads(count);
  _iCurrentQuadIndex = 0;

  _iMaxQuads = count;

  //Make sure GpuQuad2 has no Vptr
  int a = sizeof(GpuQuad3);
  int b = sizeof(v_v3n3x2*) * 4;
  AssertOrThrow2(a == b);
}
QuadBufferMesh::~QuadBufferMesh() {
  freeData();
}
void QuadBufferMesh::setTexture(std::shared_ptr<Texture2DSpec> tex) {
  _pTexture = tex;
  ///_pMesh->setTexture(tex);
}
void QuadBufferMesh::draw(RenderParams& rp) {

  AssertOrThrow2(_pTexture != nullptr);
  _pTexture->bind(TextureChannel::e::Channel0, rp.getShader());
  rp.getShader()->draw(_pMesh, _iCurrentQuadIndex * 6);
}
void QuadBufferMesh::freeData() {
  _verts.resize(0);
  _indexes.resize(0);
  //DEL_MEM(_pVerts);
  //DEL_MEM(_pIndexes);

  //if(_pVaoData!=NULL)
  //    _pGpuBufferManager->deleteVao(_pVaoData);

 // _pVaoData=NULL;
}
void QuadBufferMesh::allocateQuads(int32_t count) {
  freeData();

  //    AssertOrThrow2(_pShader!=NULL);
     //_pVerts = new Allocator<QuadBufferVert>(count*4);
     //_pIndexes = new Allocator<t_index32>(count*6);

 //
  v_v3n3x2 newV;
  _indexes.reserve(count * 6);
  _verts.reserve(count * 4);
  for (int i = 0; i < count; ++i) {
    _indexes.push_back(0);
    _indexes.push_back(0);
    _indexes.push_back(0);
    _indexes.push_back(0);
    _indexes.push_back(0);
    _indexes.push_back(0);
    _verts.push_back(newV);
    _verts.push_back(newV);
    _verts.push_back(newV);
    _verts.push_back(newV);
  }

  //Fill with zero so we don't get any junk data.
  // QuadBufferMesh::QuadBufferVert v;
  // memset((void*)&v, 0, sizeof(v));
  // _pVerts->fill(v);
  // t_index32 zi = 0;
  // _pIndexes->fill(zi); 
  //
  // _pVertexBuffer->allocateOnly(_pVerts->count());
  // _pIndexBuffer->allocateOnly(_pIndexes->count());
  // _pVaoData = _pGpuBufferManager->createVao(QuadBufferVert::getType());
  // _pVaoData->init(_pVertexBuffer, _pShader);

  assignIndexes();

  _pMesh = MeshNode::create("QuadMesh",
    std::make_shared<MeshSpec>(_verts.data(), _verts.size(),
      _indexes.data(), _indexes.size(),
      v_v3n3x2::getVertexFormat(), nullptr)
  );

  copyToGpu(-1, true);
  _pContext->chkErrDbg();

}
void QuadBufferMesh::assignIndexes() {
  /*
      Quad Winding:
      CCW
      2
  v0    +-----------+ 1/5   v1
      | T0      / |
      |     /  T1 |
  v2    + /         + 4  v3
      0/3
      indexes: 2,1,0, 2,3,1
  */
  //**************NEW 20170328
  //Swapped this for LHS coords
  // New indexes
  //
  //
  //2......3 TR
  //0......1
  //BL
  //---->+x  ^+y

  AssertOrThrow2(_indexes.size() % 6 == 0);

  size_t iInd;
  size_t idx;
  for (iInd = 0, idx = 0; iInd < _indexes.size(); iInd += 6, idx += 4) {
    _indexes[iInd + 0] = (v_index32)(idx + 0); //Tri1 CCW
    _indexes[iInd + 1] = (v_index32)(idx + 1);
    _indexes[iInd + 2] = (v_index32)(idx + 3);
    _indexes[iInd + 3] = (v_index32)(idx + 0); //Tri2 CCW
    _indexes[iInd + 4] = (v_index32)(idx + 3);
    _indexes[iInd + 5] = (v_index32)(idx + 2);
  }
}
void QuadBufferMesh::copyToGpu(int32_t iQuadCount, bool bIndexes) {
  // AssertOrThrow2(_pVertexBuffer->getNumElements()==_pVerts->count());

 // int32_t quadCount = iQuadCount<0 ? (int32_t)_verts.size() / 4 : iQuadCount;
  int32_t quadCount = iQuadCount < 0 ? (int32_t)(_iCurrentQuadIndex * 4) : iQuadCount;

  //_pVertexBuffer->copyDataClientServer(quadCount * 4, _pVerts->ptr());

  std::vector<v_index32>* vi = nullptr;
  int iVi = 0;

  if (bIndexes) {
    vi = &_indexes;
    iVi = quadCount * 6;
    //AssertOrThrow2(_pIndexBuffer->getNumElements()==_pIndexes->count());
    //_pIndexBuffer->copyDataClientServer(quadCount*6, _pIndexes->ptr());
  }


  _pMesh->getMeshSpec()->allocMesh(_verts.data(), quadCount * 4, vi, iVi);
}
void QuadBufferMesh::getGpuQuad(GpuQuad3& q) {
  GpuQuad3* pq = &q;
  pq->_vertex[0] = &_verts[_iCurrentQuadIndex * 4 + 0];
  pq->_vertex[1] = &_verts[_iCurrentQuadIndex * 4 + 1];
  pq->_vertex[2] = &_verts[_iCurrentQuadIndex * 4 + 2];
  pq->_vertex[3] = &_verts[_iCurrentQuadIndex * 4 + 3];
}
void QuadBufferMesh::getQuad(v_v3n3x2*& v0, v_v3n3x2*& v1, v_v3n3x2*& v2, v_v3n3x2*& v3) {
  v0 = &_verts[_iCurrentQuadIndex * 4 + 0];
  v1 = &_verts[_iCurrentQuadIndex * 4 + 1];
  v2 = &_verts[_iCurrentQuadIndex * 4 + 2];
  v3 = &_verts[_iCurrentQuadIndex * 4 + 3];
}
//void QuadBufferMesh::createShader(t_string& srcName)
//{
//    _pShader = new GlslShaderBase(
//        BroSafeCast<GLShaderManager*>(_pShaderManager), 
//        BroSafeCast<GLRenderSystem*>(_pRenderSystem), 
//        QuadBufferVert::getType());
//
//    _pShader->addShaderSourceFile(TStr(srcName,".vs"));
//    _pShader->addShaderSourceFile(TStr(srcName,".ps"));
//    _pShader->setDefaultNameFromFileNames();
//    _pShader->init();
//}


}//ns game
