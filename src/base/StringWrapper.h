///**
//*
//*  @file StringWrapper.h
//*  @date February 15, 2015
//*  @author MetalMario971   
//*/
//#pragma once
//#ifndef __STRINGWRAPPER_195312707012031195556901_H__
//#define __STRINGWRAPPER_195312707012031195556901_H__
//
//#include <string>
//#include "../base/BaseDefines.h"
//
//namespace BR2 {
//
//#ifdef USE_BRO_STL_ALLOCATOR_WRAPPER
//#define BroVectorTpl std::vector< Tx, AllocatorWrapper<Tx> >
//#define BroMapTpl std::map<Tx ,Ty, Compare, AllocatorWrapper< std::pair<const Tx, Ty> > >
//#define BroMultiMapTpl std::multimap<Tx, Ty, Compare ,AllocatorWrapper< std::pair<const Tx, Ty> > >
//#define BroListTpl std::list< Tx, AllocatorWrapper<Tx> >
//#define BroStackTpl std::stack<Tx >
//#define BroStringTpl std::basic_string<char, std::char_traits<char>, AllocatorWrapper<char> >
//#else
//#define BroVectorTpl std::vector< Tx >
//#define BroMapTpl std::map<Tx ,Ty, Compare >
//#define BroMultiMapTpl std::multimap<Tx, Ty, Compare >
//#define BroListTpl std::list< Tx >
//#define BroStackTpl std::stack<Tx >
//#define BroStringTpl std::string
//#endif
//
//class StlUtils {
//public:
//  static void aw_deallocate(void* ptr);
//  static void* aw_allocate(size_t count);
//};
//
//// - These MSVCRT routines are required because of the way string is implemented
//template<class _Ty> inline
//void AW_DESTROY(_Ty* _Ptr) {
//  _Ptr->~_Ty();
//}
//
//template<> inline
//void AW_DESTROY(char*) {
//  // Char pointers are handled awkwardly here in MSVCRT.  IF you try to dealloc a char* you get an error.
//}
//
//template<> inline
//void AW_DESTROY(wchar_t*) {
//  // Char pointers are handled awkwardly here in MSVCRT.  IF you try to dealloc a char* you get an error.
//}
//
//
//// TEMPLATE CLASS allocator
//template < class Tx >
//class AllocatorWrapper {
//public:
//#ifdef BR2_OS_WINDOWS
//  typedef typename Tx value_type;
//#else
//  typedef Tx value_type;
//#endif
//  typedef value_type* pointer;
//  typedef value_type& reference;
//  typedef const value_type* const_pointer;
//  typedef const value_type& const_reference;
//
//  typedef size_t size_type;
//  typedef size_t difference_type;
//
//  template<class _Other>
//  struct rebind {
//    typedef AllocatorWrapper<_Other> other;
//  };
//
//  void deallocate(Tx* _Ptr, size_type) {
//    StlUtils::aw_deallocate(_Ptr);
//  }
//  Tx* allocate(size_type _Count) {
//    return (Tx*)StlUtils::aw_allocate(_Count * sizeof(Tx));
//  }
//
//  FORCE_INLINE Tx* address(Tx& _Val) const {
//    return (&_Val);
//  }
//  FORCE_INLINE const Tx* address(const Tx& _Val) const {
//    return (&_Val);
//  }
//
//  FORCE_INLINE AllocatorWrapper() {}
//  FORCE_INLINE AllocatorWrapper(const AllocatorWrapper<Tx>&) {}
//  template<class _Other>
//  FORCE_INLINE AllocatorWrapper(const AllocatorWrapper<_Other>&) {}
//
//  template<class _Other>
//  FORCE_INLINE AllocatorWrapper<Tx>& operator=(const AllocatorWrapper<_Other>&) {
//    return (*this);
//  }
//  FORCE_INLINE Tx* allocate(size_type _Count, const void*) {
//    return (allocate(_Count));
//  }
//  FORCE_INLINE void construct(Tx* _Ptr, const Tx& _Val) {
//    ::new((void*)_Ptr) Tx(_Val);
//  }
//  FORCE_INLINE void destroy(Tx* _Ptr) {
//    AW_DESTROY(_Ptr);//StlUtils::aw_deallocate(_Ptr); 
//  }
//  FORCE_INLINE size_t max_size() const {
//    // estimate maximum array size
//    size_type _Count = (size_type)(-1) / sizeof(Tx);
//    return (0 < _Count ? _Count : 1);
//  }
//
//};//End of class
//
//
//template<class _Ty,
//  class _Other> inline
//  bool operator==(const AllocatorWrapper<_Ty>&,
//    const AllocatorWrapper<_Other>&) noexcept {    // test for allocator equality
//  return (true);
//}
//
//template<class _Ty,
//  class _Other> inline
//  bool operator!=(const AllocatorWrapper<_Ty>& _Left,
//    const AllocatorWrapper<_Other>& _Right) noexcept {    // test for allocator inequality
//  return (!(_Left == _Right));
//}
//
//typedef enum {
//  HASH_INVALID,    //valid / invalid hash
//  HASH_VALID,
//}StringHashState;
//
//typedef enum {
//  HASH_NO_HASH, HASH_FNV_32, HASH_FNV_64
//}HashFunc;
//
//class string_t : public VirtualMemory {
//public:
//  typedef BroStringTpl StlStringType;
//  typedef size_t size_type;
//  static const size_type npos = -1;
//  typedef StlStringType::iterator iterator;
//  typedef StlStringType::const_iterator const_iterator;
//  typedef StlStringType::reverse_iterator reverse_iterator;
//  typedef StlStringType::const_reverse_iterator const_reverse_iterator;
//  typedef char _Elem;
//  typedef string_t _Myt;
//  typedef _Elem& reference;// jacked up down in the msvc implementation  This is what it is.
//  typedef const _Elem& const_reference;// jacked up down in the msvc implementation  This is what it is.
//
//  FORCE_INLINE string_t() { init(); }
//  FORCE_INLINE string_t(char c) { char c2[2];  c2[0] = c; c2[1] = 0;  _str = StlStringType((char*)c2); init(); }
//  FORCE_INLINE string_t(const char* str) { _str = StlStringType(str);  init(); }
//  FORCE_INLINE string_t(const char* str, size_t len) { _str = StlStringType(str, len); init(); }
//  FORCE_INLINE string_t(const string_t& str) { _str = StlStringType(str._str); init(); }
//  FORCE_INLINE string_t(const StlStringType& str) { _str = StlStringType(str); init(); }
//  NOT_VIRTUAL ~string_t() DOES_NOT_OVERRIDE;
//
//  FORCE_INLINE StlStringType& stdStr() { return _str; }
//
//  FORCE_INLINE void init() {
//    //performance 
//    _iHashAlgorithmIndex = 0;
//  }
//  FORCE_INLINE void setHashAlgorithmIndex(int32_t idx) { _iHashAlgorithmIndex = idx + 1; }
//  FORCE_INLINE int32_t getHashAlgorithmIndex() const { return _iHashAlgorithmIndex; }
//  FORCE_INLINE friend std::ostream& operator<<(std::ostream& os, const string_t& obj) {
//    return os.write(obj.c_str(), (std::streamsize)obj.length());
//  }
//  FORCE_INLINE const char* c_str() const {
//    return _str.c_str();
//  }
//  FORCE_INLINE string_t& appendLine(const char* str) {
//    append(str);
//    append("\n");
//    return *this;
//  }
//  FORCE_INLINE string_t& appendLine(string_t& str) {
//    appendLine(str.c_str());
//    return *this;
//  }
//  FORCE_INLINE void push_back(char _Ch) {
//    _str.push_back(_Ch);
//  }
//  FORCE_INLINE size_type size() const {
//    return _str.size();
//  }
//  FORCE_INLINE size_type length() const {
//    return _str.length();
//  }
//  FORCE_INLINE void resize(size_type _Newsize) {
//    _str.resize(_Newsize);
//  }
//  FORCE_INLINE void resize(size_type _Newsize, char _Ch) {
//    _str.resize(_Newsize, _Ch);
//  }
//  FORCE_INLINE size_type capacity() const {    // return current length of allocated storage
//    return _str.capacity();
//  }
//  FORCE_INLINE void reserve(size_type _Newcap = 0) {    // determine new minimum length of allocated storage
//    return _str.reserve(_Newcap);
//  }
//  FORCE_INLINE int compare(const char* _Ptr) const {
//    return _str.compare(_Ptr);
//  }
//  FORCE_INLINE int compare(const string_t& str) const {
//    return _str.compare(str._str);
//  }
//  FORCE_INLINE string_t substr(size_type _Off = 0, size_type _Count = npos) const {
//    string_t str;
//    str._str = _str.substr(_Off, _Count);
//    return str;
//  }
//
//  //Erase
//  //Erase
//  //Erase
//  //Erase
//  //Erase
//  //Erase
//  FORCE_INLINE string_t& __CLR_OR_THIS_CALL erase(size_type _Off = 0, size_type _Count = npos) {
//    _str.erase(_Off, _Count);
//    return *this;
//  }
//  FORCE_INLINE iterator __CLR_OR_THIS_CALL erase(iterator _Where) {
//    return _str.erase(_Where);
//  }
//  FORCE_INLINE iterator erase(iterator _First, iterator _Last) {
//    return _str.erase(_First, _Last);
//  }
//
//  //Assign
//  //Assign
//  //Assign
//  //Assign
//  //Assign
//  //Assign
//  FORCE_INLINE string_t& assign(const char* ptr) {
//    _str.assign(ptr);
//    return *this;
//  }
//  FORCE_INLINE string_t& assign(string_t& str) {
//    _str.assign(str.c_str());
//    return *this;
//  }
//  FORCE_INLINE _Myt& assign(const _Myt& _Right, size_type _Roff, size_type _Count) {
//    _str.assign(_Right._str, _Roff, _Count);
//    return *this;
//  }
//  FORCE_INLINE _Myt& assign(const _Elem* _Ptr, size_type _Count) {
//    _str.assign(_Ptr, _Count);
//    return *this;
//  }
//  FORCE_INLINE _Myt& assign(size_type _Count, _Elem _Ch) {
//    _str.assign(_Count, _Ch);
//    return *this;
//  }
//
//  //Find
//  //Find
//  //Find
//  //Find
//  //Find
//  //Find
//  //Find
//  FORCE_INLINE size_type find(const string_t& _Right, size_type _Off = 0) const {
//    return _str.find(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type find(char ch, size_type _Off = 0) const {
//    return _str.find(ch, _Off);
//  }
//  FORCE_INLINE size_type rfind(const string_t& _Right, size_type _Off = npos) const {
//    return _str.rfind(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type rfind(const char* _Ptr, size_type _Off, size_type _Count) const {
//    return _str.rfind(_Ptr, _Off, _Count);
//  }
//  FORCE_INLINE size_type rfind(const char* _Ptr, size_type _Off = npos) const {
//    return _str.rfind(_Ptr, _Off);
//  }
//  FORCE_INLINE size_type __CLR_OR_THIS_CALL rfind(char _Ch, size_type _Off = npos) const {    // look for _Ch before _Off
//    return _str.rfind(_Ch, _Off);
//  }
//  FORCE_INLINE size_type find_first_of(const _Myt& _Right, size_type _Off = 0) const {
//    return _str.find_first_of(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type find_first_of(const _Elem* _Ptr, size_type _Off, size_type _Count) const {
//    return _str.find_first_of(_Ptr, _Off, _Count);
//  }
//  FORCE_INLINE size_type find_first_of(const _Elem* _Ptr, size_type _Off = 0) const {
//    return _str.find_first_of(_Ptr, _Off);
//  }
//  FORCE_INLINE size_type find_first_of(_Elem _Ch, size_type _Off = 0) const {
//    return _str.find_first_of(_Ch, _Off);
//  }
//  FORCE_INLINE size_type find_last_of(const _Myt& _Right, size_type _Off = npos) const {
//    return _str.find_last_of(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type find_last_of(const _Elem* _Ptr, size_type _Off, size_type _Count) const {
//    return _str.find_last_of(_Ptr, _Off, _Count);
//  }
//  FORCE_INLINE size_type find_last_of(const _Elem* _Ptr, size_type _Off = npos) const {
//    return _str.find_last_of(_Ptr, _Off);
//  }
//  FORCE_INLINE size_type find_last_of(_Elem _Ch, size_type _Off = npos) const {
//    return _str.find_last_of(_Ch, _Off);
//  }
//  FORCE_INLINE size_type find_first_not_of(const _Myt& _Right, size_type _Off = 0) const {
//    return _str.find_first_not_of(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type find_first_not_of(const _Elem* _Ptr, size_type _Off, size_type _Count) const {
//    return _str.find_first_not_of(_Ptr, _Off, _Count);
//  }
//  FORCE_INLINE size_type find_first_not_of(const _Elem* _Ptr, size_type _Off = 0) const {
//    return _str.find_first_not_of(_Ptr, _Off);
//  }
//  FORCE_INLINE size_type find_first_not_of(_Elem _Ch, size_type _Off = 0) const {
//    return _str.find_first_not_of(_Ch, _Off);
//  }
//  FORCE_INLINE size_type find_last_not_of(const _Myt& _Right, size_type _Off = npos) const {
//    return _str.find_last_not_of(_Right._str, _Off);
//  }
//  FORCE_INLINE size_type find_last_not_of(const _Elem* _Ptr, size_type _Off, size_type _Count) const {
//    return _str.find_last_not_of(_Ptr, _Off, _Count);
//  }
//  FORCE_INLINE size_type find_last_not_of(const _Elem* _Ptr, size_type _Off = npos) const {
//    return _str.find_last_not_of(_Ptr, _Off);
//  }
//  FORCE_INLINE size_type find_last_not_of(_Elem _Ch, size_type _Off = npos) const {
//    return _str.find_last_not_of(_Ch, _Off);
//  }
//
//  //Append
//  //Append
//  //Append
//  //Append
//  //Append
//  //Append
//  FORCE_INLINE string_t& append(const string_t& _Right) {
//    _str.append(_Right.c_str());
//    return *this;
//  }
//  FORCE_INLINE string_t& append(const char* ptr) {
//    _str.append(ptr);
//    return *this;
//  }
//  FORCE_INLINE string_t& append(const char* _Ptr, size_type _Count) {
//    _str.append(_Ptr, _Count);
//    return *this;
//  }
//  FORCE_INLINE string_t& append(size_type _Count, char _Ch) {
//    _str.append(_Count, _Ch);
//    return *this;
//  }
//
//
//  FORCE_INLINE string_t& insert(int off, string_t& str) {
//    _str.insert(off, str._str);
//    return *this;
//  }
//  // Operators
//  // Operators
//  // Operators
//  // Operators
//  // Operators
//  FORCE_INLINE string_t& operator=(const string_t& _Right) {
//    _str = _Right._str;
//    return *this;
//  }
//  FORCE_INLINE string_t& operator=(const char* ptr) {
//    _str = ptr;
//    return *this;
//  }
//  FORCE_INLINE string_t& operator=(char _Ch) {
//    _str = _Ch;
//    return *this;
//  }
//
//  FORCE_INLINE string_t& operator+=(const string_t& _Right) {
//    _str += _Right._str;
//    return *this;
//  }
//  FORCE_INLINE string_t& operator+=(const char* ptr) {
//    _str += ptr;
//    return *this;
//  }
//  FORCE_INLINE string_t& operator+=(char _Ch) {
//    _str += _Ch;
//    return *this;
//  }
//
//  FORCE_INLINE bool operator==(const string_t& _Right) const {
//    return this->_str == _Right._str;
//  }
//  FORCE_INLINE bool operator!=(const string_t& _Right) const {
//    return this->_str != _Right._str;
//  }
//
//  FORCE_INLINE string_t operator+(const string_t& _Right) const {
//    string_t ret = *this;
//    return ret += _Right;
//  }
//  FORCE_INLINE string_t operator+(const char* ptr) const {
//    string_t ret = *this;
//    return ret += ptr;
//  }
//  FORCE_INLINE string_t operator+(char _Ch) const {
//    string_t ret = *this;
//    return ret += _Ch;
//  }
//  FORCE_INLINE reference operator[](size_t s) {
//    return _str[s];
//  }
//  FORCE_INLINE reference at(size_type off) {
//    return _str.at(off);
//  }
//  FORCE_INLINE const_reference operator[](size_t s) const {
//    return _str[s];
//  }
//  FORCE_INLINE const_reference at(size_type off) const {
//    return _str.at(off);
//  }
//  FORCE_INLINE bool operator<(const _Myt& _Right) const {
//    return _str < _Right._str;
//  }
//  FORCE_INLINE bool operator>(const _Myt& _Right) const {
//    return _str > _Right._str;
//  }
//  FORCE_INLINE bool operator<=(const _Myt& _Right) const {
//    return _str <= _Right._str;
//  }
//  FORCE_INLINE bool operator>=(const _Myt& _Right) const {
//    return _str >= _Right._str;
//  }
//
//  //Iterators
//  //Iterators
//  //Iterators
//  //Iterators
//  //Iterators
//  iterator __CLR_OR_THIS_CALL begin() {    // return iterator for beginning of mutable sequence
//    return _str.begin();
//  }
//
//  const_iterator __CLR_OR_THIS_CALL begin() const {    // return iterator for beginning of nonmutable sequence
//    return _str.begin();
//  }
//
//  iterator __CLR_OR_THIS_CALL end() {    // return iterator for end of mutable sequence
//    return _str.end();
//  }
//
//  const_iterator __CLR_OR_THIS_CALL end() const {    // return iterator for end of nonmutable sequence
//    return _str.end();
//  }
//
//  reverse_iterator __CLR_OR_THIS_CALL rbegin() {    // return iterator for beginning of reversed mutable sequence
//    return _str.rbegin();
//  }
//
//  const_reverse_iterator __CLR_OR_THIS_CALL rbegin() const {    // return iterator for beginning of reversed nonmutable sequence
//    return _str.rbegin();
//  }
//
//  reverse_iterator __CLR_OR_THIS_CALL rend() {    // return iterator for end of reversed mutable sequence
//    return _str.rend();
//  }
//
//  const_reverse_iterator __CLR_OR_THIS_CALL rend() const {    // return iterator for end of reversed nonmutable sequence
//    return _str.rend();
//  }
//
//
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  // Replace and Clear
//  void clear() {
//    _str.clear();
//  }
//  _Myt& replace(size_type _Off, size_type _N0, const _Myt& _Right) {
//    _str.replace(_Off, _N0, _Right._str);
//    return *this;
//  }
//  _Myt& replace(size_type _Off, size_type _N0, const _Myt& _Right, size_type _Roff, size_type _Count) {
//    _str.replace(_Off, _N0, _Right._str, _Roff, _Count);
//    return *this;
//  }
//  _Myt& replace(size_type _Off, size_type _N0, const _Elem* _Ptr, size_type _Count) {
//    _str.replace(_Off, _N0, _Ptr, _Count);
//    return *this;
//  }
//  _Myt& replace(size_type _Off, size_type _N0, const _Elem* _Ptr) {
//    _str.replace(_Off, _N0, _Ptr);
//    return *this;
//  }
//  _Myt& replace(size_type _Off, size_type _N0, size_type _Count, _Elem _Ch) {
//    _str.replace(_Off, _N0, _Count, _Ch);
//    return *this;
//  }
//  _Myt& replace(iterator _First, iterator _Last, const _Myt& _Right) {
//    _str.replace(_First, _Last, _Right._str);
//    return *this;
//  }
//  _Myt& replace(iterator _First, iterator _Last, const _Elem* _Ptr, size_type _Count) {
//    _str.replace(_First, _Last, _Ptr, _Count);
//    return *this;
//  }
//  _Myt& replace(iterator _First, iterator _Last, const _Elem* _Ptr) {
//    _str.replace(_First, _Last, _Ptr);
//    return *this;
//  }
//  _Myt& replace(iterator _First, iterator _Last, size_type _Count, _Elem _Ch) {
//    _str.replace(_First, _Last, _Count, _Ch);
//    return *this;
//  }
//private:
//  StlStringType _str;
//  int32_t _iHashAlgorithmIndex;//index of the hash algorithm to use.
//
//};
//
//
//}//ns BR2
//
//
//#endif
