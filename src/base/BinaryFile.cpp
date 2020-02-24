#include "../base/BinaryFile.h"
#include "../base/BaseHeader.h"

#include "../base/StringUtil.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/FileSystem.h"
#include "../base/DiskFile.h"

namespace Game {
BinaryFile::BinaryFile() :
  iFilePos(0) {
}
BinaryFile::BinaryFile(size_t buffer_size) :
  iFilePos(0) {
  _data.alloca(buffer_size);
}
BinaryFile::~BinaryFile() {
  _data.dealloc();
}
//char* BinaryFile::ptr() { return getBuffer(); }

//********WRITE***********
//********WRITE***********
//********WRITE***********
//void BinaryFile::writeInt32(int32_t in)
//{
//    *((int32_t*)getBuffer()) = in;
//    iFilePos += sizeof(int32_t);
//}
//void BinaryFile::writeString(t_string& in)
//{
//    memcpy((void*)in.c_str(), (void*)getBuffer(), in.length());
//    iFilePos += in.length();
//}
//void BinaryFile::writeByte(t_byte in)
//{
//    *((t_byte*)getBuffer()) = in;
//    iFilePos += sizeof(t_byte);
//}
//void BinaryFile::writeBytes(t_byte* in, size_t length)
//{
//    if (iFilePos + length > _data.count())
//        BroThrowException("The buffered file tried to write too much data.");
//
//    memcpy((void*)in, (void*)((char*)getBuffer() + iFilePos), length);
//    iFilePos += length;
//}




//*******READ********
//*******READ********
//*******READ********
//int32_t BinaryFile::readFloat() {
//    float data = *((float*)getBuffer());
//
//    iFilePos += sizeof(float);
//
//    return data;
//}
//int32_t BinaryFile::readInt32(){
//    int32_t data = *((int32_t*)getBuffer());
//
//    iFilePos += sizeof(int32_t);
//
//    return data;
//}
//t_string BinaryFile::readString(size_t length){
//    t_string data = t_string((char*)getBuffer(), length);
//
//    iFilePos += length;
//
//    return data;
//}
//t_byte BinaryFile::readByte(){
//    int8_t data = *((t_byte*)getBuffer());
//
//    iFilePos += sizeof(t_byte);
//
//    return data;
//}

//OutSize = size of output buffer we're copying to
//readCount - number of bytes we're reading from this BinaryFile.
void BinaryFile::validateRead(size_t outSize, size_t readCount) {
  //AssertOrThrow2(getBuffer() != NULL);
  //AssertOrThrow2(iFilePos < getBufferSize());

  //size_t offSize = readCount;
  //size_t outBufSize = outSize;
  //if (outSize < readCount)
  //{
  //    BroThrowException(
  //        "Buffer read overrun. Input buffer of size "
  //        , outSize
  //        , " could not hold file of size "
  //        , getBufferSize()
  //    );
  //}

  //size_t myOff = iFilePos + readCount;
  //if (myOff > getBufferSize())
  //{
  //    BroThrowException("Buffer read overrun. Tried to read past end of buffer.");
  //}
}
//void BinaryFile::readToEnd(StaticBuffer* pbuf)
//{
//    AssertOrThrow2(pbuf != NULL);
//    size_t iReadCount = getBuffe r()->byteSize() - iFilePos;
//
//    //Handles most validation
//    validateRead(pbuf->byteSize(), iReadCount);
//
//    readBytes(pbuf->ptr(), pbuf->byteSize(), iReadCount);
//}
/**
*    @fn readBytes
*    @brief Read some bytes from the file in the buffer.
*    @param din [out] - pointer to a data buffer to
*    @param in_sz size of the input buffer din
*    @param read_sz number of bytes to read.
*/
//void BinaryFile::readBytes(void* pOutBuf, size_t in_sz, size_t read_sz)
//{
//    //Handles most validation
//    validateRead(in_sz, read_sz);
//
//    char* pMyPtr = getBufferPtrOff(iFilePos);
//
//    memcpy_s(pOutBuf, in_sz, pMyPtr, read_sz);
//
//    iFilePos += read_sz;
//}
// - Rewind the file pointer
void BinaryFile::rewind() { iFilePos = 0; }

// - Return true if the file is at eof.
bool BinaryFile::eof() { return iFilePos >= _data.count() || iFilePos == -1; }

// - returns 0 on EOF
// - Eat Whitespace (ALSO EATS \n, \r and spaces!!!)
bool BinaryFile::eatWs() {
  while (StringUtil::isWs(at()))    // is char is alphanumeric
    if (get() == -1)
      return 0;    // inc pointer

  return 1;
}
// - same as eatWs except it returns at newline.
bool BinaryFile::eatWsExceptNewline() {
  while (StringUtil::isWsExceptNewline(at()))    // is char is alphanumeric
    if (get() == -1)
      return 0;    // inc pointer

  return 1;
}
// - Eats the line past the carraige return
//
bool BinaryFile::eatLine() {
  if (!eatTo('\n')) return 0;

  while ((at() == '\r') || (at() == '\n'))
    get();

  return 1;
}
// - eats until character (increments buffer pointer)
bool BinaryFile::eatTo(int8_t k) {
  int8_t c;
  while ((c = at()) != k)
    if (get() == -1) return 0;    // inc pointer

  return 1;
}
//- Return the next whitespace separated token
t_string BinaryFile::getTok() {
  t_string ret;

  if (!eatWs())
    return ret;

  int8_t c = at();
  while (!StringUtil::isWs(at()) && !eof()) {
    c = get();

    if (c == -1)
      return ret;//eof

    ret.append(1, c);
  }

  return ret;
}
/// - Returns empty string if we hit a newline character.
t_string BinaryFile::getTokSameLineOrReturnEmpty() {
  t_string ret;

  if (!eatWsExceptNewline())
    return ret;

  int8_t c = at();
  if (c == '\n')
    return "";

  while (!StringUtil::isWs(at()) && !eof()) {
    c = get();

    if (c == -1)
      return ret;//eof

    ret += c;
  }

  return ret;
}
// returns character at pos
int8_t BinaryFile::at() {
  if (eof())
    return -1;
  return (int8_t)(*(getData().ptr() + iFilePos));
}
int8_t  BinaryFile::at(t_filepos _pos) {
  if (eof())
    return -1;
  return (int8_t)(*(getData().ptr() + _pos));
}
// Returns a character or also file_eof
int32_t BinaryFile::get() {
  if (iFilePos == -1 || iFilePos == getData().count())
    return (int32_t)(iFilePos = -1);
  return (int32_t)(*(getData().ptr() + iFilePos++));
}
bool BinaryFile::loadFromDisk(t_string fileLoc, bool bAddNull) {
  return loadFromDisk(fileLoc, 0, -1, bAddNull);
}
bool BinaryFile::loadFromDisk(t_string fileLoc, size_t offset, int64_t length, bool bAddNull) {
  //DiskFile df;
  //df.openForRead(fileLoc);

  //df.close();
  rewind();

  BroLogInfo("Reading File " + fileLoc);

  char* bufRet;
  int64_t size;
  int ret;
  ret = FileSystem::SDLFileRead(fileLoc, bufRet, size, bAddNull);
  if (ret != 0) {
    //Failure
    BroLogError("Failure, could not read file" + fileLoc + " returned " + ret);
    Gu::debugBreak();
    return false;
    // BroThrowException("Failure, could not read file", fileLoc, " returned ", ret);
  }

  if (length == -1) {
    length = size;
  }

  AssertOrThrow2(size <= length);

  _data._alloca(length + 1);
  memcpy(_data.ptr(), bufRet, length);
  FileSystem::SDLFileFree(bufRet);

  *(getData().ptr() + length) = 0;

  return true;
}


//size_t BinaryFile::getSize()
//{
//    AssertOrThrow2(getBuffer() != NULL);
//    return getBufferSize();
//}
std::string BinaryFile::toString() {
  std::string ret = "";
  if (!_data.isAllocated() || _data.count() <= 0) {
    return ret;
  }
  char* b2 = new char[_data.count() + 1];
  memset(b2, 0, _data.count() + 1);
  memcpy(b2, _data.ptr(), _data.count());
  ret.assign(b2, 0, _data.count() + 1);
  delete[] b2;
  return ret;
}
//////////////////////////////////////////////////////////////////////////
void BinaryFile::readBool(bool& val, size_t offset) {
  int8_t b;
  readByte(b, offset);
  val = b > 0;
}
void BinaryFile::readByte(int8_t& val, size_t offset) {
  size_t readSize = sizeof(int8_t);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readInt16(int16_t& val, size_t offset) {
  int32_t readSize = sizeof(int16_t);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readInt32(int32_t& val, size_t offset) {
  int32_t readSize = sizeof(int32_t);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readFloat(float& val, size_t offset) {
  int32_t readSize = sizeof(float);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readInt64(int64_t& val, size_t offset) {
  int32_t readSize = sizeof(int64_t);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readUint32(uint32_t& val, size_t offset) {
  int32_t readSize = sizeof(uint32_t);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readVec2(vec2& val, size_t offset) {
  size_t readSize = sizeof(vec2);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readVec3(vec3& val, size_t offset) {
  size_t readSize = sizeof(vec3);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readVec4(vec4& val, size_t offset) {
  size_t readSize = sizeof(vec4);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readVec3i(ivec3& val, size_t offset) {
  int32_t readSize = sizeof(ivec3);
  read((char*)&val, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
void BinaryFile::readString(std::string& val, size_t offset) {
  int16_t iStringLen = 0;
  readInt16(iStringLen);

  if (iStringLen > 0) {
    char* buf = new char[iStringLen + 1];
    read(buf, iStringLen, iStringLen, iFilePos);
    buf[iStringLen] = 0;//null terminate
    val.assign(buf);
    delete[] buf;
    iFilePos += iStringLen;
  }
  else {
    val = "";
  }
}
void BinaryFile::readMat4(mat4& val, size_t offset) {
  //This must correspond to the same order in mat4..
  readFloat(val._m11);
  readFloat(val._m12);
  readFloat(val._m13);
  readFloat(val._m14);
  readFloat(val._m21);
  readFloat(val._m22);
  readFloat(val._m23);
  readFloat(val._m24);
  readFloat(val._m31);
  readFloat(val._m32);
  readFloat(val._m33);
  readFloat(val._m34);
  readFloat(val._m41);
  readFloat(val._m42);
  readFloat(val._m43);
  readFloat(val._m44);
}
void BinaryFile::read(const char* buf, size_t readSize, size_t offset) {
  read((char*)buf, readSize, readSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += readSize;
}
RetCode BinaryFile::read(const char* buf, size_t count, size_t bufcount, size_t offset) {
  AssertOrThrow2((offset >= 0) || (offset == memsize_max));

  if (count > bufcount) {
    BroThrowException("DataBuffer - out of bounds.");
  }
  if (offset == memsize_max) {
    offset = 0;
  }

  _data.copyTo(buf, count, offset, 0);

  return GR_OK;
}
//////////////////////////////////////////////////////////////////////////
void BinaryFile::writeBool(bool&& val, size_t offset) {
  int8_t b = (val > 0) ? 1 : 0;
  writeByte(std::move(b), offset);
}
void BinaryFile::writeByte(int8_t&& val, size_t offset) {
  size_t writeSize = sizeof(int8_t);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeInt16(int16_t&& val, size_t offset) {
  int32_t writeSize = sizeof(int16_t);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeInt32(int32_t&& val, size_t offset) {
  int32_t writeSize = sizeof(int32_t);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeFloat(const float&& val, size_t offset) {
  int32_t writeSize = sizeof(float);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeInt64(int64_t&& val, size_t offset) {
  int32_t writeSize = sizeof(int64_t);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeUint32(uint32_t&& val, size_t offset) {
  int32_t writeSize = sizeof(uint32_t);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeVec2(vec2&& val, size_t offset) {
  size_t writeSize = sizeof(vec2);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeVec3(vec3&& val, size_t offset) {
  size_t writeSize = sizeof(vec3);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeVec4(vec4&& val, size_t offset) {
  size_t writeSize = sizeof(vec4);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeVec3i(ivec3&& val, size_t offset) {
  int32_t writeSize = sizeof(ivec3);
  write((char*)&val, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
void BinaryFile::writeString(std::string&& val, size_t offset) {
  writeInt16((int16_t)val.length());

  if (val.length() > 0) {
    write(val.c_str(), val.length(), val.length(), iFilePos);
    iFilePos += val.length();
  }
}
void BinaryFile::writeMat4(mat4&& val, size_t offset) {
  //This must correspond to the same order in mat4..
  writeFloat(std::move(val._m11));
  writeFloat(std::move(val._m12));
  writeFloat(std::move(val._m13));
  writeFloat(std::move(val._m14));
  writeFloat(std::move(val._m21));
  writeFloat(std::move(val._m22));
  writeFloat(std::move(val._m23));
  writeFloat(std::move(val._m24));
  writeFloat(std::move(val._m31));
  writeFloat(std::move(val._m32));
  writeFloat(std::move(val._m33));
  writeFloat(std::move(val._m34));
  writeFloat(std::move(val._m41));
  writeFloat(std::move(val._m42));
  writeFloat(std::move(val._m43));
  writeFloat(std::move(val._m44));
}
void BinaryFile::write(const char* buf, size_t writeSize, size_t offset) {
  write(buf, writeSize, writeSize, offset == memsize_max ? iFilePos : offset);
  iFilePos += writeSize;
}
RetCode BinaryFile::write(const char* buf, size_t count, size_t bufcount, size_t offset) {
  AssertOrThrow2((offset >= 0) || (offset == memsize_max));

  if (count > bufcount) {
    BroThrowException("DataBuffer - out of bounds.");
  }
  if (offset == memsize_max) {
    offset = 0;
  }

  //Dynamically reallocate.
  if (_data.count() < offset + count) {
    _data.realloca(offset + count);
  }
  _data.copyFrom(buf, count, offset, 0);

  return GR_OK;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool BinaryFile::writeToDisk(t_string fileLoc) {
  if (FileSystem::SDLFileWrite(fileLoc, _data.ptr(), _data.count()) == 0) {
    return true;
  }
  return false;
  //DiskFile df;
  //df.openForWrite(DiskLoc(fileLoc), FileWriteMode::Truncate);
  //df.write((char*)_data.ptr(), _data.count());
  //df.close();
}


}//ns game