/**
*  @file VboData.h
*  @date January 5, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __VBODATA_3010761031374305419486_H__
#define __VBODATA_3010761031374305419486_H__

#include "../model/ModelHeader.h"
#include "../model/GpuBufferData.h"
namespace BR2 {


/**
*  @class VboData
*    36 bytes
*  @brief Represents a vertex buffer object in Opengl.
*/
class VboData : public GpuBufferData {
  //  friend class GpuBufferManager;    
  //  VertexFormatType _vertexType;

  //  TBMap<VaoData*> _mapVaoReferences;//vaos that reference this buffer.
public:
  //  VertexFormatType getVertexFormatType(){return _vertexType;}

  //  GpuFuture allocateFill(size_t num_elements, const void* frags);
  //  GpuFuture allocateOnly(size_t num_elements);//Note: this also deletes previous allocation.
  //  void addVaoReference(VaoData* dat);
   // void removeVaoReference(VaoData* dat);

    VboData(std::shared_ptr<GLContext> ct, size_t iElementSize);
    virtual ~VboData() override ;
};




}//ns BR2



#endif
