/**
*  @file UiSkin.h
*  @date February 17, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __UISKIN_15188874023298355212_H__
#define __UISKIN_15188874023298355212_H__

#include "../gfx/UiBase.h"

namespace BR2 {

class UiSkinElement : public VirtualMemoryShared<UiSkinElement> {
public:
  Hash32 _iNameHash;
  std::string _strName;
  const std::string& getName() { return  _strName; }
  virtual bool validate() = 0;
  static bool setAndValid(std::shared_ptr<UiSkinElement> ele);
};
class UiTex : public UiSkinElement {
  std::shared_ptr<MtTex> _pTex = nullptr;
public:
  std::shared_ptr<MtTex> tex() { return _pTex; }

  int32_t getWidth();
  int32_t getHeight();
  float getSizeRatio();
  void setWrapU(TexWrap::e eWrap);
  void setWrapV(TexWrap::e eWrap);

  static std::shared_ptr<UiTex> create(std::shared_ptr<UiScreen> gui, std::shared_ptr<Img32> generated);
  static std::shared_ptr<UiTex> create(std::shared_ptr<UiScreen> gui, std::string file, bool bLoadNow = false);
  static std::shared_ptr<UiTex> create(std::shared_ptr<MtTex> mt);
  virtual bool validate() override {
    return _pTex != nullptr;
  }


};
class Ui3Tex : public UiSkinElement {
public:
  std::shared_ptr<UiTex> _images[3];
  std::shared_ptr<UiTex> get(size_t i) {
    AssertOrThrow2(i < 3);
    return _images[i];
  }
  static std::shared_ptr<Ui3Tex> create(std::shared_ptr<UiScreen> gui, std::string file);
  virtual bool validate() override {
    for (int i = 0; i < 3; ++i) {
      if (setAndValid(_images[i]) == false) {
        return false;
      }
    }
    return true;
  }
};
class Ui9Tex : public UiSkinElement {
public:
  std::shared_ptr<UiTex> _images[9];
  std::shared_ptr<UiTex> get(size_t i) {
    AssertOrThrow2(i < 9);
    return _images[i];
  }
  static std::shared_ptr<Ui9Tex> create(std::shared_ptr<UiScreen> gui, std::string file);
  virtual bool validate() override {
    for (int i = 0; i < 3; ++i) {
      if (setAndValid(_images[i]) == false) {
        return false;
      }
    }
    return true;
  }
};

#pragma region SkinComposites
class UiLabelSkin : public UiSkinElement {
  std::string _strFontName = "";
  uDim _uFontSize = "auto";
  vec3 _vFontColor = vec3(1, 1, 1);
  //No logner userd
  std::shared_ptr<MtFont> _pFont = nullptr;
public:
  vec3& getColor() { return _vFontColor; }
  std::shared_ptr<MtFont> getFont() { return _pFont; }
  uDim& getFontSize() { return _uFontSize; }

  static std::shared_ptr<UiLabelSkin> create(std::shared_ptr<UiScreen> gui, std::string name, uDim height = "100%");//height is also the text height - fits to the label
  UiLabelSkin() {}
  virtual ~UiLabelSkin() override {}

  virtual bool validate() override {
    return _strFontName != "";
  }
};
class UiScrollbarSkin : public UiSkinElement {
public:
  std::shared_ptr<Ui3Tex> _pThumbImgs = nullptr;
  std::shared_ptr<Ui3Tex> _pBackImgs = nullptr;
  uDim _uWidthOrHeightPx = "100px";
  Orientation::e _eOrientation = Orientation::e::Horizontal;
  bool _bAutoHideWhenMaxed = true;    //Auto hide srollbar when it is maximum size

  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pThumbImgs);
    bValid = bValid && setAndValid(_pBackImgs);
    bValid = bValid && (_uWidthOrHeightPx.getDimUnit() == UiDimUnit::e::Pixel);
    return bValid;
  }
};
class UiSliderSkin : public UiSkinElement {
public:
  std::shared_ptr<Ui3Tex> _pThumbImgs = nullptr;
  std::shared_ptr<Ui3Tex> _pBackImgs = nullptr;
  uDim _uWidthOrHeightPx = "100px";
  Orientation::e _eOrientation = Orientation::e::Horizontal;
  uDim _uTrackImgHeight = "100%"; //height of the "track" image.
  std::shared_ptr<UiLabelSkin> _pLabelFont = nullptr;
  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pThumbImgs);
    bValid = bValid && setAndValid(_pBackImgs);
    bValid = bValid && (_uWidthOrHeightPx.getDimUnit() == UiDimUnit::e::Pixel);
    return bValid;
  }
};
class UiFlexButtonSkin : public UiSkinElement {
public:
  std::shared_ptr<Ui9Tex> _pUp = nullptr;
  std::shared_ptr<Ui9Tex> _pHv = nullptr;
  std::shared_ptr<Ui9Tex> _pDn = nullptr;
  std::shared_ptr<UiLabelSkin> _pLabelFont = nullptr;
  std::shared_ptr<UiBorderDim> _border = nullptr;

  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pUp);
    bValid = bValid && setAndValid(_pHv);
    bValid = bValid && setAndValid(_pDn);
    bValid = bValid && setAndValid(_pDn);
    bValid = bValid && _border != nullptr;
    return bValid;
  }
};
class UiCheckboxSkin : public UiSkinElement {
public:
  std::shared_ptr<UiLabelSkin> _pLabelFont = nullptr;
  std::shared_ptr<UiFlexButtonSkin> _pFlexButtonSkin = nullptr;
  std::shared_ptr<UiTex> _pCheckImg = nullptr;
  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pLabelFont);
    bValid = bValid && setAndValid(_pFlexButtonSkin);
    bValid = bValid && setAndValid(_pCheckImg);
    return bValid;
  }
};
class UiButtonSkin : public UiSkinElement {
public:
  std::shared_ptr<UiTex> _pUp = nullptr;
  std::shared_ptr<UiTex> _pHv = nullptr;
  std::shared_ptr<UiTex> _pDn = nullptr;
  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pUp);
    bValid = bValid && setAndValid(_pHv);
    bValid = bValid && setAndValid(_pDn);
    return bValid;
  }
};
class UiWindowSkin : public UiSkinElement {
public:
  std::shared_ptr<UiBorderDim> _pBorder = nullptr;
  std::shared_ptr<Ui9Tex> _pBorderImgs = nullptr;
  std::shared_ptr<UiLabelSkin> _pTitleFont = nullptr;
  std::shared_ptr<UiScrollbarSkin> _pHScroll = nullptr;
  std::shared_ptr<UiScrollbarSkin> _pVScroll = nullptr;
  std::shared_ptr<UiButtonSkin> _pCloseButton = nullptr;
  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && (_pBorder != nullptr);
    bValid = bValid && setAndValid(_pBorderImgs);
    bValid = bValid && setAndValid(_pTitleFont);
    bValid = bValid && setAndValid(_pHScroll);
    bValid = bValid && setAndValid(_pVScroll);
    bValid = bValid && setAndValid(_pCloseButton);
    return bValid;
  }
};
class UiDropdwonSkin : public UiSkinElement {
public:
  std::shared_ptr<Ui9Tex> _pBorderImgs = nullptr;
  std::shared_ptr<UiFlexButtonSkin> _pArrowButtonSkin = nullptr;
  std::shared_ptr<UiBorderDim> _pBorder = nullptr;
  std::shared_ptr<UiLabelSkin> _pFont = nullptr;
  std::shared_ptr<UiTex> _arrow = nullptr;
  virtual bool validate() override {
    return setAndValid(_pBorderImgs)
      && setAndValid(_pArrowButtonSkin)
      && (_pBorder != nullptr)
      && setAndValid(_pFont)
      && setAndValid(_arrow)
      ;
  }
};
class UiToolbarSkin : public UiSkinElement {
public:
  std::shared_ptr<UiFlexButtonSkin> _pFlexButtonSkin = nullptr;
  std::shared_ptr<UiWindowSkin> _pWindowSkin = nullptr;

  virtual bool validate() override {
    return setAndValid(_pFlexButtonSkin) && setAndValid(_pWindowSkin);
  }
};
class UiCursorSkin : public UiSkinElement {
public:
  std::shared_ptr<UiTex> _pTex = nullptr;
  virtual bool validate() override {
    bool bValid = true;
    bValid = bValid && setAndValid(_pTex);
    return bValid;
  }
};
class UiAssetWindowSkin : public UiSkinElement {
public:
  std::shared_ptr<UiFlexButtonSkin> _pFlexButtonSkin = nullptr;
  std::shared_ptr<UiWindowSkin> _pWindowSkin = nullptr;

  virtual bool validate() override {
    return setAndValid(_pFlexButtonSkin) && setAndValid(_pWindowSkin);
  }
};
#pragma endregion
//
//class UiSkinFile : public PoundFile {
//protected:
//    //Inherited
//    virtual void pkp(std::vector<t_string>& tokens);
//    virtual void preLoad();
//    virtual void postLoad();
//
//    std::map<Hash32, std::shared_ptr<UiSkinElement>> _mapElements;
//    std::shared_ptr<UiTex> makeTex(std::string& name, std::string& loc);
//    void addToMap(std::shared_ptr<UiSkinElement> ele);
//
//public:
//  //  static std::vector<std::shared_ptr<Img32>> parseImagePatch(std::string file);
//    UiSkinFile();
//    virtual ~UiSkinFile();
//};
//class UiCompiler : public VirtualMemoryShared<UiCompiler> {
//    //static std::vector<std::shared_ptr<Texture2DSpec>> parseImagePatch(std::string file, TexFilter::e eTexFilter = TexFilter::e::Linear);
//public:
//    UiCompiler();
//    virtual ~UiCompiler() override;
//    void loadSkin(std::string path);
//
//};

}//ns Game



#endif
