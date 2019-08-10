/**
*
*    @file Particles.h
*    @date December 5, 2016
*    @author Derek Page
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __PARTICLES_14809610022765256194_H__
#define __PARTICLES_14809610022765256194_H__

#include "../base/BaseHeader.h"
#include "../gfx/DisplayHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*    @class Particles
*    @brief
*
*/
class Particles : public VirtualMemory {
    std::shared_ptr<QuadBufferMesh> _pQuadBufferMesh;
public:

    //TODO: use textureatlascomposer for this.



    Particles();
    virtual ~Particles() override;
    void update();
    void draw();
};

}//ns Game



#endif