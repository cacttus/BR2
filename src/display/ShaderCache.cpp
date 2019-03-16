#include "../base/BaseAll.h"
#include "../base/DiskFile.h"

#include "../display/ShaderCache.h"
#include "../display/ShaderSubProgram.h"
#include "../display/ShaderBase.h"


namespace Game {
;
ShaderCache::ShaderCache(std::shared_ptr<GLContext> ct, t_string cacheDir) : _pContext(ct),
_bCacheIsSupported(false)
{
    _strCacheDirectory = cacheDir;
    GLint n;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &n);

    if (n <= 0)
    {
        BroLogWarn("[ShaderCache] Gpu does not support any program binary formats.");
        _bCacheIsSupported = false;
    }

}
ShaderCache::~ShaderCache() {
    for (size_t i = 0; i < _vecBinaries.size(); ++i)
        delete _vecBinaries[i];
    _vecBinaries.resize(0);
}

t_string ShaderCache::getBinaryNameFromProgramName(t_string& progName)
{
    t_string fb = progName + ".sb";
    return FileSystem::combinePath(_strCacheDirectory, fb);//::appendCacheDirectory(fb);
}

GLProgramBinary* ShaderCache::getBinaryFromGpu(std::shared_ptr<ShaderBase> prog)
{
    GLint binBufSz = 0;
    GLint outLen = 0;

    _pContext->glGetProgramiv(prog->getGlId(), GL_PROGRAM_BINARY_LENGTH, &binBufSz);
    _pContext->chkErrRt();

    if (binBufSz == 0 || binBufSz > MemSize::e::MEMSZ_GIG2) {
        BroThrowException(
            "Shader program binary was 0 or exceeded ", MemSize::e::MEMSZ_GIG2, " bytes; actual: ", binBufSz);
    }

    GLProgramBinary* b = new GLProgramBinary(this, binBufSz);

    _pContext->glGetProgramBinary(prog->getGlId(), binBufSz, &outLen, &(b->_glFormat), (void*)b->_binaryData);
    _pContext->chkErrRt();

    if (binBufSz != outLen)
    {
        delete b;
        throw new Exception("GPU reported incorrect program size and returned a program with a different size.", __LINE__, __FILE__);
    }

    //Critical: set compile time.
    b->_compileTime = prog->getCompileTime();

    _vecBinaries.push_back(b);

    return b;
}

/**
*    @fn
*    @brief Attaches the binary to the already created program object and loads it to the GPU.
*        Prog must already have been created.
*
*    @return false if the program returned errors.
*/
bool ShaderCache::loadBinaryToGpu(std::shared_ptr<ShaderBase> prog, GLProgramBinary* bin)
{
    _pContext->chkErrRt();

    //if prog already loaded onto GPU (finalized) then delete it.
    if (prog->getProgramStatus() == ShaderStatus::e::Loaded) {
        BroLogDebug("[ShaderCache] Program already loaded.  Deleting program.");
        prog->recreateProgram();
    }

    _pContext->chkErrRt();

    GLboolean b1 = _pContext->glIsProgram(prog->getGlId());
    if (b1 == false) {
        BroLogWarn("[ShaderCache] Program was not valid before loading to GPU");
        return false;
    }
    //Pre-check for errors
    _pContext->chkErrRt();

    BroLogDebug("[ShaderCache] Loading Cached Program Binary to GPU");
    _pContext->glProgramBinary(prog->getGlId(), bin->_glFormat, (void*)bin->_binaryData, (GLsizei)bin->_binaryLength);
    if (_pContext->chkErrRt(true)) {
        //If we have en error here, we failed to load the binary.
        BroLogWarn("Failed to load binary to GPU - we might be on a different platform.");
        return false;
    }

    //Print Log
    std::vector<t_string> inf;
    prog->getProgramErrorLog(inf);
    for (size_t i = 0; i < inf.size(); ++i) {
        BroLogWarn("   ", inf[i]);
    }

    //validate program.
    GLint iValid;
    _pContext->glValidateProgram(prog->getGlId());
    _pContext->chkErrRt();

    _pContext->glGetProgramiv(prog->getGlId(), GL_VALIDATE_STATUS, (GLint*)&iValid);
    _pContext->chkErrRt();

    if (iValid == GL_FALSE)
    {
        // Program load faiiled
        BroLogWarn("glValidateProgram says program binary load failed.  Check the above logs for errors.");
        return false;
    }

    GLboolean b2 = _pContext->glIsProgram(prog->getGlId());
    _pContext->chkErrRt();
    if (b2 == false) {
        BroThrowException("[ShaderCache] glIsProgram says program was not valid after loading to GPU");
    }

    prog->bind();
    // - If the program failed to load it will raise an error after failing to bind.
    GLenum e = glGetError();
    if (e != GL_NO_ERROR)
    {
        BroLogWarn("[ShaderCache, GL error ", StringUtil::toHex(e, true), " , program was not valid after loading to GPU.  See log above.");
        return false;
    }

    prog->unbind();
    _pContext->chkErrRt();

    return true;
}


/**
*    @fn getBinaryFromDisk
*    @brief Pass in the program name, not the name of the binary.


Shader Binary Cache File Format

Extension: .sb

    compile time (int64)
    shader format (int32)
    binary size (int32)
    binary (char*)

*/
GLProgramBinary* ShaderCache::getBinaryFromDisk(t_string& programName)
{
    DiskFile df;
    GLProgramBinary* pbin = nullptr;
    GLenum glenum;
    size_t binSz;
    t_string binaryName = getBinaryNameFromProgramName(programName);

    time_t compTime;
    //doesn't matter
    //if(sizeof(time_t)!=8)
    //    throw new Game::Exception("Could not save bin.  Time was not 64 bits.");

    if (!FileSystem::fileExists(binaryName))
    {
        BroLogDebug(t_string("Program binary not found: ") + binaryName);

        return nullptr;
    }

    BroLogDebug(t_string("Loading program binary ") + binaryName);
    try
    {
        df.openForRead(DiskLoc(binaryName));
        df.read((char*)&(compTime), sizeof(time_t));
        df.read((char*)&(glenum), sizeof(glenum));
        df.read((char*)&(binSz), sizeof(binSz));

        // if we're too big - freak out
        if ((binSz < 0) || (binSz > MemSize::e::MEMSZ_GIG2)){
            BroLogError("Invalid shader binary file size '", binSz, "', recompiling binary.");
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
    catch (Exception* ex)
    {
        //fail silently

        BroLogError("Failed to load program binary ", binaryName, ex->what());
        if (pbin){
            delete pbin;
        }
        pbin = nullptr;
    }

    return pbin;

}
void ShaderCache::saveBinaryToDisk(t_string& programName, GLProgramBinary* bin)
{
    DiskFile df;
    t_string binaryName = getBinaryNameFromProgramName(programName);
    t_string binPath = FileSystem::getPathFromPath(binaryName);

    BroLogInfo(" Shader program Bin path = ", binPath);

    try
    {
        BroLogDebug(t_string("[ShaderCache] Caching program binary ") + binaryName);
        FileSystem::createDirectoryRecursive(binPath);
        df.openForWrite(DiskLoc(binaryName), FileWriteMode::Truncate);
        df.write((char*)&(bin->_compileTime), sizeof(bin->_compileTime));
        df.write((char*)&(bin->_glFormat), sizeof(bin->_glFormat));
        df.write((char*)&(bin->_binaryLength), sizeof(bin->_binaryLength));
        df.write((char*)bin->_binaryData, bin->_binaryLength);
        df.close();
    }
    catch (Exception* ex)
    {
        BroLogError("Failed to save program binary ", binaryName, ex->what());
    }

}
/**
*    @fn
*    @brief
*/
void ShaderCache::freeLoadedBinary(GLProgramBinary* bin)
{
    _vecBinaries.erase(std::remove(_vecBinaries.begin(), _vecBinaries.end(), bin), _vecBinaries.end());
    delete bin;
}
/**
*    @fn
*    @brief
*/
void ShaderCache::saveCompiledBinaryToDisk(std::shared_ptr<ShaderBase> pProgram)
{
    GLProgramBinary* bin = getBinaryFromGpu(pProgram);
    saveBinaryToDisk(pProgram->getProgramName(), bin);
}
/**
*    @fn tryLoadCachedBinary
*    @brief Try to load the binary to disk
*    @return false if the load failed or file was not found.
*    @return true if the program loaded successfully
*/
bool ShaderCache::tryLoadCachedBinary(std::shared_ptr<ShaderBase> prog, std::vector<std::shared_ptr<ShaderSubProgram>>& subProgs)
{
    bool bSuccess = false;
    GLProgramBinary* binary;

    binary = getBinaryFromDisk(prog->getProgramName());

    if (binary != NULL) {
        time_t maxTime = 0;
        for (size_t i = 0; i < subProgs.size(); ++i) {
            maxTime = MathUtils::broMax(subProgs[i]->getSourceLastGreatestModificationTime(), maxTime);
        }

        if (binary->_compileTime >= maxTime)
        {
            //pProgram has already asked GL for an ID.
            try
            {
                if (loadBinaryToGpu(prog, binary))
                {
                    BroLogDebug("Program binary load success.");
                    bSuccess = true;
                }
                else
                {
                    BroLogWarn("Program binary load failed, or was out of date.  Deleting from disk and recreating.");
                    deleteBinaryFromDisk(prog->getProgramName());
                }
            }
            catch (Exception* e)
            {
                BroLogWarn("[ShaderCache] Loading program binary returned warnings/errors:\r\n");
                BroLogWarn(e->what());
                deleteBinaryFromDisk(prog->getProgramName());
            }

        }

        freeLoadedBinary(binary);
    }

    return bSuccess;
}
/**
*    @fn
*    @brief
*/
void ShaderCache::deleteBinaryFromDisk(t_string& programName)
{
    t_string binaryName = getBinaryNameFromProgramName(programName);

    if (!FileSystem::fileExists(binaryName)) {
        BroLogError(t_string("Failed to delete file ") + binaryName);
    }

    FileSystem::deleteFile(binaryName);
}

//////////////////////////////////////////////////////////////////////////
GLProgramBinary::GLProgramBinary(ShaderCache* cc, size_t binLength) :
    _pShaderCache(cc),
    _binaryLength(binLength),
    _binaryData(NULL),
    _compileTime(0)
{
    _binaryData = new char[binLength];

}
GLProgramBinary::~GLProgramBinary()
{
    if (_binaryData) {
        delete[] _binaryData;
    }
    _binaryData = NULL;
}

}//ns game
