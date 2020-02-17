#include "../model/NodeData.h"
#include "../base/Hash.h"
#include "../base/BinaryFile.h"


namespace BR2 {
NodeData::NodeData(string_t strName) {
  _strName = strName;
  _iNameHashed = STRHASH(_strName);
  _mBind = mat4::identity();
  _mInvBind = _mBind.inverseOf();
  _mParentInverse = mat4::identity();
  //    _mBasis = mat4::identity();
  _pBox = new Box3f();
}
NodeData::~NodeData() {
  DEL_MEM(_pBox);
  //_pBox = nullptr;
}
void NodeData::serialize(std::shared_ptr<BinaryFile> fb) {
  fb->writeString(std::move(_strName));
  fb->writeString(std::move(_strParentName));
  fb->writeInt32(std::move((int32_t)_eParentType));
  fb->writeMat4(std::move(_mBind));
  fb->writeMat4(std::move(_mParentInverse));
  fb->writeVec3(std::move(_pBox->_min));
  fb->writeVec3(std::move(_pBox->_max));
}
void NodeData::deserialize(std::shared_ptr<BinaryFile> fb) {
  fb->readString(std::move(_strName));
  _iNameHashed = STRHASH(_strName);
  fb->readString(std::move(_strParentName));
  int32_t pt;
  fb->readInt32(pt);
  _eParentType = (ParentType::e)pt;
  fb->readMat4(std::move(_mBind));
  _mInvBind = _mBind.inverseOf();
  fb->readMat4(std::move(_mParentInverse));

  _pBox = new Box3f();
  fb->readVec3(std::move(_pBox->_min));
  fb->readVec3(std::move(_pBox->_max));
}
void NodeData::setBind(mat4& bind) {
  _mBind = bind;
  _mInvBind = _mBind.inverseOf();
}
void NodeData::setInvBind(mat4& bind) {
  //_mBind = bind;
  _mInvBind = bind;
}

}//ns BR2
