/**
*  @file Unpack.h
*  @date January 8, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __UNPACK_15154667791053603878_H__
#define __UNPACK_15154667791053603878_H__

#include "../base/BaseHeader.h"

namespace BR2 {
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
class ApplicationPackage_Internal;
class ApplicationPackage : public VirtualMemory {
public:
  ApplicationPackage();
  virtual ~ApplicationPackage() override;

  bool getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull = false);
  void build(std::string exeLoc);
  string_t debugPrint();
  bool fileExists(string_t file);
  time_t getLastModifyTime(string_t str);

  void load(string_t file_path);

  string_t makeAssetPath(string_t file);
  string_t makeAssetPath(string_t folder, string_t file);

  bool isPackage();
  string_t getDataFolder();
  string_t getScriptsFolder();
  string_t getIconPath();
  string_t getShadersFolder();
  string_t getSoundsFolder();
  string_t getModelsTextFolder();
  string_t getModelsBinFolder();
  string_t getFontsFolder();
  string_t getEnvTextureFolder();
  string_t getTextureFolder();
  string_t getAppName();
  static string_t getCacheFolder();
  static string_t getEngineConfigFilePath();
  static string_t getDataPath();
private:
  std::unique_ptr<ApplicationPackage_Internal> _pint = nullptr;

};

}//ns BR2



#endif
