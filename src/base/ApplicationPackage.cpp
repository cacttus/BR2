#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../base/StringUtil.h"
#include "../base/BinaryFile.h"
#include "../base/XmlFile.h"
#include "../base/OperatingSystem.h"
#include <fstream>

namespace BR2 {
string_t ApplicationPackage::_strCacheFolderName = "";
string_t ApplicationPackage::_strEngineConfigFileName = "";
string_t ApplicationPackage::_strDataPath = "";

ApplicationPackage::ApplicationPackage() {
  makeDefaultPaths();
}
ApplicationPackage::~ApplicationPackage() {
  for (ProjectPackageFileEntry* fe : _vecEntries) {
    delete fe;
  }
  _vecEntries.resize(0);
}
void ApplicationPackage::load(string_t file_path) {
  //Load a given package.
  std::shared_ptr<PackageConfiguration> ents = XmlFile::getXMLConfiguration(file_path);
  setSz("TextureFolder", _strTextureDir, ents);
  setSz("ShaderFolder", _strShadersDir, ents);
  setSz("ModelsTextFolder", _strModelsTextDir, ents);
  setSz("ModelsBinFolder", _strModelsBinDir, ents);
  setSz("ScriptsFolder", _strScriptsFolder, ents);
}
void ApplicationPackage::setSz(string_t name, string_t& value, std::shared_ptr<PackageConfiguration> config) {
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
    Br2LogError("Failed to find required Package attribute <" + name + " />");
    Gu::debugBreak();
  }
}
void ApplicationPackage::makeDefaultPaths() {
  FileSystem::getExecutableDirectory();
  _strAssetsDir = FileSystem::combinePath(FileSystem::getExecutableDirectory(), "/data/");

  _strTextureDir = FileSystem::combinePath(_strTextureDir, "/tex/");
  _strShadersDir = FileSystem::combinePath(_strAssetsDir, "/shaders/");
  _strModelsTextDir = FileSystem::combinePath(_strAssetsDir, "/mob/");
  _strModelsBinDir = FileSystem::combinePath(_strAssetsDir, "/mbi/");

  _strIconPath = FileSystem::combinePath(_strAssetsDir, "icon.png");
  _strEnvTexturePath = FileSystem::combinePath(_strTextureDir, "env1_huge.png");
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
  _strExeLoc = exeLoc;
  std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>();
  loadExe(fb);

  int32_t tmp;
  //the last 4 bytes are the EXE length.
  char sig0 = *(fb->getData().ptr() + (fb->getData().count() - 4));
  char sig1 = *(fb->getData().ptr() + (fb->getData().count() - 3));
  char sig2 = *(fb->getData().ptr() + (fb->getData().count() - 2));
  char sig3 = *(fb->getData().ptr() + (fb->getData().count() - 1));

  _bIsPacked = (sig0 == 'a' && sig1 == 's' && sig2 == 'd' && sig3 == 'f');
  if (_bIsPacked) {
    Br2LogInfo("Exe is packed.");
  }
  else {
    Br2LogInfo("Exe is not packed.");
    return;
  }

  tmp = (int32_t)fb->getData().count() - 8;
  _iExeLenBytes = parseInt32(fb, tmp);
  Br2LogInfo("ExeLen: " + _iExeLenBytes);

  //Start parsing at the end fo the exe
  int32_t iByteIdx = _iExeLenBytes;

  // 8 bytes, table length (total) and num entries
  _iTableLenBytes = parseInt32(fb, iByteIdx);
  int32_t iNumEntries = parseInt32(fb, iByteIdx);
  Br2LogInfo("Num Entries: " + iNumEntries);

  for (int32_t iEntry = 0; iEntry < iNumEntries; ++iEntry) {
    ProjectPackageFileEntry* fe = new ProjectPackageFileEntry();
    fe->_strUnformattedPath = parseStr(fb, iByteIdx);
    fe->_strLoc = FileSystem::formatPath(fe->_strUnformattedPath);
    fe->_iOff = parseInt32(fb, iByteIdx);
    fe->_iSize = parseInt32(fb, iByteIdx);
    _vecEntries.push_back(fe);
  }
}
bool ApplicationPackage::getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  if (fb == nullptr) {
    Br2LogError("Buffered file was nullptr, no file was read.  Make sur to initialize fb.");
    Gu::debugBreak();
    return false;
  }
  if (isPackage() == false) {
    return fb->loadFromDisk(fileLoc);
  }
  else {
    return loadPackedFile(fileLoc, fb, bAddNull);
  }
}
bool ApplicationPackage::loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  //Open executable with fstream
  //then seek to the file, copy file contents to buffer, exit.
  ProjectPackageFileEntry* fe = getEntry(fileLoc);
  if (fe == nullptr) {

    Br2ThrowException("Failed to get file entry for " + fileLoc);
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
    Br2ThrowException("ERROR File overrun: size of file is greater than the packed exe size.");
  }

  fs.seekg(iFileOff, std::ios::beg);
  fs.read(fb->getData().ptr(), fe->_iSize);
  fs.close();

  return true;
}
int32_t ApplicationPackage::parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off) {
  int32_t ret;
  ret = *((int32_t*)(fb->getData().ptr() + off));
  off += sizeof(int32_t);
  return ret;
}
std::string ApplicationPackage::parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off) {
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
ProjectPackageFileEntry* ApplicationPackage::getEntry(std::string fileLoc) {
  std::string locLow = StringUtil::lowercase(FileSystem::formatPath(fileLoc));

  for (ProjectPackageFileEntry* fe : _vecEntries) {
    std::string feLow = StringUtil::lowercase(fe->_strLoc);
    if (feLow.compare(locLow) == 0) {
      return fe;
    }
  }
  return nullptr;
}
bool ApplicationPackage::loadExe(std::shared_ptr<BinaryFile> fb) {
  return fb->loadFromDisk(_strExeLoc);
}
string_t ApplicationPackage::debugPrint() {
  string_t ret;
  ret += "Files:\r\n";
  for (ProjectPackageFileEntry* fe : _vecEntries) {
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
    BrThrowNotImplementedException();
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
void ApplicationPackage::userZoom(float amt) {
  //nothing
}
string_t ApplicationPackage::getDataPath() {
  if (StringUtil::isEmpty(_strDataPath)) {
    _strDataPath = FileSystem::combinePath(FileSystem::getExecutableDirectory(), "/data/");
  }
  return _strDataPath;
}
string_t ApplicationPackage::getCacheFolder() {
  if (StringUtil::isEmpty(_strCacheFolderName)) {
    _strCacheFolderName = FileSystem::combinePath(getDataPath(), "/cache/");
  }
  return _strCacheFolderName;
}
string_t ApplicationPackage::getEngineConfigFilePath() {
  if (StringUtil::isEmpty(_strEngineConfigFileName)) {
    _strEngineConfigFileName = FileSystem::combinePath(getDataPath(), "config.xml");
  }
  return _strEngineConfigFileName;
}

string_t ApplicationPackage::getFilePathForMobName(string_t mobName, bool bUseBinary) {
  string_t fileExt;
  string_t modelsDir;
  if (bUseBinary) {
    modelsDir = Gu::getAppPackage()->getModelsBinFolder();
    fileExt = ".mbi";
  }
  else {
    modelsDir = Gu::getAppPackage()->getModelsTextFolder();
    fileExt = ".mob";
  }

  string_t folderPath = Gu::getAppPackage()->makeAssetPath(modelsDir, mobName);
  string_t filename = FileSystem::combinePath(folderPath, mobName);
  filename += fileExt;

  return filename;
}
std::shared_ptr<ModelNode> ApplicationPackage::getOrLoadModel(string_t mobName, bool bUseBinary, std::shared_ptr<GLContext> ctx) {
  //t_timeval t0 = Gu::getMicroSeconds();
  std::shared_ptr<ModelNode> ms = nullptr;
  BrThrowNotImplementedException();
  //string_t filename = getFilePathForMobName(mobName, bUseBinary);
  //if (FileSystem::fileExists(filename) == false) {
  //  Br2LogError("Model File does not exist: '" + filename + "'");
  //  if (false) {
  //    //debugPrintAllModelNames();
  //  }
  //  return nullptr;
  //}

  //Br2LogInfo("Loading model '" + mobName + "' from '" + filename + "'..");
  //{
  //  if (bUseBinary) {
  //    MbiFile mf(ctx);
  //    if (mf.loadAndParse(filename) == false) {
  //      Br2LogError("Failed to load model " + mobName + " ");
  //    }
  //    mf.
  //    ms = getModelByName(STRHASH(mobName));
  //  }
  //  else {
  //    MobFile mf(ctx);
  //    mf.loadAndParse(filename);
  //    ms = getModelByName(STRHASH(mobName));
  //  }
  //}
  //Br2LogInfo("..Done. " + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");


  //std::shared_ptr<ModelNode> ms = getModelByName(STRHASH(mobName));
  //if (ms == nullptr) {
  //  string_t filename = getFilePathForMobName(mobName, bUseBinary);
  //  if (FileSystem::fileExists(filename) == false) {
  //    Br2LogError("Model File does not exist: '" + filename + "'");
  //    if (false) {
  //      debugPrintAllModelNames();
  //    }
  //    return nullptr;
  //  }
  //  else {


  //  }
  //}
  return ms;
}
//string_t ApplicationPackage::debugPrintAllModelNames() {
//  string_t strOut = ("Loaded:\n");
//
//  for (std::pair<Hash32, std::shared_ptr<ModelData>> p : _mapModels) {
//    strOut += Stz "   " + p.second->getName() + "\n";
//  }
//  strOut += ("In Dir:\n");
//  std::vector<string_t> vecFiles;
//  string_t mods = Gu::getAppPackage()->getModelsTextFolder();
//  mods = Gu::getAppPackage()->makeAssetPath(mods);
//  FileSystem::getAllDirs(mods, vecFiles);
//  for (string_t file : vecFiles) {
//    strOut += Stz "  " + file + "\n";
//  }
//
//  mods = Gu::getAppPackage()->getModelsBinFolder();
//  mods = Gu::getAppPackage()->makeAssetPath(mods);
//  FileSystem::getAllDirs(mods, vecFiles);
//  for (string_t file : vecFiles) {
//    strOut += Stz "  " + file + "\n";
//  }
//
//  return strOut;
//}
void ApplicationPackage::convertMobToBin(string_t strMobName, bool bOnlyIfNewer, std::string strFriendlyName) {

  BrThrowNotImplementedException();
  //string_t filepathText = getFilePathForMobName(strMobName, false);
  //string_t filepathBin = getFilePathForMobName(strMobName, true);

  //if (FileSystem::fileExists(filepathText) == false) {
  //  Br2LogError("Convert: Could not find mob file " + strMobName);
  //  return;
  //}

  //bool bConvert = true;
  //if (bOnlyIfNewer) {
  //  if (FileSystem::fileExists(filepathText) &&
  //    FileSystem::fileExists(filepathBin)) {
  //    time_t t0 = FileSystem::getLastModifyTime(filepathText);
  //    time_t t1 = FileSystem::getLastModifyTime(filepathBin);
  //    if (t1 > t0) {
  //      bConvert = false;
  //      // BroLogInfo("Convet: Mob ",strMobName," is not newer.");
  //    }
  //  }
  //}

  //if (bConvert) {
  //  unloadModel(strMobName, false);
  //  Br2LogInfo("Convert: Loading MOB " + strMobName);
  //  MobFile mf(getContext());
  //  mf.loadAndParse(filepathText);
  //  for (std::shared_ptr<ModelNode> ms : mf.getModelSpecs()) {
  //    ms->postMobConversion();
  //    ms->setFriendlyName(strFriendlyName);
  //  }

  //  Br2LogInfo("Convert: Saving MOB " + strMobName);
  //  MbiFile mb(getContext());
  //  for (std::shared_ptr<ModelNode> ms : mf.getModelSpecs()) {
  //    mb.getModelSpecs().push_back(ms);
  //  }
  //  mb.save(filepathBin);
  //  unloadModel(strMobName);
  //}
}
//void ApplicationPackage::unloadModel(string_t strMobName, bool bErrorIfFailed) {
//  std::map<Hash32, std::shared_ptr<ModelNode>>::iterator it = _mapModels.find(STRHASH(strMobName));
//
//  if (it == _mapModels.end()) {
//    if (bErrorIfFailed)
//      Br2LogError("Failed to find model " + strMobName + " to unload.");
//  }
//  else {
//    //std::shared_ptr<ModelSpec> ms = it->second;
//   // DEL_MEM(ms);
//
//    _mapModels.erase(it);
//  }
//}














}//ns BR2
