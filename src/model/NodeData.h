/**
*  @file NodeData.h
*  @date 20200215
*  @author MetalMario971
*/
#pragma once
#ifndef __NODEDATA_15818089813247272835_H__
#define __NODEDATA_15818089813247272835_H__
#include "../model/ModelHeader.h"
#include "../base/Serializable.h"

namespace BR2 {
/**
*  @class NodeData 
*  @brief Stores shared node data between @cBaseNode instances.
*/
class NodeData : public ISerializable, public VirtualMemoryShared<NodeData> {
public:
  NodeData();
  virtual ~NodeData() override;

  ParentType getParentType() { return _eParentType; }
  Box3f* getBoundBox() { return _pBox; }
  string_t getName() { return _strName; }
  Hash32 getNameHashed() { return _iNameHashed; }
  mat4& getBind() { return _mBind; }
  mat4& getInvBind() { return _mInvBind; }
  mat4& getParentInverse() { return _mParentInverse; }
  void setParentInverse(mat4& m) { _mParentInverse = m; }
  void setBind(mat4& bind);
  void setInvBind(mat4& bind);
  void setParentName(string_t str, ParentType ee) { _strParentName = str; setParentType(ee); }
  string_t getParentName() { return _strParentName; }

  virtual void serialize(std::shared_ptr<BinaryFile> fb);
  virtual void deserialize(std::shared_ptr<BinaryFile> fb);

protected:
  Box3f* _pBox = nullptr;//Base Box.
  string_t _strName;
  Hash32 _iNameHashed;
  mat4 _mInvBind;
  mat4 _mBind;
  mat4 _mParentInverse;   //This is just for mesh objects that have mesh parents.
  //mat4 _mBasis;//Where the object is - base rotation &c.
  string_t _strParentName;
  ParentType _eParentType = ParentType::None;

  void setParentType(ParentType pt) { _eParentType = pt; }
};


}//ns BR2



#endif
