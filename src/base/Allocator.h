/**
*
*    @file Allocator.h
*    @date 8/17/2010
*    @author Derek Page
*
*    © 2011
*
*
*/
#pragma once

#ifndef __MM_ALLOCATOR_02760239750123894729035_H__
#define __MM_ALLOCATOR_02760239750123894729035_H__

#include "../base/BaseHeader.h"

namespace Game {

/**
*    @class Allocator
*    24 (48) bytes
*
*    @brief Base class that manages the allocation of user-defined memory blocks.
*/
template < class Tx >
class Allocator : public VirtualMemory {
public:
    typedef Tx dataType;
    typedef size_t size_type;
protected:
    Tx* _pT;
private:
    size_t    _num;    // number of elements (T's) allocated
    size_t    _lastNum;    // last allocation.
    size_t    _allocSize;        // - The last size allocated or 0
    size_t    _lastSize;        // - The acutal size used by the last allocation    
public:
    FORCE_INLINE Allocator();
    FORCE_INLINE Allocator(size_t allocCount, Tx* ptFill = nullptr);
    FORCE_INLINE OVERRIDES ~Allocator() OVERRIDE;
    FORCE_INLINE void init();

    FORCE_INLINE bool isAllocated() const { return (_pT != NULL); } // - Return true if this Allocator instance has allocated data.
    FORCE_INLINE size_t byteSize() const { return tsize()*_num; }  // - Get the size in bytes of this buffer
    FORCE_INLINE size_t count() const { return _num; }    // - Get # of items allocated
    FORCE_INLINE size_t tsize() const { return sizeof(Tx); }
    FORCE_INLINE void alloca(size_t count = 0);    // - Allocate
    FORCE_INLINE void realloca(size_t newCount);   // - Reallocate
    FORCE_INLINE void reallocBytes(size_t newSize);
    FORCE_INLINE void grow(size_t itemCount, size_t itemOffset = -1);    // - Grow by some elements
    FORCE_INLINE void inflate(size_t itemCount, size_t itemOffset, size_t staticAllocationCount = -1);    // - Same as grow but we do not call realloc. instead buffer copies a range of elmeents 
    FORCE_INLINE void shrink(size_t itemCount, size_t itemOffset = -1);    // - Shrink the buffer - i.e. cut data from it.
    FORCE_INLINE void deflate(size_t itemCount, size_t itemOffset, size_t staticAllocationCount = -1);    // - Same as shrink but we do not call realloc. instead buffer copies a range of elmeents 
    FORCE_INLINE void erase(size_t start, size_t count);
    FORCE_INLINE void dealloc();  // - Deallocate

    FORCE_INLINE void insert(size_t off, const Tx& pt); // Inserts an item into the buffer.
    FORCE_INLINE void swapChunk(size_t iOff, size_t iCount, const Tx* pDataToSwap, size_t iCountToSwap);
    FORCE_INLINE void splice(size_t off, size_t count); //Same as shrink - why we have this?

    FORCE_INLINE Tx* ptr() { return _pT; } // return a pointer
    FORCE_INLINE char* charPtr() { return (char*)_pT; }
    FORCE_INLINE Tx* ptrOff(size_t offset);
    FORCE_INLINE const Tx* constptr() const { return (const Tx*)_pT; }  // return a pointer
    FORCE_INLINE Tx& at(size_t i); // return element [i]
    FORCE_INLINE Tx& at(size_t i) const; // return element [i]
    FORCE_INLINE void fill(Tx& base);   // - Fill with an item.
    FORCE_INLINE void fillb(Tx base);   // - Fill with an item.
    FORCE_INLINE void appendToEnd(const Allocator<Tx>* in);    // Merges this allocator's data with another, appending the second to the end.
    FORCE_INLINE void appendToBeg(const Allocator<Tx>* in);    // Merges this allocator's data with another, appending the second to the beg.
    FORCE_INLINE void copyFrom(const Tx* in, size_t item_count, size_t my_off = 0, size_t other_off = 0);  // copy a buffer into this buffer.
    FORCE_INLINE void copyFrom(const Allocator<Tx>* in);
    FORCE_INLINE void copyTo(const Tx* in, size_t item_count, size_t my_off = 0, size_t other_off = 0);  // copy a buffer into this buffer.
    FORCE_INLINE void zeroMemory();

    FORCE_INLINE Tx& operator[](size_t index);  // - Returns the item at[]
    FORCE_INLINE bool operator==(Tx* ptr);
    FORCE_INLINE bool operator!=(Tx* ptr);
    FORCE_INLINE bool operator==(Allocator<Tx>& ptr);    //NOTE: IN COMPARISON YOU MUST USE REFERENTIAL OPERATOR OR THIS WILL DEALLOC
    FORCE_INLINE bool operator!=(Allocator<Tx>& ptr);

    FORCE_INLINE void operator=(const Allocator<Tx>& rhs);



};
//////////////////////////////////////////////////////////////////////////
template< class Tx >
FORCE_INLINE Allocator<Tx>::Allocator() :
    _pT(NULL),
    _num(0),
    _lastNum(0),
    _allocSize(0),
    _lastSize(0)
{
}
template< class Tx >
FORCE_INLINE Allocator<Tx>::Allocator(size_t allocCount, Tx* ptFill ) :
    _pT(NULL),
    _num(0),
    _lastNum(0),
    _allocSize(0),
    _lastSize(0)
{
    alloca(allocCount);
    if(ptFill != nullptr) {
        copyFrom(ptFill, allocCount);
    }
}
template< class Tx >
FORCE_INLINE Allocator<Tx>::~Allocator()
{
    dealloc();
}
//////////////////////////////////////////////////////////////////////////
template< class Tx >
class GenericBuffer : public Allocator<Tx> {};

//////////////////////////////////////////////////////////////////////////
template< class Tx >
FORCE_INLINE void Allocator<Tx>::init()
{
    _pT = (NULL);
    _num = (0);
    _lastNum = (0);
    _allocSize = (0);
    _lastSize = (0);
}
template < class Tx >
void Allocator<Tx>::zeroMemory()
{
    if(isAllocated())
    {
        memset((void*)ptr(), 0, byteSize());
    }
}
/**
*    @fn append()
*    @brief Append another allocator to this allocator.
*/
template < class Tx >
void Allocator<Tx>::appendToEnd(const Allocator<Tx>* in)
{

    if(in->count() == 0)
        return;

    //**Note: there was a problem here where we kept trying to append the same buffer to
    // the end of itself but it was growing and so the algorithm messed up
    size_t oldCountThis = count();
    size_t oldCountIn = in->count();
    grow((t_int64)oldCountIn);
    copyFrom(in->constptr(), oldCountIn, oldCountThis);
}
template < class Tx >
void Allocator<Tx>::appendToBeg(const Allocator<Tx>* in)
{
    size_t oldCount = count();
    size_t oldInCount = in->count();    //must cache if we're copying a buffer to itself
    grow((t_int64)oldInCount);
    copyFrom((this)->constptr(), oldCount, oldInCount);
    copyFrom(in->constptr(), oldInCount, 0);
}
/**
*    @fn
*    @brief gets the pointer with the given offset.
*/
template < class Tx >
Tx* Allocator<Tx>::ptrOff(size_t offset)
{
    AssertOrThrow2(offset<byteSize());
    return (Tx*)(_pT + offset);
}
/**
*   @fn copyFrom()
*   @brief Copy from somewhere.
*    @note This assumes that the buffer is already allocated.
*   @param in - the buffer to copy from
*/
template < class Tx >
void Allocator<Tx>::copyFrom(const Allocator<Tx>* in)
{

    //Buffer Mismatch Check.
    //We COULD just truncate the copy, but this is probably safer as programmers should do that.
    if(in->byteSize() > _allocSize) {
        throw new Exception("Buffer Overflow",  __LINE__ , __FILE__ );
    }

    memmove((void*)(_pT), in->constptr(), _allocSize);
}
/**
*   @fn copyFrom()
*   @brief Copy from somewhere.
*    @note This assumes that the buffer is already allocated.
*   @param in - the buffer to copy from
*   @param item_count - number of bytes to copy
*   @param my_off - offset in this buffer (in items)
*/
template < class Tx >
void Allocator<Tx>::copyFrom(const Tx* other_in, size_t in_item_count, size_t my_off, size_t other_off)
{
    AssertOrThrow2(other_in != nullptr);
    //ptr(), endSize, itemOffset, itemOffset+numItemsToRemove
    size_t other_size = in_item_count * sizeof(Tx);
    size_t my_size = _allocSize - (my_off*tsize());

    if(other_size > my_size)
        throw 0;

    memmove((void*)(_pT + my_off), (void*)(other_in + other_off), other_size);
}
template < class Tx >
void Allocator<Tx>::copyTo(const Tx* other_in, size_t in_item_count, size_t my_off, size_t other_off)
{
    AssertOrThrow2(other_in != nullptr);

    size_t other_size = in_item_count * sizeof(Tx);
    size_t my_size = _allocSize - (my_off*tsize());

    if(my_off + in_item_count > count()) //>= ??
        throw 0;

    memmove((void*)(other_in + other_off), (void*)(_pT + my_off), other_size);
}
/**
*    @fn fill()
*    @brief Fill the buffer with a value.
*/
template < class Tx >
void
Allocator<Tx>::fill(Tx& base) {
    //memset((void*)_pT,
    for(size_t i = 0; i < count(); ++i)
        *(_pT + i) = base;
}
template < class Tx >
void
Allocator<Tx>::fillb(Tx base) {
    //memset((void*)_pT,
    for(size_t i = 0; i < count(); ++i)
        *(_pT + i) = base;
}

/**
*    @fn operator[]
*/
template < class Tx >
Tx&
Allocator<Tx>::operator[](size_t index)
{
    DebugValidate(index < count()); // Exception(" [MM] Allocator Index out of range.",__LINE__,__FILE__);
    DebugValidate(isAllocated());

    return (Tx&)(*(_pT + index));
}
/**
*    Perform a pointer comparison with the input pointer.
*/
template < class Tx >
bool Allocator<Tx>::operator==(Tx* in_ptr)
{
    return this->ptr() == in_ptr;
}
template < class Tx >
bool Allocator<Tx>::operator!=(Tx* in_ptr)
{
    return this->ptr() != in_ptr;
}
template < class Tx >
bool Allocator<Tx>::operator==(Allocator<Tx>& in_ptr)
{
    return this->ptr() == in_ptr.ptr();
}
template < class Tx >
bool Allocator<Tx>::operator!=(Allocator<Tx>& in_ptr)
{
    return this->ptr() != in_ptr.ptr();
}
/**
*  @fn
*  @brief assigns and deep copies rhs allocator
*/
template < class Tx >
void Allocator<Tx>::operator=(const Allocator<Tx>& rhs)
{
    _alloca(rhs.count());
    copyFrom(&rhs);
}
/**
*    @fn at()
*/
template < class Tx >
Tx& Allocator<Tx>::at(size_t index)
{
    //#if _DEBUG
    AssertOrThrow2(index < this->count());
    //#endif
    return (Tx&)(*(_pT + index));
}
/**
*    @fn alloc()
*    @brief Allocate a list of blocks.
*/
template < class Tx >
void Allocator<Tx>::alloca(size_t count)
{
    dealloc();

    if(count == 0)
        return;
    _lastSize = _allocSize;
    _lastNum = _num;

    _allocSize = count * tsize();
    _num = count;

    _pT = (Tx*)malloc(_allocSize);// GameMemoryManager::allocBlock(_allocSize);
}
/**
*    @fn realloc()
*    @remarks call realloc when the buffer is too small.
*/
template < class Tx >
void Allocator<Tx>::realloca(size_t newCount)
{
    if(_pT == NULL)
    {
        alloca(newCount);
        return;
    }

    _lastSize = _allocSize;
    _lastNum = _num;

    _allocSize = newCount * tsize();
    _num = newCount;

    _pT = (Tx*)realloc(_pT, _allocSize);// static_cast<Tx*>(GameMemoryManager::reallocBlock(_pT, _allocSize));
}
/**
*    @fn dealloc()
*    @brief Deallocate the memory.
*/
template < class Tx >
void Allocator<Tx>::dealloc()
{
    if (_pT != NULL){
        free(_pT);
    }
        //Globals::getMemoryManager()->freeBlock(_pT);
    _pT = NULL;
    _lastNum = _num;
    _num = 0;
    _lastSize = _allocSize;
    _allocSize = 0;
}
/**
*    @fn reallocBytes()
*    @remarks call realloc when the buffer is too small.
*/
template < class Tx >
void Allocator<Tx>::reallocBytes(size_t newSize)
{
    AssertOrThrow2(_pT);
    AssertOrThrow2(!(newSize%tsize()));//-must be aligned. Allocator allocates items not bytes

    _lastSize = _allocSize;
    _lastNum = _num;

    _allocSize = newSize;
    _num = newCount / tsize();
    _pT = (Tx*)realloc(_pT, _allocSize);// static_cast<Tx*>(MemoryManager::getSingleton()->mm_realloc(_pT, _allocSize));
}
/**
*    @fn grow()
*    @brief Grow the size of the buffer by some elements.
*    @details This is analogous to calling realloc() on a block of data.
*        @param itemOffset (in Tx Items) defaults to the end of the buffer.
*        @param add_count defaults to 1
*/
template < class Tx >
void Allocator<Tx>::grow(size_t add_count, size_t itemOffset)
{
    Allocator<Tx> newData;
    size_t newCount;

    if(itemOffset == -1)
        itemOffset = (size_t)count();

    //realloc(this->count()+add_count);
    AssertOrThrow2(itemOffset <= (size_t)count());

    if(add_count == 0)
        return;

    newCount = (t_int64)count() + add_count;

    newData.alloca(newCount);

    size_t countA = itemOffset;
    size_t countB = (size_t)count() - itemOffset;

    newData.copyFrom(ptr(), countA, 0, 0);
    newData.copyFrom(ptr(), countB, countA + add_count, countA);

    realloca(newCount);
    copyFrom(&newData);
    newData.dealloc();
}
/**
*    @fn    shrink()
*    @brief Shrink the buffer by some size.
*    @details Analogous to calling realloc() on a buffer with fewer elements.
*        @param itemOffset (in Tx Items) defaults to the beginning of the buffer.
*        @param numItemsToRemove defaults to 1
*/
template < class Tx >
void Allocator<Tx>::shrink(size_t itemOffset, size_t numItemsToRemove)
{
    size_t newCount;
    Allocator<Tx> newData;

    if(itemOffset == -1)
        itemOffset = 0;

    AssertOrThrow2(numItemsToRemove >= 0);

    //Avoid shrinking data past the end of the buffer.
    if(numItemsToRemove > (size_t)count())
        numItemsToRemove = (size_t)count() - itemOffset;

    if(numItemsToRemove == 0)
        return;

    newCount = count() - numItemsToRemove;

    // - we should allow zero for memory copies
    // so if count is zero it shouldn't matter.

    newData.alloca(newCount);

    size_t endSize = count() - (itemOffset + numItemsToRemove);

    newData.copyFrom(ptr(), itemOffset, 0, 0);
    newData.copyFrom(ptr(), endSize, itemOffset, itemOffset + numItemsToRemove);

    realloca(newCount);
    copyFrom(&newData);
    newData.dealloc();
}
/**
*    @fn inflate
*    @brief This method increases the size of the preallocated buffer.
*    @details It is an optimization for grow() since we don't
*    have to reallocate the buffer to do this.  The buffer must be allocated already to hold
*    the new number of elements or an exception will be thrown.
*    @note This does not alter the count() property.
*/
template < class Tx >
void Allocator<Tx>::inflate(size_t expandCount,
                               size_t itemOffset,
                               size_t staticAllocationCount //The count that you are using in the buffer. This is not the actual buffer size.
                               )
{
    //Allocator<Tx> newData;  

    if(expandCount == 0)
        return;
    if(staticAllocationCount < 0)
        staticAllocationCount = count();

    AssertOrThrow2(expandCount > 0);
    AssertOrThrow2((itemOffset + expandCount) <= (size_t)count());
    AssertOrThrow2((staticAllocationCount + expandCount) <= (size_t)count());

    size_t moveSizeBytes = (staticAllocationCount - itemOffset)*tsize();

    Tx* pDst = _pT + itemOffset + expandCount;
    Tx* pSrc = _pT + itemOffset;

    //We must use memmove if buffers overlap. Do not use memcpy.
    memmove((char*)pDst, (char*)pSrc, moveSizeBytes);
}
/**
*    @fn    deflate()
*    @brief Copies memory from one part of the buffer to another.
*    @details This is an optimization for shrink() and does not call realloc.
*       Also it does not modify the size of the buffer.
*   ESSENTIALLY this SHIFTS the items from ITEM OFFSET to the ALLOOCATION COUNT down to the DEFLATE COUNT
*       move items at [start+count, allocation_count] to (start)
*    It is assumed the buffer can hold the given number of elements.
*   @param staticAllocationCount if <0 we use the size of the buffer (20160521)
*    @note the count() property is not changed during deflate()
*/
template < class Tx >
void Allocator<Tx>::deflate(size_t deflateCount,
                                size_t itemOffset,
                                size_t staticAllocationCount //The count that you are using in the buffer. This is not the actual buffer size.
                                )
{
    //Allocator<Tx> newData;  

    if(staticAllocationCount < 0)
        staticAllocationCount = count();

    if(deflateCount > itemOffset)
        deflateCount = itemOffset;    //make sure we don't move negative. same as if (itemOffset- deflateCount < 0)

    if(deflateCount == 0)
        return;

    if(deflateCount >= staticAllocationCount)
        return;    // No elements, and so nothign to copy. same as if(staticAllocationCount - deflateCount <0)

    size_t moveSizeBytes = (staticAllocationCount - deflateCount)*tsize();

    Tx* pDst = _pT + itemOffset - deflateCount;
    Tx* pSrc = _pT + itemOffset;

    //We must use memmove if buffers overlap. Do not use memcpy.
    memmove((char*)pDst, (char*)pSrc, moveSizeBytes);
}
/**
*    @fn erase()
*    @brief Shifts elements from start+count down to start erasing any data in between.
*   @param count - number to erase after start
*/
template < class Tx >
void Allocator<Tx>::erase(size_t start, size_t count)
{
    deflate(start, start + count, -1);
}

//*    @fn Splice()
//*    @brief Remove elements from the buffer.
//*    @brief needs debugging
template < class Tx >
void Allocator<Tx>::splice(size_t iAt, size_t iCount)
{
//    if((iAt + iCount > count()) || (!count()) || (!iCount))
//        throw std::exception("Buffer over-write, size failure, or invalid param, in ManagedBuffer::splice.");
//
//    Tx* oldPtr, *newPtr;
//    size_t newCount;
//    size_t newSize;
//    size_t nCount = count();
//
//    // - Error check
//    if((int)nCount - (int)iCount < 0)
//        throw 0;//BroThrowException("[MB] Tried to allocate negative elements.",__LINE__,__FILE__);
//
//    if(iAt + iCount > count())
//        throw 0;//BroThrowException("[MB] Tried to splice elements beyond the range of the buffer.",__LINE__,__FILE__);
//
//    _lastNum = _num;
//    _lastSize = size();
//
//    // - New Params
//    newCount = nCount - iCount;
//    newSize = newCount * sizeof(Tx);
//    oldPtr = _pT;
//    newPtr = (Tx*)GameMemoryManager::allocBlock(newSize);
//
//    // Define Head and Tail sections of splice
//    if(newCount > 0)
//    {
//        size_t offA = 0;
//        size_t segACount = iAt;
//        size_t offB = sizeof(Tx)*(iAt + iCount);
//        size_t segBCount = nCount - (iAt + iCount);
//
//        AssertOrThrow2((int)nCount - (iAt + iCount) >= 0);    //make sure we're not negative
//
//        size_t sizA = sizeof(Tx)*segACount;
//        size_t sizB = sizeof(Tx)*segBCount;
//
//        AssertOrThrow2(sizA + sizB == newSize);
//
//        //Note - 2m version of safe_copy must be passed in valid block pointers.
//        GameMemoryManager::safeCopy2m(newPtr, 0, _pT, offA, sizA);
//        GameMemoryManager::safeCopy2m(newPtr, sizA, _pT, offB, sizB);
//    }
//
//    // - Free old data
//    GameMemoryManager::freeBlock(_pT);
//
//#ifdef _MM_DEBUG_CHECK_BLOCKS
//    GameMemoryManager::accessViolationCheck();
//#endif
//    // Assign new vars
//    _pT = newPtr;
//    allocSize = lastSize = newSize;
//    nCount = newCount;
}
/**
*    @fn
*    @brief Insert an item into the buffer
*/
template < class Tx >
void Allocator<Tx>::insert(size_t iAt, const Tx& pData)
{
//    Tx* oldPtr, *newPtr;
//    size_t newCount;
//    size_t newSize;
//    size_t _count = 1;
//
//    _lastNum = _num;
//    _lastSize = size();
//
//    //if(_at+_count > size())
//    //    throw new Exception("[MB] Tried to splice elements beyond the range of the buffer.",__LINE__,__FILE__);
//    //
//    // - New Params
//    newCount = nCount + _count;
//    newSize = newCount*sizeof(Tx);
//    oldPtr = _pT;
//    newPtr = (Tx*)GameMemoryManager::allocBlock(newSize);
//
//    // Define Head and Tail sections of splice
//    if(newCount > 0)
//    {
//        size_t offA = 0;
//        size_t segACount = iAt;
//        size_t offB = sizeof(Tx)*iAt;
//        size_t segBCount = nCount - iAt;
//
//        AssertOrThrow2(iAt + 1 <= newSize);
//
//        if((int)nCount - iAt < 0)    //make sure we're not negative
//            throw 0;//throw new Exception("[MB] Tried to allocate negative elements.", __LINE__, __FILE__);
//
//        size_t sizA = sizeof(Tx)*segACount;
//        size_t sizB = sizeof(Tx)*segBCount;
//        size_t sizC = sizeof(Tx)*_count;
//
//        AssertOrThrow2(sizA + sizB + sizC == newSize);
//
//        AssertOrThrow2(offB == sizA);
//        //Note - 2m version of safe_copy must be passed in valid block pointers.
//        GameMemoryManager::safeCopy2m(newPtr, 0, _pT, offA, sizA);
//        GameMemoryManager::safeCopy2u(newPtr, sizA, &pData, 0, sizC);
//        GameMemoryManager::safeCopy2m(newPtr, sizA + sizC, _pT, offB, sizB);
//    }
//
//    // - Free old data
//    GameMemoryManager::freeBlock(_pT);
//
//#ifdef _MM_DEBUG_CHECK_BLOCKS
//    GameMemoryManager::accessViolationCheck();
//#endif
//
//    // Assign new vars
//    _pT = newPtr;
//    allocSize = lastSize = newSize;
//    nCount = newCount;
}
/**
*    @fn swapChunk
*    @brief Inserts Allocator data into this mm_lalocator's data
*/
template < class Tx >
void Allocator<Tx>::swapChunk(size_t iOff, size_t iCountToRemove, const Tx* pDataToSwap, size_t iCountToSwap)
{
    if(iCountToRemove > iCountToSwap)
    {
        shrink(iOff, iCountToRemove -iCountToSwap);
    }
    else if(iCountToRemove < iCountToSwap)
    {
        grow(iCountToSwap-iCountToRemove, iOff);
    }
    //else do nothing - data same size

    copyFrom(pDataToSwap, iCountToSwap, iOff, 0);
}




}//ns game



#endif
