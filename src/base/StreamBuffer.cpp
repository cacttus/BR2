#include "../base/StreamBuffer.h"


namespace BR2 {
//////////////////////////////////////////////////////////////////////////
StreamBuffer::StreamBuffer(int32_t chunkSizeBytes) :
    _iChunkSizeBytes(chunkSizeBytes)
{
    AssertOrThrow2(_iChunkSizeBytes > 0);
    clear();
}
StreamBuffer::~StreamBuffer()
{
    clear();
}
void StreamBuffer::clear()
{
    _iAddCountBytes = 0;
    _data.dealloc();
}
RetCode StreamBuffer::write(
    const char* bytes
    , size_t len
    , int32_t offset_in
)
{
    size_t offset;

    AssertOrThrow2((offset_in >= 0) || (offset_in == -1));

    if (offset_in < 0)
        offset = _iAddCountBytes;
    else
        offset = offset_in;

    //Base Add
    _iAddCountBytes += len;
    checkToGrow();

    _data.copyFrom(bytes, len, offset);

    return GR_OK;
}
RetCode StreamBuffer::read(
    char* buf
    , size_t len
    , int64_t buflen
    , int32_t offset //Offset in BUF.
)
{
    AssertOrThrow2(_data.count() >= (offset + len));
    AssertOrThrow2(buflen >= (offset + len));

    _data.copyTo(buf, len, 0, offset);

    _iAddCountBytes -= len;
    checkToShrink();

    return GR_OK;
}
void StreamBuffer::checkToGrow()
{
    if (!_data.isAllocated()) {
        _data._alloca(_iChunkSizeBytes);
    }

    //If we are less than the current add count, then this means we're allocating
    // a new set of data WHEN we are FULL - we don't pre-allocate the next batch until we actually need it.
    // If you want to pre-allocate the next batch of memory right when the GrowBufferBase is at it's limit
    // change this comparison to <=
    while (_data.count() < _iAddCountBytes) {
        _data.grow((int32_t)_iChunkSizeBytes);
    }
}
void StreamBuffer::checkToShrink()
{
    if (!_data.isAllocated()) {
        _data._alloca(_iChunkSizeBytes);
    }

    //If we are less than the current add count, then this means we're allocating
    // a new set of data WHEN we are FULL - we don't pre-allocate the next batch until we actually need it.
    // If you want to pre-allocate the next batch of memory right when the GrowBufferBase is at it's limit
    // change this comparison to <=
    int32_t m1 = _data.count() / _iChunkSizeBytes;
    size_t m2 = _data.count() / _iAddCountBytes;

    if (m1 > m2 && m2 > 0)
    {
        _data.realloca(m2*_iChunkSizeBytes);
    }
}

Allocator<char>* StreamBuffer::getData()
{
    return &_data;
}
void StreamBuffer::shiftOutFirstByte()
{
    if (_data.count() > 0)
    {
        _data.shrink(0, 1);
        _iAddCountBytes--;
    }
}
void StreamBuffer::next(size_t nBytesToAdd)
{
    _iAddCountBytes += nBytesToAdd;
    checkToGrow();
}
string_t StreamBuffer::toString()
{
    string_t ret(_data.ptr(), _data.count());

    return ret;
}
void StreamBuffer::copyFrom(StreamBuffer* rhs)
{
    *(this) = *rhs;
    this->_data = rhs->_data;
}



}//ns BR2
