//5/22/2010

#pragma once
#ifndef __BUFFERED_FILE_H__
#define __BUFFERED_FILE_H__

#include "../base/BaseHeader.h"
#include "../base/RetCode.h"
#include "../base/Allocator.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"

namespace BR2 {
/**
*  @class BinaryFile
*  @brief Binary parsed file which is loaded into a static buffer in memory via the memory manager.
*/
class BinaryFile : public VirtualMemory { 
public:
    enum { file_eof = -1 };
    typedef int32_t t_filepos;
private:
    Allocator<char> _data;
    size_t iFilePos;

    void validateRead(size_t outSize, size_t readCount);
    RetCode read(const char* buf, size_t count, size_t bufcount, size_t offset);
    RetCode write(const char* buf, size_t count, size_t bufcount, size_t offset);
public:
    Allocator<char>& getData() { return _data; }

    BinaryFile();
    BinaryFile(size_t buffer_size);
    virtual ~BinaryFile() override;

    void reallocBuffer(size_t i) { _data.alloca(i); }

    size_t pos(){return iFilePos;}
    int8_t at();
    int8_t at( t_filepos _pos );

    void rewind();    // - Rewinds the file pointer.
    int32_t get();
    string_t getTok();    // - Returns a string token
    string_t getTokSameLineOrReturnEmpty();    // - Returns a string token only if the token is on the same line.
    bool eatWs();        // - Eat Whitespace (ALSO EATS \n, \r and spaces!!!)
    bool eatWsExceptNewline();        // - Eat Whitespace Does not eat \n
    bool eatLine();    // - Eats past \n and \r
    bool eatTo(int8_t c);
    bool eof();

    // - Read
    void readBool(bool& val, size_t offset = memsize_max);
    void readVec2(vec2& val, size_t offset = memsize_max);
    void readVec3(vec3& val, size_t offset = memsize_max);
    void readVec4(vec4& val, size_t offset = memsize_max);
    void readVec3i(ivec3& val, size_t offset = memsize_max);
    void readByte(int8_t& val, size_t offset = memsize_max);
    void readInt32(int32_t& val, size_t offset = memsize_max);
    void readInt16(int16_t& val, size_t offset = memsize_max);
    void readInt64(int64_t& val, size_t offset = memsize_max);
    void readUint32(uint32_t& val, size_t offset = memsize_max);
    void readFloat(float& val, size_t offset = memsize_max);
    void readString(std::string& val, size_t offset = memsize_max);
    void readMat4(mat4& val, size_t offset = memsize_max);
    void read(const char* buf, size_t bufsiz, size_t offset = memsize_max);

    void writeBool(bool&& val, size_t offset = memsize_max);
    void writeVec2(vec2&& val, size_t offset = memsize_max);
    void writeVec3(vec3&& val, size_t offset = memsize_max);
    void writeVec4(vec4&& val, size_t offset = memsize_max);
    void writeVec3i(ivec3&& val, size_t offset = memsize_max);
    void writeByte(int8_t&& val, size_t offset = memsize_max);
    void writeInt32(int32_t&& val, size_t offset = memsize_max);
    void writeInt16(int16_t&& val, size_t offset = memsize_max);
    void writeInt64(int64_t&& val, size_t offset = memsize_max);
    void writeUint32(uint32_t&& val, size_t offset = memsize_max);
    void writeFloat(const float&& val, size_t offset = memsize_max);
    void writeString(std::string&& val, size_t offset = memsize_max);
    void writeMat4(mat4&& val, size_t offset = memsize_max);

    void write(const char* buf, size_t bufsiz, size_t offset = memsize_max);

    //- File Operations
    bool loadFromDisk( string_t fileLoc, bool bAddNull = false );        // - Read the whole file into the buffer.
    bool loadFromDisk(string_t fileLoc, size_t offset, int64_t length, bool bAddNull = false);        // - Read a part of the file.
    bool writeToDisk(string_t fileLoc);        // - Read a part of the file.
   
    std::string toString();

};

}//ns game

#endif