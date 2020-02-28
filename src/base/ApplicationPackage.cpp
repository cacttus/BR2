#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/HashMap.h"
#include "../base/ApplicationPackage.h"
#include "../base/StringUtil.h"
#include "../base/BinaryFile.h"
#include "../base/XmlFile.h"
#include "../base/OperatingSystem.h"
#include "../base/PackageConfiguration.h"
#include <fstream>

namespace BR2 {

#pragma region ApplicationPackage_Internal

class ApplicationPackage_Internal {
public:
  std::vector<ProjectPackageFileEntry*> _vecEntries;
  int32_t _iExeLenBytes = 0;
  std::string _strExeLoc;
  int32_t _iTableLenBytes = 0;
  bool _bIsPacked = false;
  string_t _appName = "MyApp";

  static string_t _strCacheFolderName;
  static string_t _strEngineConfigFileName;
  static string_t _strDataPath;

  string_t _strIconPath = "";
  string_t _strAssetsDir = "";
  string_t _strShadersDir = "";
  string_t _strModelsTextDir = "";
  string_t _strModelsBinDir = "";
  string_t _strEnvTexturePath = "";
  string_t _strTextureDir = "";
  string_t _strScriptsFolder = "";
  string_t _strSoundsFolder = "";
  string_t _strFontsFolder = "";

  void makeDefaultPaths();
  bool loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull);
  int32_t parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off);
  std::string parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off);
  ProjectPackageFileEntry* getEntry(std::string fileLoc);
  bool loadExe(std::shared_ptr<BinaryFile> fb);
  void setSz(string_t name, string_t& value, std::shared_ptr<PackageConfiguration> entries);
  void load(string_t file_path);
};
string_t ApplicationPackage_Internal::_strCacheFolderName = "";
string_t ApplicationPackage_Internal::_strEngineConfigFileName = "";
string_t ApplicationPackage_Internal::_strDataPath = "";
void ApplicationPackage_Internal::makeDefaultPaths() {
  FileSystem::getExecutableDirectory();
  _strAssetsDir = FileSystem::combinePath(FileSystem::getExecutableDirectory(), "/data/");

  _strTextureDir = FileSystem::combinePath(_strTextureDir, "/textures/");
  _strShadersDir = FileSystem::combinePath(_strAssetsDir, "/shaders/");
  _strModelsTextDir = FileSystem::combinePath(_strAssetsDir, "/mob/");
  _strModelsBinDir = FileSystem::combinePath(_strAssetsDir, "/mbi/");
  _strSoundsFolder = FileSystem::combinePath(_strAssetsDir, "/sounds/");
  _strFontsFolder = FileSystem::combinePath(_strAssetsDir, "/fonts/");

  _strIconPath = FileSystem::combinePath(_strAssetsDir, "icon.png");
  _strEnvTexturePath = FileSystem::combinePath(_strTextureDir, "env1_huge.png");
}
bool ApplicationPackage_Internal::loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  //Open executable with fstream
  //then seek to the file, copy file contents to buffer, exit.
  ProjectPackageFileEntry* fe = getEntry(fileLoc);
  if (fe == nullptr) {
    BRThrowException("Failed to get file entry for " + fileLoc);
  }
  std::fstream fs;
  fs.open(_strExeLoc.c_str(), std::ios::in | std::ios::binary);
  if (!fs.good()) {
    fs.close();
    return false;
  }
  fb->reallocBuffer(fe->_iSize);
  size_t iFileOff = _iExeLenBytes + _iTableLenBytes + fe->_iOff;
  fs.seekg(0, std::ios::end);
  size_t iExePackSize = (size_t)fs.tellg();

  if (iFileOff + fe->_iSize > iExePackSize) {
    BRThrowException("ERROR File overrun: size of file is greater than the packed exe size.");
  }

  fs.seekg(iFileOff, std::ios::beg);
  fs.read(fb->getData().ptr(), fe->_iSize);
  fs.close();

  return true;
}
int32_t ApplicationPackage_Internal::parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off) {
  int32_t ret;
  ret = *((int32_t*)(fb->getData().ptr() + off));
  off += sizeof(int32_t);
  return ret;
}
std::string ApplicationPackage_Internal::parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off) {
  int32_t iCount = parseInt32(fb, off);

  char* tmp = new char[iCount + 1];
  memset(tmp, 0, iCount + 1);
  memcpy(tmp, fb->getData().ptr() + off, iCount);
  off += iCount;

  std::string ret;
  ret.assign(tmp);
  delete[] tmp;

  return ret;
}
ProjectPackageFileEntry* ApplicationPackage_Internal::getEntry(std::string fileLoc) {
  std::string locLow = StringUtil::lowercase(FileSystem::formatPath(fileLoc));

  for (ProjectPackageFileEntry* fe : _vecEntries) {
    std::string feLow = StringUtil::lowercase(fe->_strLoc);
    if (feLow.compare(locLow) == 0) {
      return fe;
    }
  }
  return nullptr;
}
bool ApplicationPackage_Internal::loadExe(std::shared_ptr<BinaryFile> fb) {
  return fb->loadFromDisk(_strExeLoc);
}
void ApplicationPackage_Internal::setSz(string_t name, string_t& value, std::shared_ptr<PackageConfiguration> config) {
  bool set = false;
  //Set the given value to the first attribute in the given tag identified by 'name'
  HashMapItem<XmlConfigEntry> ent = config->_entries.find(name);
  if (ent.hasValue()) {
    if (ent.value()._attrs.size() == 1) {
      std::shared_ptr<XmlConfigAttribute> attr = ent.value()._attrs.begin()->second;
      value = attr->_attrib_value;
      set = true;
    }
  }
  if (set == false) {
    BRLogError("Failed to find required Package attribute <" + name + " />");
    Gu::debugBreak();
  }
}
void ApplicationPackage_Internal::load(string_t file_path) {

}

#pragma endregion


#pragma region ApplicationPackage
ApplicationPackage::ApplicationPackage() {
  _pint = std::make_unique<ApplicationPackage_Internal>();
  _pint->makeDefaultPaths();
}
ApplicationPackage::~ApplicationPackage() {
  for (ProjectPackageFileEntry* fe : _pint->_vecEntries) {
    delete fe;
  }
  _pint->_vecEntries.resize(0);
  _pint = nullptr;
}
bool ApplicationPackage::isPackage() { return _pint->_bIsPacked; }
string_t ApplicationPackage::getAssetsFolder() { return _pint->_strAssetsDir; }
string_t ApplicationPackage::getScriptsFolder() { return _pint->_strScriptsFolder; }
string_t ApplicationPackage::getSoundsFolder() { return _pint->_strSoundsFolder; }
string_t ApplicationPackage::getFontsFolder() { return _pint->_strFontsFolder; }
string_t ApplicationPackage::getIconPath() { return _pint->_strIconPath; }
string_t ApplicationPackage::getShadersFolder() { return _pint->_strShadersDir; }
string_t ApplicationPackage::getModelsTextFolder() { return _pint->_strModelsTextDir; }
string_t ApplicationPackage::getModelsBinFolder() { return _pint->_strModelsBinDir; }
string_t ApplicationPackage::getEnvTextureFolder() { return _pint->_strEnvTexturePath; }
string_t ApplicationPackage::getTextureFolder() { return _pint->_strTextureDir; }
string_t ApplicationPackage::getAppName() { return _pint->_appName; }
void ApplicationPackage::load(string_t file_path) {
  //Load a given package.
  std::shared_ptr<PackageConfiguration> ents = XmlFile::getXMLConfiguration(file_path);
  _pint->setSz("TextureFolder", _pint->_strTextureDir, ents);
  _pint->setSz("ShaderFolder", _pint->_strShadersDir, ents);
  _pint->setSz("ModelsTextFolder", _pint->_strModelsTextDir, ents);
  _pint->setSz("ModelsBinFolder", _pint->_strModelsBinDir, ents);
  _pint->setSz("ScriptsFolder", _pint->_strScriptsFolder, ents);

  //Try to create the cache dir.
  //Make sure to check this on IOS
  string_t strCache = FileSystem::formatPath(getCacheFolder());
  BRLogInfo("Creating cache: '" + strCache + "'");
  if (FileSystem::createDirectoryRecursive(strCache) == false) {
    BRThrowException("Failed to create cache folder in '" + strCache + "'.");
  }
}
string_t ApplicationPackage::makeAssetPath(string_t file) {
  string_t ret = FileSystem::combinePath(getAssetsFolder(), file);
  return ret;
}
string_t ApplicationPackage::makeAssetPath(string_t folder, string_t file) {
  string_t fold = FileSystem::combinePath(getAssetsFolder(), folder);
  string_t ret = FileSystem::combinePath(fold, file);
  return ret;
}
void ApplicationPackage::build(std::string exeLoc) {
  _pint->_strExeLoc = exeLoc;
  std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>();
  _pint->loadExe(fb);

  int32_t tmp;
  //the last 4 bytes are the EXE length.
  char sig0 = *(fb->getData().ptr() + (fb->getData().count() - 4));
  char sig1 = *(fb->getData().ptr() + (fb->getData().count() - 3));
  char sig2 = *(fb->getData().ptr() + (fb->getData().count() - 2));
  char sig3 = *(fb->getData().ptr() + (fb->getData().count() - 1));

  _pint->_bIsPacked = (sig0 == 'a' && sig1 == 's' && sig2 == 'd' && sig3 == 'f');
  if (_pint->_bIsPacked) {
    BRLogInfo("Exe is packed.");
  }
  else {
    BRLogInfo("Exe is not packed.");
    return;
  }

  tmp = (int32_t)fb->getData().count() - 8;
  _pint->_iExeLenBytes = _pint->parseInt32(fb, tmp);
  BRLogInfo("ExeLen: " + _pint->_iExeLenBytes);

  //Start parsing at the end fo the exe
  int32_t iByteIdx = _pint->_iExeLenBytes;

  // 8 bytes, table length (total) and num entries
  _pint->_iTableLenBytes = _pint->parseInt32(fb, iByteIdx);
  int32_t iNumEntries = _pint->parseInt32(fb, iByteIdx);
  BRLogInfo("Num Entries: " + iNumEntries);

  for (int32_t iEntry = 0; iEntry < iNumEntries; ++iEntry) {
    ProjectPackageFileEntry* fe = new ProjectPackageFileEntry();
    fe->_strUnformattedPath = _pint->parseStr(fb, iByteIdx);
    fe->_strLoc = FileSystem::formatPath(fe->_strUnformattedPath);
    fe->_iOff = _pint->parseInt32(fb, iByteIdx);
    fe->_iSize = _pint->parseInt32(fb, iByteIdx);
    _pint->_vecEntries.push_back(fe);
  }
}
bool ApplicationPackage::getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  if (fb == nullptr) {
    BRLogError("Buffered file was nullptr, no file was read.  Make sur to initialize fb.");
    Gu::debugBreak();
    return false;
  }
  if (isPackage() == false) {
    return fb->loadFromDisk(fileLoc);
  }
  else {
    return _pint->loadPackedFile(fileLoc, fb, bAddNull);
  }
}
string_t ApplicationPackage::debugPrint() {
  string_t ret;
  ret += "Files:\r\n";
  for (ProjectPackageFileEntry* fe : _pint->_vecEntries) {
    ret += "  Loc:" + fe->_strLoc;
    ret += "  Off:" + fe->_iOff;
    ret += " Size: " + fe->_iSize;
    std::shared_ptr<BinaryFile> tmp = std::make_shared<BinaryFile>();
    if (getFile(fe->_strLoc, tmp)) {
      ret += " Data: " + tmp->toString();
    }
    else {
      ret += " ERROR getting file data.";
    }

  }
  return ret;
}
bool ApplicationPackage::fileExists(string_t file) {
  if (isPackage() == false) {
    return FileSystem::fileExists(file);
  }
  else {
    BRThrowNotImplementedException();
  }
}
time_t ApplicationPackage::getLastModifyTime(string_t str) {
  if (isPackage() == false) {
    return FileSystem::getLastModifyTime(str);
  }
  else {
    return 0;
  }
}
string_t ApplicationPackage::getDataPath() {
  if (StringUtil::isEmpty(ApplicationPackage_Internal::_strDataPath)) {
    ApplicationPackage_Internal::_strDataPath = FileSystem::combinePath(FileSystem::getExecutableDirectory(), "/data/");
  }
  return ApplicationPackage_Internal::_strDataPath;
}
string_t ApplicationPackage::getCacheFolder() {
  if (StringUtil::isEmpty(ApplicationPackage_Internal::_strCacheFolderName)) {
    ApplicationPackage_Internal::_strCacheFolderName = FileSystem::combinePath(getDataPath(), "/cache/");
  }
  return ApplicationPackage_Internal::_strCacheFolderName;
}
string_t ApplicationPackage::getEngineConfigFilePath() {
  if (StringUtil::isEmpty(ApplicationPackage_Internal::_strEngineConfigFileName)) {
    ApplicationPackage_Internal::_strEngineConfigFileName = FileSystem::combinePath(getDataPath(), "config.xml");
  }
  return ApplicationPackage_Internal::_strEngineConfigFileName;
}

#pragma endregion



}//ns BR2
