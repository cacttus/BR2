#include "../base/BufferedFile.h"
#include "../world/PhysicsShapes.h"


namespace Game {
///////////////////////////////////////////////////////////////////


SphereShape::SphereShape(VertexBufferPointer& vbPointer, float r) : _fRadius(r) {

}
SphereShape::~SphereShape() {

}
HullShape::HullShape(VertexBufferPointer& vbPointer) {
    Box3f box;
} 
HullShape::~HullShape() {

    _vecPoints.resize(0);
}
void PhysicsShape::deserialize( std::shared_ptr<BufferedFile> fb) {
    fb->readBool(_bKinematic);
    fb->readBool(_bDynamic);
}
void HullShape::deserialize( std::shared_ptr<BufferedFile> fb){
    PhysicsShape::deserialize(fb);
}
void SphereShape::deserialize( std::shared_ptr<BufferedFile> fb) {
    PhysicsShape::deserialize(fb);
}

void PhysicsShape::serialize( std::shared_ptr<BufferedFile> fb) {
    fb->writeBool(std::move(_bKinematic));
    fb->writeBool(std::move(_bDynamic));
}
void HullShape::serialize( std::shared_ptr<BufferedFile> fb) {
    PhysicsShape::deserialize(fb);
}
void SphereShape::serialize( std::shared_ptr<BufferedFile> fb) {
    PhysicsShape::deserialize(fb);
}


}//ns Game
