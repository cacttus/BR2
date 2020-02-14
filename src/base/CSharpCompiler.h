/**
*  @file CSharpCompiler.h
*  @date 20200213
*  @author MetalMario971
*/
#pragma once
#ifndef __CSHARPCOMPILER_1581653445241884949_H__
#define __CSHARPCOMPILER_1581653445241884949_H__

#include "../base/BaseHeader.h"
namespace BR2 {
class CSharpExpression : public VirtualMemoryShared<CSharpExpression> {
public:

};
class CSharpMethod : public VirtualMemoryShared<CSharpMethod> {
public:

};
class CSharpClass : public VirtualMemoryShared<CSharpClass> {
public:
  string_t getClassName() { return _className; }
private:
  string_t _className = "";
};
class CSharpCompileResult : public VirtualMemoryShared<CSharpClass> {
public:
  std::vector<std::string> _errors;
  std::vector<std::string> _warnings;
};
class CSharpCompiler : public VirtualMemoryShared<CSharpCompiler> {
public:
  CSharpCompiler();
  virtual ~CSharpCompiler() override;
  std::shared_ptr<CSharpCompileResult> compile(char* code);
private:
  int32_t _iLine = 1;
};

}//ns Game



#endif
