/**
*
*    @file GrowBufferBase.h
*    @date July 9, 2016
*    @author Derek Page
*
*    © 2016 
*
*
*/
#pragma once
#ifndef __GROWBUFFERBASE_14681206432405686591_H__
#define __GROWBUFFERBASE_14681206432405686591_H__

#include "../base/BaseHeader.h"
#include "../library/IOBase.h"

namespace Game {
/**
*    @calss
*    @brief Stream Buffer.  Grows/shrinks
*   ADDS - to the end
*    REMOVES - freom the beginning
*/
class StreamBuffer : public IOBase {
private:
    t_memsize _iAddCountBytes; // bytes in the buffer
    t_memsize _iChunkSizeBytes; // Amount to allocate when running out of room
    mm_allocator<char> _data;

    void checkToGrow();
    void checkToShrink();
    //void checkToGrowOrShrink();

protected:
    t_memsize getAddedByteCount() { return _iAddCountBytes; }

public:

    StreamBuffer(t_int32 chunkSize = 512);
    OVERRIDES ~StreamBuffer() OVERRIDE;

    t_bool getIsEmpty() { return _iAddCountBytes == 0; }

    void copyFrom(StreamBuffer* rhs);
    MUST_OVERRIDE RetCode write(
        const char* bytes
        , t_memsize len
        , t_int32 offset = -1
    ) OVERRIDE;
    MUST_OVERRIDE RetCode read(
        char* buf
        , t_memsize len
        , t_long buflen = -1
        , t_int32 offset = -1
    ) OVERRIDE;

    virtual void clear();
    mm_allocator<char>* getData();
    void shiftOutFirstByte();
    void next(t_memsize allocCount);
    t_string toString();
};

}//ns Game



#endif
