/**
*  @file Serializable.h
*  @date 20200216
*  @author MetalMario971
*/
#pragma once
#ifndef __SERIALIZABLE_1581885157429372404_H__
#define __SERIALIZABLE_1581885157429372404_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class Serializable
*  @brief Serializable interface for classes.
*/
template < class Tx >
class Serializable : public VirtualMemoryShared<Tx> {
public:
  Serializable() {
  }
  virtual ~Serializable() override {
  }
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) = 0;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) = 0;
};

}//ns Game



#endif
