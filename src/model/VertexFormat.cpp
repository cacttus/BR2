#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../gfx/RenderUtils.h"
#include "../model/VertexFormat.h"
#include "../model/VboData.h"

namespace Game {
GLenum VertexComponent::getAttributeType() {
  return VertexFormat::computeAttributeType(_eDataType, _iComponentCount);
}
VertexFormat::VertexFormat(std::shared_ptr<GLContext> pContext, t_string strName) :
  _pContext(pContext), _strName(strName) {
}
VertexFormat::~VertexFormat() {
  _vecComponents.clear();
  //for (size_t icomp = 0; icomp < _vecComponents.size(); icomp++) {
  //    DEL_MEM(_vecComponents[icomp]);
  //}
}
void VertexFormat::addComponent(VertexUserType::e eUserType) {

  GLenum eType;
  int compCount;
  int size;
  switch (eUserType) {
  case VertexUserType::e::v2_01:
    eType = GL_FLOAT;
    compCount = 2;
    size = sizeof(vec2);
    break;
  case VertexUserType::e::v3_01:
    //*******************************************
    //**Look at the size: vec4 - note this from the opengl wiki
    //"Implementations sometimes get the std140 layout wrong for vec3 components. 
    //You are advised to manually pad your structures/arrays out and avoid using vec3 at all."
    eType = GL_FLOAT;
    compCount = 3;
    size = sizeof(vec4);
    //*******************************************
    break;
  case VertexUserType::e::v4_01:
  case VertexUserType::e::v4_02:
  case VertexUserType::e::v4_03:
    eType = GL_FLOAT;
    compCount = 4;
    size = sizeof(vec4);
    break;
  case VertexUserType::e::c4_01:
    eType = GL_FLOAT;
    compCount = 4;
    size = sizeof(vec4);
    break;
  case VertexUserType::e::c3_01:
    eType = GL_FLOAT;
    compCount = 3;
    size = sizeof(vec4);//**Look at the size: vec4 - note this from the opengl wiki
    break;
  case VertexUserType::e::n3_01:
    eType = GL_FLOAT;
    compCount = 3;
    size = sizeof(vec4);//**Look at the size: vec4 - note this from the opengl wiki
    break;
  case VertexUserType::e::x2_01:
    eType = GL_FLOAT;
    compCount = 2;
    size = sizeof(vec4);//**Look at the size: vec4 - note this from the opengl wiki
    break;
  case VertexUserType::e::i2_01:
    eType = GL_INT;
    compCount = 2;
    size = sizeof(vec2);
    break;
  case VertexUserType::e::u2_01:
    eType = GL_UNSIGNED_INT;
    compCount = 2;
    size = sizeof(uvec2);
    break;
    //**std430 padded types.
//case VertexUserType::e::v3_01_STD430:
//    eType = GL_FLOAT;
//    compCount = 3;
//    size = sizeof(vec4);
//    break;
//case VertexUserType::e::n3_01_STD430:
//    eType = GL_FLOAT;
//    compCount = 3;
//    size = sizeof(vec4);
//    break;
//case VertexUserType::e::x2_01_STD430:
//    eType = GL_FLOAT;
//    compCount = 2;
//    size = sizeof(vec4);
//    break;
  default:
    BroThrowException("Vertex user type not impelmented.");
  }

  addComponent(eType, compCount, size, eUserType);
}
void VertexFormat::addComponent(GLenum type, int componentCount, int size, VertexUserType::e eUserType) {

  //Check duples
  for (std::pair<int, std::shared_ptr<VertexComponent>> p : _vecComponents) {
    std::shared_ptr<VertexComponent> comp = p.second;
    if (comp->getUserType() == eUserType) {
      BroLogError("Duplicate Vertex component '" + getUserTypeName(eUserType) + "' for Vertex Type '" + getName() + "'.");
      Gu::debugBreak();
    }
  }

  std::shared_ptr<VertexComponent> cmp = std::make_shared<VertexComponent>();
  cmp->_eDataType = type;
  cmp->_iComponentCount = componentCount;
  cmp->_iSizeBytes = size;
  cmp->_eUserType = eUserType;
  cmp->_iLocation = (int32_t)_vecComponents.size();

  cmp->_iByteOffset = 0;
  for (std::pair<int, std::shared_ptr<VertexComponent>> p : _vecComponents) {
    std::shared_ptr<VertexComponent> comp = p.second;
    cmp->_iByteOffset += comp->getSizeBytes();
  }

  _vecComponents.insert(std::make_pair((int)eUserType, cmp));

  //Re-calculate size of vertex
  _iVertexSizeBytes = 0;
  for (std::pair<int, std::shared_ptr<VertexComponent>> p : _vecComponents) {
    std::shared_ptr<VertexComponent> comp = p.second;
    _iVertexSizeBytes += comp->getSizeBytes();
  }
}
std::shared_ptr<VertexComponent> VertexFormat::getComponentForUserType(VertexUserType::e eUserType) {
  std::map<int, std::shared_ptr<VertexComponent>>::iterator it = _vecComponents.find((int)eUserType);
  if (it == _vecComponents.end()) {
    return nullptr;
  }
  return it->second;

}

//void VertexFormat::enableAndBindAllArraysForVaoBuffer(std::shared_ptr<VboData> pVboData) {
//    size_t iOff = 0;
//
//    //Fill the components dummy
//    AssertOrThrow2(_vecComponents.size() > 0);
//
//    RenderUtils::debugGetRenderState(false, true);
//
//    for (size_t i = 0; i<_vecComponents.size(); ++i) {
//        std::shared_ptr<VertexComponent> comp = _vecComponents[i];
//
//        _pContext->glEnableVertexAttribArray((GLuint)i);
//        _pContext->chkErrRt();
//
//        _pContext->glBindBuffer(GL_ARRAY_BUFFER, pVboData->getGlId());
//        _pContext->chkErrRt();
//
//        if (comp->getDataType() == GL_INT || comp->getDataType() == GL_UNSIGNED_INT) {
//            _pContext->glVertexAttribIPointer(
//                i,
//                comp->getComponentCount(),
//                comp->getDataType(),
//                getSizeBytes(),
//                (GLvoid*)(iOff)
//            );
//        }
//        else {
//            _pContext->glVertexAttribPointer(
//                i,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                comp->getComponentCount(),                  // size
//                comp->getDataType(),       // type
//                GL_FALSE,           // normalized?
//                getSizeBytes(),     // stride
//                (GLvoid*)(iOff)  // array buffer offset
//            );
//        }
//        _pContext->chkErrRt();
//
//
//        iOff += _vecComponents[i]->_iSizeBytes;
//    }
//    RenderUtils::debugGetRenderState(false, true);
//}
GLenum VertexFormat::computeAttributeType(GLenum type, GLuint count) {
  //We bh
  if (type == GL_FLOAT) {
    if (count == 1) {
      return GL_FLOAT;
    }
    else if (count == 2) {
      return GL_FLOAT_VEC2;
    }
    else if (count == 3) {
      return GL_FLOAT_VEC3;
    }
    else if (count == 4) {
      return GL_FLOAT_VEC4;
    }
    else {
      BroThrowNotImplementedException();
    }
  }
  else if (type == GL_INT) {
    if (count == 1) {
      return GL_INT;
    }
    else if (count == 2) {
      return GL_INT_VEC2;
    }
    else if (count == 3) {
      return GL_INT_VEC3;
    }
    else if (count == 4) {
      return GL_INT_VEC4;
    }
    else {
      BroThrowNotImplementedException();
    }
  }
  else if (type == GL_UNSIGNED_INT) {
    if (count == 1) {
      return GL_UNSIGNED_INT;
    }
    else if (count == 2) {
      return GL_UNSIGNED_INT_VEC2;
    }
    else if (count == 3) {
      return GL_UNSIGNED_INT_VEC3;
    }
    else if (count == 4) {
      return GL_UNSIGNED_INT_VEC4;
    }
    else {
      BroThrowNotImplementedException();
    }
  }
  else {
    BroThrowNotImplementedException();
  }
}
t_string VertexFormat::getUserTypeName(VertexUserType::e eUserType) {
  switch (eUserType) {
  case VertexUserType::e::c4_01: return ("Color4f"); break;
  case VertexUserType::e::v2_01: return ("Position2f"); break;
  case VertexUserType::e::v3_01: return ("Position3f"); break;
  case VertexUserType::e::n3_01: return ("Normal3f"); break;
  case VertexUserType::e::x2_01: return ("Texcoord2f"); break;
  case VertexUserType::e::u2_01: return ("Unsigned_Int_2"); break;
  case VertexUserType::e::v4_01:
  case VertexUserType::e::v4_02:
  case VertexUserType::e::v4_03:
    return ("Position4f"); break;
  };

  Gu::debugBreak();
  return ("Unknown User Type.");
}
int VertexFormat::matchTypeForShaderType(std::shared_ptr<VertexFormat> shaderType) {
  //return 0 if this type won't work for the shader.
  //return positive matching the number of components in the shader.
  //The higher the return value, the more components can be used in the shader.

  //Match to teh shader
  for (std::pair<int, std::shared_ptr<VertexComponent>> p : shaderType->getComponents()) {
    std::shared_ptr<VertexComponent> comp = p.second;
    if (getComponentForUserType(comp->getUserType()) == nullptr) {
      //if the shader has a vertex component that we lack, we can't use thsi for teh shader.
      return 0;
    }
  }

  int n = 0;
  for (std::pair<int, std::shared_ptr<VertexComponent>> p : _vecComponents) {
    std::shared_ptr<VertexComponent> comp = p.second;
    if (shaderType->getComponentForUserType(comp->getUserType()) != nullptr) {
      //if we have components the shader doesn't, just ignore them, otherwise increment this counter.
      n++;
    }
  }
  return n;
}





}//ns Game
