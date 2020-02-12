/**
*  @file ShaderSubProgram.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADERSUBPROGRAM_1494036432164858316_H__
#define __SHADERSUBPROGRAM_1494036432164858316_H__

#include "../gfx/GfxHeader.h"


namespace BR2 {
/**
*    @class ShaderSubProgram
*    @brief
*
*/
class ShaderSubProgram : public VirtualMemory {
    GLuint _glId = 0;
    time_t _compileTime = 0;
    ShaderType::e _eType;
    ShaderStatus::e _eStatus = ShaderStatus::e::Uninitialized;
    string_t _sourceLocation;
    time_t _sourceLastGreatestIncludeModificationTime = 0;
    std::vector<string_t> _sourceLines;
    std::vector<string_t> _compileErrors;
    std::vector<string_t> _generalErrors;

    ShaderType::e getShaderTypeByFileLocation(DiskLoc& loc);
    void setSourceLocation(string_t st) { _sourceLocation = st; }
    GLenum getGLShaderEnum(ShaderType::e type);
public:

    ShaderSubProgram();
    virtual ~ShaderSubProgram() override;

    void init(std::shared_ptr<GLContext> ctx, string_t loc, ShaderType::e type);

    void setCompileTime(time_t tt) { _compileTime = tt; }
    void setStatus(ShaderStatus::e ls) { _eStatus = ls; }
    ShaderStatus::e getStatus() { return _eStatus; }
    time_t getSourceLastGreatestModificationTime() { return _sourceLastGreatestIncludeModificationTime; }
    void setSourceLastGreatestModificationTime(time_t t) {  _sourceLastGreatestIncludeModificationTime = t; }
    GLuint getGlId() const { return _glId; }
    string_t getSourceLocation() { return _sourceLocation; }
    string_t getHumanReadableErrorString() const;
    void debugPrintShaderSource() const;

    ShaderType::e getShaderType() { return _eType; }

    std::vector<string_t>& getCompileErrors() { return _compileErrors; }
    std::vector<string_t>& getGeneralErrors() { return _generalErrors; }
    std::vector<string_t>& getSourceLines() { return _sourceLines; }
};

}//ns Game



#endif
