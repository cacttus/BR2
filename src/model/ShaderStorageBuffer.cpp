#include "../model/ShaderStorageBuffer.h"
#include "../math/MathAll.h"
#include "../base/GLContext.h"

namespace Game {

ShaderStorageBuffer::ShaderStorageBuffer(std::shared_ptr<GLContext> gc, size_t eleSize) : 
GpuBufferData(gc, GL_SHADER_STORAGE_BUFFER, eleSize) {
    //_eleSize = eleSize;
}
ShaderStorageBuffer::~ShaderStorageBuffer() { 
  //  detachAllShaderVarLists();
}
void ShaderStorageBuffer::allocFill(size_t num_elements, const void* frags) {
    GpuBufferData::allocate(num_elements);
    GpuBufferData::copyDataClientServer(num_elements, frags);
}
void ShaderStorageBuffer::syncRead(void* out_data, size_t num_bytes, size_t byte_offset, bool useMemoryBarrier) {

    if(getByteSize()==0) {
        BroThrowException("OGL Buffer will not bind, and throw an exception, the given buffer object has no data");
    }
    if(useMemoryBarrier){
        _pContext->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    bindBuffer(GL_SHADER_STORAGE_BUFFER);
    void* v = _pContext->glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,getByteSize(),GL_MAP_READ_BIT);
    memcpy((void*)out_data, (void*)((char*)v+byte_offset), num_bytes );
    _pContext->glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
    unbindBuffer();

   // _pContext->chkErrDbg();

}
void ShaderStorageBuffer::syncWrite(void* data, size_t num_bytes, size_t byte_offset, bool useMemoryBarrier) {
  
    if(getByteSize()==0) {
        BroThrowException("OGL Buffer will not bind, and throw an exception, the given buffer object has no data");
    }

    //Segfault
    VerifyOrThrow(getByteSize()<=(byte_offset + num_bytes),
        "Tried to write more data than OpenGL Buffer could handle, bufsize: ",getByteSize(), " tried to write ", num_bytes, " at offset ", byte_offset  );

    if(useMemoryBarrier) {
        _pContext->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    bindBuffer();
    void* v = _pContext->glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,getByteSize(),GL_MAP_WRITE_BIT);
    memcpy((void*)((char*)v+byte_offset), (void*)data, num_bytes);
    _pContext->glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
    unbindBuffer();

  //  _pContext->chkErrDbg();

}
//void GpuBufferGeneric::attachShaderVarList(GlslShaderUniformVariables* pu) {
//    _vecBoundShaderVarLists.addIfDoesNotContain(pu);
//}
//void GpuBufferGeneric::detachShaderVarList(GlslShaderUniformVariables* pu) {
//    _vecBoundShaderVarLists.removeIfContains(pu);
//}
//void GpuBufferGeneric::detachAllShaderVarLists() {
//    _vecBoundShaderVarLists.resize(0);
//}

}//ns game
