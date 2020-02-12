/**
*
*    @file IboData.h
*    @date March 30, 2014
*    @author MetalMario971
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __IBODATA_186441646911777432011087_H__
#define __IBODATA_186441646911777432011087_H__

#include "../model/ModelHeader.h"
#include "../model/GpuBufferData.h"
namespace BR2 {

//class GpuBufferManager;
//class GLRenderSystem;

/**
*    @class IboData
*    @brief Represents a vertex buffer object in Opengl.
*/
class IboData : public GpuBufferData {
    //friend class GpuBufferManager;    
    //IndexFormatType _indexType;
    //GLenum _glIndexType;
    //GLenum _glIndexMode;
public:
    //FORCE_INLINE IndexFormatType IboData::getIndexType(){ return _indexType;}
    //FORCE_INLINE GLenum IboData::getGlIndexType(){return _glIndexType; }
    //FORCE_INLINE GLenum IboData::getGlIndexMode(){return _glIndexMode; }

    //GpuFuture allocateFill(size_t num_elements, const void* frags);
    //GpuFuture allocateOnly(size_t num_elements);

    IboData(std::shared_ptr<GLContext> ctx, size_t iElementSize);
    virtual ~IboData() override;
};
}//ns game



#endif
