#include "../bottle/Goal25MoveTo.h"
#include "../bottle/Brain25.h"
#include "../bottle/WorldObj.h"
#include "../bottle/World25.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/BottleUtils.h"


namespace Game {
//////////////////////////////////////////////////////////////////2/
Goal25MoveTo::Goal25MoveTo(Brain25* pBrain, WorldCell* q) : Goal25(pBrain)
{
    _pDestinationQuad = q;
}
Goal25MoveTo::~Goal25MoveTo()
{

}
///////////////////////////////////////////////////////////////////

void Goal25MoveTo::execute(float delta) {
  //  WorldCell* pCurrentQuad;
  //  WorldCell* pTargetQuad;
  //  PixObj* pGridObj;
  //  std::shared_ptr<World25> pWorldGrid;
  //  Goal25MoveTo* that = this;
  //  PixObjSpec* pObjSpec;
  //
  //  //Set the animation to move
  //  //TODO: - we're going to set the right animations here later. by taking the move direction and snapping it.
  //  getBrain()->getPixObj()->setMoveDown(false);

    /*
    pGridObj = getBrain()->getPixObj();
    pWorldGrid = getBrain()->getWorld25();
    pObjSpec = getBrain()->getPixObj()->getSpec();
    if (pGridObj != nullptr && pWorldGrid != nullptr) {
        pCurrentQuad = getBrain()->getWorld25()->getObjQuad(pGridObj);

        if (pGridObj->getState() == PixObjState::e::Idle) {
            //if(pCurrentQuad == _pDestinationQuad){
            //    getBrain()->endGoal();
            //}
            //else{
                bool _bSet = false;
                if (_iLastHopCount == -1) {
                    _bSet = true;
                }
                _iLastHopCount = 0;
            
                //**SO - This is if we are using AI - 
                //For now circumvent any path finding and just use linear movement.
                //pWorldGrid->getShortestPath(pCurrentQuad, _pDestinationQuad, pTargetQuad, _iLastHopCount);
                pTargetQuad = _pDestinationQuad;


                if (_bSet) {
                    //Set speed if the hop count wasn't set.
                    if (_iLastHopCount <= 3) {
                        _fSpd = pObjSpec->getMoveSpeedCreep();
                    }
                    else if (_iLastHopCount <= 9) {
                        _fSpd = pObjSpec->getMoveSpeedWalk();
                    }
                    else {
                        _fSpd = pObjSpec->getMoveSpeedRun();
                    }
                }

                if (pTargetQuad != nullptr ) {
                    vec3 pos = pTargetQuad->getGPos().getVec3();

                    // if (pTargetQuad == _pDestinationQuad) {
                    //    _pDestinationQuad->setObj(getBrain()->getPixObj());

                    //Exit action
                    //      getBrain()->endGoal();
                    // }
                    //  else {
                    //Finally, move
                    pGridObj->setState(PixObjState::e::Moving);
                    _pCurrentTarget = pTargetQuad;

                    //remove always before add
                 //   pCurrentQuad->removeObj(pGridObj);
                 //   _pCurrentTarget->addObj(pGridObj);
                 //
                    _vFinalPos = pos;
                    //  }
                }
                else {
                    //We failed to find a path.
                    //We could Retry a cuople of times OR DIE. Just die for now.  We can retry later.
                    getBrain()->endGoal();
                }
            //}
        }

        if (pGridObj->getState() == PixObjState::e::Moving) {
            vec3 objPos = pGridObj->getPos();
            objPos.z = 0;
            _vFinalPos.z = 0;

            //Calc remaining length
            float lenRem = (objPos - _vFinalPos).length();
            float newLen = _fSpd * delta;

            //Truncate to avoid overshooting
            if(newLen > lenRem) {
                newLen = lenRem;
            }

            float tolerance = 0.0002f;

            if ((lenRem - newLen) <= tolerance) {
                vec3 g = _pCurrentTarget->getGPos().getVec3();
              //  g.z = CongaUtils::getObjectZIndexBase();
               // pGridObj->setPos(g);
               //pGridObj->getPhy()->setVelocity((objPos - _vFinalPos).normalize() * newLen);
                pGridObj->setState(PixObjState::e::Idle);
                getBrain()->endGoal();
            }
            else {
                //vec3 vDiff = _vFinalPos - objPos;
                //vDiff.normalize();
                //vec3 newP = objPos + vDiff * newLen;

                //newP.z = CongaUtils::getObjectZIndexBase();
                //pGridObj->setPos(newP);

                //TODO: -0 set this tof ace the sprite
              //  pGridObj->aboutFace(_pCurrentTarget->getCenter());
            }
            pGridObj->getPhy()->setVelocity((objPos - _vFinalPos).normalize() * newLen);
        }
    }*/

}
void Goal25MoveTo::exit() {
    //**Make sure to reset the state or else we will screw up.
    std::shared_ptr<ModelNode> pGridObj = getBrain()->getPixObj();
  //  WorldCell* cur = getBrain()->getWorld25()->getObjQuad(pGridObj);
  //  if (_pCurrentTarget != nullptr) {
  //      _pCurrentTarget->getGrid()->getWorld25()->updateLights();
    //    //_pCurrentTarget->getCell()->setObj(nullptr, _pCurrentTarget->getSideEnum());
    //    _pCurrentTarget->addObj(pGridObj->getId());
    //    _pCurrentTarget = nullptr;
  //  }
   // pGridObj->setState(WorldObjState::e::Idle);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//Goal25DoWork::Goal25DoWork(Brain25* pBrain, PixObjId q, std::function<void()> funcEnd) :
//    _iActionItem(q), Goal25(pBrain), _pFuncEnd(funcEnd) {
//
//}
//Goal25DoWork::~Goal25DoWork() {
//
//}
//void Goal25DoWork::execute(float delta) {
//    /*
//     std::shared_ptr<Animator> pAnim = getBrain()->getPixObj()->getAnimator();
//
//    if (pAnim == nullptr) {
//        //we must have an work animation
//        getBrain()->endGoal();
//        return;
//    }
//    if (_iActionItem == OBJ_EMPTY) {
//        //we must have an work animation
//        getBrain()->endGoal();
//        return;
//    }
//
//    if (pAnim->getAnimationId() != Animator::ci_work) {
//        //TODO: use player speed here.
//        pAnim->reset();
//        pAnim->setDuration(0.5);
//        pAnim->setAnimationId(Animator::ci_work);
//        pAnim->setLoop(false);
//        pAnim->start();
//
//        std::shared_ptr<World25> wg = getBrain()->getWorld25();
//        PixObjId iac = _iActionItem;
//        PixObj* ob = wg->getObj(iac);
//        getBrain()->getPixObj()->aboutFace(ob->getPos());
//
//        std::function<void()> chop = [wg, iac]() {
//            PixObj* ob = wg->getObj(iac);
//            if (ob != nullptr) {
//                Party* part = wg->getGlContext()->getParty();
//                part->make(
//                    ob->getBoundBoxObject()->center(), 10,
//                    Utils::c_iParticleWood,
//                    40, 80,
//                    0.1, 1,
//                    5, 5,
//                    -800, -800
//                );
//                //  _pActionItem->workDone();
//            }
//        };
//
//        std::function<void()> wiggle = [wg, iac]() {
//            PixObj* ob = wg->getObj(iac);
//            if (ob != nullptr) {
//                ob->setPos(ob->getPos() + vec3(Gu::frand1()*0.4, 0, Gu::frand1()*0.4));
//                ob->setRotation(ob->getRotation() + M_PI / 20);
//                //  _pActionItem->workDone();
//                ob->update(0.0, false, nullptr);
//            }
//        };
//
//        //Play sound or something -halfyway
//        //_pContext->getBuzzer()->play("sf-hit.ogg", 0.3, 0.5) //delay = 0.5.
//        getBrain()->getWorld25()->getGlContext()->getSequencer()->exec(chop, wiggle, nullptr, 0.25, 0.0);
//    }
//
//    if (pAnim->getState() == PlayState::e::Stopped || pAnim->isEnded()) {
//        getBrain()->endGoal();
//    }
//    */
//}
//void Goal25DoWork::exit() {
//    //Player done choppin
//    //Execute end function
//    _pFuncEnd();
//}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Goal25Animate::Goal25Animate(Brain25* pBrain, std::shared_ptr<SpriteSpec> ps, float speed, bool loop) :
_fSpeed(speed)
, _bLoop(loop)
, _pSpec(ps)
, Goal25(pBrain)
{

}
Goal25Animate::~Goal25Animate() {
}
void Goal25Animate::execute(float delta) {
    std::shared_ptr<ModelNode> ob = getBrain()->getPixObj();

    //Motion25Spec* pCurSpec = nullptr;
    //if (ob->getCurrentMotion() != nullptr) {
    //    pCurSpec = ob->getCurrentMotion()->getSpec();
    //}
    //
    //if (pCurSpec != _pSpec) {
    //    ob->setMotion(_pSpec, _pSpec->getDefaultDuration(), _pSpec->getDefaultLoop());
    //}



    //
    //    
    //     std::shared_ptr<Animator> pAnim = getBrain()->getPixObj()->getAnimator();
    //if (pAnim->getAnimationId() != _iAnimId) {
    //    //TODO: use player speed here.
    //    pAnim->reset();
    //    pAnim->setDuration(_fSpeed);
    //    pAnim->setAnimationId(_iAnimId);
    //    pAnim->setLoop(_bLoop);
    //    pAnim->start();
    //}
}











}//ns Game
