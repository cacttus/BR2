/**
*
*    @file PhysicsGridAwareness.h
*    @date January 11, 2018
*    @author Derek Page
*
*    © 2018 
*
*
*/
#pragma once
#ifndef __PHYSICSGRIDAWARENESS_15156936891785483737_H__
#define __PHYSICSGRIDAWARENESS_15156936891785483737_H__

#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class PhysicsGridAwareness
*    @brief
*
*/
class PhysicsGridAwareness : public VirtualMemory {
    float _fAwarenessRadiusXZ;
    float _fAwarenessRadiusY;
    vec3 _vLastAwarenessPos;

    float _incXz;
    MpFloat _mpXz;
    MpFloat _mpY;
    float _incY;

    void updateAwarenessSpheroidAxis(float& fAwareness, float minR, float maxR, float increment);
public:
    PhysicsGridAwareness(MpFloat rxz, float incXZ, MpFloat ry, float incY) ;
    virtual ~PhysicsGridAwareness() override ;
    void getAwarenessBox(Box3f& c);
    void getAwarenessBox(Box3f& c, float rxz, float ry);
    vec3 getAwarenessPos();
    void update(float dt);
};

}//ns Game



#endif
