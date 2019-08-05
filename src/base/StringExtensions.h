/**
*
*    @file StringExtensions.h
*    @date August 5, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __STRINGEXTENSIONS_1565033595646069092_H__
#define __STRINGEXTENSIONS_1565033595646069092_H__

#include "../base/BaseHeader.h"

namespace Game {

/**
* @class StringExtensions
*   @brief This file extends std::string allowing for implicit casts to integral types (doing away with TStr).
*/
class StringExtensions {
public:
    void Test();
};

//
//std::string& operator+=(std::string& str, const int8_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const int16_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const int32_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const int64_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//
//std::string& operator+=(std::string& str, const uint8_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const uint16_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const uint32_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string& str, const uint64_t& rhs) {
//    str = str + rhs;
//    return str;
//}
//
//std::string& operator+=(std::string& str, const float& rhs) {
//    str = str + rhs;
//    return str;
//}
//std::string& operator+=(std::string str, const double& rhs) {
//    str = str + rhs;
//    return str;
//}




}//ns Game



#endif
