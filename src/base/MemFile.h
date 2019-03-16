///**
//*
//*    @file MemFile.h
//*    @date August 16, 2011
//*    @author Derek Page
//*
//*    � 2011 
//*
//*
//*/
//#pragma once
//#ifndef __MEMFILE_26648378425827206707318_H__
//#define __MEMFILE_26648378425827206707318_H__
//
//#include "../base/DynamicBuffer.h"
//
//namespace Game {;
///**
//*    @class MemFile
//*    @brief A file loaded into Memory that we can write to dynamically. (that is
//*    it isn't a static block but it grows); this file can also be written
//*    to the disk.
//*
//*/
//class MemFile : public DynamicBuffer<char> {
//protected:
//    int32_t _off;            // - Offset in the file.
//    bool _bCompress = false;
//
//    int32_t getPointerOffset() const { return _off; }
//public:
//    MemFile(t_string& filePath);
//    MemFile(int32_t iChunkSize = 2048);
//    virtual ~MemFile() override;
//    
//    void openForRead() { } //Unused "file-like" methods
//    void openForWrite() { }
//    void close(){ }
//
//    void reset() { _off =0; }
//
//    RetCode writeToDisk(DiskLoc strFileLoc
//                        , bool append = false
//                        , size_t offset = memsize_max
//                        , size_t count = memsize_max
//                        , bool bCreateIfDoesNotExist = true);    // - Output the file to the disk.
//    void readFromDisk(DiskLoc strFileLoc);
//    size_t getFileSize();
//    void getDataAsString(t_string& __out_ str);
//    void debugPrint();
//    RetCode getFromDisk(DiskLoc strFileLoc);// - Load from the disk into the buffer.
//    RetCode readStr(char* buf, size_t bufLen);
//};
//
//
//}//ns game
//
//
//
//#endif
