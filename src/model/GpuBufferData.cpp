#include "../base/BaseAll.h"
#include "../base/Gu.h"
#include "../base/BaseHeader.h"
#include "../base/GLContext.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/ShaderManager.h"
#include "../model/GpuBufferData.h"

namespace BR2 {
GpuBufferData::GpuBufferData(std::shared_ptr<GLContext> ct, GLenum bufferType, size_t iElementSize) : GLFramework(ct) {
  _glBufferType = bufferType;
  _iElementSize = iElementSize;
  getContext()->glGenBuffers(1, &_glId);
}
GpuBufferData::~GpuBufferData() {
  getContext()->glDeleteBuffers(1, &_glId);
}
void GpuBufferData::allocate(size_t iNumElements) {
  _iNumElements = iNumElements;
  bindBuffer();
  getContext()->glBufferData(_glBufferType, _iNumElements * _iElementSize, nullptr, GL_STATIC_DRAW);
  unbindBuffer();
  _bIsAllocated = true;
}
void GpuBufferData::copyDataServerClient(size_t num_elements, void* __out_ elements, int32_t elementSize) {
  //Acutlly the error was due to not having allocated the buffer DOH
  //if(_glBufferType==GL_SHADER_STORAGE_BUFFER)
  //    BroThrowException("Tried to call base read on an SSBO - use syncRead instead.");

  return readbytes(num_elements, elements, elementSize);
}

void GpuBufferData::copyDataClientServer(std::shared_ptr<ByteBuffer> gb) {
  AssertOrThrow2(gb != NULL);
  return copyDataClientServer(gb->size(), gb->data(), 1);
}
void GpuBufferData::copyDataServerClient(std::shared_ptr<ByteBuffer> gb) {
  AssertOrThrow2(gb != NULL);
  return copyDataServerClient(gb->size(), gb->data(), 1);
}
/**
*  @fn
*  @brief
*/
void GpuBufferData::readbytes(size_t num_elements, void* __out_ buf, int32_t elementSize) {
  if (buf == nullptr) {
    Br2LogError("Tried to read to NULL Buffer reading Gpu contents.");
    Gu::debugBreak();
  }
  if (_bIsAllocated == false) {
    Br2ThrowException(" [VBO] was not allocated");
  }
  size_t readSize;

  if (elementSize == -1) {
    readSize = getEleSize() * num_elements;
  }
  else {
    readSize = ((size_t)elementSize) * num_elements;
  }

  size_t byteSize = getByteSize();
  if (readSize > byteSize) {
    Br2ThrowException("Tried to read " + readSize + " from Gpu Buffer with size of " + getByteSize() + ".");
  }
  getContext()->chkErrDbg();

  bindBuffer();
  {
    void* pData = nullptr;
    mapBuffer(GL_WRITE_ONLY, pData);
    memcpy_s((void*)buf, readSize, pData, readSize);
    unmapBuffer();
  }
  unbindBuffer();

  getContext()->chkErrDbg();

}
/**
*  @fn copyDataClientServer
*  @brief Copy data to a preallocated buffer.
*/
void GpuBufferData::copyDataClientServer(size_t num_elements, const void* frags, int32_t iElementSize) {
  if (num_elements == 0) {
    return;
  }
  if (frags == nullptr) {
    Br2LogError(" [VBO] Fragments to copy were null. ");
    Gu::debugBreak();
    return;
  }
  if (_bIsAllocated == false) {
    Br2LogError(" [VBO] was not allocated");
    Gu::debugBreak();
    return;
  }

  bindBuffer();
  {
    size_t byteSize = getByteSize();
    size_t copySizeBytes;
    if (iElementSize == -1) {
      copySizeBytes = getEleSize() * num_elements;
    }
    else {
      copySizeBytes = iElementSize * num_elements;
    }
    if (copySizeBytes > byteSize) {
      Br2LogError("Copy size " + copySizeBytes + " (" + num_elements + " elements) was larger than the buffer size " + byteSize + ". Make sure the input size is not -1 or less than 0.");
      unbindBuffer();
      return;
      Gu::debugBreak();
    }

    //**NOTE: we would HAVE to use glMapBuffer because when we initially create this buffer
    //we intend to have it to be the GIVEN size. glBufferData will reallocate it.
    void* pData = nullptr;
    mapBuffer(GL_WRITE_ONLY, pData);
    memcpy_s((void*)pData, getByteSize(), frags, copySizeBytes);
    unmapBuffer();

    getContext()->chkErrDbg();
  }
  unbindBuffer();
}
/**
*  @fn mapVbo()
*    Get a pointer to the vertex buffer data for a material.
*/
void GpuBufferData::mapBuffer(GLenum access, void*& pData) {
  void* frags;
  getContext()->chkErrDbg();

  if (_isBound == false) {
    Br2ThrowException("GPU BUffer was not bound prior to mapping");
  }

  // - Can't map more than one buffer at the same time.
  // User must call unmap
  if (_isMapped == true) {
    Br2ThrowException("GPU Buffer was already mapped prior to mapping.");
  }

  if (false) {
    GLint dat;
    getContext()->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAPPED, &dat);
    getContext()->chkErrDbg();
  }
  frags = getContext()->glMapBuffer(_glBufferType, access);
  getContext()->chkErrDbg();

  _isMapped = true;

  if (frags == nullptr) {
    static bool sbLogged = false;
    if (sbLogged == false) {
      sbLogged = true;
      getContext()->chkErrDbg();
      Br2LogError("Error - glMapBuffer returned nullptr.");
      Gu::debugBreak();
    }
  }

  pData = frags;
}
/*
*  @fn unmap
*  @brief Unmap all vbo's
*/
void GpuBufferData::unmapBuffer() {
  verifyValidBuffer();
  if (getContext()->glUnmapBuffer(_glBufferType) == false) {
    //video memory just got trashed.
    Br2LogError("Video Memory has been trashed by the OS. Graphics may be unstable.");
  }
  getContext()->chkErrDbg();

  _isMapped = false;
}

/**
*  @fn bindVBO()
*  @brief Bind the VBO.
*/
void GpuBufferData::bindBuffer() {
  verifyValidBuffer();
  getContext()->glBindBuffer(_glBufferType, _glId);
  getContext()->chkErrDbg();
  _isBound = true;
}
void GpuBufferData::bindBuffer(GLenum e) {
  verifyValidBuffer();
  getContext()->glBindBuffer(e, _glId);
  getContext()->chkErrDbg();
  _isBound = true;
}
/**
*  @fn unbind()
*  @brief Unbind the VBO.
*/
void GpuBufferData::unbindBuffer() {
  getContext()->glBindBuffer(_glBufferType, 0);
  getContext()->chkErrDbg();
  _isBound = false;
}
void GpuBufferData::verifyValidBuffer() {
  //GDebugger doesn't like this for some reason.
  if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking()) {
    if (!getContext()->glIsBuffer(_glId)) {
      //Buffer object doesn't exist in the context.
      //Error somewhere..
      getContext()->glBindBuffer(_glBufferType, _glId);
      if (!getContext()->glIsBuffer(_glId)) {
        Gu::debugBreak();
      }
    }
  }

}


//+-- CTOR/DTOR --+

}//ns BR2
