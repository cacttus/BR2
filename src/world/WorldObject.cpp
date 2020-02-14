#include "WorldObject.h"


namespace BR2 {
WorldObject::WorldObject() {
}
WorldObject::~WorldObject() {
}
void WorldObject::addComponent(std::shared_ptr<Component> comp) {
  _components.push_back(comp);
}










}//ns Game
