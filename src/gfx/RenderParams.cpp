#include "../base/Logger.h"
#include "../gfx/RenderParams.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/TexCache.h"
#include "../gfx/RenderParams.h"
#include "../gfx/ShaderUniform.h"
#include "../model/Material.h"


namespace BR2 {
class RenderParamsInternal : public VirtualMemory {
public:
  std::shared_ptr<ShaderBase> _pShader = nullptr;
  //  std::vector<std::shared_ptr<Texture2DSpec>> _setTextures; //Essentially we shouldjsut be using the material on MeshSpec
  int32_t _iCount = -1; //Number of indexes

  GLenum _eDrawMode = GL_TRIANGLES;

  //Only one of these must be set.
  std::shared_ptr<MeshComponent> _pMeshNode = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoDataGeneric = nullptr;
  std::shared_ptr<VaoShader> _pVaoShader = nullptr;
  std::shared_ptr<CameraNode> _pCamera = nullptr;
  // std::shared_ptr<Material> _pMaterial = nullptr;
  //std::shared_ptr<GraphicsWindow> _pWindow = nullptr;
};
RenderParams::RenderParams(std::shared_ptr<ShaderBase> pShader) {
  _internal = std::make_shared<RenderParamsInternal>();
  _internal->_pShader = pShader;
}
RenderParams::~RenderParams() {
  _internal = nullptr;
}

int32_t RenderParams::getCount() { return _internal->_iCount; }
void RenderParams::setCount(int32_t i) { _internal->_iCount = i; }
std::shared_ptr<ShaderBase> RenderParams::getShader() { return _internal->_pShader; }
void RenderParams::setShader(std::shared_ptr<ShaderBase> sb) { _internal->_pShader = sb; }
void RenderParams::setCamera(std::shared_ptr<CameraNode> c) { _internal->_pCamera = c; }
std::shared_ptr<CameraNode> RenderParams::getCamera() { return _internal->_pCamera; }
void RenderParams::setMesh(std::shared_ptr<MeshComponent> x) { _internal->_pMeshNode = x; }
void RenderParams::setVaoGeneric(std::shared_ptr<VaoDataGeneric> x) { _internal->_pVaoDataGeneric = x; }
void RenderParams::setVaoShader(std::shared_ptr<VaoShader> x) { _internal->_pVaoShader = x; }

void RenderParams::draw() {
  //Check some stuff
  int nd = 0;
  if (_internal->_pMeshNode != nullptr) {
    nd++;
  }
  if (_internal->_pVaoDataGeneric != nullptr) {
    nd++;
  }
  if (_internal->_pVaoShader != nullptr) {
    nd++;
  }
  if (nd != 1) {
    Br2LogWarnCycle("RenderParams] Invalid number of render items set on RenderParams.");
    return;
  }

  if (_internal->_pShader == nullptr) {
    Br2LogWarnCycle("[RenderParams] Couldn't render - shader was not set.");
    return;
  }

  //if (_pMaterial != nullptr && _setTextures.size() > 0) {
  //    BroLogWarnCycle("Both Material and textures set for render params! Material will render,");
  //}
  ////Bind textures
  //if (_pMaterial != nullptr) {
  //    _pMaterial->bind(_pShader);
  //}
  //else {
  //    //We should never be rendering without a material anymroe.
  //   // BroLogError("Tried to render a mesh without a material");
  //    //TODO: remove all this - remove channel property on Texture2dspec
  //    //1/15/18 tried to remove this.. were still using it for some stuff
  //    std::set<int> channelsBound;
  //    for (std::shared_ptr<Texture2DSpec> pTex : _setTextures) {
  //        if(channelsBound.find(0) != channelsBound.end()) {
  //            BroLogWarnCycle("[RenderParams] Multiple textures bound to channel 0");
  //        }
  //        else {
  //            pTex->bind();
  //            _pShader->setTextureUf(0);
  //            channelsBound.insert(0);
  //        }
  //    }
  //}

  //Draw via shader.
  if (_internal->_pMeshNode != nullptr) {
    _internal->_pShader->draw(_internal->_pMeshNode, _internal->_iCount, _internal->_eDrawMode);
  }
  else if (_internal->_pVaoDataGeneric != nullptr) {
    _internal->_pShader->draw(_internal->_pVaoDataGeneric, _internal->_iCount, _internal->_eDrawMode);
  }
  else if (_internal->_pVaoShader != nullptr) {
    _internal->_pShader->draw(_internal->_pVaoShader, _internal->_iCount, _internal->_eDrawMode);
  }
  else {
    Br2LogWarnCycle("Drawable Item was not set on RenderParams.");
  }


}




}//ns BR2
