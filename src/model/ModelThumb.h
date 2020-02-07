/**
*
*    @file ModelThumb.h
*    @date February 21, 2018
*    @author MetalMario971
*
*    © 2018 
*
*
*/
#pragma once
#ifndef __MODELTHUMB_15192738821472686508_H__
#define __MODELTHUMB_15192738821472686508_H__

#include "../model/ModelHeader.h"

namespace Game {
/**
*    @class ModelThumb
*    @brief
*
*/
class ModelThumb : public VirtualMemory {

public:
    static std::shared_ptr<Img32> genThumb(std::shared_ptr<ModelSpec> mod, int32_t iThumbSize);
    
};

}//ns Game



#endif
