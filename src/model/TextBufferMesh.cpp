#include "../model/TextBufferMesh.h"
#include "../gfx/GpuQuad3.h"
#include "../gfx/ShaderBase.h"
#include "../model/VaoDataGeneric.h"
#include "../model/MeshNode.h"
#include "../base/GLContext.h"
#include "../model/VertexFormat.h"

namespace Game {
;
TextBufferMesh::TextBufferMesh(std::shared_ptr<GLContext> c, int32_t count) : _pContext(c) {
    _iMaxQuads = count;
    _iCurrentQuadIndex = 0;
    allocateQuads(count);
}
TextBufferMesh::~TextBufferMesh()
{
    freeData();
}
//////////////////////////////////////////////////////////////////////////

void TextBufferMesh::draw(RenderParams& rp)
{
    rp.setCount(_iCurrentQuadIndex * 6);
    rp.setVaoGeneric(_pVaoData);
    rp.draw();
}
void TextBufferMesh::freeData()
{
    _verts.resize(0);
    _indexes.resize(0);
    _pVaoData = nullptr;
   // DEL_MEM(_pVaoData);
}
void TextBufferMesh::allocateQuads(int32_t count)
{
    freeData();

    v_v2c4x2 newV;
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

    assignIndexes();

    AssertOrThrow2(_pVaoData == nullptr);
    _pVaoData = std::make_shared<VaoDataGeneric>(_pContext, v_v2c4x2::getVertexFormat());

    copyToGpu( true);

    _pContext->chkErrDbg();

}
void TextBufferMesh::assignIndexes()
{
    AssertOrThrow2(_indexes.size() % 6 == 0);

    size_t iInd;
    size_t idx;
    for (iInd = 0, idx = 0; iInd<_indexes.size(); iInd += 6, idx += 4)
    {

        //Note:See also FontSpec::addQuad
        _indexes[iInd + 0] = (v_index32)(idx + 0); //Tri1 CCW
        _indexes[iInd + 1] = (v_index32)(idx + 3);
        _indexes[iInd + 2] = (v_index32)(idx + 1);
        _indexes[iInd + 3] = (v_index32)(idx + 0); //Tri2 CCW
        _indexes[iInd + 4] = (v_index32)(idx + 2);
        _indexes[iInd + 5] = (v_index32)(idx + 3);
    }
}
void TextBufferMesh::copyToGpu(bool bInit) {
    if (bInit) {
        _pVaoData->fillData(_verts.data(), _verts.size(), _indexes.data(), _indexes.size());
    }
    else {
        _pVaoData->fillData(_verts.data(), _iCurrentQuadIndex * 4, nullptr, 0);
    }
}
void TextBufferMesh::getQuad(v_v2c4x2*& v0, v_v2c4x2*& v1, v_v2c4x2*& v2, v_v2c4x2*& v3)
{
    v0 = &(_verts[_iCurrentQuadIndex * 4 + 0]);
    v1 = &(_verts[_iCurrentQuadIndex * 4 + 1]);
    v2 = &(_verts[_iCurrentQuadIndex * 4 + 2]);
    v3 = &(_verts[_iCurrentQuadIndex * 4 + 3]);
}


}//ns game
