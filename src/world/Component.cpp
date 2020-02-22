#include "../base/BaseHeader.h"
#include "./Component.h"


namespace BR2 {
Component::Component(std::shared_ptr<SceneNode> ob) : _pWorldObject(ob) {
}
Component::~Component() {
}

}//ns BR2
