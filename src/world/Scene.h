/**
*
*    @file Scene.h
*    @date February 9, 2020
*    @author MetalMario971
*
*/
#pragma once
#ifndef __SCENE_1581228482668944733_H__
#define __SCENE_1581228482668944733_H__

#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class Scene
*    @brief The scenegraph holding all relevant objects.
*
*/
class Scene : public VirtualMemoryShared<Scene> {
public:
    Scene();
    virtual ~Scene() override;
};

}//ns Game



#endif
