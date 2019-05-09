/**
*
*    @file Serializable.h
*    @date July 9, 2016
*    @author Derek Page
*
*    © 2016
*
*
*/
#pragma once
#ifndef __SERIALIZABLE_1468121228429372404_H__
#define __SERIALIZABLE_1468121228429372404_H__

#include "../net/NetHeader.h"

namespace Game {

/**
*    @fn
*    @brief For serializing data structures across the network.

    Do not inherit.  Serializable must be a pure c object.

*/
class Serializable : public DOES_NOT_INHERIT {
public:
//    MUST_OVERRIDE void serialize(DynamicBuffer<char>* pBuf) = 0;
//    MUST_OVERRIDE void deserialize(DynamicBuffer<char>* pBuf, char* __out_ pObject) = 0;
};


}//ns Game



#endif
