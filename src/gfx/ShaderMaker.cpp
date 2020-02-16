#include "../base/Base.h"
#include "../base/AppBase.h"
#include "../base/GLContext.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderSubProgram.h"
#include "../gfx/ShaderCache.h"
#include "../gfx/ShaderCompiler.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderUniform.h"
#include "../gfx/ShaderAttribute.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"

namespace BR2 {
ShaderMaker::ShaderMaker() {
}
ShaderMaker::~ShaderMaker() {
  //Delete all Shaders
  //DELETE_VECTOR_ELEMENTS(_vecShaders);
  _pShaderCache = nullptr;
  _pShaderCompiler = nullptr;
}
void ShaderMaker::initialize() {
  _pShaderCache = std::make_shared<ShaderCache>(Gu::getAppPackage()->getCacheDir());
  //We might want to verify, that the graphics context here can in fact share shader objects without mucking-up context state.
  _pShaderCompiler = std::make_shared<ShaderCompiler>(Gu::getContext(), Gu::getAppPackage()->getShadersFolder());

  //Single VT shaders
  _pImageShader = makeShader(std::vector<string_t> { "f_v3x2_diffuse.vs", "f_v3x2_diffuse.ps" });
  _pNormalsShader = makeShader(std::vector<string_t> { "f_v3n3_normals.vs", "f_v3n3_normals.gs", "f_v3n3_normals.ps" });
  _pShadowBlendShader = makeShader(std::vector<string_t> {"v2x2_shadow_blend.vs", "v2x2_shadow_blend.ps" });
  _pDepthOfField = makeShader(std::vector<string_t> {"v3x2_depth_of_field.vs", "v3x2_depth_of_field.ps" });
  _pSmoothGen = makeShader(std::vector<string_t> {"v3x2_smooth_gen.vs", "v3x2_smooth_gen.ps" });
  //Multiple VT shaders
  _pShadowShaders.insert(std::make_pair(v_v3::getVertexFormat(), makeShader(std::vector<string_t> {"v3_shadowmap.vs", "v3_shadowmap.ps" })));
  _pShadowShaders.insert(std::make_pair(v_v3x2::getVertexFormat(), makeShader(std::vector<string_t> {"v3x2_shadowmap.vs", "v3x2_shadowmap.ps" })));

  _pDiffuseShaders.insert(std::make_pair(v_v3n3x2::getVertexFormat(), makeShader(std::vector<string_t> { "d_v3n3x2_diffuse.vs", "d_v3n3x2_diffuse.ps" })));
  _pDiffuseShaders.insert(std::make_pair(v_v3x2::getVertexFormat(), makeShader(std::vector<string_t> { "d_v3x2_diffuse.vs", "d_v3x2_diffuse.ps" })));
  _pDiffuseShaders.insert(std::make_pair(v_v3n3::getVertexFormat(), makeShader(std::vector<string_t> { "d_v3n3_diffuse.vs", "d_v3n3_diffuse.ps" })));
  _pDiffuseShaders.insert(std::make_pair(v_v3::getVertexFormat(), makeShader(std::vector<string_t> { "d_v3_diffuse.vs", "d_v3_diffuse.ps" })));
  _pDiffuseShaders.insert(std::make_pair(v_v2x2::getVertexFormat(), makeShader(std::vector<string_t> { "d_v2x2_diffuse.vs", "d_v2x2_diffuse.ps" })));

  _pGlassShaders.insert(std::make_pair(v_v3n3x2::getVertexFormat(), makeShader(std::vector<string_t> { "f_v3n3x2_glass.vs", "f_v3n3x2_glass.ps" })));
  _pGlassShaders.insert(std::make_pair(v_v3n3::getVertexFormat(), makeShader(std::vector<string_t> { "f_v3n3_glass.vs", "f_v3n3_glass.ps" })));

  _pFlatShaders.insert(std::make_pair(v_v2c4::getVertexFormat(), makeShader(std::vector<string_t> { "f_v2c4_flat.vs", "f_v2c4_flat.ps" })));
  _pFlatShaders.insert(std::make_pair(v_v3c4::getVertexFormat(), makeShader(std::vector<string_t> { "f_v3c4_flat.vs", "f_v3c4_flat.ps" })));

  //_pGuiShader = makeShader(std::vector<t_string> { "f_v2c4x2_gui.vs", "f_v2c4x2_gui.ps" });
  _pGuiShader = makeShader(std::vector<string_t> { "f_gui.vs", "f_gui.gs", "f_gui.ps" });


  //////////////////////////////////////////////////////////////////////////
  getComputeLimits();
}
std::shared_ptr<ShaderBase> ShaderMaker::getDiffuseShader(std::shared_ptr<VertexFormat> reqType) {
  return getValidShaderForVertexType(_pDiffuseShaders, reqType);
}
std::shared_ptr<ShaderBase> ShaderMaker::getGlassShader(std::shared_ptr<VertexFormat> reqType) {
  return getValidShaderForVertexType(_pGlassShaders, reqType);
}
std::shared_ptr<ShaderBase> ShaderMaker::getShadowShader(std::shared_ptr<VertexFormat> reqType) {
  return getValidShaderForVertexType(_pShadowShaders, reqType);
}
std::shared_ptr<ShaderBase> ShaderMaker::getFlatShader(std::shared_ptr<VertexFormat> reqType) {
  return getValidShaderForVertexType(_pFlatShaders, reqType);
}
std::shared_ptr<ShaderBase> ShaderMaker::getValidShaderForVertexType(ShaderMap& shaders, std::shared_ptr<VertexFormat> reqType) {
  std::shared_ptr<VertexFormat> match = nullptr;
  int n = 0;
  for (std::pair<std::shared_ptr<VertexFormat>, std::shared_ptr<ShaderBase>> p : shaders) {
    int x = reqType->matchTypeForShaderType(p.first);
    if (x > n) {
      n = x;
      match = p.first;
    }
  }
  if (match == nullptr) {
    //BroLogError("Could not match shader for vertex type.");
   // BroThrowNotImplementedException();
    return nullptr;
  }
  return shaders.find(match)->second;
}
void ShaderMaker::getComputeLimits() {
  Gu::getContext()->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &_maxWorkGroupDims[0]);
  Gu::getContext()->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &_maxWorkGroupDims[1]);
  Gu::getContext()->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &_maxWorkGroupDims[2]);

  GLint maxBindings;
  glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindings);

  if (maxBindings <= 0) {
    Br2ThrowException("The max number of shader storage buffers is zero. This likely means you are running an older graphics card where Compute Shading isn't supported.");
  }
  _maxBufferBindings = maxBindings;
}

std::shared_ptr<ShaderBase> ShaderMaker::makeShader(std::vector<string_t>& vecFiles) {
  std::vector<std::shared_ptr<ShaderSubProgram>> vecSubProgs;
  std::shared_ptr<ShaderBase> pShader = nullptr;

  //20160608 Note:
  //  1) We create a shader sub-program with glCreateShader
  //  2) We then load all source
  //  3) We then TRY TO GET THE CACHED PROGRAM
  //          This is a problem.  We should attempt to get the cached program first
  //  4) Then we compile/link all programs
  try {
    removeDuplicateSourceFiles(vecFiles);
    string_t strShaderName = getShaderNameFromFileNames(vecFiles);
    fullyQualifyFiles(vecFiles);
    Br2LogInfo("Making '" + strShaderName + "', Processing " + vecFiles.size() + " files.");

    pShader = _pShaderCache->tryLoadCachedBinary(strShaderName, vecFiles);

    if (pShader != nullptr) {
      pShader->setProgramStatus(ShaderStatus::Linked);
      _mapPrograms.insert(std::make_pair(pShader->getNameHashed(), pShader));
    }
    else {
      loadSubPrograms(vecFiles, vecSubProgs);
      if (checkForErrors(vecSubProgs, pShader)) {
        return nullptr;
      }

      pShader = makeProgram(vecSubProgs, strShaderName);
      if (checkForErrors(vecSubProgs, pShader)) {
        deleteShader(pShader);
        return nullptr;
      }
    }

    parseUniforms(pShader);
    if (checkForErrors(vecSubProgs, pShader)) {
      deleteShader(pShader);
      return nullptr;
    }

    parseUniformBlocks(pShader);
    if (checkForErrors(vecSubProgs, pShader)) {
      deleteShader(pShader);
      return nullptr;
    }

    parseAttributes(pShader);
    if (checkForErrors(vecSubProgs, pShader)) {
      deleteShader(pShader);
      return nullptr;
    }


    Br2LogInfo("Load Success\r\n------------------------------\r\n");
  }
  catch (Exception * ex) {
    Br2LogError(ex->what());
  }

  return pShader;
}
void ShaderMaker::deleteShader(std::shared_ptr<ShaderBase> ps) {
  if (ps != nullptr) {
    std::map<Hash32, std::shared_ptr<ShaderBase>>::iterator it = _mapPrograms.find(ps->getNameHashed());
    if (it != _mapPrograms.end()) {
      _mapPrograms.erase(it);
    }
    // delete ps;
  }
}
void ShaderMaker::fullyQualifyFiles(std::vector<string_t>& vecFiles) {
  for (size_t iFile = 0; iFile < vecFiles.size(); iFile++) { // strFile : vecFiles) {
    string_t qual = FileSystem::combinePath(Gu::getAppPackage()->getShadersFolder(), vecFiles[iFile]);
    vecFiles[iFile] = qual;
  }
}
string_t ShaderMaker::getShaderNameFromFileNames(std::vector<string_t>& vecFiles) {
  string_t progName = StringUtil::empty();

  for (size_t iFile = 0; iFile < vecFiles.size(); ++iFile) {
    progName.append(FileSystem::getFilePartOfFileName(vecFiles[iFile]));
    if (iFile < vecFiles.size() - 1) {
      //Add a separator.
      progName = progName + "-";
    }
  }

  AssertOrThrow2(progName.compare(StringUtil::empty()) != 0);

  return progName;
}
string_t ShaderMaker::getGeneralErrorsAsString(bool clear) {
  if (_vecGeneralErrors.size() == 0) {
    return "";
  }

  string_t ret = "";
  ret += string_t("\n\n[  General Errors  ]\n");
  for (string_t str : _vecGeneralErrors) {
    ret += ">> ";
    ret += str;
    ret += "\n";
  }
  ret += "\n\n";

  if (clear) {
    _vecGeneralErrors.clear();
  }
  return ret;
}
bool ShaderMaker::checkForErrors(std::vector<std::shared_ptr<ShaderSubProgram>>& vecSubProgs, std::shared_ptr<ShaderBase> sp) {

  string_t errStr = getGeneralErrorsAsString();

  for (std::shared_ptr<ShaderSubProgram> subProg : vecSubProgs) {
    if (subProg->getGeneralErrors().size() > 0) {
      errStr += subProg->getSourceLocation() + "\r\nGen. Errors\r\n, " + subProg->getSourceLocation() + "\r\n";
      for (string_t gErr : subProg->getGeneralErrors()) {
        errStr += gErr + "\r\n";
      }
    }
    if (subProg->getCompileErrors().size() > 0) {
      errStr += subProg->getSourceLocation() + "\r\nCompile Errors\r\n, " + subProg->getSourceLocation() + "\r\n";
      for (string_t gErr : subProg->getCompileErrors()) {
        errStr += gErr + "\r\n";
      }
    }
  }
  if (sp != nullptr) {
    if (sp->getLinkErrors().size() > 0) {
      errStr += Stz"Link Errors:" + "\r\n";
      for (string_t str : sp->getLinkErrors()) {
        errStr += str + "\r\n";
      }
    }
  }

  if (errStr != "") {
    if (StringUtil::lowercase(errStr).find("error") == string_t::npos) {
    }
    else {
      //for (std::shared_ptr<ShaderSubProgram> sub : vecSubProgs) {
      //    sub = nullptr///delete sub;
      //}
      vecSubProgs.resize(0);

      // If there were no errors there still may be warnings and we want to print those to
      // the console
      Br2LogError(errStr);
      Gu::debugBreak();
      return true;
    }
  }
  return false;

}

void ShaderMaker::loadSubPrograms(std::vector<string_t>& vecFiles, std::vector<std::shared_ptr<ShaderSubProgram>>& __out_ subProgs) {
  std::vector<std::shared_ptr<ShaderSubProgram>> vx;
  std::shared_ptr<ShaderSubProgram> pSubProg;

  // - Remove dupes in source files.
  for (string_t strFile : vecFiles) {

    if (!FileSystem::fileExists(strFile)) {
      _vecGeneralErrors.push_back(Stz "Shader source file '" + strFile + "' was not found.");
      continue;
    }

    pSubProg = preloadShaderSubProgram(strFile);

    if (pSubProg != nullptr) {
      subProgs.push_back(pSubProg);
    }
  }
}
void ShaderMaker::removeDuplicateSourceFiles(std::vector<string_t>& vecFiles) {
  for (int i = 0; i < (int)vecFiles.size(); ++i) {
    for (int j = i + 1; j < (int)vecFiles.size(); ++j) {
      if (StringUtil::lowercase(vecFiles[j]) == StringUtil::lowercase(vecFiles[i])) {
        vecFiles.erase(vecFiles.begin() + j);
        j--;
      }

    }
  }
}
std::shared_ptr<ShaderSubProgram> ShaderMaker::getShaderSubProgramByLocation(DiskLoc loc) {
  for (std::shared_ptr<ShaderSubProgram> sp : _setSubPrograms) {
    if (StringUtil::equalsi(sp->getSourceLocation(), loc)) {
      return sp;
    }
  }
  return nullptr;
}
std::shared_ptr<ShaderSubProgram> ShaderMaker::preloadShaderSubProgram(DiskLoc loc) {
  // - See if we've already loaded the program.  If we have then compile it.
  std::shared_ptr<ShaderSubProgram> pSubProg;

  // - If we're null we haven't created the shader yet
  pSubProg = getShaderSubProgramByLocation(loc);
  if (pSubProg == nullptr) {
    //Adds subprog to map.
    pSubProg = std::make_shared<ShaderSubProgram>();
    pSubProg->init(Gu::getContext(), loc, ShaderType::e::st_use_extension);

    _setSubPrograms.insert(pSubProg);
  }
  else {
    Br2LogDebug("Shader subprog " + loc + " already loaded.");
  }

  // - Load source, if it fails dump to console and return.
  _pShaderCompiler->loadSource(pSubProg);
  if (pSubProg->getStatus() != ShaderStatus::Loaded) {
    string_t str = pSubProg->getHumanReadableErrorString();

    addGeneralError(str);

    pSubProg = nullptr;
    //DEL_MEM(pSubProg);
    return nullptr;
  }

  return pSubProg;
}
void ShaderMaker::compileShaderSubProgram(std::shared_ptr<ShaderSubProgram> pShader) {
  // - Next compile it
  _pShaderCompiler->compile(pShader);
  if (pShader->getStatus() != ShaderStatus::Compiled) {
    string_t str = pShader->getHumanReadableErrorString();
    addGeneralError(str);
  }

}
std::shared_ptr<ShaderBase> ShaderMaker::makeProgram(std::vector<std::shared_ptr<ShaderSubProgram>>& vecpsp, string_t& programName) {
  std::shared_ptr<ShaderBase> pProgram = std::make_shared<ShaderBase>(programName);
  pProgram->init(); //The program must be constructed for the shader cache.

  //One compilation time which is the same across all sub-programs.
  time_t compileTime = time(NULL);

  // - Compile and add all sub programs
  for (std::shared_ptr<ShaderSubProgram> subProg : vecpsp) {

    if (subProg == nullptr) {
      Br2ThrowException(" [OpenGL] Tried to bind a null shader program.  Check for shader compile errors. ");
    }

    //Compile Program
    compileShaderSubProgram(subProg);

    if (subProg->getStatus() == ShaderStatus::e::Compiled) {
      subProg->setCompileTime(compileTime);
      pProgram->getSubPrograms().push_back(subProg);
    }
    else {
      addGeneralError(Stz "[Link] Sub program not compiled or compile failed." + subProg->getSourceLocation());

      _setSubPrograms.erase(_setSubPrograms.find(subProg));
      // removeSubProgram(vecpsp[i]);
      subProg = nullptr;
      pProgram = nullptr;
      return nullptr;
    }
  }


  for (std::shared_ptr<ShaderSubProgram> subProg : pProgram->getSubPrograms()) {//size_t i = 0; i<pProgram->_vecSubPrograms.size(); ++i) {
    Gu::getContext()->glAttachShader(pProgram->getGlId(), subProg->getGlId());
    GLuint err = glGetError();

    if (err != GL_NO_ERROR) {
      addGeneralError(Stz "[Link] Failed to attach all sub-programs. Error:" + err);
      //DEL_MEM(pProgram);
      pProgram = nullptr;
      return nullptr;
    }
    subProg->setStatus(ShaderStatus::e::Linked);
  }


  // - Issue here - do we need to validate? is it really a performance issue?
  if (!validateShadersForProgram(pProgram)) {
    addGeneralError("[Link] Failed to validate all shaders.");

    pProgram = nullptr;
    //DEL_MEM(pProgram);
    return nullptr;
  }

  // - Link the program.
  Gu::getContext()->glLinkProgram(pProgram->getGlId());
  pProgram->setProgramStatus(ShaderStatus::Linked);

  // - Add all remaining errors from the GL
  Br2LogDebug("Program linked.. getting info log.");
  std::vector<string_t> errorLog;
  getProgramErrorLog(pProgram, errorLog);
  for (string_t strErr : errorLog) {
    addGeneralError(strErr);
  }

  // - Try to use it to see if we get an error, if so exit out.
  //Do not use Gd::BindSHader here -- this might fail.
  Gu::getContext()->glUseProgram(pProgram->getGlId());
  GLenum ex = glGetError();
  if (ex != GL_NO_ERROR) {
    string_t str = "";

    for (size_t i = 0; i < _vecGeneralErrors.size(); ++i) {
      str += _vecGeneralErrors[i] + string_t("\n");
    }

    for (std::shared_ptr<ShaderSubProgram> subProg : pProgram->getSubPrograms()) {
      subProg->debugPrintShaderSource();
    }
    Br2LogError(str);
    Br2LogDebug("Note: any of the previous errors can be from any of the given shader source files.");

    //delete pProgram;
    pProgram = nullptr;

    Gu::debugBreak();

    return nullptr;
  }
  else {
    // - set compile time.
    pProgram->setCompileTime(compileTime);

    _mapPrograms.insert(std::make_pair(pProgram->getNameHashed(), pProgram));

    Br2LogInfo("Created shader " + pProgram->getProgramName());

    _pShaderCache->saveCompiledBinaryToDisk(pProgram);

    //NOTE: must come last - this ensures we know the program is 100% complete in the system.
    pProgram->setProgramStatus(ShaderStatus::Loaded);
  }

  AssertOrThrow2(pProgram != nullptr);

  return pProgram;
}
bool ShaderMaker::validateShadersForProgram(std::shared_ptr<ShaderBase> psp) {
  bool retVal = true;

  for (std::shared_ptr<ShaderSubProgram> subProg : psp->getSubPrograms()) {
    if (!validateSubProgram(subProg, psp)) {
      Gu::getContext()->glDetachShader(psp->getGlId(), subProg->getGlId());
      psp->getLinkErrors().push_back(Stz
        "[Link] Failed to validate sub-program " + subProg->getSourceLocation()
      );
      retVal = false;
    }
    subProg->setStatus(ShaderStatus::e::CreateComplete);
  }


  return retVal;
}
/**
*  @fn validateProgram()
*  @brief Validates a shader program compiled ok.
*  @return True if the program is valid.
*  @return False if one or more shaders are invalid, or something else.
*/
bool ShaderMaker::validateSubProgram(std::shared_ptr<ShaderSubProgram> prog, std::shared_ptr<ShaderBase> psp) {
  if (prog == nullptr) {
    psp->getLinkErrors().push_back(Stz " Program was null: " + prog->getSourceLocation());
    return false;
  }
  if (prog->getStatus() != ShaderStatus::e::Linked) {
    psp->getLinkErrors().push_back(
      Stz " Sub-Program invalid status: " + (int)prog->getStatus() + " : " + prog->getSourceLocation()
    );
    return false;
  }

  return true;
}
void ShaderMaker::getProgramErrorLog(std::shared_ptr<ShaderBase> psp, std::vector<string_t>& __out_ outLog) {
  AssertOrThrow2(psp != NULL);

  // - Do your stuff
  GLsizei buf_size;
  Gu::getContext()->glGetProgramiv(psp->getGlId(), GL_INFO_LOG_LENGTH, &buf_size);

  char* log_out = (char*)GameMemoryManager::allocBlock(buf_size);
  GLsizei length_out;
  Gu::getContext()->glGetProgramInfoLog(psp->getGlId(), buf_size, &length_out, log_out);

  string_t tempStr;
  char* c = log_out;

  for (int i = 0; i < length_out; ++i) {
    while ((*c) && (*c) != '\n' && (i < length_out)) {
      tempStr += (*c);
      c++;
      i++;
    }
    outLog.push_back(tempStr);
    tempStr.clear();
    c++;
  }

  GameMemoryManager::freeBlock(log_out);
}
std::shared_ptr<ShaderUniformBlock> ShaderMaker::getUniformBlockByName(string_t& blockName) {
  Hash32 blockHash = STRHASH(blockName);
  std::map<Hash32, std::shared_ptr<ShaderUniformBlock>>::iterator ite = _mapUniformBlocks.find(blockHash);
  if (ite == _mapUniformBlocks.end()) {
    return nullptr;
  }
  return ite->second;
}

void ShaderMaker::parseUniforms(std::shared_ptr<ShaderBase> sb) {
  GLint nUniforms = 0;
  static const int NBUFSIZ = 512;
  char name[NBUFSIZ];
  sb->deleteUniforms();

  //Uniforms
  Br2LogInfo(" Parsing " + nUniforms + " shader Uniforms..");
  Gu::getContext()->glGetProgramiv(sb->getGlId(), GL_ACTIVE_UNIFORMS, &nUniforms);
  for (int32_t i = 0; i < nUniforms; ++i) {
    GLint name_len = -1;
    GLint iArraySize = -1;
    GLenum uniformType = GL_ZERO;
    string_t uniformName;

    //Get name an d type
    Gu::getContext()->glGetActiveUniform(sb->getGlId(), (GLuint)i, NBUFSIZ, &name_len, &iArraySize, &uniformType, (char*)name);
    AssertOrThrow2(name_len < NBUFSIZ);
    name[name_len] = 0;
    uniformName = string_t(name);

    //get location
    GLint glLocation = Gu::getContext()->glGetUniformLocation((GLuint)sb->getGlId(), (GLchar*)uniformName.c_str());

    if (glLocation >= 0) {
      //If location >=0 - then we are not a buffer.
      if (StringUtil::contains(uniformName, "_ufShadowBoxSamples")) {
        Br2LogWarn("Altering Uniform '" + name + "' to '_ufShadowBoxSamples'.  This is inconsistent among vendors.");

        uniformName = "_ufShadowBoxSamples";
      }
      else if (StringUtil::contains(uniformName, "_ufGaussianWeight")) {
        Br2LogWarn("Altering Uniform '" + name + "' to '_ufGaussianWeight'.  This is inconsistent among vendors.");

        uniformName = "_ufGaussianWeight";
      }
      else if (StringUtil::contains(uniformName, "_ufShadowFrustumSamples")) {
        Br2LogWarn("Altering Uniform '" + name + "' to '_ufShadowFrustumSamples'.  This is inconsistent among vendors.");

        uniformName = "_ufShadowFrustumSamples";
      }
      else if (StringUtil::findFirstOf(name, std::vector<char> { '.', '[', ']' }) != string_t::npos) {
        Br2LogWarn("Uniform name '" + name + "' was not valid but was parsed as a basic uniform. Could be a buffer. (parse error).");
        Gu::debugBreak();
      }


      if (StringUtil::equals(uniformName, "gl_NumWorkGroups")) {
        Br2LogError(" [The GPU implementation thought the system variable " + uniformName + " was a uniform variable.  This is incorrect.  Ignoring...");
        continue;
      }

      std::shared_ptr<ShaderUniform> glVar = std::make_shared<ShaderUniform>(Gu::getGraphicsContext(), uniformType, glLocation, uniformName, iArraySize);

      sb->getUniforms().insert(std::make_pair(glVar->getNameHashed(), glVar));
    }
    else {
      //Uniform is a member of a block.
    }
  }

}
void ShaderMaker::parseUniformBlocks(std::shared_ptr<ShaderBase> sb) {
  //Blocks are global to all shaders.

  GLint nUniformBlocks = 0;
  static const int NBUFSIZ = 512;
  char name[NBUFSIZ];

  GLint iMaxUniformBufferBindings = 0;
  glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &iMaxUniformBufferBindings);

  //Uniform Blocks
  Br2LogInfo(" Parsing " + nUniformBlocks + " shader Uniform Blocks..");
  Gu::getContext()->glGetProgramiv(sb->getGlId(), GL_ACTIVE_UNIFORM_BLOCKS, &nUniformBlocks);
  for (int32_t iBlock = 0; iBlock < nUniformBlocks; ++iBlock) {
    string_t blockName;
    GLint blockDataSize;
    GLint activeUniforms;

    //Because we haven't bound the block yet, the binding will be zero.
    //   GLint binding;
    //  Gu::getContext()->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_BINDING, &binding);
    Gu::getContext()->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_DATA_SIZE, &blockDataSize);
    Gu::getContext()->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniforms);

    Gu::getContext()->glGetActiveUniformBlockName(sb->getGlId(), iBlock, NBUFSIZ, NULL, name);
    blockName = string_t(name);

    std::shared_ptr<ShaderUniformBlock> pBlock;

    pBlock = getUniformBlockByName(blockName);
    if (pBlock == nullptr) {
      //Increment the global block binding index.
      static int s_iBindingIndex = 0;
      GLint bufferIndex;
      GLint bindingIndex = s_iBindingIndex++;

      if (bindingIndex > iMaxUniformBufferBindings) {
        Br2LogWarn("The binding index " + bindingIndex + " was greater than the maximum number of UBO bindings: " + iMaxUniformBufferBindings + ".");
        Gu::debugBreak();
      }

      //set the program's buffer to be bound to this one.
      bufferIndex = Gu::getContext()->glGetUniformBlockIndex(sb->getGlId(), blockName.c_str());
      Gu::getContext()->glUniformBlockBinding(sb->getGlId(), bufferIndex, bindingIndex);

      pBlock = std::make_shared<ShaderUniformBlock>(Gu::getGraphicsContext(), blockName, bufferIndex, bindingIndex, blockDataSize);

      //Uniform blocks are shared between programs.  We doulbe up the reference here.
      _mapUniformBlocks.insert(std::make_pair(STRHASH(blockName), pBlock));
      sb->getUniformBlocks().insert(std::make_pair(STRHASH(blockName), pBlock));
    }
    else {
      Br2LogInfo("Shader block '" + blockName + "' already created.");
    }
  }
}
void ShaderMaker::parseAttributes(std::shared_ptr<ShaderBase> sb) {
  if (!sb->confirmInit())
    return;

  sb->deleteAttributes();

  string_t err = "";

  GLsizei nAttributes;
  Gu::getContext()->glGetProgramiv(sb->getGlId(), GL_ACTIVE_ATTRIBUTES, &nAttributes);

  // std::vector<GlslShaderAttribute*> tempAttributes;
  for (GLsizei iAttr = 0; iAttr < nAttributes; ++iAttr) {
    ShaderAttribute* attr = new ShaderAttribute(sb, (int32_t)iAttr);
    err += attr->getError();

    //Verify no dupes.
    for (ShaderAttribute* attrOther : sb->getAttributes()) {
      if (attrOther->getUserType() == attr->getUserType()) {
        if (attrOther->getUserType() != VertexUserType::e::None) {
          err += Stz "  " + sb->getProgramName() +
            " - Duplicate shader attribute '" + VertexFormat::getUserTypeName(attrOther->getUserType()) + "'.";
          Gu::debugBreak();
        }
      }
    }

    //Add
    sb->getAttributes().insert(attr);
  }

  if (err != "") {
    sb->setProgramStatus(ShaderStatus::e::Error);
    addGeneralError(Stz "Shader " + sb->getProgramName() + "\r\n" + err);
  }
}
bool ShaderMaker::isGoodStatus(ShaderStatus::e stat) {
  bool b = true;
  b = b && (stat != ShaderStatus::e::CompileError);
  b = b && (stat != ShaderStatus::e::FileNotFound);
  b = b && (stat != ShaderStatus::e::Error);
  b = b && (stat != ShaderStatus::e::Uninitialized);
  b = b && (stat != ShaderStatus::e::CreateFailed);
  b = b && (stat != ShaderStatus::e::LoadError);
  b = b && (stat != ShaderStatus::e::Unloaded);
  return b;
}
std::shared_ptr<ShaderBase> ShaderMaker::getShaderByName(const string_t& name) {
  Hash32 h = STRHASH(name);
  std::map<Hash32, std::shared_ptr<ShaderBase>>::iterator it = _mapPrograms.find(h);
  if (it != _mapPrograms.end()) {
    return it->second;
  }
  return nullptr;
}
std::shared_ptr<ShaderBase> ShaderMaker::getShaderById(GLuint id) {
  for (std::pair<Hash32, std::shared_ptr<ShaderBase>> p : _mapPrograms) {
    if (p.second->getGlId() == id) {
      return p.second;
    }
  }
  return nullptr;
}
string_t ShaderMaker::getShaderNameForId(GLuint id) {
  std::shared_ptr<ShaderBase> sb = getShaderById(id);
  if (sb != nullptr) {
    return sb->getProgramName();
  }
  return "";
}
void ShaderMaker::shaderBound(std::shared_ptr<ShaderBase> sb) {
  //This system is here for sanity checking of uniform bindings.
  //It's not "necessary", but it ensures uniforms are set.
  if (_pBound != nullptr && _pBound != sb) {
    _pBound->unbindAllUniforms();
  }
  if (sb == nullptr) {
    Gu::getContext()->glUseProgram(0);
  }
  else {// if(_pBound != sb ){ //*this was causing errors because we must be calling UseProgram somewhere.
    Gu::getContext()->glUseProgram(sb->getGlId());
  }
  _pBound = sb;

}


string_t ShaderMaker::systemTypeToSTring(OpenGLShaderVarType::e eType) {
  switch (eType) {
  case OpenGLShaderVarType::ut_notype: return  "ut_notype "; break;
  case OpenGLShaderVarType::GpuInt1: return    "GpuInt1   "; break;
  case OpenGLShaderVarType::GpuInt2: return    "GpuInt2   "; break;
  case OpenGLShaderVarType::GpuInt3: return    "GpuInt3   "; break;
  case OpenGLShaderVarType::GpuInt4: return    "GpuInt4   "; break;
  case OpenGLShaderVarType::GpuUint1: return   "GpuUint1  "; break;
  case OpenGLShaderVarType::GpuUint2: return   "GpuUint2  "; break;
  case OpenGLShaderVarType::GpuUint3: return   "GpuUint3  "; break;
  case OpenGLShaderVarType::GpuUint4: return   "GpuUint4  "; break;
  case OpenGLShaderVarType::GpuFloat1: return  "GpuFloat1 "; break;
  case OpenGLShaderVarType::GpuFloat2: return  "GpuFloat2 "; break;
  case OpenGLShaderVarType::GpuFloat3: return  "GpuFloat3 "; break;
  case OpenGLShaderVarType::GpuFloat4: return  "GpuFloat4 "; break;
  case OpenGLShaderVarType::GpuBool1: return   "GpuBool1  "; break;
  case OpenGLShaderVarType::GpuBool2: return   "GpuBool2  "; break;
  case OpenGLShaderVarType::GpuBool3: return   "GpuBool3  "; break;
  case OpenGLShaderVarType::GpuBool4: return   "GpuBool4  "; break;
  case OpenGLShaderVarType::GpuMat2: return    "GpuMat2   "; break;
  case OpenGLShaderVarType::GpuMat3: return    "GpuMat3   "; break;
  case OpenGLShaderVarType::GpuMat4: return    "GpuMat4   "; break;
  case OpenGLShaderVarType::GpuDouble1: return "GpuDouble1"; break;
  case OpenGLShaderVarType::GpuDouble2: return "GpuDouble2"; break;
  case OpenGLShaderVarType::GpuDouble3: return "GpuDouble3"; break;
  case OpenGLShaderVarType::GpuDouble4: return "GpuDouble4"; break;
  default: return "Unknown.";
    break;
  }
}
void ShaderMaker::setUfBlock(string_t name, void* value, size_t copySizeBytes, bool bIgnore) {
  std::shared_ptr<ShaderUniformBlock> uf = getUniformBlockByName(name);
  if (uf == nullptr) {
    if (bIgnore == false) {
      Br2LogWarnCycle("Uniform Block '" + name + "' could not be found.");
    }
  }
  else {
    uf->copyUniformData(value, copySizeBytes);
  }
}
void ShaderMaker::addGeneralError(string_t str) {
  str = Stz "error: " + str + "\r\n";
  _vecGeneralErrors.push_back(str);
}






}//ns Game
