/**
*
*    @file Goal25MoveTo.h
*    @date March 16, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __GOAL25MOVETO_1489679362480048777_H__
#define __GOAL25MOVETO_1489679362480048777_H__

#include "../bottle/BottleHeader.h"
#include "../bottle/Goal25.h"


namespace Game {
/**
*    @class Goal25MoveTo
*    @brief
*
*/
class Goal25MoveTo : public Goal25 {
    WorldCell* _pDestinationQuad = nullptr;//final destination
    WorldCell* _pCurrentTarget = nullptr;//current one we're moving to.
    vec3 _vFinalPos;
    int32_t _iLastHopCount = -1;
    float _fSpd;
public:
    Goal25MoveTo(Brain25* pBrain, WorldCell* q);
    virtual ~Goal25MoveTo() override;

    virtual void execute(float delta) override;
    virtual void enter() override { }
    virtual void exit() override;

};

//class Goal25DoWork : public Goal25 {
//    PixObjId _iActionItem = OBJ_EMPTY;
//    std::function<void()> _pFuncEnd = nullptr;
//public:
//    Goal25DoWork(Brain25* pBrain, PixObjId actionItem, std::function<void()> funcEnd);
//    virtual ~Goal25DoWork() override;
//
//    virtual void execute(float delta) override;
//    virtual void enter() override { }
//    virtual void exit() override;
//};

class Goal25Animate : public Goal25 {
    std::shared_ptr<SpriteSpec> _pSpec = nullptr;
    float _fSpeed;
    bool _bLoop;
public:
    Goal25Animate(Brain25* pBrain, std::shared_ptr<SpriteSpec> spec, float speed, bool loop);
    virtual ~Goal25Animate() override;

    virtual void execute(float delta) override;
    virtual void enter() override { }
    virtual void exit() override { };
};


}//ns Game



#endif
