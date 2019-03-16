#include "../base/IOBase.h"
#include "../base/TStr.h"

namespace Game {;
void IOBase<char>::readVec3(vec3& val)
{
    int32_t readSiz;
    readSiz = sizeof(vec3);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, memsize_max);
}
void IOBase<char>::readByte(t_byte& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_byte);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, memsize_max);
}
void IOBase<char>::readInt16(t_int16& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_int16);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, memsize_max);
}
void IOBase<char>::readInt32(int32_t& val, size_t offset)
{
    int32_t readSiz;
    readSiz = sizeof(int32_t);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, offset);
}
void IOBase<char>::readInt64(t_int64& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_int64);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, memsize_max);
}
void IOBase<char>::readUint32(t_uint32& val, size_t offset)
{
    int32_t readSiz;
    readSiz = sizeof(t_uint32);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, offset);
}
void IOBase<char>::readVec3i(ivec3& val)
{
    int32_t readSiz;
    readSiz = sizeof(Vec3i);
    AssertGoodRead(readSiz);

    read((char*)&val, readSiz, readSiz, memsize_max);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void IOBase<char>::writeVec3i(ivec3& val)
{
    int32_t readSiz;
    readSiz = sizeof(Vec3i);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, memsize_max);
}
void IOBase<char>::writeInt32(int32_t& val, size_t offset)
{
    int32_t readSiz;
    readSiz = sizeof(int32_t);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, offset);
}
void IOBase<char>::writeVec3(vec3& val)
{
    int32_t readSiz;
    readSiz = sizeof(vec3);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, memsize_max);
}
void IOBase<char>::writeByte(t_byte& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_byte);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, memsize_max);
}
void IOBase<char>::writeInt16(t_int16& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_int16);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, memsize_max);
}
void IOBase<char>::writeInt64(t_int64& val)
{
    int32_t readSiz;
    readSiz = sizeof(t_int64);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, memsize_max);
}
void IOBase<char>::writeUint32(t_uint32& val, size_t offset)
{
    int32_t readSiz;
    readSiz = sizeof(t_uint32);
    AssertGoodWrite(readSiz);

    write((char*)&val, readSiz, offset);
}
void IOBase<char>::writeStr(const t_string& str)
{
    write(str.c_str(), str.length(), memsize_max);
}
void IOBase<char>::writeTextLine(const t_string& str)
{
    writeStr((str+"\n").c_str());
}










}//ns game
