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
*  @brief The base class for all node data blocks.  See NodeDataManager for instance.
*/
class NodeData : public ISerializable<NodeData> {
public:
  NodeData(string_t name = "");
  virtual ~NodeData() override;

  string_t getName() { return _strName; }
  void setName(string_t n) { _strName = n; }
  uint32_t getHashedName();

  virtual void deserialize(std::shared_ptr<BinaryFile> bf) override;
  virtual void serialize(std::shared_ptr<BinaryFile> bf) override;

  virtual std::shared_ptr<NodeData> clone() = 0;
  virtual void copy(std::shared_ptr<NodeData> other) = 0;

private:
  string_t _strName;
  uint32_t _iNameHash = 0;
};

}

#endif
