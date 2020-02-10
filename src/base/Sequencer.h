/**
*  @file Sequencer.h
*  @date December 23, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __SEQUENCER_14825455351090302744_H__
#define __SEQUENCER_14825455351090302744_H__

#include "../base/BaseHeader.h"

namespace Game {
/**
*    @class Sequencer
*    @brief manages sequenceas of actions (lambdas)
*
*/
class Sequencer : public VirtualMemory {
    std::vector<Sequence*> _vecSequences;
public:
    Sequencer() { }
    virtual ~Sequencer() override { }
    void update();

    Sequence* exec(std::function<void()> begin, std::function<void()> run, std::function<void()> end,
        float delaySeconds, float durationSeconds);
};

/**
*    @class Sequence
*    @brief dsrequences of actions / events /etc. based on time. very generic.  execute functions in sequence.
*/
class Sequence : public VirtualMemory {
    std::function<void()> _pBegin;
    std::function<void()> _pRun;
    std::function<void()> _pEnd;
    t_timeval _iDuration = 0; //seconds
    t_timeval _iDelay = 0; //seconds
    Sequence* _pPrev = nullptr;
    Sequence* _pNext = nullptr;
    t_timeval _started = 0;
    t_timeval _executed = 0;
public:
    
    Sequence(std::function<void()> begin, std::function<void()> run, std::function<void()> end, float delaySeconds, float durationSeconds);
    Sequence(Sequence* prev);
    virtual ~Sequence() override;
    Sequence* getNext() { return _pNext; }
    bool hasAction() { 
        return (_pBegin != nullptr) || (_pRun != nullptr) || (_pEnd != nullptr);
    }
    bool run(t_timeval curTime);
    Sequence* exec(std::function<void()> begin, std::function<void()> run, std::function<void()> end, float delaySeconds, float durationSeconds);

};

}//ns Game



#endif
