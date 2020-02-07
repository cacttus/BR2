/**
*
*    @file ShaderSubProgram.h
*    @date May 5, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __SHADERSUBPROGRAM_1494036432164858316_H__
#define __SHADERSUBPROGRAM_1494036432164858316_H__

#include "../gfx/GfxHeader.h"


namespace Game {
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
    t_string _sourceLocation;
    time_t _sourceLastGreatestIncludeModificationTime = 0;
    std::vector<t_string> _sourceLines;
    std::vector<t_string> _compileErrors;
    std::vector<t_string> _generalErrors;

    ShaderType::e getShaderTypeByFileLocation(DiskLoc& loc);
    void setSourceLocation(t_string st) { _sourceLocation = st; }
    GLenum getGLShaderEnum(ShaderType::e type);
public:

    ShaderSubProgram();
    virtual ~ShaderSubProgram() override;

    void init(std::shared_ptr<GLContext> ctx, t_string loc, ShaderType::e type);

    void setCompileTime(time_t tt) { _compileTime = tt; }
    void setStatus(ShaderStatus::e ls) { _eStatus = ls; }
    ShaderStatus::e getStatus() { return _eStatus; }
    time_t getSourceLastGreatestModificationTime() { return _sourceLastGreatestIncludeModificationTime; }
    void setSourceLastGreatestModificationTime(time_t t) {  _sourceLastGreatestIncludeModificationTime = t; }
    GLuint getGlId() const { return _glId; }
    t_string getSourceLocation() { return _sourceLocation; }
    t_string getHumanReadableErrorString() const;
    void debugPrintShaderSource() const;

    ShaderType::e getShaderType() { return _eType; }

    std::vector<t_string>& getCompileErrors() { return _compileErrors; }
    std::vector<t_string>& getGeneralErrors() { return _generalErrors; }
    std::vector<t_string>& getSourceLines() { return _sourceLines; }
};

}//ns Game



#endif
