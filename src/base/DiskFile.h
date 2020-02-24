/**
*  @file DiskFile.h
*  @date September 6, 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __DISKFILE_22684246896097193333998_H__
#define __DISKFILE_22684246896097193333998_H__

#include "../base/IOBase.h"
namespace BR2 {
class DiskFile_Internal;
/**
*  @class DiskFile
*  @brief Basic disk file access, unbuffered. For buffered files see various @cBufferedFile.
*/
class DiskFile : public IOBase<char>, public IFileStateBase {
public:
  DiskFile();
  virtual ~DiskFile();

  STATIC RetCode readAllBytes(t_string loc, Allocator<char>& __out_ outBuf);
  STATIC RetCode writeAllBytes(t_string loc, Allocator<char>& __out_ outBuf);

  size_t getFileSize() const;
  RetCode create(t_string szloc, size_t offset = 0);// - only create a file.
  bool checkEOF();

  virtual RetCode read(char* buf, size_t len, size_t buflen = memsize_max, size_t offset = memsize_max) override; // read into a buffer
  virtual RetCode write(const char* bytes, size_t len, size_t offset = memsize_max) override;

  RetCode openForRead(DiskLoc& szloc);
  //  RetCode read( char* buf, size_t len, size_t offset, size_t buflen=-1 ); // read from an offset into a buffer
  RetCode readTo(char* buf, const t_string& delims, size_t buflen); // - read to a delimiter
  RetCode openForWrite(DiskLoc& szloc, FileWriteMode::e mode = FileWriteMode::e::Truncate);

  RetCode writeStrWithLen(const t_string& str);    // - Write a string with [len][str..]
  RetCode writeStrText(const t_string& str);    // - Write a string with [len][str..]
  RetCode readStr(char* buf, size_t bufLen);    // - Read a string with [len][str..]

  void close();

  //RetCode getWriteStream( std::fstream& newStream );
  RetCode getReadStream(std::fstream& newStream);
protected:
  void setState(file_state s) { state = s; }
  size_t getPointerOffset();
private:
  std::unique_ptr<DiskFile_Internal> _internal = nullptr;
};



}//ns game



#endif
