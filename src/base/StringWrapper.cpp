//#include "../base/StringWrapper.h"
//
//
//namespace Game {;
//string_t::~string_t()
//{ 
//    //_str.~basic_string(); 
//}
//
//void StlUtils::aw_deallocate(void* ptr) {
//  GameMemoryManager::freeBlock(ptr);
//}
//
//void* StlUtils::aw_allocate(t_memsize count) {
//  return GameMemoryManager::allocBlock(count);
//}
//
////Hash32 t_string::updateHash()
////{
////    if(_hashFunc==HASH_FNV_32)
////        _iHash32 = Hash::computeStringHash32bit(*this);
////    else if(_hashFunc==HASH_FNV_64)
////        BroThrowException(" [String] 64 bit hash not avail. ");
////        //_iHash64 = Hash::computeStringHash64bit(*this);
////    else 
////        BroThrowException(" [String] Invalid string hash parameters specified");
////
////    _eHashState = StringHashState::HASH_VALID;
////    return _iHash32;
////}
//
//}//ns game
