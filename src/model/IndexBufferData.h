/**
*
*    @file IndexBufferData.h
*    @date September 26, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __INDEXBUFFERDATA_3255418442216532730025046_H__
#define __INDEXBUFFERDATA_3255418442216532730025046_H__

#include "../model/ModelHeader.h"

namespace Game {;
/**
*    @class IndexBufferData
*    @brief stores indexes in a generic buffer with either a 16 or 32 bit index format.
*
    Note: We only use 32 bit indexes across the board, this is more for compatibility.
*/
class IndexBufferData : public VirtualMemory {
    Allocator<v_index32>* _pIndexes = nullptr;
public:
    IndexBufferData();
    virtual ~IndexBufferData() override ;
    void allocate(size_t count);
    void deallocate();
    v_index32& i32(size_t index);
    size_t count() const;
    void copyFrom(std::shared_ptr<IndexBufferData> rhs);
    void copyFrom(const v_index32* fb, size_t countt);
    Allocator<v_index32>* getBuf() { return _pIndexes; }
    void appendToEnd(IndexBufferData* fb);
    void* ptr();
};

}//ns game



#endif
