#include "../base/BaseAll.h"
#include "../base/DiskFile.h"
#include "../base/BinaryFile.h"
#include "../gfx/ShaderCompiler.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderSubProgram.h"


namespace BR2 {
ShaderCompiler::ShaderCompiler(std::shared_ptr<GLContext> ct, string_t fileDir) : _fileDir(fileDir), _pContext(ct) {
}
/**
*  @fn fileToArray()
*  @brief I believe this turns a file into an array of lines.
*/
void ShaderCompiler::loadSource(std::shared_ptr<ShaderSubProgram> pSubProg) {
  AssertOrThrow2(pSubProg != NULL);

  _loadStatus = ShaderStatus::Uninitialized;
  pSubProg->setStatus(ShaderStatus::Uninitialized);
  pSubProg->getSourceLines().clear();

  time_t greatestModifyTime = 0;//TIME_T_MIN;

  // - First try to load the srouce
  try {
    Br2LogDebug("Loading source for Shader " + pSubProg->getSourceLocation());
    loadSource_r(pSubProg, pSubProg->getSourceLocation(), pSubProg->getSourceLines(), greatestModifyTime);
  }
  catch (Exception * e) {
    //pSubProg->debugPrintShaderSource();
    _loadStatus = ShaderStatus::CompileError;
    _error = e->what();
  }

  // - If shader is not uninitialized there was an error during loading.
  if (_loadStatus != ShaderStatus::Uninitialized) {
    return;
  }

  // - Cache the modification time of the whole shader include hierarchy
  pSubProg->setSourceLastGreatestModificationTime(greatestModifyTime);

  pSubProg->setStatus(ShaderStatus::e::Loaded);
}
void ShaderCompiler::loadSource_r(std::shared_ptr<ShaderSubProgram> pSubProg, string_t& location, std::vector<string_t>& lines, time_t& greatestModifyTime) {
  //char* data;
  time_t modTime;

  // data = NULL;

  if (pSubProg->getStatus() != ShaderStatus::e::Uninitialized && !ShaderMaker::isGoodStatus(pSubProg->getStatus())) {
    pSubProg->getGeneralErrors().push_back("Subprogram was not in good state.");
    return;
  }

  if (!Gu::getAppPackage()->fileExists((string_t)location)) {
    pSubProg->setStatus(ShaderStatus::e::CompileError);
    Gu::debugBreak();
    Br2ThrowException("Could not find shader file or #include file, " + location);
  }
  // - Store the greater modify time for shader cache.
  modTime = Gu::getAppPackage()->getLastModifyTime((string_t)location);
  greatestModifyTime = MathUtils::broMax(modTime, greatestModifyTime);

  // - Load all source bytes
  std::shared_ptr<BinaryFile> bf = std::make_shared<BinaryFile>();
  //Allocator<char> data;
  loadSourceData(location, bf);

  // - If we screwed up. return
  if (_loadStatus != ShaderStatus::Uninitialized) {
    return;
  }

  // Parse Lines
  parseSourceIntoLines(bf, lines);

  //Helps Identify files.
  string_t slashslash = "//-------------------------------------------------";
  string_t nameHdr = Stz "//           " + FileSystem::getFileNameFromPath(location);
  addSourceLineAt(0, lines, slashslash);
  addSourceLineAt(0, lines, nameHdr);
  addSourceLineAt(0, lines, slashslash);


  // Recursively do includes
  searchIncludes(pSubProg, lines, greatestModifyTime);

}
void ShaderCompiler::addSourceLineAt(size_t pos, std::vector<string_t>& vec, string_t line) {
  string_t linemod = line;
  linemod += '\n';
  linemod += '\0';

  vec.insert(vec.begin() + pos, linemod);
}
/**
*  @fn
*  @brief Includes files.
*/
void ShaderCompiler::searchIncludes(std::shared_ptr<ShaderSubProgram> subProg, std::vector<string_t>& lines, time_t& greatestModifyTime) {
  IncludeVec _includes;    //map of include offsets in the data to their source locations.
  string_t locStr;
  std::vector<string_t> includeLines;
  IncludeVec::iterator ite2;
  size_t includeOff;

  _includes = getIncludes(lines);
  IncludeVec::iterator ite = _includes.begin();

  // - Recursively parse all includes
  //TODO: is there a trap here to make sure we don't recur forever?
  for (; ite != _includes.end(); ++ite) {
    includeOff = ite->lineNo;
    locStr = *(ite->str);
    locStr = FileSystem::combinePath(_fileDir, locStr);
    includeLines.clear();

    loadSource_r(subProg, locStr, includeLines, greatestModifyTime);


    lines.insert(lines.begin() + includeOff, includeLines.begin(), includeLines.end());

    ite2 = ite;
    ite2++;
    for (; ite2 != _includes.end(); ite2++) {
      ite2->lineNo += includeLines.size();
    }

    delete ite->str;    //delete the allocated string
  }

}
/**
*  @fn
*  @brief Compiles all includes in the source lines into a map of include to its line number
*/
ShaderCompiler::IncludeVec ShaderCompiler::getIncludes(std::vector<string_t>& lines) {
  IncludeVec _includes;    //map of include offsets in the data to their source locations.
  string_t locStr;

  for (size_t i = 0; i < lines.size(); ++i) {
    locStr = lines[i].substr(0, 8);
    locStr = StringUtil::trim(locStr);
    //this check is to make sure there is no space or comments before the include.
    if (locStr.compare("#include") != 0) {
      continue;
    }

    // - Expand the include
    // - Expand the include
    // - Expand the include
    locStr = lines[i];
    lines.erase(lines.begin() + i);
    i--;

    // - Split our include data
    std::vector<string_t> vs = StringUtil::split(locStr, ' ');
    vs[0] = StringUtil::trim(vs[0]);

    // error checking
    if (vs.size() != 2) {
      _loadStatus = ShaderStatus::e::CompileError;
      _error = string_t("Compile Error -->\"") + vs[0] + string_t("\"");

      //free data
      IncludeVec::iterator ite = _includes.begin();
      for (; ite != _includes.end(); ite++)
        delete ite->str;
      Br2ThrowException("Compile Error -->\"Not enough arguments for include directive. \"");
    }

    if (vs[0].compare("#include") != 0) {
      _loadStatus = ShaderStatus::e::CompileError;
      _error = string_t("Compile Error -->\"") + vs[0] + string_t("\"");
      //free data
      IncludeVec::iterator ite = _includes.begin();
      for (; ite != _includes.end(); ite++)
        delete ite->str;
      Br2ThrowException("Compile Error -->\"Not enough arguments for include directive. \"");

    }

    vs[1] = StringUtil::trim(vs[1]);
    vs[1] = StringUtil::stripDoubleQuotes(vs[1]);

    // - Insert the include by its offset in our base data so we can go back and paste it in.
    ShaderIncludeRef srf;
    srf.str = new string_t(vs[1]);
    srf.lineNo = i + 1;
    _includes.push_back(srf);
  }

  return _includes;
}
void ShaderCompiler::loadSourceData(string_t& location, std::shared_ptr<BinaryFile> __out_ sourceData) {
  if (!Gu::getAppPackage()->fileExists(location)) {
    sourceData = NULL;
    _loadStatus = ShaderStatus::e::FileNotFound;
    Br2LogError("Shader Source File not found : " + location);
    Br2LogError(" CWD: " + FileSystem::getCurrentDirectory());
    return;
  }

  Gu::getAppPackage()->getFile(location, sourceData, true);
  //  sourceData.fread(location, true);
    //DiskFile::readAllBytes(location, sourceData);

}
void ShaderCompiler::parseSourceIntoLines(std::shared_ptr<BinaryFile> data, std::vector<string_t>& out_lines) {

  // - Parse file into lines
  string_t strTemp;
  char* c = data->getData().ptr(), * d;
  int len;
  int temp_filesize = 0;
  int filesize = (int)data->getData().count();

  while (temp_filesize < (int)filesize) {
    d = c;
    len = 0;
    strTemp.clear();
    while (((temp_filesize + len) < filesize) && ((int)(*d)) && ((int)(*d) != ('\n')) && ((int)(*d) != ('\r'))) {
      len++;
      d++;
    }

    d = c;
    len = 0;    // - Reuse of len.  It is not the length now but an index.
    while (((temp_filesize + len) < filesize) && ((int)(*d)) && ((int)(*d) != ('\n')) && ((int)(*d) != ('\r'))) {
      strTemp += (*d);
      d++;
    }

    // - We want newlines. Also this removes the \r
    if (((*d) == '\n') || ((*d) == '\r') || ((*d) == '\0')) {
      strTemp += '\n';
    }

    if (strTemp.length()) {
      strTemp += '\0';
      out_lines.push_back(strTemp);
      c += strTemp.length() - 1;
      temp_filesize += (int)strTemp.length() - 1;
    }

    // - Remove any file format garbage at the end (windows)
    len = 0;
    while (((temp_filesize + len) < filesize) && (((int)(*c) == ('\r')))) {
      len++;
      c++;
    }

    // increment the Newline. !important
    if (((temp_filesize + len) < filesize) && (((int)(*c) == ('\n')))) {
      len++;
      c++;
    }
    temp_filesize += len;

  }
}
/**
*  @fn compile
*  @brief Compile a shader.
*  @remarks Compiles a shader.
*/
void ShaderCompiler::compile(std::shared_ptr<ShaderSubProgram> pSubProg) {
  Br2LogInfo("Compiling shader " + pSubProg->getSourceLocation());

  //DOWNCAST:
 // GLstd::shared_ptr<ShaderSubProgram> shader = dynamic_cast<GLstd::shared_ptr<ShaderSubProgram>>(pSubProg);
  GLint b;

  if (pSubProg->getStatus() != ShaderStatus::e::Loaded) {
    Br2ThrowException("Shader was in an invalid state when trying to compile.");
  }

  GLchar** arg = new char* [pSubProg->getSourceLines().size()];
  for (size_t i = 0; i < pSubProg->getSourceLines().size(); ++i) {
    if (pSubProg->getSourceLines()[i].size()) {
      arg[i] = new char[pSubProg->getSourceLines()[i].size()];
      memcpy_s(arg[i], pSubProg->getSourceLines()[i].size(), pSubProg->getSourceLines()[i].c_str(), pSubProg->getSourceLines()[i].size());
    }
  }

  _pContext->glShaderSource(pSubProg->getGlId(), (GLsizei)pSubProg->getSourceLines().size(), (const GLchar**)arg, NULL);
  _pContext->glCompileShader(pSubProg->getGlId());
  _pContext->glGetShaderiv(pSubProg->getGlId(), GL_COMPILE_STATUS, &b);

  // - Gets the Gpu's error list.  This may also include warnings and stuff.
  pSubProg->getCompileErrors() = getErrorList(pSubProg);

  //  if (EngineSetup::getSystemConfig()->getPrintShaderSourceOnError() == TRUE)
  {
    if (pSubProg->getCompileErrors().size() > 0) {
      string_t str = pSubProg->getHumanReadableErrorString();
      if (StringUtil::lowercase(str).find("error") != string_t::npos) {
        pSubProg->debugPrintShaderSource();
        Br2LogErrorNoStack(str);
        Gu::debugBreak();
      }
      else {
        Br2LogWarn(str);
      }
    }
  }

  if (!b) {
    pSubProg->setStatus(ShaderStatus::CompileError);
  }
  else {
    pSubProg->setStatus(ShaderStatus::Compiled);
  }
}
/**
*  @fn getErrorList()
*  @brief Returns a list of strings that are the errors of the compiled shader source.
*/
std::vector<string_t> ShaderCompiler::getErrorList(const std::shared_ptr<ShaderSubProgram> shader) const {
  int buf_size = 16384;
  char* log_out = (char*)GameMemoryManager::allocBlock(buf_size);
  GLsizei length_out;

  _pContext->glGetShaderInfoLog(shader->getGlId(), buf_size, &length_out, log_out);

  std::vector<string_t> ret;
  string_t tempStr;
  char* c = log_out;

  while ((*c)) {
    while (((*c) != '\n') && ((*c))) {
      tempStr += (*c);
      c++;
    }
    ret.push_back(tempStr);
    tempStr.clear();
    c++;
  }
  GameMemoryManager::freeBlock(log_out);

  return ret;
}









}//ns Game
