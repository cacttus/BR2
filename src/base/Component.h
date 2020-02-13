/**
*  @file Component.h
*  @date 20200213
*  @author MetalMario971
*/
#pragma once
#ifndef __COMPONENT_15816346511616768678_H__
#define __COMPONENT_15816346511616768678_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class Component
*  @brief Component of a gameobject.
*/
class Component : public VirtualMemoryShared<Component> {
public:
    Component();
    virtual ~Component() override;
};

}//ns Game



#endif
