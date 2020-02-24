/**
*  @file IboData.h
*  @date March 30, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __IBODATA_186441646911777432011087_H__
#define __IBODATA_186441646911777432011087_H__

#include "../model/ModelHeader.h"
#include "../model/GpuBufferData.h"

namespace BR2 {
/**
*  @class IboData
*  @brief Represents a vertex buffer object in Opengl.
*/
class IboData : public GpuBufferData {
public:
    IboData(std::shared_ptr<GLContext> ctx, size_t iElementSize);
    virtual ~IboData() override;
};
}//ns game



#endif
