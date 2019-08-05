#include "../base/BaseAll.h"
#include "../base/Logger.h"
#include "../base/BinaryFile.h"
#include "../model/Material.h"
#include "../display/ShaderBase.h"
#include "../display/Texture2DSpec.h"
#include "../display/TexCache.h"
#include "../display/RenderSettings.h"  

namespace Game {
Material::Material(t_string name) {
    _strName = name;
    _v4Spec.construct(1, 1, 1, 1);
    _v4Diffuse.construct(1, 1, 1, 1);
   // _v4Ambient.construct(1, 1, 1, 1);
    _v4Mirror.construct(1, 1, 1, 0);//last is mirror param
    _fHardness = 50.0f; //Shininess exponent
}
Material::~Material() {
    _mapTextureBindings.clear();
}
//////////////////////////////////////////////////////////////////////////
std::shared_ptr<Material> Material::makeCopy() {
    std::shared_ptr<Material> ret = std::make_shared<Material>(_strName);
    //ret->_pContext                 = this->_pContext                ;
    //ret->_strName                  = this->_strName                 ;
    ret->_v4Spec = this->_v4Spec;
    ret->_v4Diffuse = this->_v4Diffuse;
   // ret->_v4Ambient = this->_v4Ambient;
    ret->_fHardness = this->_fHardness;
    ret->_v4Mirror = this->_v4Mirror;
    ret->_nTransparentTextureCount = this->_nTransparentTextureCount;
    ret->_mapTextureBindings = this->_mapTextureBindings;
    ret->_bEnableTransparency = this->_bEnableTransparency;
    ret->_fTpAlpha = this->_fTpAlpha;
    ret->_fTpIOR = this->_fTpIOR;
    ret->_fTpFilter = this->_fTpFilter;
    return ret;
}
std::shared_ptr<TextureSlot> Material::getMapByType(TextureType::e texType){
    for (std::pair<TextureChannel::e, std::shared_ptr<TextureSlot>> b : _mapTextureBindings) {
        if(b.second->_eTextureType == texType){
            return b.second;
        }
    }
    return nullptr;
}
void Material::bind(std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound, bool bBindParameters) {

    std::set<TextureChannel::e> channelsBound;
    for (std::pair<TextureChannel::e, std::shared_ptr<TextureSlot>> b : _mapTextureBindings) {
        TextureChannel::e channel = b.first;

        if (channelsBound.find(channel) != channelsBound.end()) {
            BroLogWarn("[Material] Multiple textures bound to channel " + channel);
        }
        else {
            //TODO: set tex influence
            if (b.second->_pTex != nullptr) {
                b.second->_pTex->bind(channel, pShader, bIgnoreIfNotFound);

                channelsBound.insert(channel);
            }
            else {
                BroLogError("Texture was null for material.");
            }
        }
    }

    if(bBindParameters) {
        //2/1/18 mirror has a color, but we're not setting it.  (later.. maybe..)
        pShader->setUf("_ufSpecIntensity", (void*)&(_v4Spec.w));  //uniform float _ufSpecIntensity;
        pShader->setUf("_ufSpecHardness", (void*)&_fHardness);  //uniform float _ufSpecHardness;
        pShader->setUf("_ufMirrorPower", (void*)&(_v4Mirror.w));  //uniform float _ufMirrorPower;
        pShader->setUf("_ufSpecColor", (void*)&_v4Spec);  //uniform vec3 _ufSpecColor;
        pShader->setUf("_ufDiffuseColor", (void*)&_v4Diffuse);  //This is now used by all shaders because of the "intensity" parameter (per blender)

        if(_bEnableTransparency && Gu::getContext()->getRenderSettings()->enableTransparency()) {
            pShader->setUf("_fTpAlpha" , (void*)&(_fTpAlpha ));
            pShader->setUf("_fTpIOR"   , (void*)&(_fTpIOR   ));
            pShader->setUf("_fTpFilter", (void*)&(_fTpFilter));
        }

    }

    std::shared_ptr<ShaderUniform> u = pShader->getUniformByName("_ufTexture0");
    if (u != nullptr) {
        std::shared_ptr<TextureSlot> ts = getMapByType(TextureType::e::Color);
        if (ts == nullptr) {
            Gu::getContext()->glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Gu::getContext()->getTexCache()->getDummy1x1Texture2D());
            //Material has no normal map, set it to the default texture.
            pShader->setTextureUf(0);
        }
        else {
            int nnnn = 0;
            nnnn++;
        }
    }

    //Set the default bump map if we don't have one.
    std::shared_ptr<ShaderUniform> u2 = pShader->getUniformByName("_ufTexture1");
    if (u2 != nullptr) {
        std::shared_ptr<TextureSlot> ts = getMapByType(TextureType::e::Normal);
        if (ts == nullptr) {
            Gu::getContext()->glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, Gu::getContext()->getTexCache()->getDummy1x1NormalTexture2D());
            //Material has no normal map, set it to the default texture.
            pShader->setTextureUf(1);
        }
        else {
            int nnnn = 0;
            nnnn++;
        }
    }


}
void Material::addTextureBinding(std::shared_ptr<Texture2DSpec> ptm, TextureChannel::e channel, TextureType::e etype, float influence) {
    AssertOrThrow2(ptm != NULL);

    if (_mapTextureBindings.find(channel) != _mapTextureBindings.end()) {
        BroLogWarn("Texture channel " + (int)channel + " already used in material " + getName());
    }

    if (ptm->getIsTransparent()) {
        _nTransparentTextureCount++;
    }
    std::shared_ptr<TextureSlot> ts = std::make_shared<TextureSlot>();
    ts->_eChannel = channel;
    ts->_pTex = ptm;
    ts->_fInfluence = influence;
    ts->_eTextureType = etype;

    _mapTextureBindings.insert(std::make_pair(channel, ts));
}
void Material::removeTextureBinding(std::shared_ptr<Texture2DSpec> ptm) {
    std::map<TextureChannel::e, std::shared_ptr<TextureSlot>>::iterator ite = _mapTextureBindings.begin();

    for (; ite != _mapTextureBindings.end(); ite++) {

        if (ite->second->_pTex == ptm) {
            if (ite->second->_pTex->getIsTransparent()) {
                _nTransparentTextureCount--;
            }

            _mapTextureBindings.erase(ite);
            break;
        }
    }

    _mapTextureBindings.erase(ite);
}
void Material::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Material::deserialize( std::shared_ptr<BinaryFile> fb) {
    fb->readString(_strName);
    int32_t nTextures;
    fb->readInt32(nTextures);
    for (int32_t iTexture = 0; iTexture < nTextures; ++iTexture) {
        std::shared_ptr<TextureSlot> ts = std::make_shared<TextureSlot>();
        ts->deserialize(fb);
        if(_mapTextureBindings.find(ts->_eChannel) != _mapTextureBindings.end()){
            BroLogWarn("Duplicate Texture binding for material." + (int)ts->_eChannel);
            Gu::debugBreak();
        }
        _mapTextureBindings.insert(std::make_pair(ts->_eChannel, ts));
    }
    fb->readVec4(_v4Diffuse);
    fb->readVec4(_v4Spec);
    fb->readVec4(_v4Mirror);
    fb->readFloat(_fHardness);
    
    fb->readBool(_bEnableTransparency);
    fb->readFloat(_fTpAlpha);
    fb->readFloat(_fTpFilter);
    fb->readFloat(_fTpIOR);
}
void Material::serialize( std::shared_ptr<BinaryFile> fb) {
    fb->writeString(std::move(_strName));
    fb->writeInt32((int32_t)_mapTextureBindings.size());
    for (std::pair<TextureChannel::e, std::shared_ptr<TextureSlot>> p : _mapTextureBindings) {
        p.second->serialize(fb);
    }
    fb->writeVec4(std::move(_v4Diffuse));
    fb->writeVec4(std::move(_v4Spec));
    fb->writeVec4(std::move(_v4Mirror));
    fb->writeFloat(std::move(_fHardness));
    
    fb->writeBool(std::move(_bEnableTransparency));
    fb->writeFloat(std::move(_fTpAlpha));
    fb->writeFloat(std::move(_fTpFilter));
    fb->writeFloat(std::move(_fTpIOR));

}
void TextureSlot::deserialize( std::shared_ptr<BinaryFile> fb) {
    fb->readUint32((uint32_t)_iTexFileHashed);
    fb->readFloat(_fInfluence);
    int32_t n;
    fb->readInt32(n);
    _eTextureType = (TextureType::e)n;
    fb->readInt32(n);
    _eChannel= (TextureChannel::e)n;
}
void TextureSlot::serialize( std::shared_ptr<BinaryFile> fb) {
    if (_pTex != nullptr) {
        t_string fn = FileSystem::getFileNameFromPath(_pTex->getLocation());
        if (StringUtil::isNotEmpty(_pTex->getLocation())) {
            Hash32 h = STRHASH(fn);
            _strDebugTextureFileName = fn;
            _iTexFileHashed = h;

            fb->writeUint32(std::move(h));
        }
    }
    fb->writeFloat(std::move(_fInfluence));
    fb->writeInt32(std::move((int32_t)_eTextureType));
    fb->writeInt32(std::move((int32_t)_eChannel));
}




}//ns game