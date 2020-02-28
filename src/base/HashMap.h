/**
*  @file HashMap.h
*  @date February 13, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __HASHMAP_2256332660273822544327474_H__
#define __HASHMAP_2256332660273822544327474_H__

#include "../base/BaseHeader.h"
#include "../base/Hash.h"
#include "../base/Logger.h"
#include "../base/DebugHelper.h"

namespace BR2 {
/**
*  @class HashMapItem
*  @brief Container, storing a reference to a hashmap item, or nullptr.
*/
template <class TItem>
class HashMapItem {
public:
  HashMapItem(TItem* t) : _val(t) {}

  TItem* _val;
  bool hasValue() {
    return _val != nullptr;
  }
  TItem value() {
    //Note, always use hasValue, or value() may return garbage.
    return *_val;
  }
};
/**
*  @class HashMap
*  @brief Maps strings to items.  This allows you to use strings in a lookup table.
*/
template <class Tx, class THashSize>
class _HashMap : public VirtualMemory {
public:
  typedef std::map<THashSize, Tx> HashMapType;
  typedef typename HashMapType::iterator iterator;
  typedef typename HashMapType::const_iterator const_iterator;
  typedef typename HashMapItem<Tx> Item;

  _HashMap();
  virtual ~_HashMap() override;

  iterator begin() { return _map.begin(); }
  iterator end() { return _map.end(); }
  void add(string_t key, Tx& x);//Can't be const
  bool remove(string_t key);
  HashMapItem<Tx> find(string_t key);
  size_t size() { return _map.size(); }
  void erase(const_iterator _Where) { _map.erase(_Where); }
  void clear() { _map.clear(); }
  bool contains(string_t key);

protected:
  HashMapType _map;
  virtual int32_t getNumAlgorithms() = 0;
  virtual THashSize computeHash(string_t str, int32_t iAlgorithm) = 0;
};
template < class Tx, class THashSize >
_HashMap<Tx, THashSize>::_HashMap() {
}
template < class Tx, class THashSize >
_HashMap<Tx, THashSize>::~_HashMap() {
  _map.clear();
}
template < class Tx, class THashSize >
void _HashMap<Tx, THashSize>::add(string_t str, Tx& x) {
  THashSize nHashVal;
  int i;
  for (i = 0; i < getNumAlgorithms(); ++i) {
    nHashVal = computeHash(str, i);
    if (_map.find(nHashVal) != _map.end()) {
      BRLogWarn("Hash Map collision using algorithm " + i);
      //TODO: ITF this may be needed
      //str.setHashAlgorithmIndex(i + 1);
    }
    else {
      break;
    }
  }

#ifdef _DEBUG
  //We had a collision and ran out of algorithms.
  if (i == getNumAlgorithms() - 1) {
    BRLogWarn("No suitable hash algorithm for string.");
    DebugHelper::debugBreak();
  }
#endif

  _map.insert(std::make_pair(nHashVal, x));
}
// - Returns false if remove failed.
template < class Tx, class THashSize >
bool _HashMap<Tx, THashSize>::remove(string_t key) {
  THashSize n = computeHash(key, key.getHashAlgorithmIndex());
  bool b = _map.remove(n, true);

  if (b == false) {
    BRLogWarn(" [Hm] Failed to remove hashed string");
  }

  return b;
}
/**
*  @fn find()
*  @brief finds a string.  Returns a nullptr RefItem if not found.
*/
template < class Tx, class THashSize >
typename HashMapItem<Tx> _HashMap<Tx, THashSize>::find(string_t key) {
  THashSize n = computeHash(key, 0/*key.getHashAlgorithmIndex()*/); //The string should in fact have a hash algorithm, in case of collisions.  We are just crossing our fingers here.

  typename HashMapType::iterator ite = _map.find(n);
  if (ite == _map.end()) {
    return HashMapItem<Tx>(nullptr);
  }

  return HashMapItem<Tx>(&(ite->second));
}
template < class Tx, class THashSize >
bool _HashMap<Tx, THashSize>::contains(string_t key) {
  HashMapItem<Tx> ri = find(str);
  if (ri._val == nullptr) {
    return false;
  }
  return true;
}

/**
*  @class HashMap32
*  @brief Implementation of _HashMap for 32-bit hash.
*/
template < class Tx >
class HashMap32 : public _HashMap<Tx, uint32_t> {
public:
  HashMap32() {}
  virtual ~HashMap32() override {}

  int32_t getNumAlgorithms() override {
    return FNV_MAX_ALGORITHMS_32;
  }
  uint32_t computeHash(string_t str, int32_t iAlgorithm) override {
    return Hash::computeStringHash32bit(str, iAlgorithm);
  }
};
/**
*  @class HashMap32
*  @brief Implementation of _HashMap for 64-bit hash.
*/
template < class Tx >
class HashMap64 : public _HashMap<Tx, uint64_t> {
public:
  HashMap64() {}
  virtual ~HashMap64() override {}

  int32_t getNumAlgorithms() override {
    return FNV_MAX_ALGORITHMS_64;
  }
  uint64_t computeHash(string_t str, int32_t iAlgorithm) override {
    return Hash::computeStringHash64bit(str, iAlgorithm);
  }

};
/**
*  @class HashMap
*  @brief Hash map with 32-bit hash.
*/
template < class Tx >
class HashMap : public HashMap32<Tx> {
public:
  HashMap() {}
  virtual ~HashMap() override {}
};





}//ns BR2



#endif
