/**
*
*    @file Unpack.h
*    @date January 8, 2018
*    @author Derek Page
*
*    © 2018 
*
*/
#pragma once
#ifndef __UNPACK_15154667791053603878_H__
#define __UNPACK_15154667791053603878_H__

#include "../base/BaseHeader.h"

namespace Game {
/**
*    @class Package
*    @brief package of all our data in the filesystem on the end of the EE
*/
class Package : public VirtualMemory {
public:
	//class FileBuffer : public VirtualMemory {
	//public:
	//	char* _pBytes = nullptr;
	//	uint32_t _iLength = 0;
 //       FileBuffer();
 //       virtual ~FileBuffer() override;
	//	void alloc(int32_t iLength);
 //       void freeData();
	//	std::string toString();
	//	bool loadFileDisk(std::string loc);
	//};
	class FileEntry {
	public:
		std::string _strUnformattedPath;
		std::string _strLoc;
		int32_t _iOff;
		int32_t _iSize;
	};
private:
    std::vector<FileEntry*> _vecEntries;
    int32_t _iExeLenBytes = 0;
    std::string _strExeLoc;
    int32_t _iTableLenBytes = 0;
    bool _bIsPackage = false;

    bool loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull);
    int32_t parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off);
    std::string parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off);
    FileEntry* getEntry(std::string fileLoc);
    bool loadExe(std::shared_ptr<BinaryFile> fb);
public:
	Package();
    virtual ~Package() override;

    bool isPackage(){ return _bIsPackage; }
    bool getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull = false);
    void build(std::string exeLoc);
    t_string debugPrint();
	bool fileExists(t_string file);
    time_t getFileModifyTime(t_string str);


};

}//ns Game



#endif
