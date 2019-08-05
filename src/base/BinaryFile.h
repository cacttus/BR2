//5/22/2010

#pragma once
#ifndef __BUFFERED_FILE_H__
#define __BUFFERED_FILE_H__
#include "../base/BaseHeader.h"
#include "../base/RetCode.h"
#include "../base/Allocator.h"
#include "../math/Vec3x.h"
#include "../math/Matrix4x4.h"
namespace Game {

/**
*    @class BinaryFile
*    @brief Binary parsed file which is loaded into a static buffer in memory via the memory manager.
*/
class BinaryFile : public VirtualMemory { 
public:
    enum { file_eof = -1 };
    typedef int32_t t_filepos;
private:
    Allocator<char> _data;
    size_t iFilePos;
    //char* _pData = nullptr;;
    //size_t _iLength;  //length in bytes

    //char* getBuffer() { return _data.ptr(); }
    //size_t getBufferSize() { return _data.count();  }
    //char* getBufferPtrOff(size_t i) { return (char*)(_data.ptr() + i);  }
    void validateRead(size_t outSize, size_t readCount);
    RetCode read(const char* buf, size_t count, size_t bufcount, size_t offset);
    RetCode write(const char* buf, size_t count, size_t bufcount, size_t offset);
public:
    Allocator<char>& getData() { return _data; }

    BinaryFile();
    BinaryFile(size_t buffer_size);
    virtual ~BinaryFile() override;

    //char* ptr();
    //size_t getSize();    // - Returns the size of the buffered file.
    void reallocBuffer(size_t i) { _data.alloca(i); }

    size_t pos(){return iFilePos;}
    int8_t at();
    int8_t at( t_filepos _pos );

    void rewind();    // - Rewinds the file pointer.
    int32_t get();
    t_string getTok();    // - Returns a string token
    t_string getTokSameLineOrReturnEmpty();    // - Returns a string token only if the token is on the same line.
    bool eatWs();        // - Eat Whitespace (ALSO EATS \n, \r and spaces!!!)
    bool eatWsExceptNewline();        // - Eat Whitespace Does not eat \n
    bool eatLine();    // - Eats past \n and \r
    bool eatTo(int8_t c);
    bool eof();


    // - Read
    //int32_t readInt32();
    //t_string readString(size_t length);
    //t_byte readByte();
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
    //  void readBytes(void* in, size_t in_sz, size_t read_sz);

    // - Write
    //void writeInt32(int32_t in);
    //void writeString(t_string& in);
    //void writeByte(t_byte in);
    //void writeBytes(t_byte* in, size_t length);
        
    //- File Operations
    bool loadFromDisk( t_string fileLoc, bool bAddNull = false );        // - Read the whole file into the buffer.
    bool loadFromDisk(t_string fileLoc, size_t offset, int32_t length, bool bAddNull = false);        // - Read a part of the file.
    bool writeToDisk(t_string fileLoc);        // - Read a part of the file.
    std::string toString();

};

}//ns game

#endif