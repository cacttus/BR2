#include "../base/BaseHeader.h"
#include "./Component.h"


namespace BR2 {
Component::Component(std::shared_ptr<WorldObject> ob) : _pWorldObject(ob) {

}
Component::~Component() {

}

}//ns BR2
