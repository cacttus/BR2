/**
*
*    @file ShaderCache.h
*    @date October 22, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __ShaderCache_434517993163531317228885_H__
#define __ShaderCache_434517993163531317228885_H__

#include "../display/DisplayHeader.h"
//#include "../gpu/ShaderHeader.h"
//#include "../hardware/HardwareHeader.h"

namespace Game {

class ShaderCache;

class GLProgramBinary : public VirtualMemory {
public:
    GLenum _glFormat;
    size_t _binaryLength;
    char* _binaryData;
    ShaderCache* _pShaderCache;
    time_t _compileTime;    // Time the binary was compiled.

    GLProgramBinary(ShaderCache* c, size_t binLength);
    virtual ~GLProgramBinary() override ;
};
/**
*    @class ShaderCache
*    @brief Caches shader binaries.

*/
class ShaderCache : public VirtualMemory {
    std::shared_ptr<GLContext> _pContext = nullptr;
    bool _bCacheIsSupported;
    //GLShaderManager* _pShaderManager;
    std::vector<GLProgramBinary*> _vecBinaries;
    t_string _strCacheDirectory;
protected:

    t_string getBinaryNameFromProgramName(t_string& progName);
    bool isSupported(){ return _bCacheIsSupported; }
    void freeLoadedBinary(GLProgramBinary* bin);
    GLProgramBinary* getBinaryFromGpu(std::shared_ptr<ShaderBase> prog);
    bool loadBinaryToGpu(std::shared_ptr<ShaderBase> prog, GLProgramBinary* bin);
    GLProgramBinary* getBinaryFromDisk(t_string& programName);
    void saveBinaryToDisk(t_string& programName, GLProgramBinary* bin);
    void deleteBinaryFromDisk(t_string& programName);

public:
    //+-- CTOR/DTOR --+
    ShaderCache(std::shared_ptr<GLContext> ct, t_string cacheDir);
    virtual ~ShaderCache() override;



    void saveCompiledBinaryToDisk(std::shared_ptr<ShaderBase> pProgram);
    bool tryLoadCachedBinary(std::shared_ptr<ShaderBase> prog, std::vector<std::shared_ptr<ShaderSubProgram>>& subProgs);


};

}//ns game



#endif
