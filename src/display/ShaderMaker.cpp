#include "../base/BaseAll.h"
#include "../display/ShaderMaker.h"
#include "../display/ShaderBase.h"
#include "../display/ShaderSubProgram.h"
#include "../display/ShaderCache.h"
#include "../display/ShaderCompiler.h"
#include "../display/ShaderBase.h"
#include "../display/ShaderUniform.h"
#include "../display/ShaderAttribute.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"


namespace Game {
///////////////////////////////////////////////////////////////////
ShaderMaker::ShaderMaker(std::shared_ptr<GLContext> ct) : _pContext(ct)
{

}
ShaderMaker::~ShaderMaker()
{
    //Delete all Shaders
    //DELETE_VECTOR_ELEMENTS(_vecShaders);
    _pShaderCache = nullptr;
    _pShaderCompiler = nullptr;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ShaderMaker::initialize(std::shared_ptr<RoomBase> mainRoom) {
    t_string assetPath = mainRoom->getAssetsDir();
    t_string cacheDir = FileSystem::combinePath(assetPath, mainRoom->getCacheDir());
    t_string shadersDir = FileSystem::combinePath(assetPath, mainRoom->getShadersDir());

    _pShaderCache = std::make_shared<ShaderCache>(_pContext, cacheDir);
    _pShaderCompiler = std::make_shared<ShaderCompiler>(_pContext, shadersDir);

    //Single VT shaders
    _pImageShader = makeShader(std::vector<t_string> { "f_v3x2_diffuse.vs", "f_v3x2_diffuse.ps" });
    _pNormalsShader = makeShader(std::vector<t_string> { "f_v3n3_normals.vs", "f_v3n3_normals.gs", "f_v3n3_normals.ps" });
    _pShadowBlendShader = makeShader(std::vector<t_string> {"v2x2_shadow_blend.vs", "v2x2_shadow_blend.ps" });
    _pDepthOfField = makeShader(std::vector<t_string> {"v3x2_depth_of_field.vs", "v3x2_depth_of_field.ps" });
    _pSmoothGen = makeShader(std::vector<t_string> {"v3x2_smooth_gen.vs", "v3x2_smooth_gen.ps" });
    //Multiple VT shaders
    _pShadowShaders.insert(std::make_pair(v_v3::getVertexFormat(), makeShader(std::vector<t_string> {"v3_shadowmap.vs", "v3_shadowmap.ps" })));
    _pShadowShaders.insert(std::make_pair(v_v3x2::getVertexFormat(), makeShader(std::vector<t_string> {"v3x2_shadowmap.vs", "v3x2_shadowmap.ps" })));

    _pDiffuseShaders.insert(std::make_pair(v_v3n3x2::getVertexFormat(), makeShader(std::vector<t_string> { "d_v3n3x2_diffuse.vs", "d_v3n3x2_diffuse.ps" })));
    _pDiffuseShaders.insert(std::make_pair(v_v3x2::getVertexFormat(), makeShader(std::vector<t_string> { "d_v3x2_diffuse.vs", "d_v3x2_diffuse.ps" })));
    _pDiffuseShaders.insert(std::make_pair(v_v3n3::getVertexFormat(), makeShader(std::vector<t_string> { "d_v3n3_diffuse.vs", "d_v3n3_diffuse.ps" })));
    _pDiffuseShaders.insert(std::make_pair(v_v3::getVertexFormat(), makeShader(std::vector<t_string> { "d_v3_diffuse.vs", "d_v3_diffuse.ps" })));
    _pDiffuseShaders.insert(std::make_pair(v_v2x2::getVertexFormat(), makeShader(std::vector<t_string> { "d_v2x2_diffuse.vs", "d_v2x2_diffuse.ps" })));
    
    _pGlassShaders.insert(std::make_pair(v_v3n3x2::getVertexFormat(), makeShader(std::vector<t_string> { "f_v3n3x2_glass.vs", "f_v3n3x2_glass.ps" })));
    _pGlassShaders.insert(std::make_pair(v_v3n3::getVertexFormat(), makeShader(std::vector<t_string> { "f_v3n3_glass.vs", "f_v3n3_glass.ps" })));

    _pFlatShaders.insert(std::make_pair(v_v2c4::getVertexFormat(), makeShader(std::vector<t_string> { "f_v2c4_flat.vs", "f_v2c4_flat.ps" })));
    _pFlatShaders.insert(std::make_pair(v_v3c4::getVertexFormat(), makeShader(std::vector<t_string> { "f_v3c4_flat.vs", "f_v3c4_flat.ps" })));

    //_pGuiShader = makeShader(std::vector<t_string> { "f_v2c4x2_gui.vs", "f_v2c4x2_gui.ps" });
    _pGuiShader = makeShader(std::vector<t_string> { "f_gui.vs", "f_gui.gs", "f_gui.ps" });


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

    _pContext->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &_maxWorkGroupDims[0]);
    _pContext->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &_maxWorkGroupDims[1]);
    _pContext->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &_maxWorkGroupDims[2]);

    GLint maxBindings;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxBindings);

    if (maxBindings <= 0) {
        BroThrowException("The max number of shader storage buffers is zero. This likely means you are running an older graphics card where Compute Shading isn't supported.");
    }
    _maxBufferBindings = maxBindings;

}

std::shared_ptr<ShaderBase> ShaderMaker::makeShader(std::vector<t_string>& vecFiles) {
    std::vector<std::shared_ptr<ShaderSubProgram>> vecSubProgs;
    std::shared_ptr<ShaderBase> pShader = nullptr;

    //20160608 Note:
    //  1) We create a shader sub-program with glCreateShader
    //  2) We then load all source
    //  3) We then TRY TO GET THE CACHED PROGRAM
    //          This is a problem.  We should attempt to get the cached program first
    //  4) Then we compile/link all programs
    try
    {
        BroLogInfo("\r\n------------------------------\r\nShaderMaker: Processing ", vecFiles.size(), " files.");
        removeDuplicateSourceFiles(vecFiles);
        t_string strShaderName = getShaderNameFromFileNames(vecFiles);
        fullyQualifyFiles(vecFiles);

        BroLogInfo("Processing Shader: ", strShaderName);

        loadShaderSubPrograms(vecFiles, vecSubProgs);
        if (checkForErrors(vecSubProgs, pShader)) {
            return nullptr;
        }

        pShader = makeProgram(vecSubProgs, strShaderName);
        if (checkForErrors(vecSubProgs, pShader)) {
            deleteShader(pShader);
            return nullptr;
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

        BroLogInfo("Load Success\r\n------------------------------\r\n");
    }
    catch (Exception* ex)
    {
        BroLogError(ex->what());
    }

    return pShader;
}
void ShaderMaker::deleteShader(std::shared_ptr<ShaderBase> ps) {
    if (ps != nullptr) {
        std::map<Hash32, std::shared_ptr<ShaderBase>>::iterator it = _mapPrograms.find(ps->getNameHashed());
        if (it != _mapPrograms.end())
        {
            _mapPrograms.erase(it);
        }
        // delete ps;
    }
}
void ShaderMaker::fullyQualifyFiles(std::vector<t_string>& vecFiles) {
    AssertOrThrow2(_pContext->getRoom() != nullptr);

    for (size_t iFile = 0; iFile < vecFiles.size(); iFile++) { // strFile : vecFiles) {
        t_string qual = _pContext->getRoom()->makeAssetPath(_pContext->getRoom()->getShadersDir(), vecFiles[iFile]);
        vecFiles[iFile] = qual;
    }
}
t_string ShaderMaker::getShaderNameFromFileNames(std::vector<t_string>& vecFiles)
{
    t_string progName = StringUtil::empty();

    for (size_t iFile = 0; iFile < vecFiles.size(); ++iFile) {
        progName.append(FileSystem::getFilePartOfFileName(vecFiles[iFile]));
        if (iFile < vecFiles.size() - 1) {
            //Add a separator.
            progName = TStr(progName, "-");
        }
    }

    AssertOrThrow2(progName.compare(StringUtil::empty()) != 0);

    return progName;
}
t_string ShaderMaker::getGeneralErrorsAsString()
{
    if (_vecGeneralErrors.size() == 0) {
        return "";
    }

    t_string ret = "";
    ret += t_string("\n\n[  General Errors  ]\n");
    for (t_string str : _vecGeneralErrors) {
        ret += ">> ";
        ret += str;
        ret += "\n";
    }
    ret += "\n\n";
    return ret;
}
bool ShaderMaker::checkForErrors(std::vector<std::shared_ptr<ShaderSubProgram>>& vecSubProgs, std::shared_ptr<ShaderBase> sp) {

    t_string errStr = getGeneralErrorsAsString();

    for (std::shared_ptr<ShaderSubProgram> subProg : vecSubProgs) {
        if (subProg->getGeneralErrors().size() > 0) {
            errStr += TStr(subProg->getSourceLocation(), "\r\nGen. Errors\r\n, ", subProg->getSourceLocation(), "\r\n");
            for (t_string gErr : subProg->getGeneralErrors()) {
                errStr += TStr(gErr, "\r\n");
            }
        }
        if (subProg->getCompileErrors().size() > 0) {
            errStr += TStr(subProg->getSourceLocation(), "\r\nCompile Errors\r\n, ", subProg->getSourceLocation(), "\r\n");
            for (t_string gErr : subProg->getCompileErrors()) {
                errStr += TStr(gErr, "\r\n");
            }
        }
    }
    if (sp != nullptr) {
        if (sp->getLinkErrors().size() > 0) {
            errStr += TStr("Link Errors:", "\r\n");
            for (t_string str : sp->getLinkErrors()) {
                errStr += TStr(str, "\r\n");
            }
        }
    }

    if (errStr != "") {
        if (StringUtil::lowercase(errStr).find("error") == t_string::npos)
        {
        }
        else
        {
            //for (std::shared_ptr<ShaderSubProgram> sub : vecSubProgs) {
            //    sub = nullptr///delete sub;
            //}
            vecSubProgs.resize(0);

            // If there were no errors there still may be warnings and we want to print those to
            // the console
            BroLogError(errStr);
            Gu::debugBreak();
            return true;
        }
    }
    return false;

}

void ShaderMaker::loadShaderSubPrograms(std::vector<t_string>& vecFiles, std::vector<std::shared_ptr<ShaderSubProgram>>&  __out_ subProgs)
{
    std::vector<std::shared_ptr<ShaderSubProgram>> vx;
    std::shared_ptr<ShaderSubProgram> pSubProg;

    // - Remove dupes in source files.
    for (t_string strFile : vecFiles) {

        if (!FileSystem::fileExists(strFile)) {
            _vecGeneralErrors.push_back(TStr("Shader source file '", strFile, "' was not found."));
            continue;
        }

        pSubProg = preloadShaderSubProgram(strFile);

        if (pSubProg != nullptr) {
            subProgs.push_back(pSubProg);
        }
    }
}
void ShaderMaker::removeDuplicateSourceFiles(std::vector<t_string>& vecFiles)
{
    for (int i = 0; i < (int)vecFiles.size(); ++i)
    {
        for (int j = i + 1; j < (int)vecFiles.size(); ++j)
        {
            if (StringUtil::lowercase(vecFiles[j]) == StringUtil::lowercase(vecFiles[i]))
            {
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
std::shared_ptr<ShaderSubProgram> ShaderMaker::preloadShaderSubProgram(DiskLoc loc)
{
    // - See if we've already loaded the program.  If we have then compile it.
    std::shared_ptr<ShaderSubProgram> pSubProg;

    // - If we're null we haven't created the shader yet
    pSubProg = getShaderSubProgramByLocation(loc);
    if (pSubProg == nullptr) {
        //Adds subprog to map.
        pSubProg = std::make_shared<ShaderSubProgram>();
        pSubProg->init(_pContext, loc, ShaderType::e::st_use_extension);

        _setSubPrograms.insert(pSubProg);
    }
    else {
        BroLogDebug("Shader subprog ", loc, " already loaded.");
    }

    // - Load source, if it fails dump to console and return.
    _pShaderCompiler->loadSource(pSubProg);
    if (pSubProg->getStatus() != ShaderStatus::Loaded) {
        t_string str = pSubProg->getHumanReadableErrorString();

        addGeneralError(str);

        pSubProg = nullptr;
        //DEL_MEM(pSubProg);
        return nullptr;
    }

    return pSubProg;
}
void ShaderMaker::compileShaderSubProgram(std::shared_ptr<ShaderSubProgram> pShader)
{
    // - Next compile it
    _pShaderCompiler->compile(pShader);
    if (pShader->getStatus() != ShaderStatus::Compiled) {
        t_string str = pShader->getHumanReadableErrorString();
        addGeneralError(str);
    }

}
std::shared_ptr<ShaderBase> ShaderMaker::makeProgram(std::vector<std::shared_ptr<ShaderSubProgram>>& vecpsp, t_string& programName)
{
    std::shared_ptr<ShaderBase> pProgram = std::make_shared<ShaderBase>(_pContext, programName);
    pProgram->init(); //The program must be constructed for the shader cache.

    //One compilation time which is the same across all sub-programs.
    time_t compileTime = time(NULL);

    //TODO: 20160608 in the future, move this code up ABOVE the shader creation stage.
    // because we're actually creating the shader objects and caching them.  If we try
    //to access one of those shader sub programs it will fail.
    // Try to load the cached binary. Return if successful
    if (_pShaderCache->tryLoadCachedBinary(pProgram, vecpsp))
    {
        pProgram->setProgramStatus(ShaderStatus::Linked);
        _mapPrograms.insert(std::make_pair(pProgram->getNameHashed(), pProgram));
        return pProgram;
    }


    // - Compile and add all sub programs
    for (std::shared_ptr<ShaderSubProgram> subProg : vecpsp) {

        if (subProg == nullptr) {
            BroThrowException(" [OpenGL] Tried to bind a null shader program.  Check for shader compile errors. ");
        }

        //Compile Program
        compileShaderSubProgram(subProg);

        if (subProg->getStatus() == ShaderStatus::e::Compiled) {
            subProg->setCompileTime(compileTime);
            pProgram->getSubPrograms().push_back(subProg);
        }
        else {
            addGeneralError(TStr("[Link] Sub program not compiled or compile failed.", subProg->getSourceLocation()));

            _setSubPrograms.erase(_setSubPrograms.find(subProg));
            // removeSubProgram(vecpsp[i]);
            subProg = nullptr;
            pProgram = nullptr;
            return nullptr;
        }
    }


    for (std::shared_ptr<ShaderSubProgram> subProg : pProgram->getSubPrograms()) {//size_t i = 0; i<pProgram->_vecSubPrograms.size(); ++i) {
        _pContext->glAttachShader(pProgram->getGlId(), subProg->getGlId());
        GLuint err = glGetError();

        if (err != GL_NO_ERROR) {
            addGeneralError(TStr("[Link] Failed to attach all sub-programs. Error:", err));
            //DEL_MEM(pProgram);
            pProgram = nullptr;
            return nullptr;
        }
        subProg->setStatus(ShaderStatus::e::Linked);
    }


    // - Issue here - do we need to validate? is it really a performance issue?
    if (!validateShadersForProgram(pProgram))
    {
        addGeneralError("[Link] Failed to validate all shaders.");

        pProgram = nullptr;
        //DEL_MEM(pProgram);
        return nullptr;
    }

    // - Link the program.
    _pContext->glLinkProgram(pProgram->getGlId());
    pProgram->setProgramStatus(ShaderStatus::Linked);

    // - Add all remaining errors from the GL
    BroLogDebug("Program linked.. getting info log.");
    std::vector<t_string> errorLog;
    getProgramErrorLog(pProgram, errorLog);
    for (t_string strErr : errorLog) {
        addGeneralError(strErr);
    }

    // - Try to use it to see if we get an error, if so exit out.
    //Do not use Gd::BindSHader here -- this might fail.
    _pContext->glUseProgram(pProgram->getGlId());
    GLenum ex = glGetError();
    if (ex != GL_NO_ERROR) {
        t_string str = "";

        for (size_t i = 0; i < _vecGeneralErrors.size(); ++i) {
            str += _vecGeneralErrors[i] + t_string("\n");
        }

        for (std::shared_ptr<ShaderSubProgram> subProg : pProgram->getSubPrograms()) {
            subProg->debugPrintShaderSource();
        }
        BroLogError(str);
        BroLogDebug("Note: any of the previous errors can be from any of the given shader source files.");

        //delete pProgram;
        pProgram = nullptr;

        Gu::debugBreak();

        return nullptr;
    }
    else
    {
        // - set compile time.
        pProgram->setCompileTime(compileTime);

        _mapPrograms.insert(std::make_pair(pProgram->getNameHashed(), pProgram));

        BroLogInfo("Created shader ", pProgram->getProgramName());

        _pShaderCache->saveCompiledBinaryToDisk(pProgram);

        //NOTE: must come last - this ensures we know the program is 100% complete in the system.
        pProgram->setProgramStatus(ShaderStatus::Loaded);
    }

    AssertOrThrow2(pProgram != nullptr);

    return pProgram;
}
bool ShaderMaker::validateShadersForProgram(std::shared_ptr<ShaderBase> psp)
{
    bool retVal = true;

    for (std::shared_ptr<ShaderSubProgram> subProg : psp->getSubPrograms())
    {
        if (!validateSubProgram(subProg, psp))
        {
            _pContext->glDetachShader(psp->getGlId(), subProg->getGlId());
            psp->getLinkErrors().push_back(TStr(
                "[Link] Failed to validate sub-program ", subProg->getSourceLocation()
            ));
            retVal = false;
        }
        subProg->setStatus(ShaderStatus::e::CreateComplete);
    }


    return retVal;
}
/**
*    @fn validateProgram()
*    @brief Validates a shader program compiled ok.
*    @return True if the program is valid.
*    @return False if one or more shaders are invalid, or something else.
*/
bool ShaderMaker::validateSubProgram(std::shared_ptr<ShaderSubProgram> prog, std::shared_ptr<ShaderBase> psp) {
    if (prog == nullptr) {
        psp->getLinkErrors().push_back(TStr(" Program was null: ", prog->getSourceLocation()));
        return false;
    }
    if (prog->getStatus() != ShaderStatus::e::Linked) {
        psp->getLinkErrors().push_back(TStr(
            " Sub-Program invalid status: ", (int)prog->getStatus(), " : ", prog->getSourceLocation()
        ));
        return false;
    }

    return true;
}
void ShaderMaker::getProgramErrorLog(std::shared_ptr<ShaderBase> psp, std::vector<t_string>& __out_ outLog) {
    AssertOrThrow2(psp != NULL);

    // - Do your stuff
    GLsizei buf_size;
    _pContext->glGetProgramiv(psp->getGlId(), GL_INFO_LOG_LENGTH, &buf_size);

    char* log_out = (char*)GameMemoryManager::allocBlock(buf_size);
    GLsizei length_out;
    _pContext->glGetProgramInfoLog(psp->getGlId(), buf_size, &length_out, log_out);

    t_string tempStr;
    char* c = log_out;

    for (int i = 0; i < length_out; ++i)
    {
        while ((*c) && (*c) != '\n' && (i < length_out))
        {
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
std::shared_ptr<ShaderUniformBlock> ShaderMaker::getUniformBlockByName(t_string& blockName) {
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
    t_char name[NBUFSIZ];
    sb->deleteUniforms();

    //Uniforms
    BroLogInfo(" Parsing ", nUniforms, " shader Uniforms..");
    _pContext->glGetProgramiv(sb->getGlId(), GL_ACTIVE_UNIFORMS, &nUniforms);
    for (int32_t i = 0; i < nUniforms; ++i)
    {
        GLint name_len = -1;
        GLint iArraySize = -1;
        GLenum uniformType = GL_ZERO;
        t_string uniformName;

        //Get name an d type
        _pContext->glGetActiveUniform(sb->getGlId(), (GLuint)i, NBUFSIZ, &name_len, &iArraySize, &uniformType, (char*)name);
        AssertOrThrow2(name_len < NBUFSIZ);
        name[name_len] = 0;
        uniformName = t_string(name);

        //get location
        GLint glLocation = _pContext->glGetUniformLocation((GLuint)sb->getGlId(), (GLchar*)uniformName.c_str());

        if (glLocation >= 0) {
            //If location >=0 - then we are not a buffer.
            if (StringUtil::contains(uniformName, TStr("_ufShadowBoxSamples"))) {
                BroLogWarn("Altering Uniform '", name, "' to '_ufShadowBoxSamples'.  This is inconsistent among vendors.");

                uniformName = "_ufShadowBoxSamples";
            }
            else if (StringUtil::contains(uniformName, TStr("_ufGaussianWeight"))) {
                BroLogWarn("Altering Uniform '", name, "' to '_ufGaussianWeight'.  This is inconsistent among vendors.");

                uniformName = "_ufGaussianWeight";
            }
            else if (StringUtil::contains(uniformName, TStr("_ufShadowFrustumSamples"))) {
                BroLogWarn("Altering Uniform '", name, "' to '_ufShadowFrustumSamples'.  This is inconsistent among vendors.");

                uniformName = "_ufShadowFrustumSamples";
            }
            else if (StringUtil::findFirstOf(name, std::vector<char> { '.', '[', ']' }) != t_string::npos) {
                BroLogWarn("Uniform name '", name, "' was not valid but was parsed as a basic uniform. Could be a buffer. (parse error).");
                Gu::debugBreak();
            }


            if (StringUtil::equals(uniformName, TStr("gl_NumWorkGroups")))
            {
                BroLogError(" [The GPU implementation thought the system variable ", uniformName, " was a uniform variable.  This is incorrect.  Ignoring...");
                continue;
            }

            std::shared_ptr<ShaderUniform> glVar = std::make_shared<ShaderUniform>(_pContext, uniformType, glLocation, uniformName, iArraySize);

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
    t_char name[NBUFSIZ];

    GLint iMaxUniformBufferBindings = 0;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &iMaxUniformBufferBindings);

    //Uniform Blocks
    BroLogInfo(" Parsing ", nUniformBlocks, " shader Uniform Blocks..");
    _pContext->glGetProgramiv(sb->getGlId(), GL_ACTIVE_UNIFORM_BLOCKS, &nUniformBlocks);
    for (int32_t iBlock = 0; iBlock < nUniformBlocks; ++iBlock) {
        t_string blockName;
        GLint blockDataSize;
        GLint activeUniforms;

        //Because we haven't bound the block yet, the binding will be zero.
        //   GLint binding;
        //  _pContext->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_BINDING, &binding);
        _pContext->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_DATA_SIZE, &blockDataSize);
        _pContext->glGetActiveUniformBlockiv(sb->getGlId(), (GLuint)iBlock, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniforms);

        _pContext->glGetActiveUniformBlockName(sb->getGlId(), iBlock, NBUFSIZ, NULL, name);
        blockName = t_string(name);

        std::shared_ptr<ShaderUniformBlock> pBlock;

        pBlock = getUniformBlockByName(blockName);
        if (pBlock == nullptr) {
            //Increment the global block binding index.
            static int s_iBindingIndex = 0;
            GLint bufferIndex;
            GLint bindingIndex = s_iBindingIndex++;

            if (bindingIndex > iMaxUniformBufferBindings) {
                BroLogWarn("The binding index ", bindingIndex, " was greater than the maximum number of UBO bindings: ", iMaxUniformBufferBindings, ".");
                Gu::debugBreak();
            }

            //set the program's buffer to be bound to this one.
            bufferIndex = _pContext->glGetUniformBlockIndex(sb->getGlId(), blockName.c_str());
            _pContext->glUniformBlockBinding(sb->getGlId(), bufferIndex, bindingIndex);

            pBlock = std::make_shared<ShaderUniformBlock>(_pContext, blockName, bufferIndex, bindingIndex, blockDataSize);

            //Uniform blocks are shared between programs.  We doulbe up the reference here.
            _mapUniformBlocks.insert(std::make_pair(STRHASH(blockName), pBlock));
            sb->getUniformBlocks().insert(std::make_pair(STRHASH(blockName), pBlock));
        }
        else {
            BroLogInfo("Shader block '", blockName, "' already created.");
        }
    }
}
void ShaderMaker::parseAttributes(std::shared_ptr<ShaderBase> sb)
{
    if (!sb->confirmInit())
        return;

    sb->deleteAttributes();

    t_string err = "";

    GLsizei nAttributes;
    _pContext->glGetProgramiv(sb->getGlId(), GL_ACTIVE_ATTRIBUTES, &nAttributes);

    // std::vector<GlslShaderAttribute*> tempAttributes;
    for (GLsizei iAttr = 0; iAttr < nAttributes; ++iAttr)
    {
        ShaderAttribute* attr = new ShaderAttribute(sb, (int32_t)iAttr);
        err += attr->getError();

        //Verify no dupes.
        for (ShaderAttribute* attrOther : sb->getAttributes()) {
            if (attrOther->getUserType() == attr->getUserType()) {
                if (attrOther->getUserType() != VertexUserType::e::None) {
                    err += TStr("  ", sb->getProgramName(),
                        " - Duplicate shader attribute '", VertexFormat::getUserTypeName(attrOther->getUserType()), "'.");
                    Gu::debugBreak();
                }
            }
        }

        //Add
        sb->getAttributes().insert(attr);
    }

    if (err != "")
    {
        sb->setProgramStatus(ShaderStatus::e::Error);
        addGeneralError(TStr("Shader ", sb->getProgramName(), "\r\n", err));
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
std::shared_ptr<ShaderBase> ShaderMaker::getShaderByName(const t_string& name) {
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
t_string ShaderMaker::getShaderNameForId(GLuint id) {
    std::shared_ptr<ShaderBase> sb = getShaderById(id);
    if (sb != nullptr) {
        return sb->getProgramName();
    }
    return TStr("");
}
void ShaderMaker::shaderBound(std::shared_ptr<ShaderBase> sb) {
    //This system is here for sanity checking of uniform bindings.
    //It's not "necessary", but it ensures uniforms are set.
    if (_pBound != nullptr && _pBound != sb) {
        _pBound->unbindAllUniforms();
    }
    if (sb == nullptr) {
        _pContext->glUseProgram(0);
    }
    else {// if(_pBound != sb ){ //*this was causing errors because we must be calling UseProgram somewhere.
        _pContext->glUseProgram(sb->getGlId());
    }
    _pBound = sb;

}


t_string ShaderMaker::systemTypeToSTring(OpenGLShaderVarType::e eType) {
    switch (eType) {
    case OpenGLShaderVarType::ut_notype: return TStr("ut_notype "); break;
    case OpenGLShaderVarType::GpuInt1: return TStr("GpuInt1   "); break;
    case OpenGLShaderVarType::GpuInt2: return TStr("GpuInt2   "); break;
    case OpenGLShaderVarType::GpuInt3: return TStr("GpuInt3   "); break;
    case OpenGLShaderVarType::GpuInt4: return TStr("GpuInt4   "); break;
    case OpenGLShaderVarType::GpuUint1: return TStr("GpuUint1  "); break;
    case OpenGLShaderVarType::GpuUint2: return TStr("GpuUint2  "); break;
    case OpenGLShaderVarType::GpuUint3: return TStr("GpuUint3  "); break;
    case OpenGLShaderVarType::GpuUint4: return TStr("GpuUint4  "); break;
    case OpenGLShaderVarType::GpuFloat1: return TStr("GpuFloat1 "); break;
    case OpenGLShaderVarType::GpuFloat2: return TStr("GpuFloat2 "); break;
    case OpenGLShaderVarType::GpuFloat3: return TStr("GpuFloat3 "); break;
    case OpenGLShaderVarType::GpuFloat4: return TStr("GpuFloat4 "); break;
    case OpenGLShaderVarType::GpuBool1: return TStr("GpuBool1  "); break;
    case OpenGLShaderVarType::GpuBool2: return TStr("GpuBool2  "); break;
    case OpenGLShaderVarType::GpuBool3: return TStr("GpuBool3  "); break;
    case OpenGLShaderVarType::GpuBool4: return TStr("GpuBool4  "); break;
    case OpenGLShaderVarType::GpuMat2: return TStr("GpuMat2   "); break;
    case OpenGLShaderVarType::GpuMat3: return TStr("GpuMat3   "); break;
    case OpenGLShaderVarType::GpuMat4: return TStr("GpuMat4   "); break;
    case OpenGLShaderVarType::GpuDouble1: return TStr("GpuDouble1"); break;
    case OpenGLShaderVarType::GpuDouble2: return TStr("GpuDouble2"); break;
    case OpenGLShaderVarType::GpuDouble3: return TStr("GpuDouble3"); break;
    case OpenGLShaderVarType::GpuDouble4: return TStr("GpuDouble4"); break;
    default: return TStr("Unknown.");
        break;
    }
}
void ShaderMaker::setUfBlock(t_string name, void* value, size_t copySizeBytes, bool bIgnore) {
    std::shared_ptr<ShaderUniformBlock> uf = getUniformBlockByName(name);
    if (uf == nullptr) {
        if (bIgnore == false) {
            BroLogWarnCycle("Uniform Block '", name, "' could not be found.");
        }
    }
    else {
        uf->copyUniformData(value, copySizeBytes);
    }
}






}//ns Game
