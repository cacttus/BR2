
#include "../model/VboData.h"

namespace Game {
VboData::VboData(std::shared_ptr<GLContext> ct, size_t iElementSize) : GpuBufferData(ct, GL_ARRAY_BUFFER, iElementSize) {
}

VboData::~VboData() {
}
//GpuFuture VboData::allocateFill(size_t num_elements, const void* frags)
//{
//    return GpuBufferData::allocateFill(num_elements, VertexUtils::getFragmentSizeForType(_vertexType), frags);
//}
//GpuFuture VboData::allocateOnly(size_t num_elements)
//{
//    return GpuBufferData::allocateFill(num_elements, VertexUtils::getFragmentSizeForType(_vertexType), NULL);
//}
//void VboData::addVaoReference(VaoData* dat)
//{
//#ifdef _DEBUG
//    if(_mapVaoReferences.find(dat)!=_mapVaoReferences.end())
//        BroThrowException("Duplicate VAO refernece found for vertex buffer.");
//#endif
//    _mapVaoReferences.add(dat);
//}
//void VboData::removeVaoReference(VaoData* dat)
//{
//    //throws an ex if fail.
//    _mapVaoReferences.remove(dat);
//}
//VboData::VboData(GLRenderSystem* renderSystem, VertexFormatType vt) : 
//GpuBufferData(renderSystem, GL_ARRAY_BUFFER),
//_vertexType(vt)
//{
//
//}
//VboData::~VboData()
//{ 
//    BMap<VaoData*>::iterator ite = _mapVaoReferences.begin();
//    for(;ite!=_mapVaoReferences.end();ite++)
//        ite->second->unbindBuffer(this);
//    _mapVaoReferences.clear();
//}


}//ns game
