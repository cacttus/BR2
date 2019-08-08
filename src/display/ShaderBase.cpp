#include "../base/BaseAll.h"
#include "../display/ShaderBase.h"
#include "../display/CameraNode.h"
#include "../display/ShaderSubProgram.h"
#include "../display/Atlas.h"
#include "../display/ShaderUniform.h"
#include "../display/ShaderAttribute.h"
#include "../display/ShaderMaker.h"
#include "../display/RenderUtils.h"
#include "../display/LightManager.h"
#include "../display/LightNode.h"
#include "../display/Viewport.h"
#include "../display/Picker.h"
#include "../display/FrustumBase.h"
#include "../display/GpuComputeSync.h"
#include "../display/ShadowBoxSide.h"
#include "../display/ShadowFrustum.h"
#include "../model/VaoDataGeneric.h"
#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/VboData.h"
#include "../model/IboData.h"
#include "../world/PhysicsWorld.h"
#include "../world/RenderBucket.h"



namespace Game {
///////////////////////////////////////////////////////////////////
ShaderBase::ShaderBase(t_string strName) {
    setProgramName(strName);
}
ShaderBase::~ShaderBase() {
    deleteUniforms();
    deleteAttributes();
    Gu::getContext()->glDeleteProgram(_glId);
}
///////////////////////////////////////////////////////////////////
void ShaderBase::init() {
    _glId = Gu::getContext()->glCreateProgram();
}
bool ShaderBase::confirmInit() {
    return ShaderMaker::isGoodStatus(_eProgramStatus);
}
void ShaderBase::deleteUniforms() {
    //for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> p : _vecUniforms) {
    //    std::shared_ptr<ShaderUniform> ui = p.second;
    //    delete ui;
    //}
    _vecUniforms.clear();
}
void ShaderBase::deleteAttributes() {
    for (ShaderAttribute* ui : _setAttributes) {
        delete ui;
    }
    _setAttributes.clear();
}
std::shared_ptr<ShaderUniform> ShaderBase::getUniformByName(t_string name) {
    Hash32 h = STRHASH(name);
    std::map<Hash32, std::shared_ptr<ShaderUniform>>::iterator ite = _vecUniforms.find(h);
    if (ite != _vecUniforms.end()) {
        return ite->second;
    }

    return nullptr;
}
void ShaderBase::bind() {
    //This system is used to make sure we don't bind multiple
    //shaders in between invocations - thus disturbing the
    //bound uniform state
    Gu::getContext()->getShaderMaker()->shaderBound(shared_from_this());
}
void ShaderBase::unbind() {
    //This system is used to make sure we don't bind multiple
    //shaders in between invocations - thus disturbing the
    //bound uniform state
    Gu::getContext()->getShaderMaker()->shaderBound(nullptr);
}
void ShaderBase::unbindAllUniforms() {
    //Ensures our uniforms are properly bound for each shader invocation
    for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> uf : _vecUniforms) {
        uf.second->unbind();
    }
}
void ShaderBase::bindAllUniforms() {
    //*Note: this can easily be optimized.
    //we copy these PER shader invocation however stuff like camera
    //projection etc may not be changed between invocations so
    //we could just say "if already copied, then exit"
    for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> uf : _vecUniforms) {
        uf.second->bindUniformFast();
    }
    for (std::pair<Hash32, std::shared_ptr<ShaderUniformBlock>> uf : _vecUniformBlocks) {
        uf.second->bindUniformFast();
    }

}
void ShaderBase::recreateProgram() {
    //20160608 to do this we must rebind all VAO datas to the correctg programs
    //20160505 apparently this is needed by the shader cache.
    if (Gu::getContext()->glIsProgram(_glId)) {
        Gu::getContext()->glDeleteProgram(_glId);
    }
    _glId = Gu::getContext()->glCreateProgram();

    _eProgramStatus = ShaderStatus::e::CreateComplete;

    _vecLinkErrors.clear();
    _vecSubPrograms.clear();
}

void ShaderBase::getProgramErrorLog(std::vector<t_string>& __out_ errs) {
    //GLShaderProgram* psp = (GLShaderProgram*)sp;


    // - Do your stuff
    GLsizei buf_size;
    Gu::getContext()->glGetProgramiv(getGlId(), GL_INFO_LOG_LENGTH, &buf_size);

    char* log_out = (char*)GameMemoryManager::allocBlock(buf_size);
    GLsizei length_out;
    Gu::getContext()->glGetProgramInfoLog(getGlId(), buf_size, &length_out, log_out);

    errs.clear();
    t_string tempStr;
    char* c = log_out;

    for (int i = 0; i < length_out; ++i) {
        while ((*c) && (*c) != '\n' && (i < length_out)) {
            tempStr += (*c);
            c++;
            i++;
        }
        errs.push_back(tempStr);
        tempStr.clear();
        c++;
    }

    // - Here we do some custom error stuff.
    //if( psp->getStatus()==)
    //{
    //    if( psp->prog_status.findShaderFailed )
    //        ret.push_back( "One or more shaders were not found in the system." );
    //    if( psp->prog_status.vecFailedPrograms.size() )
    //    {
    //        for( size_t i=0; i<psp->prog_status.vecFailedPrograms.size(); ++i )
    //            ret.push_back( "Shader ID " + intToStr((t_int)psp->prog_status.vecFailedPrograms[i].id) + " failed to compile." );
    //    }
    //}

    GameMemoryManager::freeBlock(log_out);
}
void ShaderBase::setCameraUf(std::shared_ptr<CameraNode> cam, mat4* model) {
    AssertOrThrow2(cam != nullptr);
    mat4 mModel;
    //   mat4 mNormal;
    mat4 mView;
    mat4 mProj;
    if (model != nullptr) {
        mModel = *model;
        //1/21/18  Instead of a normal matrix we're just adding v + n and multiplying by the model matrix, then subbing v
   //     mNormal = mModel.inverseOf();
    }
    else {
        mModel = mat4::identity();
        //     mNormal = mat4::identity();
    }
    mView = cam->getView();
    mProj = cam->getProj();

    //mView.transpose();
    //mProj.transpose();
    //mModel.transpose();
    //mNormal.transpose();

    setUf("_ufView", (void*)& mView, 1, true);
    setUf("_ufProj", (void*)& mProj, 1, true);
    setUf("_ufModel", (void*)& mModel, 1, true);
    //   setUf("_ufNormal", (void*)&mNormal, 1, true);
}
void ShaderBase::setAtlasUf(std::shared_ptr<Atlas> pa) {
    float _fSprW = (float)pa->getSpriteSize().x / (float)pa->getWidth();
    float _fSprH = (float)pa->getSpriteSize().y / (float)pa->getHeight();
    int _iTilesW = (int)pa->getGridSize().x;
    int _iTilesH = (int)pa->getGridSize().y;

    setUf("_ufTilesW", &_iTilesW);
    setUf("_ufTilesH", &_iTilesH);
    setUf("_ufSprW", &_fSprW);
    setUf("_ufSprH", &_fSprH);
}
void ShaderBase::setUf(t_string name, void* value, GLint count, bool bIgnore) {
    AssertOrThrow2(value != nullptr);
    std::shared_ptr<ShaderUniform> uf = getUniformByName(name);
    if (uf == nullptr) {
        if (bIgnore == false) {
            BroLogWarnCycle("Shader uniform '" + name + "' could not be found for shader " + getProgramName() + "");
        }
    }
    else {
        uf->copyUniformData(value, count);
    }
}

void ShaderBase::verifyBound() {
    //This is a sanity system.

    if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking()) {
        //This should be debug only/
        GLint prog = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
        if (prog > 0 && getGlId() != prog) {
            t_string na, nb;
            na = Gu::getContext()->getShaderMaker()->getShaderNameForId(prog);
            nb = Gu::getContext()->getShaderMaker()->getShaderNameForId(getGlId());

            BroLogError("Invalid shader was bound. '" + na + "' was bound, but we expected '" + nb + "'.");
        }
    }

    for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> uf : _vecUniforms) {
        if (uf.second->hasBeenSet() == false) {
            if (StringUtil::equals(uf.second->getName(), "_ufPickId")) {
                uint32_t iNullPickId = Picker::c_iInvalidPickId;
                setUf("_ufPickId", (void*)& iNullPickId);
            }
            //else if (StringUtil::equals(uf.second->getName(), "_ufTexture0")) {
            //    TexCache::getDummy1x1NormalTexture2D()
            //    uint32_t iNullPickId = Picker::c_iInvalidPickId;
            //    setUf("_ufTexture0", (void*)&iNullPickId);
            //}
            else {
                BroLogWarnCycle("Uniform " + uf.second->getName() + " was not set for shader " + getProgramName());
            }
        }
    }
    for (std::pair<Hash32, std::shared_ptr<ShaderUniformBlock>> uf : _vecUniformBlocks) {
        if (uf.second->hasBeenSet() == false) {
            BroLogWarnCycle("Uniform Block " + uf.second->getName() + " was not set for shader " + getProgramName());
        }
    }
}
void ShaderBase::setProgramName(t_string name) {
    _strProgramName = name;
    _iNameHashed = STRHASH(name);
}
void ShaderBase::setTextureUf(uint32_t iChannel, bool bIgnoreIfNotFound) {
    //Uniform should be "_ufTexturen"
    t_string ufName = Stz "_ufTexture" + iChannel;

    //TODDO: add some error checking ehre to make sure we're not trying
   //to bind too many textures more than GL_MAX_TEXTURE_UNITS

    setUf(ufName, &iChannel, -1, bIgnoreIfNotFound);
}

void ShaderBase::draw(std::shared_ptr<MeshNode> mesh, int32_t iCount, GLenum eDrawMode) {
    Gu::pushPerf();

    draw(mesh->getMeshSpec()->getVaoData(), iCount, eDrawMode);
    Gu::popPerf();
}
void ShaderBase::draw(std::shared_ptr<VaoDataGeneric> vao, int32_t iCount, GLenum eDrawMode) {
    std::shared_ptr<VaoShader> vs = nullptr;
    RenderUtils::debugGetRenderState();
    vs = vao->getOrCreateVaoForShader(shared_from_this());
    RenderUtils::debugGetRenderState();
    draw(vs, iCount, eDrawMode);
}
void ShaderBase::draw(std::shared_ptr<VaoShader> vao, int32_t iCount, GLenum eDrawMode) {
    //Removing the loopstate
    if (Gu::getContext()->getLoopState() != EngineLoopState::e::Render) {
        BroLogWarn("Called a draw() function when the engine wan't in a valid render loop.");
    }
    AssertOrThrow2(vao != nullptr);
    Gu::getContext()->chkErrDbg();

    RenderUtils::debugGetRenderState();
    bind();
    {
        Gu::getContext()->chkErrDbg();
        bindAllUniforms();

        Gu::getContext()->chkErrDbg();
        verifyBound();

        RenderUtils::debugGetRenderState();

        vao->bind();
        {
            if (iCount == -1) {
                iCount = (int32_t)vao->getIbo()->getNumElements();
            }
            if (iCount > 0) {
                RenderUtils::debugGetRenderState();
                //GL_TRIANGLES = 0x0004
                glDrawElements(eDrawMode, iCount, GL_UNSIGNED_INT, (GLvoid*)0);
                Gu::getContext()->chkErrDbg();
            }
        }
        vao->unbind();
    }
    // unbind();
     //Do not unbind so we keep the uniforms.
}
t_string ShaderBase::debugGetUniformValues() {
    t_string str = "\r\n";

    for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> uf : _vecUniforms) {
        str += "Uniform '" + uf.second->getName() + "':\r\n";
        if (uf.second->hasBeenSet() == false) {
            str += " was not set.\r\n";
            // Gu::debugBreak();
        }
        else {
            str += " " + uf.second->debugGetUniformValueAsString() + " \r\n";

        }
    }

    return str;
}
//void ShaderBase::setFreebieDirLightUf(bool bEnable){
//    ////Set the "freebie" dir light.
//    //bool bSet = false;
//    //if(bEnable){
//    //    if (Gu::getContext()->getPhysicsWorld()) {
//    //        if (Gu::getContext()->getPhysicsWorld()->getRenderBucket()) {
//    //            if (Gu::getContext()->getPhysicsWorld()->getRenderBucket()->getDirLights().size() > 0) {
//    //                std::shared_ptr<LightNodeDir> freebie = Gu::getContext()->getPhysicsWorld()->getRenderBucket()->getDirLights().begin()->second;
//
//    //                vec3 fp = freebie->getFinalPos();
//    //                setUf("_ufShadowLightPos", (void*)&fp);
//    //                setUf("_ufShadowLightViewProj", (void*)freebie->getShadowFrustum()->getViewProjMatrixPtr());
//    //                bSet = true;
//    //            }
//    //        }
//    //    }
//    //}
//    //if (bSet == false) {
//    //    vec3 fp = vec3(FLT_MAX, FLT_MAX,FLT_MAX);
//    //    setUf("_ufShadowLightPos", (void*)&fp);
//    //    mat4 m = mat4::identity();
//    //    setUf("_ufShadowLightViewProj", (void*)&m);
//    //}
//
//}
void ShaderBase::setLightUf() {
    std::shared_ptr<LightManager> pLightManager = Gu::getContext()->getLightManager();

    void* lightPtr;
    size_t lightCopySizeBytes;
    lightPtr = pLightManager->getGpuPointLightBuffer();
    if (lightPtr == nullptr) {
        lightPtr = (void*)1;//No lights - not an error, Prevent the render system from thinking this wasn't set.
    }
    lightCopySizeBytes = pLightManager->getDeferredParams()->_iPointLightCount * sizeof(GpuPointLight);
    Gu::getContext()->getShaderMaker()->setUfBlock("UfPointLights", lightPtr, lightCopySizeBytes, false);

    lightPtr = pLightManager->getGpuDirLightBuffer();
    if (lightPtr == nullptr) {
        lightPtr = (void*)1;//No lights - not an error, Prevent the render system from thinking this wasn't set.
    }
    lightCopySizeBytes = pLightManager->getDeferredParams()->_iDirLightCount * sizeof(GpuDirLight);
    Gu::getContext()->getShaderMaker()->setUfBlock("UfDirLights", lightPtr, lightCopySizeBytes, false);

    void* dpPtr = (void*)pLightManager->getDeferredParams().get();
    size_t dpSizeBytes = sizeof(GpuDeferredParams);
    Gu::getContext()->getShaderMaker()->setUfBlock("UfDeferredParams", dpPtr, dpSizeBytes, false);

}

void ShaderBase::beginRaster() {
    Gu::pushDepthTest();
    Gu::pushCullFace();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    int iOrthoWidth = Gu::getContext()->getCamera()->getViewport()->getWidth();
    int iOrthoHeight = Gu::getContext()->getCamera()->getViewport()->getHeight();
    mat4 ident = mat4::identity();

    //Do not use w-1 or h-1 or else you'll get that weird "wiggly line" across the screen
    mat4 _mOrthoProj = mat4::getOrtho(0, (float)iOrthoWidth, 0, (float)iOrthoHeight, -1.0f, 1.0f);
    bind();
    setUf("_ufProj", &_mOrthoProj);
    // setUf("_ufView", &ident);
    Gu::getContext()->chkErrDbg();
}
void ShaderBase::endRaster() {
    Gu::popCullFace();
    Gu::popDepthTest();
    Gu::getContext()->chkErrDbg();
}


void ShaderBase::dispatchCompute() {
    dispatchCompute(
        Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[0],
        Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[1],
        Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[2]);
}
/**
*    @fn
*    @brief Basically we partition the number of elements to process.
*/
void ShaderBase::dispatchCompute(int32_t elementCount) {
    float f;
    int remainder = elementCount;
    int x = 0, y = 0, z = 0;
    int nf, npow;

    while (remainder > 0) {
        f = powf((float)remainder, 1.0 / 3.0);

        if (f < 1.0f) {
            // add the remaining to X
            if (x + remainder < Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[0]) {
                x += remainder;
                remainder = 0;
                break;
            }
            else {
                BroThrowException("Compute shader distribution was out of range.");
            }
        }
        nf = (int)f;
        npow = nf * nf * nf;
        remainder = remainder - npow;
        x += nf;
        y += nf;
        z += nf;

    }
    int tv = x * y * z;
    //assert(tv==elementCount);

    dispatchCompute(x, y, z);

}
void ShaderBase::dispatchCompute(int32_t x, int32_t y, int32_t z, GpuComputeSync* sync) {
    AssertOrThrow2(sync != NULL);

    dispatchCompute(x, y, z);
    sync->createFence();
    Gu::checkErrorsDbg();
}
void ShaderBase::dispatchCompute(int32_t x, int32_t y, int32_t z) {

    if (x > Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[0]) {
        BroThrowException("[Compute] X group greater than max work group GPU can handle which is " + Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[0]);
    }
    if (y > Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[1]) {
        BroThrowException("[Compute] Y group greater than max work group GPU can handle which is " + Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[1]);
    }
    if (z > Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[2]) {
        BroThrowException("[Compute] Z group greater than max work group GPU can handle which is " + Gu::getContext()->getShaderMaker()->getMaxWorkGroupDims()[2]);
    }

    if ((x == 0) || (y == 0) || (z == 0)) {
        BroThrowException("[Compute] Can't dispatch a compute with a zero dimension brosaurus. if need be use glDisbatchCompute(x,1,1)");
    }

    Gu::getContext()->glDispatchCompute(x, y, z);
    Gu::checkErrorsDbg();

    // unbindAllSsbos();
}
void ShaderBase::bindSsbo(std::shared_ptr<GpuBufferData> pDat, const char* shaderBufferName, uint32_t shaderSsboIndex) {
    GLint blockIndex;

    // Weight Data
    pDat->bindBuffer(GL_SHADER_STORAGE_BUFFER);
    Gu::checkErrorsDbg();

    blockIndex = Gu::getContext()->glGetProgramResourceIndex(getGlId(), GL_SHADER_STORAGE_BLOCK, shaderBufferName);

    if (blockIndex < 0) {
        BroLogError(
            "BIND FAILED: uniform buffer name: " + shaderBufferName
            + " .Binding Block Shader Id " + getGlId()
            + " shader name (may be invalid) " + getProgramName()
            + " block idx " + blockIndex
            + " ssbo idx: " + shaderSsboIndex
            + "\r\n"
        );
    }

    AssertOrThrow2(blockIndex >= 0);
    //Gu::checkErrorsDbg();

    Gu::getContext()->glShaderStorageBlockBinding(getGlId(), blockIndex, shaderSsboIndex);
    //Gu::checkErrorsDbg();

    Gu::getContext()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, shaderSsboIndex, pDat->getGlId());
    //Gu::checkErrorsDbg();

    // - Cache max bound index.
    //if ((int32_t)shaderSsboIndex > Gu::getContext()->getShaderMaker()->getMaxSsboBindingIndex()) {
    //    Gu::getContext()->getShaderMaker()->getMaxSsboBindingIndex() = shaderSsboIndex;
    //}
}




}//ns Game
