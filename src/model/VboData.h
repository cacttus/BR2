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
namespace Game {
/**
*  @class VboData
*  @brief Represents a vertex buffer object in Opengl.
*/
class VboData : public GpuBufferData {
public:
  VboData(std::shared_ptr<GLContext> ct, size_t iElementSize);
  virtual ~VboData() override;
};



}//ns game



#endif
