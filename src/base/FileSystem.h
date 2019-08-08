/**
*    @file FileSystem.h
*    @date August 24, 2013
*    @author Derek Page
*
*    © 2013
*
*
*    @file FileSystem.h
*    @date January 19, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __FILESYSTEM_14848814891654953164_H__
#define __FILESYSTEM_14848814891654953164_H__

#include "../base/BaseHeader.h"

namespace Game {
#ifdef BRO_OS_WINDOWS
typedef unsigned short mode_t;
#endif
/**
*   @class FileInfo
*   @brief Boiled down unix stat() structure.
*/
class FileInfo : public VirtualMemory {
public:
    bool _exists;
    std::string _originalPath;
    std::string _rootedPath;
    time_t _accssed;//Last read time
    time_t _modified;//Last write time
    time_t _status;//Last status change time
    mode_t _protection; // should be mode_t
    int64_t _size; //size in bytes
    FileInfo() {}
    virtual ~FileInfo() {}
};
/**
*    @class FileSystem
*    @brief Filesystem functions.
*       In the future a lot of this can be refactored with C++17's std::filesystem
*/
class FileSystem : public VirtualMemory {
    static t_string _strExePath;
    STATIC bool getAllFilesOrDirs(t_string dir, std::vector<t_string>& __out_ dirs, bool bFiles);
    STATIC t_string getFileNameOrDirectoryFromPath(const t_string& name, bool bformatPath = true);
public:
    FileSystem();
    virtual ~FileSystem() override;
    static t_string getExecutableFullPath();
    static t_string getExecutableName();
    static void setExecutablePath(t_string st) { _strExePath = st; }
    static t_string getExecutableDirectory();
    STATIC t_string combinePath(t_string a, t_string b);
    STATIC t_string formatPath(t_string p);
    STATIC bool isUNC(t_string p);
    STATIC t_string getRoot(t_string p);
    STATIC t_string getRootedPath(t_string p);
    STATIC bool pathIsAbsolute(t_string p);
    STATIC bool createFile(const t_string& filename, bool truncate = false, bool bLog = true);
    STATIC void createFileIfFileDoesNotExist(t_string& filename, bool bAlsoCreateDirectoryPath = true);
    STATIC bool createDirectorySingle(t_string& dirName);//Note: this usually fails. windwos is dubm
    STATIC bool createDirectoryRecursive(t_string dirName);
    STATIC bool directoryExists(t_string dirName);
    STATIC bool fileExists(t_string filename);
    STATIC bool getAllFiles(t_string dir, std::vector<t_string>& __out_ files);
    STATIC bool getAllDirs(t_string dir, std::vector<t_string>& __out_ dirs);

    STATIC time_t getLastModifyTime(t_string& location);
    STATIC t_string getCurrentDirectory();
	STATIC void setCurrentDirectory(t_string str);

    STATIC bool deleteFile(t_string filename);
    STATIC bool deleteAllFiles(t_string dir, std::vector<t_string>& vecFileExts);
    STATIC bool deleteDirectory(t_string dirname, std::vector<t_string>& vecFileExts);
    STATIC bool deleteDirectoryRecursive(t_string dirName, std::vector<t_string>& vecFileExts);

    STATIC t_string getExtensionPartOfFileName(const t_string& fileName);
    STATIC t_string getFilePartOfFileName(const t_string& fileName);
    STATIC t_string getPathFromPath(const t_string& name, bool bformatPath = true); // - Gets the file name with extension from a / path 
    STATIC t_string getFileNameFromPath(const t_string& name, bool bformatPath = true);
    STATIC t_string getDirectoryFromPath(const t_string& name, bool bformatPath = true);
    STATIC t_string appendCachePathToFile(const t_string& file);

    STATIC bool isFile(t_string fileOrDirPath);
    STATIC bool isDir(t_string fileOrDirPath);

    STATIC t_string getScreenshotFilename();
    STATIC t_string replaceInvalidCharsFromFilename(const t_string& __in_ fnIn, char replaceChar = '_', bool bIgnoreSlashes = false);

    static void SDLFileFree(char*& pOutData);
    static int SDLFileRead(std::string fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false);
    static int SDLFileWrite(std::string fname, char* pData, size_t _iDataSizeBytes);
    static std::string getFilePath(std::string name);

    static FileInfo getFileInfo(std::string loc);

}; 

}//ns Game



#endif
