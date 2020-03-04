/**
*  @file Goal25.h
*  @date March 16, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __GOAL25_14896793623877413061_H__
#define __GOAL25_14896793623877413061_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
/**
*  @class Goal25
*  @brief
*/
class Goal25 : public VirtualMemory {
public:

  Goal25(Brain25* pBrain);
  virtual ~Goal25() override;

  virtual void execute(float delta) = 0;
  virtual void enter() = 0;
  virtual void exit() = 0;
  
protected:
  Brain25* getBrain() { return _pBrain; }

private:
  Brain25* _pBrain = nullptr; //pBrain...
  typedef enum { Entered, Running, Exit } GoalState;
  GoalState _eGoalState;
};
}//ns Game

#endif
