/**
*  @file DataBlock.h
*  @date 20200219
*  @author MetalMario971
*/
#pragma once
#ifndef __DATABLOCK_15821020163536115780_H__
#define __DATABLOCK_15821020163536115780_H__

#include "../model/ModelHeader.h"

namespace BR2 {
//static uint64_t __NamedDataBlock_IdGen;
template < class Tx >
class IDataBlock : public VirtualMemoryShared<IDataBlock> {
public:
  IDataBlock() {}
  virtual IDataBlock() override {}

  std::shared_ptr<Tx> clone() = 0;
  void copy(std::shared_ptr<Tx> other) = 0;
};
//template <class Tx>
//class NamedDataBlock : public ISerializable, public IDataBlock<Tx>, public VirtualMemoryShared<NamedDataBlock> {
//public:
//  NamedDataBlock::NamedDataBlock(string_t name="") {
//    _iId = __NamedDataBlock_IdGen++;
//  }
//  virtual ~NamedDataBlock() override {
//  }
//
//  string_t getName() { return _strName; }
//  void setName(string_t n) { _strName = n; }
//
//  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override {
//    bf->readUint64(_iId);
//    if (_iId > _iMaxIdGen) {
//      __NamedDataBlock_IdGen = _iId;
//    }
//    bf->readString(std::move(_strName));
//  }
//  virtual void serialize(std::shared_ptr<BinaryFile> bf) override {
//    bf->writeUint64(std::move(_iId));
//    bf->writeString(std::move(_strName));
//  }
//
//private:
//  string_t _strName;
//  uint64_t _iId = 0;
//};


}//ns Game



#endif
