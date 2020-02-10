/**
*  @file ShaderParser.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADERPARSER_14940011921511191713_H__
#define __SHADERPARSER_14940011921511191713_H__

#include "../gfx/GfxHeader.h"

namespace Game {
struct ShaderIncludeRef {
    t_string* str;
    size_t lineNo;
};


/**
*    @class GLShaderCompiler
*    @brief Helpful class for shader compile errors.
*/
class ShaderCompiler : public VirtualMemory {
    //typedef MapEx<t_string*, t_vecsize > IncludeMap;    //maps strings to offsets in the source list.
    //typedef MapEx<int, ShaderIncludeRef > IncludeMap;    //maps strings to offsets in the source list.
    typedef std::vector<ShaderIncludeRef> IncludeVec;
    std::shared_ptr<GLContext> _pContext = nullptr;
    char** ySrcPtr;    // - The pointer to a the source code.

    ShaderStatus::e _loadStatus;    //for temp errors
    t_string _error;//for temp errors
    t_string _fileDir;
                    //void expandIncludes(char*& data,size_t& dataSize );
    virtual IncludeVec getIncludes(std::vector<t_string>& lines);
    void debugPrintShaderSource(t_string& loc, std::vector<t_string>& sourceLines);//PRINTS ENTIRE INCLUDED SOURCE

    void searchIncludes(std::shared_ptr<ShaderSubProgram> subProg, std::vector<t_string>& lines, time_t& greatestModifyTime);
    void loadSource_r(std::shared_ptr<ShaderSubProgram> subProg, t_string& location, std::vector<t_string>& out_lines, time_t& greatestModifyTime);
    void parseSourceIntoLines(std::shared_ptr<BinaryFile>, std::vector<t_string>& out_lines);
    void loadSourceData(t_string& location, std::shared_ptr<BinaryFile> __out_ data);
    void addSourceLineAt(size_t pos, std::vector<t_string>& vec, t_string line);
public:
    ShaderCompiler(std::shared_ptr<GLContext> ct, t_string fileDir);
    virtual ~ShaderCompiler() { }
    virtual void loadSource(std::shared_ptr<ShaderSubProgram>);
    virtual std::vector<t_string> getErrorList(const std::shared_ptr<ShaderSubProgram> shader) const;        // - Returns a vector of error strings.
    virtual void compile(std::shared_ptr<ShaderSubProgram>);

};

}//ns Game



#endif
