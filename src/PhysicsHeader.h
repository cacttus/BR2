/**
*  @file PhysicsHeader.h
*  @date 20200306
*  @author MetalMario971
*/
#pragma once
#ifndef __PHYSICSHEADER_158352308057574117_H__
#define __PHYSICSHEADER_158352308057574117_H__


namespace BR2 {
/**
*  @class PhysicsHeader
*  @brief
*/
class PhysicsHeader : public VirtualMemoryShared<PhysicsHeader> {
public:
    PhysicsHeader();
    virtual ~PhysicsHeader() override;
};

}//ns Game



#endif
