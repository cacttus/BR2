/**
*
*    @file Goal25Think.h
*    @date March 16, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __GOAL25THINK_1489679362676868611_H__
#define __GOAL25THINK_1489679362676868611_H__

#include "../bottle/BottleHeader.h"
#include "../bottle/Goal25.h"

namespace Game {
/**
*    @class Goal25Think
*    @brief
*
*/
class Goal25Think : public Goal25 {
    float _fAccum = 1.0f;
public:
    Goal25Think(Brain25* pBrain);
    virtual ~Goal25Think() override;

    virtual void execute(float delta) override;
    virtual void enter() override { }
    virtual void exit() override { }
};

}//ns Game



#endif
