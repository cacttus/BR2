/**
*  @file ShaderParser.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADERPARSER_14940011921511191713_H__
#define __SHADERPARSER_14940011921511191713_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
struct ShaderIncludeRef {
  string_t* str;
  size_t lineNo;
};

/**
*  @class GLShaderCompiler
*  @brief Helpful class for shader compile errors.
*/
class ShaderCompiler : public VirtualMemory {
public:
  ShaderCompiler(std::shared_ptr<GLContext> ct, string_t fileDir);
  virtual ~ShaderCompiler() {}
  virtual void loadSource(std::shared_ptr<ShaderSubProgram>);
  virtual std::vector<string_t> getErrorList(const std::shared_ptr<ShaderSubProgram> shader) const;        // - Returns a vector of error strings.
  virtual void compile(std::shared_ptr<ShaderSubProgram>);

private:
  typedef std::vector<ShaderIncludeRef> IncludeVec;
  std::shared_ptr<GLContext> _pContext = nullptr;
  char** ySrcPtr;    // - The pointer to a the source code.
  ShaderStatus::e _loadStatus;    //for temp errors
  string_t _error;//for temp errors
  string_t _fileDir;

  virtual IncludeVec getIncludes(std::vector<string_t>& lines);
  void debugPrintShaderSource(string_t& loc, std::vector<string_t>& sourceLines);//PRINTS ENTIRE INCLUDED SOURCE
  void searchIncludes(std::shared_ptr<ShaderSubProgram> subProg, std::vector<string_t>& lines, time_t& greatestModifyTime);
  void loadSource_r(std::shared_ptr<ShaderSubProgram> subProg, string_t& location, std::vector<string_t>& out_lines, time_t& greatestModifyTime);
  void parseSourceIntoLines(std::shared_ptr<BinaryFile>, std::vector<string_t>& out_lines);
  void loadSourceData(string_t& location, std::shared_ptr<BinaryFile> __out_ data);
  void addSourceLineAt(size_t pos, std::vector<string_t>& vec, string_t line);
};

}//ns Game



#endif
