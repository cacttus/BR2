/**
*  @file IOBase.h
*  @date September 8, 2010
*  @author MetalMario971
* 2016 07 09 Renamed to IOBase
*/
#pragma once
#ifndef __IOFILE_2872614527326632257223491_H__
#define __IOFILE_2872614527326632257223491_H__

#include "../base/BaseHeader.h"
#include "../base/Allocator.h"
#include "../base/RetCode.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"
namespace Game {
/**
*   @class IOBase
*   @brief Abstract class to derive file classes from, this class supports basic static file operations.
*   @remarks An IOFile Doesn't specify weather the file is on disk, in mem, or anywhere else, so
*   it is used as the base class for all file resources.
*
*/
template < class Tx >
class IOBase : public VirtualMemory {
public:
    enum { TSize = sizeof(Tx) };
public:
    IOBase();
    virtual ~IOBase() override;

    MUST_OVERRIDE RetCode write(const Tx* bytes, size_t len, size_t myOff = memsize_max) = 0;
    MUST_OVERRIDE RetCode read(Tx* buf, size_t len, size_t buflen = memsize_max, size_t myOff = memsize_max) = 0;                        // - Read into a buffer, increements the pointer as well.

    VIRTUAL void AssertGoodRead(int length) { }
    VIRTUAL void AssertGoodWrite(int length) { }

    //**Char only template members.
    void readVec3(vec3& val);
    void readVec3i(ivec3& val);
    void readByte(t_byte& val);
    void readInt32(int32_t& val, size_t offset = memsize_max);
    void readInt16(int16_t& val);
    void readInt64(int64_t& val);
    void readUint32(uint32_t& val, size_t offset = memsize_max);

    void writeVec3(vec3& val);
    void writeVec3i(ivec3& val);
    void writeByte(t_byte& val);
    void writeInt16(int16_t& val);
    void writeInt32(int32_t& val, size_t offset = memsize_max);
    void writeInt64(int64_t& val);
    void writeUint32(uint32_t& val, size_t offset = memsize_max);
    void writeStr(const t_string& str);
    void writeTextLine(const t_string& str); //Writes a /n
};
template <class Tx>
IOBase<Tx>::IOBase()
{
}
template <class Tx>
IOBase<Tx>::~IOBase()
{
}

//////////////////////////////////////////////////////////////////////////
//Uniform buffer - base class
template < class Tx >
class IOBufferBase : public IOBase<Tx> {
public:

protected:
    Allocator<Tx>* _pBuffer;

public:
    IOBufferBase();
    IOBufferBase(size_t bufsiz);
    virtual ~IOBufferBase() override;

    Allocator<Tx>* getBuffer() { return _pBuffer; }

    OVERRIDES RetCode write(const Tx* bytes, size_t len, size_t offset = memsize_max);
    OVERRIDES RetCode read(Tx* buf, size_t len, size_t buflen = memsize_max, size_t offset = memsize_max);
};
template < typename Tx >
IOBufferBase<Tx>::IOBufferBase() :
    IOBase<Tx>()
{
    _pBuffer = new Allocator<Tx>();
}
template < typename Tx >
IOBufferBase<Tx>::IOBufferBase(size_t count) :
    IOBase<Tx>()
{
    _pBuffer = new Allocator<Tx>(count);
}
template < typename Tx >
IOBufferBase<Tx>::~IOBufferBase()
{
    DEL_MEM(_pBuffer);
}
template < typename Tx >
RetCode IOBufferBase<Tx>::write(const Tx* pTx, size_t count, size_t offset)
{
    AssertOrThrow2((offset >= 0) || (offset == memsize_max));

    if (offset > count)
        return GR_FAIL;
    if (offset == memsize_max)
        offset = 0;

    _pBuffer->copyFrom(pTx, count, offset, 0);
    return GR_OK;
}
template < typename Tx >
RetCode IOBufferBase<Tx>::read(Tx* buf, size_t count, size_t bufcount, size_t offset)
{
    AssertOrThrow2((offset >= 0) || (offset == memsize_max));

    if (count > bufcount)
        BroThrowException("DataBuffer - out of bounds.");
    if (offset == memsize_max)
        offset = 0;

    _pBuffer->copyTo(buf, count, offset, 0);

    return GR_OK;
}
//////////////////////////////////////////////////////////////////////////
class IFileStateBase {
public:
    enum file_state {
        file_opened_read   // File is open
        , file_opened_write  // File is open
        , file_closed        // File is closed
        , file_created       // File is created but not opened / closed
        , file_empty         // The file does not exist
        , file_error         // There was a critical error in the file and we can not continue.
    };
protected:
    file_state state;
public:
    file_state getState() const { return state; }   // - returns the condition of the file.
    void setState(file_state s) { state = s; }
    IFileStateBase() : state(file_state::file_closed) { }
};
//////////////////////////////////////////////////////////////////////////
class IOFileBuffer : public IOBufferBase<char>, public IFileStateBase {
public:
    IOFileBuffer() { state = file_empty; }
    virtual ~IOFileBuffer() override {}
};







}//ns game



#endif
