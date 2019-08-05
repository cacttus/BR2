#include "../base/GLContext.h"
#include "../base/Fingers.h"
#include "../base/Gu.h"
#include "../base/Img32.h"

#include "../math/Box2x.h"
#include "../math/Vec4x.h"

#include "../display/Gui2d.h"
#include "../display/TexCache.h"
#include "../display/CameraNode.h"
#include "../display/Viewport.h"
#include "../display/FrustumBase.h"
#include "../display/ShaderBase.h"
#include "../display/Picker.h"
#include "../display/ShaderBase.h"
#include "../display/ShaderMaker.h"
#include "../display/RenderUtils.h" 
#include "../display/UiSkin.h"
#include "../display/MegaTex.h"

#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/UtilMeshInline.h"
#include "../model/TextBufferMesh.h"


namespace Game {
#pragma region UiSkinElement
bool UiSkinElement::setAndValid(std::shared_ptr<UiSkinElement> ele) {
    bool b = (ele != nullptr && ele->validate());
    if (b == false) {
        Gui2d::error( Stz "Skin element " + ele->_strName + " was not valid.");
    }
    return b;
}
#pragma  endregion

#pragma region UiTex
int32_t UiTex::getWidth() { return _pTex->getWidth(); }
int32_t UiTex::getHeight() { return _pTex->getHeight(); }
float UiTex::getSizeRatio() { return _pTex->getSizeRatio(); }
void UiTex::setWrapU(TexWrap::e eWrap) { _pTex->setWrapU(eWrap); }
void UiTex::setWrapV(TexWrap::e eWrap) { _pTex->setWrapV(eWrap); }

std::shared_ptr<UiTex> UiTex::create(std::string file, bool bLoadNow) {
    std::shared_ptr<MtTexPatch> mts = Gu::getContext()->getGui()->getTex()->getTex(file, 1, false, bLoadNow);
    if(mts==nullptr) {
        return nullptr;
    }
    return UiTex::create(mts->getTexs()[0]);
}
std::shared_ptr<UiTex> UiTex::create(std::shared_ptr<Img32> generated){
    std::shared_ptr<MtTexPatch> mts = Gu::getContext()->getGui()->getTex()->getTex(generated);
    if (mts == nullptr) {
        return nullptr;
    }
    return UiTex::create(mts->getTexs()[0]);
}
std::shared_ptr<UiTex> UiTex::create(std::shared_ptr<MtTex> ptex) {
    std::shared_ptr<UiTex> tex = std::make_shared<UiTex>();
    tex->_pTex = ptex;
    return tex;
}
#pragma  endregion

#pragma region Ui3Tex
std::shared_ptr<Ui3Tex> Ui3Tex::create(std::string file) {
    std::shared_ptr<Ui3Tex> ret = std::make_shared<Ui3Tex>();
    std::shared_ptr<MtTexPatch> mts = Gu::getContext()->getGui()->getTex()->getTex(file, 3);

    for (int i = 0; i < 3; ++i) {
        ret->_images[i] = UiTex::create(mts->getTexs()[i]);
    }
    return ret;
}
#pragma endregion

#pragma region Ui9Tex
std::shared_ptr<Ui9Tex> Ui9Tex::create(std::string file) {
    std::shared_ptr<Ui9Tex> ret = std::make_shared<Ui9Tex>();
    std::shared_ptr<MtTexPatch> mts = Gu::getContext()->getGui()->getTex()->getTex(file, 9);

    for (int i = 0; i < 9; ++i) {
        ret->_images[i] = UiTex::create(mts->getTexs()[i]);
    }
    return ret;
}
#pragma endregion

#pragma region UiLabelSkin
std::shared_ptr<UiLabelSkin> UiLabelSkin::create(std::string name, uDim size) { 
    std::shared_ptr<UiLabelSkin> x = std::make_shared<UiLabelSkin>();
    x->_strFontName = name;
    x->_uFontSize = size; 
    x->_pFont = Gu::getContext()->getGui()->getTex()->getFont(name);
    return x;
}
#pragma endregion

#pragma region UiSkinFile
//UiSkinFile::UiSkinFile() {
//}
//UiSkinFile::~UiSkinFile() {
//}
//void UiSkinFile::addToMap(std::shared_ptr<UiSkinElement> ele) {
//    if (ele->_iNameHash == 0) {
//        Gui2d::error("UI Parser: Invalid skin element - hashed name was zero.");
//        return;
//    }
//    if (_mapElements.find(ele->_iNameHash) != _mapElements.end()) {
//        Gui2d::error(TStr("UI Parser: Invalid skin element '", ele->getName(), "', elemnet was already present in map."));
//        return;
//    }
//    _mapElements.insert(std::make_pair(ele->_iNameHash, ele));
//}
////std::shared_ptr<UiTex> UiSkinFile::makeTex(std::string& name, std::string& loc) {
////    std::shared_ptr<UiTex> tx = std::make_shared<UiTex>();
////    tx->_strName = name;
////    tx->_iNameHash = STRHASH(name);
////    tx->_imgLoc = loc;
////    return tx;
////}
//void UiSkinFile::pkp(std::vector<t_string>& tokens) {
//    int iind = 1;
//    if (lcmp(tokens[0], "im", 3)) {
//        std::string imgName = getCleanToken(tokens, iind);
//        std::shared_ptr<Img32> img = Gu::loadImage(imgName);
//
//        //  if (imgs.size() == 1) {
//        //      std::shared_ptr<UiTex> tx = makeTex(imgName, tokens[2]);
//        //      addToMap(tx);
//        //  }
//        //  else {
//        //      Gui2d::error(TStr("Invalid image", tokens[2]));
//        //  }
//
//    }
//    else if (lcmp(tokens[0], "i3", 3)) {
//        std::shared_ptr<UiTex> tx = makeTex(tokens[1], tokens[2]);
//        addToMap(tx);
//    }
//    else if (lcmp(tokens[0], "i9", 3)) {
//        std::shared_ptr<UiTex> tx = makeTex(tokens[1], tokens[2]);
//        addToMap(tx);
//    }
//}
//void UiSkinFile::preLoad() {
//}
//void UiSkinFile::postLoad() {
//}

#pragma endregion 


}//ns Game
