/**
*  @file ModelThumb.h
*  @date February 21, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __MODELTHUMB_15192738821472686508_H__
#define __MODELTHUMB_15192738821472686508_H__

#include "../model/ModelHeader.h"

namespace BR2 {
/**
*  @class ModelThumb
*  @brief Generates thumbnail pictures for models
*/
class ModelThumb : public VirtualMemory {
public:
    static std::shared_ptr<Img32> genThumb(std::shared_ptr<ModelData> mod, int32_t iThumbSize);
    
};

}//ns BR2



#endif
