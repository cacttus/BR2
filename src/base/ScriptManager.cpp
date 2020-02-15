#include "../base/BaseHeader.h"
#include "../base/ScriptManager.h"
#include "../base/FileSystem.h"
#include "../base/CSharpCompiler.h"


namespace BR2 {
ScriptManager::ScriptManager() {
  _compiler = std::make_shared<CSharpCompiler>();

}
ScriptManager::~ScriptManager() {

}

std::shared_ptr<CSharpScript> loadScript(string_t filepath) {
  //char* fdata;
  //int64_t fsize;
  //FileSystem::SDLFileRead(filepath, fdata, fsize, true);

  //std::shared_ptr<CSharpCompileResult> _compiler->compile(fdata);
  //FileSystem::SDLFileFree(fdata);
  //= std::make_shared<CSharpScript>();

  std::shared_ptr<CSharpScript> ret = nullptr;
  return ret;

}


}//ns Game
