#include "../base/Logger.h"
#include "../base/Perf.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/Hash.h"
#include "../base/EngineConfig.h"
#include "../base/Allocator.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderSubProgram.h"
#include "../gfx/Atlas.h"
#include "../gfx/ShaderUniform.h"
#include "../gfx/ShaderAttribute.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/LightManager.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/Picker.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/GpuComputeSync.h"
#include "../gfx/ShadowBoxSide.h"
#include "../gfx/ShadowFrustum.h"
#include "../gfx/GraphicsContext.h"
#include "../model/VaoDataGeneric.h"
#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/VboData.h"
#include "../model/IboData.h"
#include "../world/PhysicsWorld.h"
#include "../world/RenderBucket.h"

namespace BR2 {
ShaderBase::ShaderBase(string_t strName) {
  setProgramName(strName);
}
ShaderBase::~ShaderBase() {
  deleteUniforms();
  deleteAttributes();
  Gu::getGraphicsContext();
  Gu::getGraphicsContext()->glDeleteProgram(_glId);
}
void ShaderBase::init() {
  _glId = std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glCreateProgram();
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
std::shared_ptr<ShaderUniform> ShaderBase::getUniformByName(string_t name) {
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
  Gu::getShaderMaker()->shaderBound(shared_from_this());
}
void ShaderBase::unbind() {
  //This system is used to make sure we don't bind multiple
  //shaders in between invocations - thus disturbing the
  //bound uniform state
  Gu::getShaderMaker()->shaderBound(nullptr);
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
  if (std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glIsProgram(_glId)) {
    std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glDeleteProgram(_glId);
  }
  _glId = std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glCreateProgram();

  _eProgramStatus = ShaderStatus::e::CreateComplete;

  _vecLinkErrors.clear();
  _vecSubPrograms.clear();
}

void ShaderBase::getProgramErrorLog(std::vector<string_t>& __out_ errs) {
  //GLShaderProgram* psp = (GLShaderProgram*)sp;


  // - Do your stuff
  GLsizei buf_size;
  std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glGetProgramiv(getGlId(), GL_INFO_LOG_LENGTH, &buf_size);

  char* log_out = (char*)GameMemoryManager::allocBlock(buf_size);
  GLsizei length_out;
  std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glGetProgramInfoLog(getGlId(), buf_size, &length_out, log_out);

  errs.clear();
  string_t tempStr;
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

  setUf("_ufView", (void*)&mView, 1, true);
  setUf("_ufProj", (void*)&mProj, 1, true);
  setUf("_ufModel", (void*)&mModel, 1, true);
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
void ShaderBase::setUf(string_t name, void* value, GLint count, bool bIgnore) {
  AssertOrThrow2(value != nullptr);
  std::shared_ptr<ShaderUniform> uf = getUniformByName(name);
  if (uf == nullptr) {
    if (bIgnore == false) {
      BRLogWarnCycle("Shader uniform '" + name + "' could not be found for shader " + getProgramName() + "");
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
      string_t na, nb;
      na = Gu::getShaderMaker()->getShaderNameForId(prog);
      nb = Gu::getShaderMaker()->getShaderNameForId(getGlId());

      BRLogError("Invalid shader was bound. '" + na + "' was bound, but we expected '" + nb + "'.");
    }
  }

  for (std::pair<Hash32, std::shared_ptr<ShaderUniform>> uf : _vecUniforms) {
    if (uf.second->hasBeenSet() == false) {
      if (StringUtil::equals(uf.second->getName(), "_ufPickId")) {
        uint32_t iNullPickId = Picker::c_iInvalidPickId;
        setUf("_ufPickId", (void*)&iNullPickId);
      }
      //else if (StringUtil::equals(uf.second->getName(), "_ufTexture0")) {
      //    TexCache::getDummy1x1NormalTexture2D()
      //    uint32_t iNullPickId = Picker::c_iInvalidPickId;
      //    setUf("_ufTexture0", (void*)&iNullPickId);
      //}
      else {
        BRLogWarnCycle("Uniform " + uf.second->getName() + " was not set for shader " + getProgramName());
      }
    }
  }
  for (std::pair<Hash32, std::shared_ptr<ShaderUniformBlock>> uf : _vecUniformBlocks) {
    if (uf.second->hasBeenSet() == false) {
      BRLogWarnCycle("Uniform Block " + uf.second->getName() + " was not set for shader " + getProgramName());
    }
  }
}
void ShaderBase::setProgramName(string_t name) {
  _strProgramName = name;
  _iNameHashed = STRHASH(name);
}
void ShaderBase::setTextureUf(uint32_t iChannel, bool bIgnoreIfNotFound) {
  //Uniform should be "_ufTexturen"
  string_t ufName = Stz "_ufTexture" + iChannel;

  //TODDO: add some error checking ehre to make sure we're not trying
 //to bind too many textures more than GL_MAX_TEXTURE_UNITS

  setUf(ufName, &iChannel, -1, bIgnoreIfNotFound);
}

void ShaderBase::draw(std::shared_ptr<MeshNode> mesh, int32_t iCount, GLenum eDrawMode) {
  Perf::pushPerf();

  draw(mesh->getMeshSpec()->getVaoData(), iCount, eDrawMode);
  Perf::popPerf();
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
  if (Gu::getGraphicsContext()->getLoopState() != EngineLoopState::e::Render) {
    BRLogWarn("Called a draw() function when the engine wan't in a valid render loop.");
  }
  AssertOrThrow2(vao != nullptr);
  Gu::getGraphicsContext()->chkErrDbg();

  RenderUtils::debugGetRenderState();
  bind();
  {
    Gu::getGraphicsContext()->chkErrDbg();
    bindAllUniforms();

    Gu::getGraphicsContext()->chkErrDbg();
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
        Gu::getGraphicsContext()->chkErrDbg();
      }
    }
    vao->unbind();
  }
  // unbind();
   //Do not unbind so we keep the uniforms.
}
string_t ShaderBase::debugGetUniformValues() {
  string_t str = "\r\n";

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
void ShaderBase::setLightUf(std::shared_ptr<LightManager> pLightManager) {
  void* lightPtr;
  size_t lightCopySizeBytes;
  lightPtr = pLightManager->getGpuPointLightBuffer();
  if (lightPtr == nullptr) {
    lightPtr = (void*)1;//No lights - not an error, Prevent the render system from thinking this wasn't set.
  }
  lightCopySizeBytes = pLightManager->getDeferredParams()->_iPointLightCount * sizeof(GpuPointLight);
  Gu::getShaderMaker()->setUfBlock("UfPointLights", lightPtr, lightCopySizeBytes, false);

  lightPtr = pLightManager->getGpuDirLightBuffer();
  if (lightPtr == nullptr) {
    lightPtr = (void*)1;//No lights - not an error, Prevent the render system from thinking this wasn't set.
  }
  lightCopySizeBytes = pLightManager->getDeferredParams()->_iDirLightCount * sizeof(GpuDirLight);
  Gu::getShaderMaker()->setUfBlock("UfDirLights", lightPtr, lightCopySizeBytes, false);

  void* dpPtr = (void*)pLightManager->getDeferredParams().get();
  size_t dpSizeBytes = sizeof(GpuDeferredParams);
  Gu::getShaderMaker()->setUfBlock("UfDeferredParams", dpPtr, dpSizeBytes, false);
}

void ShaderBase::beginRaster(int iOrthoWidth, int iOrthoHeight) {
  Gu::getGraphicsContext()->pushDepthTest();
  Gu::getGraphicsContext()->pushCullFace();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  mat4 ident = mat4::identity();

  //Do not use w-1 or h-1 or else you'll get that weird "wiggly line" across the screen
  mat4 _mOrthoProj = mat4::getOrtho(0, (float)iOrthoWidth, 0, (float)iOrthoHeight, -1.0f, 1.0f);
  bind();
  setUf("_ufProj", &_mOrthoProj);
  // setUf("_ufView", &ident);
  Gu::getGraphicsContext()->chkErrDbg();
}
void ShaderBase::endRaster() {
  Gu::getGraphicsContext()->popCullFace();
  Gu::getGraphicsContext()->popDepthTest();
  Gu::getGraphicsContext()->chkErrDbg();
}


void ShaderBase::dispatchCompute() {
  dispatchCompute(
    Gu::getShaderMaker()->getMaxWorkGroupDims()[0],
    Gu::getShaderMaker()->getMaxWorkGroupDims()[1],
    Gu::getShaderMaker()->getMaxWorkGroupDims()[2]);
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
      if (x + remainder < Gu::getShaderMaker()->getMaxWorkGroupDims()[0]) {
        x += remainder;
        remainder = 0;
        break;
      }
      else {
        BRThrowException("Compute shader distribution was out of range.");
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

  if (x > Gu::getShaderMaker()->getMaxWorkGroupDims()[0]) {
    BRThrowException("[Compute] X group greater than max work group GPU can handle which is " + Gu::getShaderMaker()->getMaxWorkGroupDims()[0]);
  }
  if (y > Gu::getShaderMaker()->getMaxWorkGroupDims()[1]) {
    BRThrowException("[Compute] Y group greater than max work group GPU can handle which is " + Gu::getShaderMaker()->getMaxWorkGroupDims()[1]);
  }
  if (z > Gu::getShaderMaker()->getMaxWorkGroupDims()[2]) {
    BRThrowException("[Compute] Z group greater than max work group GPU can handle which is " + Gu::getShaderMaker()->getMaxWorkGroupDims()[2]);
  }

  if ((x == 0) || (y == 0) || (z == 0)) {
    BRThrowException("[Compute] Can't dispatch a compute with a zero dimension brosaurus. if need be use glDisbatchCompute(x,1,1)");
  }

  std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glDispatchCompute(x, y, z);
  Gu::checkErrorsDbg();

  // unbindAllSsbos();
}
void ShaderBase::bindSsbo(std::shared_ptr<GpuBufferData> pDat, const char* shaderBufferName, uint32_t shaderSsboIndex) {
  GLint blockIndex;

  // Weight Data
  pDat->bindBuffer(GL_SHADER_STORAGE_BUFFER);
  Gu::checkErrorsDbg();

  blockIndex = std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glGetProgramResourceIndex(getGlId(), GL_SHADER_STORAGE_BLOCK, shaderBufferName);

  if (blockIndex < 0) {
    BRLogError(
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

  std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glShaderStorageBlockBinding(getGlId(), blockIndex, shaderSsboIndex);
  //Gu::checkErrorsDbg();

  std::dynamic_pointer_cast<GLContext>(Gu::getGraphicsContext())->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, shaderSsboIndex, pDat->getGlId());
  //Gu::checkErrorsDbg();

  // - Cache max bound index.
  //if ((int32_t)shaderSsboIndex > Gu::getShaderMaker()->getMaxSsboBindingIndex()) {
  //    Gu::getShaderMaker()->getMaxSsboBindingIndex() = shaderSsboIndex;
  //}
}




}//ns Game
