/**
*  @file GpuComputeSync.h
*  @date October 7, 2014
*  @author MetalMario971
*/
#pragma once
#pragma once
#ifndef __GPUDISPATCHEDANIMATION_1127415804133681036130024_H__
#define __GPUDISPATCHEDANIMATION_1127415804133681036130024_H__

#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class GpuComputeSync
*  @brief Stores information pertaining to the dispatching of computes to the compute shader system 
Usage
    For Gpu
        call createFence()
        check isComputeComplete()==true

    for Cpu
        call createCpuFence();
        when done call signalCpu()
        check isComputeComplete()==true
*/
class GpuComputeSync : public VirtualMemory {
    std::shared_ptr<GLContext> _pContext = nullptr;
public:
    //TRef<ModelNode> _pModelOfMesh; //unneeded if stored on meshbuf
    //MeshBuf* _pBufToCompute;
    GLsync _glSyncObject = nullptr;
    bool _bCpuDispatched = false;
    bool _bGpuDispatched  = false;
    bool _bCpuSignaled = false;
    bool isDispatched(){ return (_bCpuDispatched||_bGpuDispatched); }
    bool isCpuDispatched(){ return _bCpuDispatched; }
    //void clearDispatched(){ _bDispatched=false; }

    void createCpuFence();    // - Creates a "virtual" fence to simulate the same GPU stuff on the CPU.
    void signalCpu(); // - Manually signal via the CPU.  For the GPU signals simply call  isComputeComplete()

    void createFence();    // - Creates an OPENGL GPU instruction fence.
    bool isComputeComplete();    // Returns true if the given compute fence has been passed and the compute is complete.

    GpuComputeSync(std::shared_ptr<GLContext> pContext);
    virtual ~GpuComputeSync() override ;

};

}//ns BR2



#endif
