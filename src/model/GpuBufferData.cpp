#include "../base/BaseAll.h"
#include "../base/Gu.h"
#include "../base/BaseHeader.h"
#include "../base/GLContext.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/ShaderMaker.h"
#include "../model/GpuBufferData.h"

namespace BR2 {
GpuBufferData::GpuBufferData(std::shared_ptr<GLContext> ct, GLenum bufferType, size_t iElementSize) :
  _glBufferType(bufferType)
  , _iElementSize(iElementSize)
  , _pContext(ct) {
  ct->glGenBuffers(1, &_glId);
}
GpuBufferData::~GpuBufferData() {
  _pContext->glDeleteBuffers(1, &_glId);
}
void GpuBufferData::allocate(size_t iNumElements) {
  _iNumElements = iNumElements;
  bindBuffer();
  _pContext->glBufferData(_glBufferType, _iNumElements * _iElementSize, nullptr, GL_STATIC_DRAW);
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
*    @fn
*    @brief
*/
void GpuBufferData::readbytes(size_t num_elements, void* __out_ buf, int32_t elementSize) {
  if (buf == nullptr) {
    BroLogError("Tried to read to NULL Buffer reading Gpu contents.");
    Gu::debugBreak();
  }
  if (_bIsAllocated == false) {
    BroThrowException(" [VBO] was not allocated");
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
    BroThrowException("Tried to read " + readSize + " from Gpu Buffer with size of " + getByteSize() + ".");
  }
  _pContext->chkErrDbg();

  bindBuffer();
  {
    void* pData = nullptr;
    mapBuffer(GL_WRITE_ONLY, pData);
    memcpy_s((void*)buf, readSize, pData, readSize);
    unmapBuffer();
  }
  unbindBuffer();

  _pContext->chkErrDbg();

}
/**
*    @fn copyDataClientServer
*    @brief Copy data to a preallocated buffer.
*/
void GpuBufferData::copyDataClientServer(size_t num_elements, const void* frags, int32_t iElementSize) {
  if (num_elements == 0) {
    return;
  }
  if (frags == nullptr) {
    BroLogError(" [VBO] Fragments to copy were null. ");
    Gu::debugBreak();
    return;
  }
  if (_bIsAllocated == false) {
    BroLogError(" [VBO] was not allocated");
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
      BroLogError("Copy size " + copySizeBytes + " (" + num_elements + " elements) was larger than the buffer size " + byteSize + ". Make sure the input size is not -1 or less than 0.");
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

    _pContext->chkErrDbg();
  }
  unbindBuffer();
}
/**
*    @fn mapVbo()
*    Get a pointer to the vertex buffer data for a material.
*/
void GpuBufferData::mapBuffer(GLenum access, void*& pData) {
  void* frags;
  _pContext->chkErrDbg();

  if (_isBound == false) {
    BroThrowException("GPU BUffer was not bound prior to mapping");
  }

  // - Can't map more than one buffer at the same time.
  // User must call unmap
  if (_isMapped == true) {
    BroThrowException("GPU Buffer was already mapped prior to mapping.");
  }

  if (false) {
    GLint dat;
    _pContext->glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAPPED, &dat);
    _pContext->chkErrDbg();
  }
  frags = _pContext->glMapBuffer(_glBufferType, access);
  _pContext->chkErrDbg();

  _isMapped = true;

  if (frags == nullptr) {
    static bool sbLogged = false;
    if (sbLogged == false) {
      sbLogged = true;
      _pContext->chkErrDbg();
      BroLogError("Error - glMapBuffer returned nullptr.");
      Gu::debugBreak();
    }
  }

  pData = frags;
}
/*
*    @fn unmap
*    @brief Unmap all vbo's
*/
void GpuBufferData::unmapBuffer() {
  verifyValidBuffer();
  if (_pContext->glUnmapBuffer(_glBufferType) == false) {
    //video memory just got trashed.
    BroLogError("Video Memory has been trashed by the OS. Graphics may be unstable.");
  }
  _pContext->chkErrDbg();

  _isMapped = false;
}

/**
*    @fn bindVBO()
*    @brief Bind the VBO.
*/
void GpuBufferData::bindBuffer() {
  verifyValidBuffer();
  _pContext->glBindBuffer(_glBufferType, _glId);
  _pContext->chkErrDbg();
  _isBound = true;
}
void GpuBufferData::bindBuffer(GLenum e) {
  verifyValidBuffer();
  _pContext->glBindBuffer(e, _glId);
  _pContext->chkErrDbg();
  _isBound = true;
}
/**
*    @fn unbind()
*    @brief Unbind the VBO.
*/
void GpuBufferData::unbindBuffer() {
  _pContext->glBindBuffer(_glBufferType, 0);
  _pContext->chkErrDbg();
  _isBound = false;
}
void GpuBufferData::verifyValidBuffer() {
  //GDebugger doesn't like this for some reason.
  if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking()) {
    if (!_pContext->glIsBuffer(_glId)) {
      //Buffer object doesn't exist in the context.
      //Error somewhere..
      _pContext->glBindBuffer(_glBufferType, _glId);
      if (!_pContext->glIsBuffer(_glId)) {
        Gu::debugBreak();
      }
    }
  }

}


//+-- CTOR/DTOR --+

}//ns game
