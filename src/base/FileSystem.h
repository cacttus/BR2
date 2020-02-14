/**
*  @file FileSystem.h
*  @date August 24, 2013
*  @date January 19, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __FILESYSTEM_14848814891654953164_H__
#define __FILESYSTEM_14848814891654953164_H__

#include "../base/BaseHeader.h"

namespace BR2 {
#ifdef BRO_OS_WINDOWS
typedef unsigned short mode_t;
#endif
/**
*   @class FileInfo
*   @brief Boiled down unix stat() structure.
*/
class FileInfo : public VirtualMemory {
public:
  bool _exists = false;
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
*  @class FileSystem
*  @brief Filesystem functions.
*       In the future a lot of this can be refactored with C++17's std::filesystem
*/
class FileSystem : public VirtualMemory {
  static string_t _strExePath;
  static bool getAllFilesOrDirs(string_t dir, std::vector<string_t>& __out_ dirs, bool bFiles);
  static string_t getFileNameOrDirectoryFromPath(const string_t& name, bool bformatPath = true);
public:
  FileSystem();
  virtual ~FileSystem() override;
  static void init(string_t executablePath);
  static string_t getExecutableFullPath();
  static string_t getExecutableName();
  static void setExecutablePath(string_t st) { _strExePath = st; }
  static string_t getExecutableDirectory();
  static string_t combinePath(string_t a, string_t b);
  static string_t formatPath(string_t p);
  static bool isUNC(string_t p);
  static string_t getRoot(string_t p);
  static string_t getRootedPath(string_t p);
  static bool pathIsAbsolute(string_t p);
  static bool createFile(const string_t& filename, bool truncate = false, bool bLog = true);
  static void createFileIfFileDoesNotExist(string_t& filename, bool bAlsoCreateDirectoryPath = true);
  static bool createDirectorySingle(string_t& dirName);//Note: this usually fails. windwos is dubm
  static bool createDirectoryRecursive(string_t dirName);
  static bool directoryExists(string_t dirName);
  static bool fileExists(string_t filename);
  static bool getAllFiles(string_t dir, std::vector<string_t>& __out_ files);
  static bool getAllDirs(string_t dir, std::vector<string_t>& __out_ dirs);

  static time_t getLastModifyTime(string_t& location);
  static string_t getCurrentDirectory();
  static void setCurrentDirectory(string_t str);

  static bool deleteFile(string_t filename);
  static bool deleteAllFiles(string_t dir, std::vector<string_t>& vecFileExts);
  static bool deleteDirectory(string_t dirname, std::vector<string_t>& vecFileExts);
  static bool deleteDirectoryRecursive(string_t dirName, std::vector<string_t>& vecFileExts);

  static string_t getExtensionPartOfFileName(const string_t& fileName);
  static string_t getFilePartOfFileName(const string_t& fileName);
  static string_t getPathFromPath(const string_t& name, bool bformatPath = true); // - Gets the file name with extension from a / path 
  static string_t getFileNameFromPath(const string_t& name, bool bformatPath = true);
  static string_t getDirectoryFromPath(const string_t& name, bool bformatPath = true);

  static bool isFile(string_t fileOrDirPath);
  static bool isDir(string_t fileOrDirPath);

  static string_t getScreenshotFilename();
  static string_t replaceInvalidCharsFromFilename(const string_t& __in_ fnIn, char replaceChar = '_', bool bIgnoreSlashes = false);

  static void SDLFileFree(char*& pOutData);
  static int SDLFileRead(string_t fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false);
  static int SDLFileWrite(string_t fname, char* pData, size_t _iDataSizeBytes);
  static string_t getFilePath(string_t name);

  static FileInfo getFileInfo(string_t loc);
  static void findFile(string_t searchRoot, string_t name, FileInfo& __out info);
};

}//ns Game



#endif
