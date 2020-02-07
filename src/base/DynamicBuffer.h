/**
*
*    @file GrowBufferBase.h
*    @date July 9, 2016
*    @author MetalMario971
*
*    © 2016
*
*
*/
#pragma once
#ifndef __GROWBUFFERBASE_14681206432405686591_H__
#define __GROWBUFFERBASE_14681206432405686591_H__

#include "../base/BaseHeader.h"
#include "../base/IOBase.h"


namespace Game {
/**
*   @class DynamicBuffer
*   @details 
*   Grows/shrinks based on the read/writing of data.
*   **No max memory size limit.
*   Effectively calls realloc() when needed.
*   Reading shifts data to the left and shrinks buffer size.  Writing grows the buffer to the right.
*   *Adds to the end
*   *Removes from the beginning
*/
template < class Tx >
class DynamicBuffer : public IOBufferBase<Tx> {
private:
    size_t _iAddCountItems; // bytes in the buffer
    size_t _iChunkSizeItems; // Amount to allocate when running out of room

    void checkToGrow(size_t numToAdd);
    void checkToShrink(size_t numToRemove);

protected:
    size_t getAddedItemCount() { return _iAddCountItems; }
    size_t getNumChunksNeeded(size_t nItems) ;
    size_t getNumUsedChunks() ;

public:
    DynamicBuffer(int32_t chunkSizeItems);
    OVERRIDES ~DynamicBuffer() OVERRIDE;

    bool getIsEmpty() { return _iAddCountItems == 0; }

    void copyFrom(DynamicBuffer<Tx>* rhs);
    OVERRIDES RetCode write(const Tx* bytes, size_t len, size_t offset = memsize_max) OVERRIDE;
    OVERRIDES RetCode read(Tx* buf, size_t len, size_t buflen = memsize_max, size_t offset = memsize_max) OVERRIDE;

    virtual void clear();
    Allocator<Tx>* getData();
    void shiftOutFirstElement();
    void add(size_t nItemsToAdd); //Add items to the end
    t_string toString();
};
//////////////////////////////////////////////////////////////////////////
template < class Tx >
DynamicBuffer<Tx>::DynamicBuffer(int32_t chunkSizeItems) :
    _iChunkSizeItems(chunkSizeItems)
{
    AssertOrThrow2(_iChunkSizeItems > 0);
    clear();
}
template < class Tx >
DynamicBuffer<Tx>::~DynamicBuffer()
{
}
template < class Tx >
void DynamicBuffer<Tx>::clear()
{
    _iAddCountItems = 0;
    getBuffer()->dealloc();
}
template < class Tx >
RetCode DynamicBuffer<Tx>::write(
    const Tx* bytes
    , size_t count
    , size_t offset_in
)
{
    size_t offset;

    if (offset_in == memsize_max)
        offset = _iAddCountItems;
    else
        offset = offset_in;
    
    checkToGrow(count);

    RetCode gr = IOBufferBase<Tx>::write(bytes, count, offset);

    return gr;
}
template < class Tx >
RetCode DynamicBuffer<Tx>::read(
    Tx* buf
    , size_t count
    , size_t buflen // in items
    , size_t offset // offset in items
)
{
    AssertOrThrow2(getBuffer()->count() >= (offset + count));
    AssertOrThrow2(buflen >= (offset + count));

    RetCode gr = IOBufferBase<Tx>::read(buf, count, buflen, offset);

    checkToShrink(count);

    return gr;
}
template < class Tx >
void DynamicBuffer<Tx>::checkToGrow(size_t numToAdd)
{
    if (!getBuffer()->isAllocated())
        getBuffer()->_alloca(_iChunkSizeItems);

    size_t needed = getNumChunksNeeded(_iAddCountItems + numToAdd);
    size_t used = getNumUsedChunks();
    size_t chunksToAdd = (needed - used);

    if(chunksToAdd>0)
    {
        getBuffer()->grow(chunksToAdd * _iChunkSizeItems, used * _iChunkSizeItems);
    }

    _iAddCountItems += numToAdd;
}
template < class Tx >
void DynamicBuffer<Tx>::checkToShrink(size_t numToRemove)
{
    if (!getBuffer()->isAllocated())
        getBuffer()->_alloca(_iChunkSizeItems);

    size_t nUsed = getNumUsedChunks();
    size_t numToShrink = _iChunkSizeItems - nUsed;
    
    //Shrinking must retain at least chunksize items when allocated.
    if(numToShrink > 0)
    {
        getBuffer()->shrink(nUsed * _iChunkSizeItems, numToShrink * _iChunkSizeItems);
    }

    _iAddCountItems -= numToRemove;
}
template < class Tx >
Allocator<Tx>* DynamicBuffer<Tx>::getData()
{
    return &_data;
}
template < class Tx >
void DynamicBuffer<Tx>::shiftOutFirstElement()
{
    if (getBuffer()->count() > 0)
    {
        getBuffer()->shrink(0, 1);
        _iAddCountItems--;
    }
}
template < class Tx >
void DynamicBuffer<Tx>::add(size_t nItemsToAdd)
{
    // - Adds items
    checkToGrow(nItemsToAdd);
}
template < class Tx >
t_string DynamicBuffer<Tx>::toString()
{
    t_string ret(getBuffer()->ptr(), getBuffer()->count()*TSize);
    return ret;
}
template < class Tx >
void DynamicBuffer<Tx>::copyFrom(DynamicBuffer<Tx>* rhs)
{
    AssertOrThrow2(rhs!=NULL);
    if(getBuffer()->byteSize() != rhs->getBuffer()->byteSize())
    {
        getBuffer()->dealloc();
        getBuffer()->alloca(rhs->getBuffer()->count());
    }
    getBuffer()->copyFrom(rhs->getBuffer());
}
template < class Tx >
size_t DynamicBuffer<Tx>::getNumChunksNeeded(size_t nItems)
{
    size_t n = nItems / _iChunkSizeItems; //Full Chunks
    n += (nItems%_iChunkSizeItems)>0; // Remainder of a chunk
    return n;
}
template < class Tx >
size_t DynamicBuffer<Tx>::getNumUsedChunks()
{
    return getNumChunksNeeded(_iAddCountItems);
}









}//ns Game



#endif
