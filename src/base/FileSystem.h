/**
*  @file FileSystem.h
*  @date August 24, 2013
*  @author MetalMario971
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
*  @class FileInfo
*  @brief Boiled down unix stat() structure.
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
  public:
  FileSystem();
  virtual ~FileSystem() override;
  static t_string getExecutableFullPath();
  static t_string getExecutableName();
  static void setExecutablePath(t_string st) { _strExePath = st; }
  static t_string getExecutableDirectory();
  static t_string combinePath(t_string a, t_string b);
  static t_string formatPath(t_string p);
  static bool isUNC(t_string p);
  static t_string getRoot(t_string p);
  static t_string getRootedPath(t_string p);
  static bool pathIsAbsolute(t_string p);
  static bool createFile(const t_string& filename, bool truncate = false, bool bLog = true);
  static void createFileIfFileDoesNotExist(t_string& filename, bool bAlsoCreateDirectoryPath = true);
  static bool createDirectorySingle(t_string& dirName);//Note: this usually fails. windwos is dubm
  static bool createDirectoryRecursive(t_string dirName);
  static bool directoryExists(t_string dirName);
  static bool fileExists(t_string filename);
  static bool getAllFiles(t_string dir, std::vector<t_string>& __out_ files);
  static bool getAllDirs(t_string dir, std::vector<t_string>& __out_ dirs);

  static time_t getLastModifyTime(t_string& location);
  static t_string getCurrentDirectory();
  static void setCurrentDirectory(t_string str);

  static bool deleteFile(t_string filename);
  static bool deleteAllFiles(t_string dir, std::vector<t_string>& vecFileExts);
  static bool deleteDirectory(t_string dirname, std::vector<t_string>& vecFileExts);
  static bool deleteDirectoryRecursive(t_string dirName, std::vector<t_string>& vecFileExts);

  static t_string getExtensionPartOfFileName(const t_string& fileName);
  static t_string getFilePartOfFileName(const t_string& fileName);
  static t_string getPathFromPath(const t_string& name, bool bformatPath = true); // - Gets the file name with extension from a / path 
  static t_string getFileNameFromPath(const t_string& name, bool bformatPath = true);
  static t_string getDirectoryFromPath(const t_string& name, bool bformatPath = true);
  static t_string appendCachePathToFile(const t_string& file);

  static bool isFile(t_string fileOrDirPath);
  static bool isDir(t_string fileOrDirPath);

  static t_string getScreenshotFilename();
  static t_string replaceInvalidCharsFromFilename(const t_string& __in_ fnIn, char replaceChar = '_', bool bIgnoreSlashes = false);

  static void SDLFileFree(char*& pOutData);
  static int SDLFileRead(std::string fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false);
  static int SDLFileWrite(std::string fname, char* pData, size_t _iDataSizeBytes);
  static std::string getFilePath(std::string name);

  static FileInfo getFileInfo(std::string loc);
private:
  static t_string _strExePath;
  static bool getAllFilesOrDirs(t_string dir, std::vector<t_string>& __out_ dirs, bool bFiles);
  static t_string getFileNameOrDirectoryFromPath(const t_string& name, bool bformatPath = true);
};

}//ns Game



#endif
