/**
*
*    @file ShaderCache.h
*    @date October 22, 2014
*    @author MetalMario971
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __ShaderCache_434517993163531317228885_H__
#define __ShaderCache_434517993163531317228885_H__

#include "../gfx/GfxHeader.h"
//#include "../gpu/ShaderHeader.h"
//#include "../hardware/HardwareHeader.h"

namespace Game {



/**
*    @class ShaderCache
*    @brief Caches shader binaries.

*/
class ShaderCache : public VirtualMemory {
private:
    class GLProgramBinary : public VirtualMemory {
    public:
        GLenum _glFormat;
        size_t _binaryLength;
        char* _binaryData;
        ShaderCache* _pShaderCache;
        time_t _compileTime;    // Time the binary was compiled.

        GLProgramBinary(ShaderCache* c, size_t binLength);
        virtual ~GLProgramBinary() override;
    };

private:
    bool _bCacheIsSupported = false;
    std::vector<GLProgramBinary*> _vecBinaries;
    t_string _strCacheDirectory;

protected:

    t_string getBinaryNameFromProgramName(t_string& progName);
    bool isSupported(){ return _bCacheIsSupported; }
    void freeLoadedBinary(GLProgramBinary* bin);
    GLProgramBinary* getBinaryFromGpu(std::shared_ptr<ShaderBase> prog);
    std::shared_ptr<ShaderBase> loadBinaryToGpu(std::string,GLProgramBinary* bin);
    GLProgramBinary* getBinaryFromDisk(t_string& programName);
    void saveBinaryToDisk(t_string& programName, GLProgramBinary* bin);
    void deleteBinaryFromDisk(t_string& programName);

public:
    ShaderCache(t_string cacheDir);
    virtual ~ShaderCache() override;

    void saveCompiledBinaryToDisk(std::shared_ptr<ShaderBase> pProgram);
    std::shared_ptr<ShaderBase> tryLoadCachedBinary(t_string programName, std::vector<t_string> shaderFiles);


};

}//ns game



#endif
