#include "../base/BaseHeader.h"
#include "./Script.h"

namespace BR2 {
Script::Script() {
}
Script::~Script() {
}
void Script::onAddedToNode() {
  onStart();
}

}//ns Game
