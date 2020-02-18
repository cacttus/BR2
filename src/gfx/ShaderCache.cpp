#include "../base/DiskFile.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/Gu.h"
#include "../base/Oglerr.h"
#include "../base/GLContext.h"

#include "../gfx/ShaderCache.h"
#include "../gfx/ShaderSubProgram.h"
#include "../gfx/ShaderBase.h"


namespace BR2 {
GLProgramBinary::GLProgramBinary(ShaderCache* cc, size_t binLength) {
  _pShaderCache = (cc);
  _binaryLength = (binLength);
  _binaryData = (NULL);
  _compileTime = (0);

  _binaryData = new char[binLength];
}
GLProgramBinary::~GLProgramBinary() {
  if (_binaryData) {
    delete[] _binaryData;
  }
  _binaryData = NULL;
}
ShaderCache::ShaderCache(std::shared_ptr<GLContext> ct, string_t cacheDir) :GLFramework(ct){
  _strCacheDirectory = cacheDir;

  GLint n;
  glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &n);

  if (n <= 0) {
    Br2LogWarn("[ShaderCache] Gpu does not support any program binary formats.");
    _bCacheIsSupported = false;
  }

}
ShaderCache::~ShaderCache() {
  for (size_t i = 0; i < _vecBinaries.size(); ++i)
    delete _vecBinaries[i];
  _vecBinaries.resize(0);
}
string_t ShaderCache::getBinaryNameFromProgramName(string_t& progName) {
  string_t fb = progName + ".sb";
  return FileSystem::combinePath(_strCacheDirectory, fb);//::appendCacheDirectory(fb);
}
GLProgramBinary* ShaderCache::getBinaryFromGpu(std::shared_ptr<ShaderBase> prog) {
  GLint binBufSz = 0;
  GLint outLen = 0;

  getContext()->glGetProgramiv(prog->getGlId(), GL_PROGRAM_BINARY_LENGTH, &binBufSz);
  getContext()->chkErrRt();

  if (binBufSz == 0 || binBufSz > MemSize::e::MEMSZ_GIG2) {
    Br2ThrowException("Shader program binary was 0 or exceeded " + MemSize::e::MEMSZ_GIG2 + " bytes; actual: " + binBufSz);
  }

  GLProgramBinary* b = new GLProgramBinary(this, binBufSz);

  getContext()->glGetProgramBinary(prog->getGlId(), binBufSz, &outLen, &(b->_glFormat), (void*)b->_binaryData);
  getContext()->chkErrRt();

  if (binBufSz != outLen) {
    delete b;
    Br2ThrowException("GPU reported incorrect program size and returned a program with a different size.");
  }

  //Critical: set compile time.
  b->_compileTime = prog->getCompileTime();

  _vecBinaries.push_back(b);

  return b;
}
/**
*  @fn getBinaryFromDisk
*  @brief Pass in the program name, not the name of the binary.
Shader Binary Cache File Format
Extension: .sb
    compile time (int64)
    shader format (int32)
    binary size (int32)
    binary (char*)

*/
GLProgramBinary* ShaderCache::getBinaryFromDisk(string_t& programName) {
  DiskFile df;
  GLProgramBinary* pbin = nullptr;
  GLenum glenum;
  size_t binSz;
  string_t binaryName = getBinaryNameFromProgramName(programName);

  time_t compTime;
  //doesn't matter
  //if(sizeof(time_t)!=8)
  //    throw new Game::Exception("Could not save bin.  Time was not 64 bits.");

  if (!FileSystem::fileExists(binaryName)) {
    Br2LogDebug(string_t("Program binary not found: ") + binaryName);

    return nullptr;
  }

  Br2LogDebug(string_t("Loading program binary ") + binaryName);
  try {
    df.openForRead(DiskLoc(binaryName));
    df.read((char*)&(compTime), sizeof(time_t));
    df.read((char*)&(glenum), sizeof(glenum));
    df.read((char*)&(binSz), sizeof(binSz));

    // if we're too big - freak out
    if ((binSz < 0) || (binSz > MemSize::e::MEMSZ_GIG2)) {
      Br2LogError("Invalid shader binary file size '" + binSz + "', recompiling binary.");
      pbin = nullptr;
    }
    else {
      pbin = new GLProgramBinary(this, binSz);
      pbin->_glFormat = glenum;
      pbin->_compileTime = compTime;

      df.read((char*)pbin->_binaryData, pbin->_binaryLength);

      //This won't work.  DiskFIle is random access and doesn't keep track of how many byte it read.
      //if(!df.checkEOF())
      //    throw new Game::Exception("Failure in reading shader binary cache file. There was a file integrity error somewhere",__LINE__,__FILE__);
      df.close();

      _vecBinaries.push_back(pbin);
    }
  }
  catch (Exception * ex) {
    //fail silently

    Br2LogError("Failed to load program binary " + binaryName + ex->what());
    if (pbin) {
      delete pbin;
    }
    pbin = nullptr;
  }

  return pbin;

}
void ShaderCache::saveBinaryToDisk(string_t& programName, GLProgramBinary* bin) {
  DiskFile df;
  string_t binaryName = getBinaryNameFromProgramName(programName);
  string_t binPath = FileSystem::getPathFromPath(binaryName);

  Br2LogInfo(" Shader program Bin path = " + binPath);

  try {
    Br2LogDebug(string_t("[ShaderCache] Caching program binary ") + binaryName);
    FileSystem::createDirectoryRecursive(binPath);
    df.openForWrite(DiskLoc(binaryName), FileWriteMode::Truncate);
    df.write((char*)&(bin->_compileTime), sizeof(bin->_compileTime));
    df.write((char*)&(bin->_glFormat), sizeof(bin->_glFormat));
    df.write((char*)&(bin->_binaryLength), sizeof(bin->_binaryLength));
    df.write((char*)bin->_binaryData, bin->_binaryLength);
    df.close();
  }
  catch (Exception * ex) {
    Br2LogError("Failed to save program binary " + binaryName + ex->what());
  }

}
/**
*  @fn deleteBinaryFromDisk
*/
void ShaderCache::deleteBinaryFromDisk(string_t& programName) {
  string_t binaryName = getBinaryNameFromProgramName(programName);

  if (!FileSystem::fileExists(binaryName)) {
    Br2LogError(string_t("Failed to delete file ") + binaryName);
  }

  FileSystem::deleteFile(binaryName);
}
/**
*  @fn freeLoadedBinary
*/
void ShaderCache::freeLoadedBinary(GLProgramBinary* bin) {
  _vecBinaries.erase(std::remove(_vecBinaries.begin(), _vecBinaries.end(), bin), _vecBinaries.end());
  delete bin;
}
/**
*  @fn saveCompiledBinaryToDisk
*  @brief
*/
void ShaderCache::saveCompiledBinaryToDisk(std::shared_ptr<ShaderBase> pProgram) {
  GLProgramBinary* bin = getBinaryFromGpu(pProgram);
  saveBinaryToDisk(pProgram->getProgramName(), bin);
}
/**
*  @fn tryLoadCachedBinary()
*  @brief Try to load a cached GLSL binary to the GPU.
*  @return false if the load failed or file was not found.
*  @return true if the program loaded successfully
*/
std::shared_ptr<ShaderBase> ShaderCache::tryLoadCachedBinary(std::string programName, std::vector<string_t> shaderFiles) {
  bool bSuccess = false;
  GLProgramBinary* binary;
  std::shared_ptr<ShaderBase> ret = nullptr;

  binary = getBinaryFromDisk(programName);

  if (binary != NULL) {
    time_t maxTime = 0;
    for (string_t file : shaderFiles) {
      FileInfo inf = FileSystem::getFileInfo(file);
      if (!inf._exists) {
        Br2LogError("Shader source file '" + file + "' does not exist.");
      }
      else {
        maxTime = MathUtils::broMax(inf._modified, maxTime);
      }
    }

    if (binary->_compileTime >= maxTime) {
      //pProgram has already asked GL for an ID.
      try {
        ret = loadBinaryToGpu(programName, binary);
        if (ret == nullptr) {
          Br2LogInfo("Program binary for '" + programName + "' out of date.  Deleting from disk");
          deleteBinaryFromDisk(programName);
        }
      }
      catch (Exception * e) {
        Br2LogWarn("[ShaderCache] Loading program binary returned warnings/errors:\r\n");
        Br2LogWarn(e->what());
        deleteBinaryFromDisk(programName);
      }

    }

    freeLoadedBinary(binary);
  }

  return ret;
}

/**
*  @fn loadBinaryToGpu()
*  @brief Attaches the binary to the already created program object and loads it to the GPU.
*        Prog must already have been created.
*
*  @return false if the program returned errors.
*/
std::shared_ptr<ShaderBase> ShaderCache::loadBinaryToGpu(std::string programName, GLProgramBinary* bin) {
  getContext()->chkErrRt();

  std::shared_ptr<ShaderBase> pProgram = std::make_shared<ShaderBase>(getContext(),programName);
  pProgram->init();
  getContext()->chkErrRt();

  GLboolean b1 = getContext()->glIsProgram(pProgram->getGlId());
  if (b1 == false) {
    Br2LogWarn("[ShaderCache] Program was not valid before loading to GPU");
    return nullptr;
  }
  getContext()->chkErrRt();


  Br2LogDebug("[ShaderCache] Loading Cached Program Binary to GPU");
  getContext()->glProgramBinary(pProgram->getGlId(), bin->_glFormat, (void*)bin->_binaryData, (GLsizei)bin->_binaryLength);
  if (getContext()->chkErrRt(true, true)) {
    //If we have en error here, we failed to load the binary.
    Br2LogWarn("[ShaderCache] Failed to load binary to GPU - we might be on a different platform.");
    return nullptr;
  }

  //Print Log
  std::vector<string_t> inf;
  pProgram->getProgramErrorLog(inf);
  for (size_t i = 0; i < inf.size(); ++i) {
    Br2LogWarn("   " + inf[i]);
  }

  //validate program.
  GLint iValid;
  getContext()->glValidateProgram(pProgram->getGlId());
  getContext()->chkErrRt();

  getContext()->glGetProgramiv(pProgram->getGlId(), GL_VALIDATE_STATUS, (GLint*)&iValid);
  getContext()->chkErrRt();

  if (iValid == GL_FALSE) {
    // Program load faiiled
    Br2LogWarn("[ShaderCache] glValidateProgram says program binary load failed.  Check the above logs for errors.");
    return nullptr;
  }

  GLboolean b2 = getContext()->glIsProgram(pProgram->getGlId());
  getContext()->chkErrRt();

  if (b2 == false) {
    Br2ThrowException("[ShaderCache] glIsProgram says program was not valid after loading to GPU");
  }

  pProgram->bind();
  // - If the program failed to load it will raise an error after failing to bind.
  GLenum e = glGetError();
  if (e != GL_NO_ERROR) {
    Br2LogWarn("[ShaderCache], GL error " + StringUtil::toHex(e, true) + " , program was not valid after loading to GPU.");
    return nullptr;
  }

  pProgram->unbind();
  getContext()->chkErrRt();

  return pProgram;
}


}//ns BR2
