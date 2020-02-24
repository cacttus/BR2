/**
*  @file MegaTex.h
*  @date February 19, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __MEGATEX_151906300468150670_H__
#define __MEGATEX_151906300468150670_H__

#include "../gfx/Texture2DSpec.h"
#include "../ext/stb_truetype.h"

namespace BR2 {
namespace MtTexType { typedef enum { Image, Font } e; }
/**
*  @class MtNode
*  @brief Node in the MegaTexture class.
*/
class MtNode : public VirtualMemoryShared<MtNode> {
public:
  std::shared_ptr<MtTex> _pTex = nullptr;
  std::shared_ptr<MtNode> _pChild[2];
  Box2i _b2Rect;
  std::shared_ptr<MtNode> plop(std::shared_ptr<MtTex> tex);
};
class MtTex : public VirtualMemoryShared<MtTex> {
public:
  MtTex(std::string imgName, int32_t iPatch) { _strImgName = imgName; _iPatchImg = iPatch; }
  std::string imgName() { return _strImgName; }
  virtual ~MtTex() override {}
  int32_t getWidth() { return _iWidth; }
  int32_t getHeight() { return _iHeight; }
  float getSizeRatio() { return _fSizeRatio; }
  vec2& uv0() { return _uv_p0; }
  vec2& uv1() { return _uv_p1; }
  std::shared_ptr<MtNode> node() { return _pMtNode; }//mega texture node
  void setNode(std::shared_ptr<MtNode> n) { _pMtNode = n; }//mega texture node
  std::shared_ptr<Img32> img() { return _pImg; }
  void setImg(std::shared_ptr<Img32> img);
  void freeTmp();
  void setWrapU(TexWrap::e e) { _eWrapU = e; }
  void setWrapV(TexWrap::e e) { _eWrapV = e; }
  TexWrap::e getWrapU() { return _eWrapU; }
  TexWrap::e getWrapV() { return _eWrapV; }
private:
  std::shared_ptr<MtNode> _pMtNode = nullptr;//mega texture node
  std::shared_ptr<Img32> _pImg = nullptr;
  int32_t _iWidth = 0;
  int32_t _iHeight = 0;
  float _fSizeRatio = 0;
  TexWrap::e _eWrapU = TexWrap::e::Repeat;
  TexWrap::e _eWrapV = TexWrap::e::Repeat;
  vec2 _uv_p0, _uv_p1;
  std::string _strImgName;
  int32_t _iPatchImg = 0; //0-8 for 9p, or 0-2 for 3p


};
class MtTexPatch : public VirtualMemoryShared<MtTexPatch> {
public:
  void addTexImage(std::string img, int32_t iPatch);
  MtTexPatch(std::string imgName, Hash32 nameHash) { _strName = imgName; _iNameHash = nameHash; }
  virtual void loadData();
  std::string getName() { return _strName; }
  std::vector<std::shared_ptr<MtTex>>& getTexs() { return _vecTexs; }
private:
  std::vector<std::shared_ptr<Img32>> parseImagePatch(std::string file);
  std::string _strName; //Image Or Font name
  Hash32 _iNameHash = 0;
  std::vector<std::shared_ptr<MtTex>> _vecTexs;
};
class MtFont : public MtTexPatch {
public:
  MtFont(std::string name, Hash32 nameHash) : MtTexPatch(name, nameHash) {}
  virtual ~MtFont() override;
  void createFont();
  void getCharQuad(int32_t cCode, int32_t cCodePrev, FontSize fontSize, float& outWidth, float& outHeight, Box2f& texs, float& padTop, float& padRight, float& padBot, float& padLeft);
  virtual void loadData() override;
  float getLineGap() { return _fLineGap; }
private:
  //std::string _strFontName;
  uint32_t _iBakedCharSizePixels = 40;
  uint32_t _atlasWidth = 1024;
  uint32_t _atlasHeight = 1024;
  uint32_t _oversampleX = 2;
  uint32_t _oversampleY = 2;
  uint32_t _firstChar = ' ';
  uint32_t _charCount = '~' - ' ';
  float _fAscent;
  float _fDescent;
  float _fLineGap;
  std::unique_ptr<stbtt_packedchar[]> _charInfo;
  stbtt_fontinfo _fontInfo;
  float _fScaleForPixelHeight;//return value of stbtt_ScaleForPixelHeight\
    

  std::shared_ptr<BinaryFile> _pFontBuffer;// STB:  "Load" a font file from a memory buffer (you have to keep the buffer loaded)
  bool _bInitialized = false;
  //GLuint _texture = 0;
  float fontSizeToFontScale(float fontSz);
  void scaleStbQuad(const stbtt_aligned_quad* const stbQuad, Box2f* __out_ worldQuad, const vec2& basePos, float fScale);
  std::shared_ptr<Img32> createFontImage(std::unique_ptr<uint8_t[]>& pData);
};
/**
*    @class MegaTex
*    @brief Composes mega textures (non-uniform grid atlas)
*/
class MegaTex : public Texture2DSpec {
public:
  MegaTex(std::shared_ptr<GLContext> ct) : Texture2DSpec(ct) {}
  virtual ~MegaTex() override {}

  std::shared_ptr<MtTexPatch> getTex(std::shared_ptr<Img32> tx);
  std::shared_ptr<MtTexPatch> getTex(std::string img, int32_t nPatches = 1, bool bPreloaded = false, bool bLoadNow = false);
  std::shared_ptr<MtFont> getFont(std::string fontName);
  void loadImages();
  std::shared_ptr<Img32> compile();
private:
  //All images correspond to one or more textures due to "9-patch" and "3-patch" texs.
  std::map<Hash32, std::shared_ptr<MtTexPatch>> _mapTexs;
  //std::map<Hash32, std::shared_ptr<MtFont>> _mapFonts;

  Box2i _b2GrowRect; // Grow this to crop the image.
  int32_t _iStartWH = 256;//Starting tex size
  int32_t _iGrowWH = 128;//Grow by this much (pixels)
  int32_t _iMaxTexSize = 0;
  std::shared_ptr<Img32> _pMaster = nullptr;
  std::shared_ptr<MtNode> _pRoot = nullptr;

  bool _bImagesLoaded = false;
};

}//ns Game



#endif
