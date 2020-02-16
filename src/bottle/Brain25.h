/**
*
*    @file Brain25.h
*    @date March 16, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __BRAIN25_1489679362978360372_H__
#define __BRAIN25_1489679362978360372_H__

#include "../bottle/BottleHeader.h"


namespace Game {
/**
*    @class Brain25
*    @brief
*
*/
class Brain25 : public VirtualMemory {
public:
    const float LOWEST_PRIORITY = FLT_MAX;
    const float HIGHEST_PRIORITY = 0.0f;

private:
    std::shared_ptr<World25> _pWorld25;
    std::shared_ptr<ModelNode> _pPixObj;
    std::vector<Goal25*> _vecGoals;
    bool _bIsPlayer;

    Goal25* getCurrentGoal();
    Goal25* _pNextGoal;

    bool _bEndGoalFlag = false;
    void killCurrentGoal();

    //std::vector<Action*> _vecActions;
    //void updateActions();
public:
    //void pushAction(Action* act);

    Brain25(std::shared_ptr<ModelNode> obj, std::shared_ptr<World25> pGrid);
    virtual ~Brain25() override;

    void findStuff(std::vector<Goal25*>& outGoals);

    void init();
    void update(float delta);
    // void force(float priority, Goal* g);
    void pushGoal(Goal25* g);

    void setIsPlayer(bool b) { _bIsPlayer = b; }
    bool getIsPlayer() { return _bIsPlayer; }
    std::shared_ptr<World25> getWorld25() { return _pWorld25; }
    std::shared_ptr<ModelNode> getPixObj() { return _pPixObj; }
    void endGoal(); //Ends the current goal.
    size_t getNumPendingGoals() {
        return _vecGoals.size();
    }
};

}//ns Game



#endif
