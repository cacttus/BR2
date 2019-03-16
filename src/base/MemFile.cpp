//#include "../mem/mm_allocator.h"
//#include "../library/MemFile.h"
//#include "../library/DiskFile.h"
//#include "../library/FileSystem.h"
//namespace Game {;
////////////////////////////////////////////////////////////////////////////
//MemFile::MemFile(int32_t iChunkSize) :
//    DynamicBuffer(iChunkSize),
//    _off(0)
//{
//}
//MemFile::MemFile(t_string& filePath) : DynamicBuffer(2048)
//{
//    getFromDisk(filePath);
//}
//MemFile::~MemFile()
//{
//}
////////////////////////////////////////////////////////////////////////////
//void MemFile::getDataAsString(t_string& __out_ str)
//{
//    str.assign(getBuffer()->ptr(), getBuffer()->byteSize());
//}
//RetCode MemFile::getFromDisk(DiskLoc loc)
//{
//    DiskFile f;
//    RetCode code;
//    reset();
//
//    code = f.openForRead(loc);
//
//    if (code != GR_OK)
//        return code;
//
//    _off = 0;
//    int size = f.getFileSize();
//
//    //pMyBuf = new StaticBuffer( f.getFileSize() );//(t_char*)GameMemoryManager::allocBlock( length );
//    getBuffer()->alloca(size);
//
//    code = f.read((char*)getBuffer()->ptr(), f.getFileSize());
//
//    if (code != GR_OK)
//        return code;
//
//    f.close();
//
//    //Null terminate the file.  From disk the raw data isn't terminated.
//    //getBuffer()[size] = nullptr;
//
//    return GR_OK;
//}
///**
//*    @fn readStr
//*    @details Read a string
//*
//*/
//RetCode MemFile::readStr(char* buf, size_t bufLen)
//{
//    t_uint32 len;
//    RetCode f;
//
//    if ((f = read((char*)&len, sizeof(t_uint32))) != GR_OK)
//        return f;
//
//    if ((len + 1) > (int32_t)bufLen)
//        return FILE_BUFFEROVERFLOW;
//
//    f = read(buf, len);
//    buf[len] = '\0';
//
//    return f;
//}
///**
//*    Writes this memory file to the disk.
//*/
//RetCode MemFile::writeToDisk(DiskLoc loc, bool append, size_t offset, size_t count, bool bCreateIfDoesNotExist)
//{
//    DiskFile df;
//    RetCode r;
//
//    if (offset == (size_t)-1)
//        offset = 0;
//
//    if (count == (size_t)-1)
//        count = getBuffer()->byteSize();
//
//    if (bCreateIfDoesNotExist == true)
//        FileSystem::createFileIfFileDoesNotExist(loc, true);
//
//    if (_bCompress)
//    {
//        FileSystem::compressFile(getBuffer());
//    }
//
//    if ((r = df.openForWrite(loc, (append ? FileWriteMode::Append : FileWriteMode::Truncate))) != GR_OK)
//        BroThrowException(TStr("File error ", r));
//    if ((r = df.write(((const char*)getBuffer()->ptr()) + offset, count)) != GR_OK)
//        BroThrowException(TStr("File error ", r));
//    df.close();
//
//    if (_bCompress)
//    {
//        //Undo previous compression
//        FileSystem::decompressFile(getBuffer());
//    }
//
//    return r;
//}
//void MemFile::readFromDisk(DiskLoc loc)
//{
//    DiskFile df;
//    RetCode r;
//    size_t fileSize;
//
//    r = df.openForRead(loc);
//    if (r != GR_OK)
//        BroThrowException(TStr("File error ", r));
//
//    fileSize = df.getFileSize();
//
//    if (fileSize == 0)
//        return;
//
//    getBuffer()->alloca(fileSize);
//
//    r = df.read(getBuffer()->ptr(), fileSize);
//
//    if (r != GR_OK)
//        BroThrowException(TStr("File error ", r));
//    df.close();
//
//    if (_bCompress)
//    {
//        FileSystem::decompressFile(getBuffer());
//    }
//}
//
//void MemFile::debugPrint()
//{
//    // std::cout.write(getBuffer()->ptr(),getBuffer()->byteSize());
//}
//size_t MemFile::getFileSize()
//{
//    return getBuffer()->byteSize();
//}
//
//}//ns game
