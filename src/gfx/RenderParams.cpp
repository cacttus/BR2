#include "../base/Logger.h"
#include "../model/Material.h"
#include "../gfx/RenderParams.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/TexCache.h"
#include "../gfx/RenderParams.h"
#include "../gfx/ShaderUniform.h"


namespace BR2 {
class RenderParams_Internal {
public:
  std::shared_ptr<ShaderBase> _pShader = nullptr;
  int32_t _iCount = -1; //Number of indexes
  GLenum _eDrawMode = GL_TRIANGLES;
  //Only one of these must be set.
  std::shared_ptr<MeshNode> _pMeshNode = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoDataGeneric = nullptr;
  std::shared_ptr<VaoShader> _pVaoShader = nullptr;
  std::shared_ptr<CameraNode> _pCamera = nullptr;
};
//////////////////////////////////////////////////////////////////////////
RenderParams::RenderParams(){
  _pint = std::make_unique<RenderParams_Internal>();
}
RenderParams::RenderParams(std::shared_ptr<ShaderBase> pShader) : RenderParams() {
  _pint->_pShader = pShader;
}
RenderParams::~RenderParams() {
  _pint = nullptr;
}
std::shared_ptr<CameraNode> RenderParams::getCamera() { return _pint->_pCamera; }
int32_t RenderParams::getCount() { return _pint->_iCount; }
void RenderParams::setCount(int32_t i) { _pint->_iCount = i; }
void RenderParams::setDrawMode(GLenum e) { _pint->_eDrawMode = e; }
std::shared_ptr<ShaderBase> RenderParams::getShader() { return _pint->_pShader; }
void RenderParams::setShader(std::shared_ptr<ShaderBase> sb) { _pint->_pShader = sb; }
void RenderParams::setMesh(std::shared_ptr<MeshNode> x) { _pint->_pMeshNode = x; }
void RenderParams::setVaoGeneric(std::shared_ptr<VaoDataGeneric> x) { _pint->_pVaoDataGeneric = x; }
void RenderParams::setVaoShader(std::shared_ptr<VaoShader> x) { _pint->_pVaoShader = x; }

void RenderParams::draw() {
    //Check some stuff
    int nd = 0;
    if (_pint->_pMeshNode != nullptr) {
        nd++;
    }
    if (_pint->_pVaoDataGeneric != nullptr) {
        nd++;
    }
    if (_pint->_pVaoShader != nullptr) {
        nd++;
    }
    if (nd != 1) {
        BRLogWarnCycle("RenderParams] Invalid number of render items set on RenderParams.");
        return;
    }

    if (_pint->_pShader == nullptr) {
        BRLogWarnCycle("[RenderParams] Couldn't render - shader was not set.");
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
    if (_pint->_pMeshNode != nullptr) {
      _pint->_pShader->draw(_pint->_pMeshNode, _pint->_iCount, _pint->_eDrawMode);
    }
    else if (_pint->_pVaoDataGeneric != nullptr) {
      _pint->_pShader->draw(_pint->_pVaoDataGeneric, _pint->_iCount, _pint->_eDrawMode);
    }
    else if (_pint->_pVaoShader != nullptr) {
      _pint->_pShader->draw(_pint->_pVaoShader, _pint->_iCount, _pint->_eDrawMode);
    }
    else {
        BRLogWarnCycle("Drawable Item was not set on RenderParams.");
    }


}




}//ns Game
