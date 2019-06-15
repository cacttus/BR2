/**
*
*    @file DiskFile.h
*    @date September 6, 2010
*    @author Derek Page
*
*    © 2011
*
*
*/
#pragma once
#ifndef __DISKFILE_22684246896097193333998_H__
#define __DISKFILE_22684246896097193333998_H__

#include "../base/IOBase.h"
namespace Game {

/**
*   @class DiskFile
*   @brief Basic disk file access, unbuffered. For buffered files see various @cBufferedFile.
*
*    TODO: in the future we might want to use the SDLFileRead &c for reading files.
*
*/
class DiskFile : public IOBase<char>, public IFileStateBase {
private:
    size_t off; // - offset in the file
    DiskLoc loc;  // - location
    std::fstream pStream; // - The saved file stream.
    size_t filesize;
protected:
    void setState(file_state s) { state = s; }
    size_t getPointerOffset() { return off; }
public:
    DiskFile();
    virtual ~DiskFile();

    STATIC RetCode readAllBytes(t_string loc, Allocator<char>& __out_ outBuf);
    STATIC RetCode writeAllBytes(t_string loc, Allocator<char>& __out_ outBuf);

    size_t getFileSize() const;
    RetCode create(t_string szloc, size_t offset = 0);// - only create a file.
    bool checkEOF();

    OVERRIDES RetCode read(char* buf, size_t len, size_t buflen = memsize_max, size_t offset = memsize_max) OVERRIDE; // read into a buffer
    OVERRIDES RetCode write(const char* bytes, size_t len, size_t offset = memsize_max) OVERRIDE;

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
};



}//ns game



#endif
