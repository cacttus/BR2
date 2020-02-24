#include "../base/GLContext.h"
#include "../base/OglErr.h"
#include "../base/Logger.h"

#include "../gfx/GpuQuad3.h"
#include "../gfx/GfxHeader.h"
#include "../gfx/ShaderBase.h"

#include "../model/MeshNode.h"

#include "../model/VboData.h"
#include "../model/IboData.h"
#include "../gfx/ShaderAttribute.h"

#include "../model/VaoDataGeneric.h"
#include "../model/VertexFormat.h"
#include "../gfx/RenderUtils.h"


namespace BR2 {
//static int g_debugNumconst = 0;
//static int g_debugNumcr    = 0;
//static int g_debugNumde    = 0;
VaoShader::VaoShader(std::shared_ptr<GLContext> ctx, std::shared_ptr<ShaderBase> pShader, std::shared_ptr<VertexFormat> fmt) :
  _pVertexFormat(fmt), _pShader(pShader), _pContext(ctx) {
  // g_debugNumcr++;
}
VaoShader::~VaoShader() {
  deleteVao();
  // g_debugNumde++;
}
void VaoShader::constructVao(std::shared_ptr<VboData> pVboData, std::shared_ptr<IboData> pIboData) {

  // g_debugNumconst++;
  _pContext->chkErrRt();

  deleteVao();
  _pContext->chkErrRt();

  _pVboData = pVboData;
  _pIboData = pIboData;

  _pContext->glGenVertexArrays(1, &_iGlVaoId);
  _pContext->chkErrRt();

  //You haveto add vertex components to the vertex dummy
  AssertOrThrow2(_pVertexFormat->getSizeBytes() > 0);
  AssertOrThrow2(pVboData != nullptr);
  AssertOrThrow2(pIboData != nullptr);

  _pContext->chkErrRt();

  bind();
  {
    _pVboData->bindBuffer();

    enableAttributesForShader(_pShader, _pVboData);

    _pVboData->unbindBuffer();
  }
  unbind();
  _pContext->chkErrRt();
}
void VaoShader::deleteVao() {
  if (_iGlVaoId != 0) {
    _pContext->glDeleteVertexArrays(1, &_iGlVaoId);
  }
}
void VaoShader::bind() {
  _pContext->glBindVertexArray(_iGlVaoId);
  _pIboData->bindBuffer();
}
void VaoShader::unbind() {
  _pIboData->unbindBuffer();
  _pContext->glBindVertexArray(0);
}
void VaoShader::enableAttributesForShader(std::shared_ptr<ShaderBase> pShader, std::shared_ptr<VboData> pVboData) {

  //Fill the components dummy
  AssertOrThrow2(_pVertexFormat->getComponents().size() > 0);

  t_string errStr = "";

  for (ShaderAttribute* attr : pShader->getAttributes()) {
    if (attr->isGLSLBuiltInAttrib()) {
      continue;
    }
    std::shared_ptr<VertexComponent> comp = _pVertexFormat->getComponentForUserType(attr->getUserType());

    if (comp == nullptr) {
      errStr += Stz "  Error: Could not match vertex attribute '" + attr->getName() + "'\r\n";
      errStr += Stz "     UserType:'" + VertexFormat::getUserTypeName(attr->getUserType()) + "'\r\n";
      errStr += Stz "     GL Type:'" + RenderUtils::openGlTypeToString(attr->getGlAttributeType()) + "'\r\n";
      errStr += Stz "     GL Location:" + attr->getGlLocation() + ".\r\n";
      errStr += Stz "\r\n";
    }
    else {
      _pContext->glEnableVertexAttribArray(attr->getGlLocation());
      _pContext->chkErrDbg();

      if (comp->getDataType() == GL_INT || comp->getDataType() == GL_UNSIGNED_INT) {
        _pContext->glVertexAttribIPointer(
          attr->getGlLocation(),
          comp->getComponentCount(),
          comp->getDataType(),
          _pVertexFormat->getSizeBytes(),
          (GLvoid*)(0 + comp->getByteOffset())
        );
      }
      else if (comp->getDataType() == GL_FLOAT) {
        _pContext->glVertexAttribPointer(
          attr->getGlLocation(),                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
          comp->getComponentCount(),                  // size
          comp->getDataType(),       // type
          GL_FALSE,           // normalized?
          _pVertexFormat->getSizeBytes(),     // stride
          (GLvoid*)(0 + comp->getByteOffset())  // array buffer offset
        );
      }
      else {
        BRThrowNotImplementedException();
      }
      _pContext->chkErrDbg();
    }
  }

  if (StringUtil::isNotEmpty(errStr)) {
    errStr = Stz "\r\n  For Shader: " + pShader->getProgramName() + "\r\n" + errStr + "\r\n";
    errStr = Stz errStr + "  when binding to Vertex Type '" + _pVertexFormat->getName() + "'\r\n\r\n";

    errStr = Stz "\r\n===========================================================\r\n" + errStr;
    errStr = Stz errStr + "\r\n  Check if the shader optimized out the attribute (if it's used).\r\n";
    errStr = Stz errStr + "\r\n  Check the callstack below for more info.\r\n";
    errStr = Stz errStr + "\r\n===========================================================\r\n";

    BRLogError(errStr);
    Gu::debugBreak();
  }


  RenderUtils::debugGetRenderState(false, true);
}
///////////////////////////////////////////////////////////////////

VaoDataGeneric::VaoDataGeneric(std::shared_ptr<GLContext> ctx, std::shared_ptr<VertexFormat> fmt) : _pContext(ctx), _pVertexInfo(fmt) {
  //You haveto add vertex components to the vertex dummy
  AssertOrThrow2(_pVertexInfo->getSizeBytes() > 0);

  _pContext->chkErrRt();
  _pVboData = std::make_shared<VboData>(_pContext, _pVertexInfo->getSizeBytes());
  _pIboData = std::make_shared<IboData>(_pContext, sizeof(v_index32));
}
VaoDataGeneric::~VaoDataGeneric() {
  //DEL_MEM(_pVboData);
  //DEL_MEM(_pIboData);
   //for (std::pair<std::shared_ptr<ShaderBase>, std::shared_ptr<VaoShader>> p : _mapVaoShaders) {
   //    std::shared_ptr<VaoShader> ps = p.second;
   //   // DEL_MEM(ps);
   //}
  _mapVaoShaders.clear();
}
///////////////////////////////////////////////////////////////////
void VaoDataGeneric::allocate(size_t vcount, size_t icount) {
  if (!_pVboData->isAllocated()) {
    _pVboData->allocate(vcount);
  }

  if (!_pIboData->isAllocated()) {
    _pIboData->allocate(icount);
  }
}
void VaoDataGeneric::fillData(const void* verts, size_t vcount, const void* inds, size_t icount) {
  allocate(vcount, icount);
  if (verts != nullptr) {
    _pVboData->copyDataClientServer(vcount, verts, _pVertexInfo->getSizeBytes());
  }
  if (inds != nullptr) {
    _pIboData->copyDataClientServer(icount, inds, sizeof(v_index32));
  }
}

bool VaoDataGeneric::hasDrawableData() {
  if (_pIboData == nullptr) {
    return false;
  }
  return _pIboData->getByteSize() > 0;
}

std::shared_ptr<VaoShader> VaoDataGeneric::getOrCreateVaoForShader(std::shared_ptr<ShaderBase> pShader) {
  std::map<std::shared_ptr<ShaderBase>, std::shared_ptr<VaoShader>>::iterator ite;
  ite = _mapVaoShaders.find(pShader);
  if (ite != _mapVaoShaders.end()) {
    return ite->second;
  }

  std::shared_ptr<VaoShader> vao = std::make_shared<VaoShader>(_pContext, pShader, _pVertexInfo);
  vao->constructVao(_pVboData, _pIboData);
  _mapVaoShaders.insert(std::make_pair(pShader, vao));

  return vao;
}



}//ns Game
