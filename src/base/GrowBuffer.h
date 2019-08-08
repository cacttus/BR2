/**
*
*    @file GrowBuffer.h
*    @date January 19, 2016
*    @author Derek Page
*
*    © 2016
*
*
*/
#pragma once
#ifndef __GROWBUFFER_29540295831844359018513_H__
#define __GROWBUFFER_29540295831844359018513_H__

#include "../base/BaseHeader.h"
#include "../base/Allocator.h"
#include "../base/DynamicBuffer.h"

namespace Game {
//Stub for when we add this.
//template < class Tx >
//class DynamicBuffer : public VirtualMemory {
//    Allocator<char> buf;
//public:
//    size_t getAddedItemCount() { return 0; }
//    void add(Tx& t) { }
//    void add(int n) { }
//    void clear() { }
//    DynamicBuffer(uint32_t chk) { }
//    Allocator<char>* getBuffer() { return &buf; }
//};
/**
*    @class GrowBuffer
*    @brief AUtomatically grows when items are added.
*     @details Grows both on access and on add.  So if you index here it will grow to your index.
*    8===D...  8=======D
* GrowBuffer - Private inherit from base in order to hide Byte-level operations
*
*/
template < class Tx >
class GrowBuffer : public DynamicBuffer<Tx> {
public:
    GrowBuffer(uint32_t iChunkSize);
    OVERRIDES ~GrowBuffer() OVERRIDE;

    Tx* ptr();
    Tx* next();
    Tx* cur();
    Tx* at(size_t i);
    size_t count() { return getAddedItemCount(); }
    void add(const Tx& item);
};
//////////////////////////////////////////////////////////////////////////
template < typename Tx >
GrowBuffer<Tx>::GrowBuffer(uint32_t iChunkSize) :
    DynamicBuffer(iChunkSize)
{
}
template < typename Tx >
GrowBuffer<Tx>::~GrowBuffer()
{
}
//////////////////////////////////////////////////////////////////////////
template < typename Tx >
Tx* GrowBuffer<Tx>::ptr()
{
    return (Tx*)getBuffer()->ptr();
}
template < class Tx >
void GrowBuffer<Tx>::add(const Tx& item) 
{
    *(next()) = item;
}
template < typename Tx >
Tx* GrowBuffer<Tx>::next()
{
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    //*THIS CLAS HAS AN MOV OR ALLOC ERROR WHEN CALLING NEXT() 8/20/2017
    DynamicBuffer::add(1);

    //SH: (getAddedByteCount() / TSize - 1)*TSize;
    size_t off = getAddedItemCount();

    Tx* t = (Tx*)(getBuffer()->ptrOff(off));

    return t;
}
template < typename Tx >
Tx* GrowBuffer<Tx>::cur()
{
    //SH: (getAddedByteCount() / TSize - 1)*TSize;
    size_t off = getAddedItemCount();

    Tx* pRet = (Tx*)(getBuffer()->ptrOff(off));

    return pRet;
}
template < typename Tx >
Tx* GrowBuffer<Tx>::at(size_t i)
{
    size_t off = i;
    Tx* pRet = (Tx*)getBuffer()->ptrOff(off);
    return pRet;
}








}//ns game



#endif
