//#include "../model/VaoData2.h"
//#include "../model/VboData.h"
//#include "../model/IboData.h"
//#include "../base/GLContext.h"
//namespace Game {
/////////////////////////////////////////////////////////////////////
////class VaoDataGeneric : public VirtualMemory {
////    GLuint _iGlVaoId = 0;
////
////    std::shared_ptr<VboData> _pVboData = nullptr;
////    std::shared_ptr<IboData> _pIboData = nullptr;
////    std::shared_ptr<GLContext> _pContext = nullptr;
////public:
////    VaoData2(std::shared_ptr<GLContext> ctx);
////    virtual ~VaoData2() override;
////
////    bool hasDrawableData();
////
////    void fillDataInit(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds);//Initialize the VAO data
////    void refillData(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds); //Update the mesh data
////    void refillData(std::vector<v_v3n3x2>* verts, int32_t vcount, std::vector<v_index>* inds, int32_t icount); //Update the mesh data
////    void bind();
////    void draw(int32_t iCount = -1);
////};
//
//VaoData2::VaoData2(std::shared_ptr<GLContext> ctx) : _pContext(ctx)
//{
//    _pContext->glGenVertexArrays(1, &_iGlVaoId);
//
//}
//VaoData2::~VaoData2()
//{
//    DEL_MEM(_pVboData);
//    DEL_MEM(_pIboData);
//    _pContext->glDeleteVertexArrays(1, &_iGlVaoId);
//}
/////////////////////////////////////////////////////////////////////
//void VaoData2::refillData(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds) {
//    if (verts != nullptr) {
//        _pVboData->copyDataClientServer(verts->size(), verts->data(), sizeof(v_v3n3x2));
//    }
//    if (inds != nullptr) {
//        _pIboData->copyDataClientServer(inds->size(), inds->data(), sizeof(v_index));
//    }
//}
//void VaoData2::refillData(std::vector<v_v3n3x2>* verts, int32_t vcount, std::vector<v_index>* inds, int32_t icount) {
//    if (verts != nullptr && vcount > 0) {
//        _pVboData->copyDataClientServer(vcount, verts->data(), sizeof(v_v3n3x2));
//    }
//    if (inds != nullptr && icount > 0) {
//        _pIboData->copyDataClientServer(icount, inds->data(), sizeof(v_index));
//    }
//}
//
//void VaoData2::fillDataInit(std::vector<v_v3n3x2>* verts, std::vector<v_index>* inds) {
//    _pContext->chkErrRt();
//    if(_pVboData==nullptr) {
//        _pVboData = new VboData(_pContext, sizeof(v_v3n3x2));
//    }
//    if(_pIboData ==nullptr) {
//        _pIboData = new IboData(_pContext, sizeof(v_index));
//    }
//
//    _pContext->glBindVertexArray(_iGlVaoId);
//    {
//        _pContext->chkErrRt();
//        _pVboData->allocate(verts->size());
//        _pVboData->copyDataClientServer(verts->size(), verts->data());
//        _pContext->chkErrRt();
//        _pIboData->allocate(inds->size());
//        _pIboData->copyDataClientServer(inds->size(), inds->data());
//        _pContext->chkErrRt();
//
//        _pContext->glEnableVertexAttribArray(0);
//        _pContext->chkErrRt();
//        _pContext->glBindBuffer(GL_ARRAY_BUFFER, _pVboData->getGlId());
//        _pContext->chkErrRt();
//        _pContext->glVertexAttribPointer(
//            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//            3,                  // size
//            GL_FLOAT,           // type
//            GL_FALSE,           // normalized?
//            sizeof(v_v3n3x2),   // stride
//            (GLvoid*)0            // array buffer offset
//        );
//        _pContext->chkErrRt();
//
//        _pContext->glEnableVertexAttribArray(1);
//        _pContext->chkErrRt();
//        _pContext->glBindBuffer(GL_ARRAY_BUFFER, _pVboData->getGlId());
//        _pContext->chkErrRt();
//        _pContext->glVertexAttribPointer(
//            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//            3,                  // size
//            GL_FLOAT,           // type
//            GL_FALSE,           // normalized?
//            sizeof(v_v3n3x2),   // stride
//            (GLvoid*)(sizeof(vec4))            // array buffer offset
//        );
//        _pContext->chkErrRt();
//        _pContext->glEnableVertexAttribArray(2);
//        _pContext->chkErrRt();
//        _pContext->glBindBuffer(GL_ARRAY_BUFFER, _pVboData->getGlId());
//        _pContext->chkErrRt();
//        _pContext->glVertexAttribPointer(
//            2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//            2,                  // size
//            GL_FLOAT,           // type
//            GL_FALSE,           // normalized?
//            sizeof(v_v3n3x2),   // stride
//            (GLvoid*)(sizeof(vec4) + sizeof(vec4))            // array buffer offset
//        );
//        _pContext->chkErrRt();
//    }
//    _pContext->glBindVertexArray(0);
//}
//void VaoData2::bind() {
//    _pContext->glBindVertexArray(_iGlVaoId);
//    _pContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pIboData->getGlId());
//}
//
//void VaoData2::draw(int32_t iCount) {
//    if (iCount == -1) {
//        iCount = _pIboData->getNumElements();
//    }
//    if (iCount > 0) {
//        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_INT, (GLvoid*)0);
//    }
//}
//bool VaoData2::hasDrawableData() {
//    if (_pIboData == nullptr) {
//        return false;
//    }
//    return _pIboData->getByteSize() > 0;
//}
//
//}//ns BR2
