/**
*  @file Component.h
*  @date 20200213
*  @author MetalMario971
*/
#pragma once
#ifndef __COMPONENT_15816346511616768678_H__
#define __COMPONENT_15816346511616768678_H__

#include "../base/BaseHeader.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class Component
*  @brief Component of a gameobject.
*/
class Component : public VirtualMemoryShared<Component> {
public:
    Component(std::shared_ptr<WorldObject> ob);
    virtual ~Component() override;
    void setWorldObject(std::shared_ptr<WorldObject> ob) { _pWorldObject = ob;  }
    
    template < class Tx = WorldObject >
    std::shared_ptr<Tx> getWorldObject() { return std::dynamic_pointer_cast<Tx>(_pWorldObject); }
private:
  std::shared_ptr<WorldObject> _pWorldObject = nullptr;
};

}//ns BR2



#endif
