#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/Package.h"
#include "../base/StringUtil.h"
#include "../base/BinaryFile.h"

namespace Game {
Package::Package() {
  makeDefaultPaths();
}
Package::~Package() {
  for (ProjectPackageFileEntry* fe : _vecEntries) {
    delete fe;
  }
  _vecEntries.resize(0);
}
void Package::makeDefaultPaths() {
  FileSystem::getExecutableDirectory();
  _strAssetsDir = FileSystem::combinePath(FileSystem::getExecutableDirectory(), "/data/");

  _strTextureDir = FileSystem::combinePath(_strTextureDir, "/tex/");
  _strCacheDir = FileSystem::combinePath(_strAssetsDir, "/cache/");
  _strShadersDir = FileSystem::combinePath(_strAssetsDir, "/shaders/");
  _strModelsTextDir = FileSystem::combinePath(_strAssetsDir, "/mob/");
  _strModelsBinDir = FileSystem::combinePath(_strAssetsDir, "/mbi/");

  _strConfigPath = FileSystem::combinePath(_strAssetsDir, "config.xml");
  _strIconPath = FileSystem::combinePath(_strAssetsDir, "icon.png");
  _strEnvTexturePath = FileSystem::combinePath(_strTextureDir, "env1_huge.png");
}
string_t Package::makeAssetPath(string_t file) {
  string_t ret = FileSystem::combinePath(getAssetsDir(), file);
  return ret;
}
string_t Package::makeAssetPath(string_t folder, string_t file) {
  string_t fold = FileSystem::combinePath(getAssetsDir(), folder);
  string_t ret = FileSystem::combinePath(fold, file);
  return ret;
}
void Package::build(std::string exeLoc) {
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
    BroLogInfo("Exe is packed.");
  }
  else {
    BroLogInfo("Exe is not packed.");
    return;
  }

  tmp = (int32_t)fb->getData().count() - 8;
  _iExeLenBytes = parseInt32(fb, tmp);
  BroLogInfo("ExeLen: " + _iExeLenBytes);

  //Start parsing at the end fo the exe
  int32_t iByteIdx = _iExeLenBytes;

  // 8 bytes, table length (total) and num entries
  _iTableLenBytes = parseInt32(fb, iByteIdx);
  int32_t iNumEntries = parseInt32(fb, iByteIdx);
  BroLogInfo("Num Entries: " + iNumEntries);

  for (int32_t iEntry = 0; iEntry < iNumEntries; ++iEntry) {
    ProjectPackageFileEntry* fe = new ProjectPackageFileEntry();
    fe->_strUnformattedPath = parseStr(fb, iByteIdx);
    fe->_strLoc = FileSystem::formatPath(fe->_strUnformattedPath);
    fe->_iOff = parseInt32(fb, iByteIdx);
    fe->_iSize = parseInt32(fb, iByteIdx);
    _vecEntries.push_back(fe);
  }
}
bool Package::getFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  if (fb == nullptr) {
    BroLogError("Buffered file was nullptr, no file was read.  Make sur to initialize fb.");
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
bool Package::loadPackedFile(std::string fileLoc, std::shared_ptr<BinaryFile> fb, bool bAddNull) {
  //Open executable with fstream
  //then seek to the file, copy file contents to buffer, exit.
  ProjectPackageFileEntry* fe = getEntry(fileLoc);
  if (fe == nullptr) {

    BroThrowException("Failed to get file entry for " + fileLoc);
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
    BroThrowException("ERROR File overrun: size of file is greater than the packed exe size.");
  }

  fs.seekg(iFileOff, std::ios::beg);
  fs.read(fb->getData().ptr(), fe->_iSize);
  fs.close();

  return true;
}
int32_t Package::parseInt32(std::shared_ptr<BinaryFile> fb, int32_t& off) {
  int32_t ret;
  ret = *((int32_t*)(fb->getData().ptr() + off));
  off += sizeof(int32_t);
  return ret;
}
std::string Package::parseStr(std::shared_ptr<BinaryFile> fb, int32_t& off) {
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
ProjectPackageFileEntry* Package::getEntry(std::string fileLoc) {
  std::string locLow = StringUtil::lowercase(FileSystem::formatPath(fileLoc));

  for (ProjectPackageFileEntry* fe : _vecEntries) {
    std::string feLow = StringUtil::lowercase(fe->_strLoc);
    if (feLow.compare(locLow) == 0) {
      return fe;
    }
  }
  return nullptr;
}
bool Package::loadExe(std::shared_ptr<BinaryFile> fb) {
  return fb->loadFromDisk(_strExeLoc);
}
string_t Package::debugPrint() {
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
bool Package::fileExists(string_t file) {
  if (isPackage() == false) {
    return FileSystem::fileExists(file);
  }
  else {
    BroThrowNotImplementedException();
  }
}

time_t Package::getLastModifyTime(string_t str) {
  if (isPackage() == false) {
    return FileSystem::getLastModifyTime(str);
  }
  else {
    return 0;
  }
}



}//ns Game
