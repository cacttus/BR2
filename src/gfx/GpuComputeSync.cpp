#include "../base/GLContext.h"
#include "../gfx/GpuComputeSync.h"


namespace BR2 {
;
GpuComputeSync::GpuComputeSync(std::shared_ptr<GLContext> pc) : _pContext(pc) {
}
GpuComputeSync::~GpuComputeSync() {
}
/**
*  @fn createCpuFence
*  @brief
*/
void GpuComputeSync::createCpuFence() {
    if (_bGpuDispatched) {
        BroThrowException("Signal object tried to create fence on cpu when gpu was signaled.");
    }
    _bGpuDispatched = false;
    _bCpuDispatched = true;
    _bCpuSignaled = false;
}
/**
*  @fn signalCpu
*  @brief
*/
void GpuComputeSync::signalCpu() {
    _bCpuSignaled = true;
}
/**
*  @fn createFence
*  @brief
*/
void GpuComputeSync::createFence() {
    if (_bCpuDispatched) {
        BroThrowException("Signal object tried to create fence on gpu when cpu was signaled.");
    }
    _bGpuDispatched = true;
    _bCpuDispatched = false;
    _glSyncObject = _pContext->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}
/**
*  @fn isComputeComplete
*  @brief Returns true if the given operation is complete.
*/
bool GpuComputeSync::isComputeComplete() {
    GLint result = GL_UNSIGNALED;
    if (_bCpuDispatched) {
        if (_bCpuSignaled) {
            result = GL_SIGNALED;
        }
    }
    else {
        if (_pContext->glIsSync(_glSyncObject))
        {
            _pContext->glGetSynciv(_glSyncObject, GL_SYNC_STATUS, sizeof(GLint), nullptr, &result);
            //This was causing memory leak
            if (result == GL_SIGNALED) {
                _pContext->glDeleteSync(_glSyncObject);
            }
        }
        else {
            result = GL_SIGNALED;
        }

    }

    if (result == GL_SIGNALED) {
        _bCpuDispatched = false;
        _bGpuDispatched = false;
    }
    return result == GL_SIGNALED;
}

//+-- CTOR/DTOR --+


}//ns game
