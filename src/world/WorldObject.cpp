#include "../world/WorldObject.h"
#include "../world/Scene.h"
#include "../base/Component.h"


namespace BR2 {
WorldObject::WorldObject() {
}
WorldObject::~WorldObject() {
}
void WorldObject::addComponent(std::shared_ptr<Component> comp) {
  comp->setWorldObject(getThis<WorldObject>());
  _components.push_back(comp);
}
std::shared_ptr<Scene> WorldObject::getScene() {
  std::shared_ptr<Scene> x = findParent<Scene>();
  return x;
}










}//ns Game
