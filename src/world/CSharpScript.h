/**
*  @file CSharpScript.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __CSHARPSCRIPT_15665746341154937548_H__
#define __CSHARPSCRIPT_15665746341154937548_H__

#include "../world/Script.h"

namespace BR2 {
class CSToken;

class CSharpFile : public VirtualMemory {
public:
  string_t _name;
  std::shared_ptr<CSharpScript> _script = nullptr;// this is null if the file is not compiled.
};

class CSharpCompileContext {
public:
  std::vector<std::shared_ptr<CSharpFile>> _files;
  bool _bGatheredClasses = false;
private:
};

/**
*  @class CSharpScript
*  @brief A minimally functional C Sharp script.
*/
class CSharpScript_Internal;
class CSharpScript : public Script {
public:
  CSharpScript();
  virtual ~CSharpScript() override;
  
  static std::vector<CSToken*> lexTest(string_t str);
  static string_t tokensToString(std::vector<CSToken*> toks);

  static void initScriptSystem();
  static std::shared_ptr<CSharpScript> compile(std::shared_ptr<CSharpCompileContext> files);

  virtual void onStart() override;
  virtual void onUpdate(float delta) override;
  virtual void onExit() override;
private:
  std::unique_ptr<CSharpScript_Internal> _pint = nullptr;
};

}//ns BR2



#endif
