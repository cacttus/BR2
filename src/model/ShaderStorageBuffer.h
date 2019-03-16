/**
*
*    @file GpuBufferGeneric.h
*    @date October 7, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#pragma once
#ifndef __GPUBUFFERGENERIC_220156197467321424683_H__
#define __GPUBUFFERGENERIC_220156197467321424683_H__


#include "../model/GpuBufferData.h"

namespace Game {;
/**
*    @class ShaderStorageBuffer  SSBO
*    @brief TODO: change the name to GpuShaderStorageBuffer
*
*/
class ShaderStorageBuffer  : public GpuBufferData {

  //  std::vector<GlslShaderUniformVariables*> _vecBoundShaderVarLists;
public:
    ShaderStorageBuffer(std::shared_ptr<GLContext> gc, size_t eleSize);
    virtual ~ShaderStorageBuffer() override;

  //  void attachShaderVarList(GlslShaderUniformVariables* pu);
  //  void detachShaderVarList(GlslShaderUniformVariables* pu);
  //  void detachAllShaderVarLists();

    void syncRead(void* out_data, size_t num_bytes, size_t byte_offset=0, bool useMemoryBarrier=true);
    void syncWrite(void* in_data, size_t num_bytes, size_t byte_offset=0,bool useMemoryBarrier=true);

    void allocFill(size_t num_elements, const void* frags=NULL);


};//ns game


}

#endif
