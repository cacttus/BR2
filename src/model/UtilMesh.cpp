#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../model/UtilMesh.h"
#include "../model/FragmentBufferData.h"
#include "../model/MeshSpec.h"
#include "../model/IndexBufferData.h"
#include "../model/VaoDataGeneric.h"
#include "../display/ShaderBase.h"
#include "../display/RenderUtils.h"
#include "../display/ShaderMaker.h"

namespace Game {
//////////////////////////////////////////////////////////////////////////
UtilMesh::UtilMesh(std::shared_ptr<GLContext> ctx, std::shared_ptr<VertexFormat> fmt, std::shared_ptr<ShaderBase> ps, GLenum drawMode) :
    _pShader(ps), _pContext(ctx), _pVertexFormat(fmt), _eDrawMode(drawMode)
{
    _m4ModelMatrix = mat4::identity();
}
UtilMesh::~UtilMesh() {
    cleanup();
}
//////////////////////////////////////////////////////////////////////////
void UtilMesh::copyFromSpec(std::shared_ptr<MeshSpec> sp){
    _pVaoData = nullptr;
    //    DEL_MEM(_pVaoData);
    _pVerts = nullptr;
    _pIndexes = nullptr;

    _pVertexFormat = sp->getVertexFormat();
    _pVaoData = std::make_shared<VaoDataGeneric>(_pContext, _pVertexFormat);

    sp->beginEdit();
    {
        _pVerts = std::make_shared<FragmentBufferData>(_pVertexFormat);
        _pVerts->allocate(sp->getFrags()->count());
        _pVerts->copyFrom(sp->getFrags());

        _pIndexes = std::make_unique<IndexBufferData>();
        _pIndexes->allocate(sp->getIndexes()->count());
        _pIndexes->copyFrom(sp->getIndexes());

        copyBuffersToVao();
    }
    sp->endEdit();
}
void UtilMesh::allocData(int nVerts, int nIndexes, std::shared_ptr<VertexFormat> fmt) {
    AssertOrThrow2(_pVaoData != nullptr);
    AssertOrThrow2(fmt != nullptr);
    _pVertexFormat = fmt;

    _pVerts = nullptr;
    //DEL_MEM(_pVerts);
    _pVerts = std::make_unique<FragmentBufferData>(_pVertexFormat);
    _pVerts->allocate(nVerts);
    _bInitialized = false;

    _pIndexes = nullptr;
    //DEL_MEM(_pIndexes);
    _pIndexes = std::make_unique<IndexBufferData>();
    _pIndexes->allocate(nIndexes);
    _bInitialized = false;
}
void UtilMesh::init() {

    _pVaoData = std::make_shared<VaoDataGeneric>(_pContext, _pVertexFormat);
    generate();
}
void UtilMesh::copyBuffersToVao() {
    getVaoData()->fillData(getVerts()->ptr(), getVerts()->count(), getIndexes()->ptr(), getIndexes()->count());
}
std::shared_ptr<ShaderBase> UtilMesh::getShader() { 
    if(_pShader == nullptr) { 
        return _pContext->getShaderMaker()->getFlatShader(_pVertexFormat);
    }
    return _pShader; 
}
void UtilMesh::draw()
{
    preDraw();
    {
        //Allcate and generate verts
        std::shared_ptr<ShaderBase> pShader = getShader();
        AssertOrThrow2(pShader != NULL);
        std::shared_ptr<CameraNode> bc = _pContext->getCamera();
        pShader->setCameraUf(bc, &_m4ModelMatrix);
        pShader->draw(_pVaoData, -1, _eDrawMode);
    }
    postDraw();
}
void UtilMesh::cleanup()
{
    _pVerts = nullptr;
    _pIndexes = nullptr;
    _pVaoData = nullptr;
}

}//ns game
