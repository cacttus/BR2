#include "../bottle/Brain25.h"
#include "../bottle/Goal25.h"
#include "../bottle/Goal25Think.h"
#include "../bottle/WorldObj.h"


namespace Game {
///////////////////////////////////////////////////////////////////
Brain25::Brain25(std::shared_ptr<ModelNode> pObj, std::shared_ptr<World25> pGrid)
{
    _pWorld25 = pGrid;
    _pPixObj = pObj;
}
Brain25::~Brain25()
{
    for (size_t i = 0; i < _vecGoals.size(); ++i) {
        Goal25* g = _vecGoals[i];
        DEL_MEM(g);
    }
    _vecGoals.resize(0);
}
///////////////////////////////////////////////////////////////////
void Brain25::init() {
    Goal25Think* pt = new Goal25Think(this);

    _vecGoals.push_back(pt); //Lowes
}
void Brain25::killCurrentGoal() {
    Goal25* g = getCurrentGoal();
    g->exit();
    _vecGoals.erase(_vecGoals.begin());
    DEL_MEM(g);
}
void Brain25::update(float delta) {
    if (_bEndGoalFlag == true) {
        killCurrentGoal();
        _bEndGoalFlag = false;
    }

    //make sutre to call init()
    getCurrentGoal()->execute(delta);
}

void Brain25::pushGoal(Goal25* g) {
    //if (_pPixObj->getIsPlayer()) {
    //    //if player replace any outstanding goals. so ew only have 1 goal ever (besides the think goal)
    //    if (_vecGoals.size() > 1) {
    //        killCurrentGoal();
    //    }
    //}

    _vecGoals.insert(_vecGoals.begin(), g);
}
Goal25* Brain25::getCurrentGoal() {
    //    CCASSERT(_vecGoals.size() > 0, "Vec goals had no size.");
    return _vecGoals[0];
}
void Brain25::findStuff(std::vector<Goal25*>& outGoals) {
    //GridCell* gq;
    //std::vector<GridCell*> qusds;
    //std::vector<GridCell*> empty_explore;

    //gq = _pWorld25->getObjQuad(_pPixObj);
    //if (gq == nullptr) {
    //    int n = 0;
    //    Gu::debugBreak();//this happens sometiems IDK why
    //    return;
    //}

    //_pWorld25->getVisibleQuadsFromLocation(gq, _pPixObj->getProps()->_fAwareRadius, qusds);

    //Goal25* outG = nullptr;
    //for (GridCell* cq : qusds) {
    //    if(cq->getObj()!=nullptr) {
    //        //GridQuad*_pWorld25->getObjQuad(cq->getObj());
    //        //** TODO:
    //        //object actions.
    //        if (cq->getObj()->getType() == GridObjType::Wall) {
    //            // if we are next to the wall then attack
    //            if (cq->isNextToS4(gq)) {
    //                //TODO: attack animation

    //                //TODO: attack animation
    //                //TODO: attack animation

    //                //TODO: attack animation &c goal
    //                //GoalDestroyTarget
    //                outG = new GoalDestroy(this, cq->getObj()->getId());
    //                break;
    //            } 
    //            else {
    //                //Find a place to move next to the object.
    //                std::vector<GridCell*> spots;
    //                _pWorld25->findEmptySpotsS4(cq, spots);
    //                GridCell* pNextInPath = nullptr;
    //                for (GridCell* spot : spots) {
    //                    //see if we can move to that spot.
    //                    int iHops = 0;
    //                    if (spot != nullptr) {
    //                        _pWorld25->getShortestPath(gq, spot, pNextInPath, iHops);
    //                        if (pNextInPath != nullptr) {
    //                            break;
    //                        }
    //                    }
    //                }
    //                if (pNextInPath != nullptr) {
    //                    outG = new GoalMoveTo(this, pNextInPath);
    //                    outGoals.push_back(outG);
    //                }
    //            }
    //            // if we are not next to the wall move towards.
    //        }

    //    }
    //    else {
    //        //Wander / explore grid
    //        empty_explore.push_back(cq);
    //    }
    //}

    ////If we found no object tasks just wander around 
    //if (outG == nullptr && empty_explore.size()>0) {
    //    int rnd = rand() % empty_explore.size();
    //    if (rnd < empty_explore.size()) {
    //        GridCell* q = empty_explore[rnd];
    //        outG = new GoalMoveTo(this, q);
    //    }
    //}

    //if (outG != nullptr) {
    //    outGoals.push_back(outG);
    //}
}
//////////////////////////////////////////////////////////////////////////

void Brain25::endGoal() {
    _bEndGoalFlag = true;
}

//ObjProps::ObjProps() {
//    _fAwareRadius = Utils::getCellWidth() * 6;
//}









}//ns Game
