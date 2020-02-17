#include "Sequencer.h"
#include "../base/Gu.h"

namespace BR2 {
///////////////////////////////////////////////////////////////////
Sequence* Sequencer::exec(
  std::function<void()> begin, std::function<void()> run, std::function<void()> end,
  float delaySeconds, float durationSeconds) {
  Sequence* pFirst = new Sequence(begin, run, end, delaySeconds, durationSeconds);
  _vecSequences.push_back(pFirst);
  return pFirst->getNext();
}
void Sequencer::update() {

  std::vector<int> toRemove;
  std::vector<Sequence*> toAdd;

  t_timeval ct = Gu::getMicroSeconds();

  for (size_t ia = 0; ia < _vecSequences.size(); ++ia) {
    Sequence* ss = _vecSequences[ia];
    if (ss->run(ct)) {
      Sequence* s2 = ss->getNext();
      if (s2->hasAction()) {
        toAdd.push_back(s2);
      }
      else {
        //Delete the final action in the chain - this will delete all the parent actions.
        delete s2;
        s2 = nullptr;
      }
      toRemove.push_back((int)ia);
    }
  }

  //delete dead sequences
  for (int ia = (int)toRemove.size() - 1; ia >= 0; --ia) {
    int rem = toRemove[ia];
    _vecSequences.erase(_vecSequences.begin() + rem);
  }

  //add new, or chained
  for (size_t ia = 0; ia < toAdd.size(); ++ia) {
    _vecSequences.push_back(toAdd[ia]);
  }

  toRemove.resize(0);
  toAdd.resize(0);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
Sequence::Sequence(std::function<void()> begin, std::function<void()> run, std::function<void()> end,
  float delaySeconds, float durationSeconds) {
  exec(begin, run, end, delaySeconds, durationSeconds);
}
Sequence::Sequence(Sequence* prev) {
  _pPrev = prev;
}
Sequence::~Sequence() {
  //Delete the sequence in reverse.
  if (_pPrev != nullptr) {
    delete _pPrev;
    _pPrev = nullptr;
  }
}
bool Sequence::run(t_timeval curTime) {
  //returns true if this action has ended. 
  if (_started == 0) {
    _started = curTime;
    if (_pBegin != nullptr) {
      _pBegin();
    }
  }

  if (_executed == 0) {
    t_timeval dtime = curTime - _started;

    if (dtime >= _iDelay) {
      _executed = curTime;
    }
    else {
      if (_pRun != nullptr) {
        _pRun();
      }
    }
  }

  if (_executed != 0) {
    t_timeval dtime = curTime - _executed;

    if (dtime >= _iDuration) {
      if (_pEnd != nullptr) {
        _pEnd();
      }
      return true;
    }
  }
  return false;
}
Sequence* Sequence::exec(std::function<void()> begin, std::function<void()> run, std::function<void()> end,
  float delaySeconds, float durationSeconds) {
  //Sets this action to execute.
  //If this is the first call to "exec" in a chain, then
  //this is executed right away after delay
  // * Delay - the amount of time after the last *duration has ended which this action will execute
  // * Duratio - The amount of time taken fot his taks to exeucte
  // So - tasks will execute in the order of
  //  Delay..[task a] Duration + Delay..[task b] Duration + delay.. [task c] Duration..

  if (durationSeconds < 0) {
    durationSeconds = 0;
    Gu::debugBreak();//errr
  }
  if (delaySeconds < 0) {
    delaySeconds = 0;
    Gu::debugBreak();//errr
  }

  _started = 0;
  _executed = 0;
  _pBegin = begin;
  _pRun = run;
  _pEnd = end;
  _iDuration = (t_timeval)((double)durationSeconds * 1000000.0);
  _iDelay = (t_timeval)((double)delaySeconds * 1000000.0);

  _pNext = new Sequence(this); //Setting a previous action will indicate this is a 'next' Action.

  return _pNext;
}







}//ns BR2
