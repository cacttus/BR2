/**
*  @file Unpack.h
*  @date January 8, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __UNPACK_15154667791053603878_H__
#define __UNPACK_15154667791053603878_H__

#include "../base/BaseHeader.h"

namespace Game {
class ProjectPackageFileEntry {
public:
  std::string _strUnformattedPath;
  std::string _strLoc;
  int32_t _iOff;
  int32_t _iSize;
};
/**
*  @class Package
*  @brief The game project settings, and filesystem, which may be packed to the end of the game project for convenience.
*  @note The game project root is defined where the config.xml file is placed in your project directory.
*/
class ApplicationPackage : public VirtualMemory {
public:
  ApplicationPackage();
  virtual ~ApplicationPackage() override;

  bool isPackage() { return _bIsPacked; }
  bool getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull = false);
  void build(std::string exeLoc);
  string_t debugPrint();
  bool fileExists(string_t file);
  time_t getLastModifyTime(string_t str);

  void load(string_t file_path);

  string_t makeAssetPath(string_t file);
  string_t makeAssetPath(string_t folder, string_t file);

  string_t getConfigPath() { return _strConfigPath; }
  string_t getAssetsDir() { return _strAssetsDir; }
  string_t getCacheDir() { return _strCacheDir; }
  string_t getIconPath() { return _strIconPath; }
  string_t getShadersDir() { return _strShadersDir; }
  string_t getModelsTextDir() { return _strModelsTextDir; }
  string_t getModelsBinDir() { return _strModelsBinDir; }
  string_t getEnvTexturePath() { return _strEnvTexturePath; }
  string_t getTextureDir() { return _strTextureDir; }

  void userZoom(float amt);
private:
  std::vector<ProjectPackageFileEntry*> _vecEntries;
  int32_t _iExeLenBytes = 0;
  std::string _strExeLoc;
  int32_t _iTableLenBytes = 0;
  bool _bIsPacked = false;

  string_t _strConfigPath = "";
  string_t _strIconPath = "";
  string_t _strAssetsDir = "";
  string_t _strCacheDir = "";
  string_t _strIconPath = "";
  string_t _strShadersDir = "";
  string_t _strModelsTextDir = "";
  string_t _strModelsBinDir = "";
  string_t _strEnvTexturePath = "";
  string_t _strTextureDir = "";

  void makeDefaultPaths();

  bool loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull);
  int32_t parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off);
  std::string parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off);
  ProjectPackageFileEntry* getEntry(std::string fileLoc);
  bool loadExe(std::shared_ptr<BinaryFile> fb);
  void setSz(string_t name, string_t& value, HashMap<XmlConfigEntry> entries);

};

}//ns Game



#endif
