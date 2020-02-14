/**
*
*    @file WorldObject.h
*    @date February 9, 2020
*    @author MetalMario971
*
*    © 2020 
*/
#pragma once
#ifndef __WORLDOBJECT_15812292363701244110_H__
#define __WORLDOBJECT_15812292363701244110_H__

#include "../world/WorldHeader.h"
#include "../model/BaseNode.h"
#include "../world/PhysicsNode.h"

namespace BR2 {
/**
*  @class WorldObject
*  @brief Essentially this is a "PhysicsNode".  We should combine these classes.  Then we will add the necessary modifiers as composites.
*/
class WorldObject : public PhysicsNode {
public:
    WorldObject();
    virtual ~WorldObject() override;
    
    void addComponent(std::shared_ptr<Component> comp);

private:
  std::vector<std::shared_ptr<Component>> _components;
};

}//ns Game



#endif
