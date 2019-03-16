#include "../base/BaseAll.h"
#include "../model/Material.h"
#include "../display/RenderParams.h"
#include "../display/ShaderBase.h"
#include "../display/Texture2DSpec.h"
#include "../display/TexCache.h"
#include "../display/RenderParams.h"
#include "../display/ShaderUniform.h"


namespace Game {
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

RenderParams::RenderParams(std::shared_ptr<ShaderBase> pShader) : _pShader(pShader) {
}
RenderParams::~RenderParams() {
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void RenderParams::draw() {
    //Check some stuff
    int nd = 0;
    if (_pMeshNode != nullptr) {
        nd++;
    }
    if (_pVaoDataGeneric != nullptr) {
        nd++;
    }
    if (_pVaoShader != nullptr) {
        nd++;
    }
    if (nd != 1) {
        BroLogWarnCycle("RenderParams] Invalid number of render items set on RenderParams.");
        return;
    }

    if (_pShader == nullptr) {
        BroLogWarnCycle("[RenderParams] Couldn't render - shader was not set.");
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
    if (_pMeshNode != nullptr) {
        _pShader->draw(_pMeshNode, _iCount, _eDrawMode);
    }
    else if (_pVaoDataGeneric != nullptr) {
        _pShader->draw(_pVaoDataGeneric, _iCount, _eDrawMode);
    }
    else if (_pVaoShader != nullptr) {
        _pShader->draw(_pVaoShader, _iCount, _eDrawMode);
    }
    else {
        BroLogWarnCycle("Drawable Item was not set on RenderParams.");
    }


}




}//ns Game
