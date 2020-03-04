#include "../bottle/Goal25Think.h"
#include "../bottle/Brain25.h"
#include "../bottle/SpriteBucket.h"

namespace BR2 {
Goal25Think::Goal25Think(Brain25* pBrain) : Goal25(pBrain) {
}
Goal25Think::~Goal25Think() {
}
void Goal25Think::execute(float delta) {
  std::shared_ptr<ModelNode> pPixObj = getBrain()->getPixObj();

  float thinkSpeed = 0.5f;//pPixObj->getSpec()->getThinkSpeed();
  _fAccum += delta;

  //Make the player idle
 // pPixObj->setIdle(false);

  if (getBrain()->getIsPlayer() == true) {
    //If player don't think.
  }
  else if (_fAccum > thinkSpeed) {
    //Stop to think..
    _fAccum = fmodf(_fAccum, thinkSpeed);

    std::vector<Goal25*> options;
    getBrain()->findStuff(options);

    //execute first option
    if (options.size() > 0) {
      getBrain()->pushGoal(options[0]);
    }
    //evaulate all goals.
    //for (Goal* g : options) {
  }
}
}//ns Game