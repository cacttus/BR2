#include "../model/IndexBufferData.h"
#include "../base/Allocator.h"


namespace Game {
IndexBufferData::IndexBufferData() 
{
}
IndexBufferData::~IndexBufferData(){
    deallocate();
}

void IndexBufferData::allocate(size_t count){
    
    deallocate();
    _pIndexes = new Allocator<v_index32>(count);
}
void IndexBufferData::deallocate(){
    DEL_MEM(_pIndexes);
}
v_index32& IndexBufferData::i32(size_t index){
    return _pIndexes->at(index);
}
size_t IndexBufferData::count() const { 
    return _pIndexes->count(); 
}
void IndexBufferData::copyFrom(std::shared_ptr<IndexBufferData> rhs){
    _pIndexes->copyFrom(rhs->getBuf());
}
void IndexBufferData::copyFrom(const v_index32* fb, size_t countt) {
    _pIndexes->copyFrom(fb, countt);
}
void IndexBufferData::appendToEnd(IndexBufferData* fb) {
    _pIndexes->appendToEnd(fb->getBuf());
}
void* IndexBufferData::ptr() {
    return _pIndexes->ptr();
}
}//ns game
