#include "../model/FragmentBufferData.h"
#include "../model/VertexFormat.h"

namespace Game {
FragmentBufferData::FragmentBufferData(std::shared_ptr<VertexFormat> vft) :  _pVertexFormat(vft)
{
    _v2f1 = _pVertexFormat->getComponentForUserType(VertexUserType::e::v2_01);
    _v3f1 = _pVertexFormat->getComponentForUserType(VertexUserType::e::v3_01);
    _n3f1 = _pVertexFormat->getComponentForUserType(VertexUserType::e::n3_01);
    _c4f1 = _pVertexFormat->getComponentForUserType(VertexUserType::e::c4_01);
    _x2f1 = _pVertexFormat->getComponentForUserType(VertexUserType::e::x2_01);
}
FragmentBufferData::~FragmentBufferData()
{
    deallocate();
}
size_t FragmentBufferData::count() {
    _iLastFragCount = _pFrags->byteSize() / _pVertexFormat->getSizeBytes();
    return _iLastFragCount;
}
void* FragmentBufferData::getPointerToComponentAtIndex(size_t index, std::shared_ptr<VertexComponent> pc) {
#ifdef _DEBUG
    if (pc == nullptr) {
        BroThrowException(
            "Vertex array access violation - tried to access a nonexistent vertex component" +
            " - check the vertex (fragment) type being referenced");
    }
#endif
    //Tested:this works to return a settable value
    size_t offBytes = _pVertexFormat->getSizeBytes() * index + pc->getByteOffset();
    return _pFrags->ptrOff(offBytes);
}
void FragmentBufferData::copyFrom(std::shared_ptr<FragmentBufferData> fb){
    _pFrags->copyFrom(fb->getBuf());
}
void FragmentBufferData::copyFrom(const void* fb, int byteSize){
    _pFrags->copyFrom((const char*)fb, byteSize);
}
void FragmentBufferData::appendToEnd(std::shared_ptr<FragmentBufferData> fb) {
    _pFrags->appendToEnd(this->getBuf());
}
vec2& FragmentBufferData::v2f(size_t index) {
    return accessComponent<vec2>(index, _v2f1);
}
vec3& FragmentBufferData::v3f(size_t index){
    return accessComponent<vec3>(index, _v3f1);
}
vec4& FragmentBufferData::c4f(size_t index){
    return accessComponent<vec4>(index, _c4f1);
}
vec3& FragmentBufferData::n3f(size_t index){
    return accessComponent<vec3>(index, _n3f1);
}
vec2& FragmentBufferData::x2f(size_t index){
    return accessComponent<vec2>(index, _x2f1);
}
void FragmentBufferData::allocate(size_t count){
    deallocate();
    _pFrags = new Allocator<char>(_pVertexFormat->getSizeBytes() * count);
}
void FragmentBufferData::deallocate(){
    DEL_MEM(_pFrags);
}
void* FragmentBufferData::ptr() {
    return _pFrags->ptr();
}











}//ns game
