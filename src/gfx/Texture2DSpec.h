/**
*  @file Texture2D.h
*  @date August 4, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __TEXTURE2D_202381107519012904417182_H__
#define __TEXTURE2D_202381107519012904417182_H__

#include "../gfx/GfxHeader.h"
#include "../base/BaseHeader.h"

namespace Game {
/**
*  @class Texture2D
*  @brief Stores information about a texture map.
*/
class Texture2DSpec : public VirtualMemoryShared<Texture2DSpec> {
public:
  Texture2DSpec(std::shared_ptr<GLContext> ct) : _pContext(ct) {}
  Texture2DSpec(t_string loc, std::shared_ptr<GLContext> ctx, bool bRepeatU, bool bRepeatV);
  Texture2DSpec(const std::shared_ptr<Img32>, std::shared_ptr<GLContext> ctx, TexFilter::e eFilter = TexFilter::e::Nearest);
  Texture2DSpec(std::shared_ptr<GLContext> ctx, unsigned  char* texData, int iWidth, int iHeight, bool mipmaps);
  virtual ~Texture2DSpec() override;

  t_string getLocation() { return _strLocation; }
  GLuint getGlId() { return _glId; }
  bool getIsTransparent() { return _bTransparent; }
  void setIsTransparent(bool b) { _bTransparent = b; }
  void create(unsigned char* imageData, uint32_t w, uint32_t h, bool genMipmaps, bool bRepeatU, bool bRepeatV);
  void calculateTextureFormat();
  void dispose();
  void bind(TextureChannel::e eChannel, std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound = false);
  void unbind();
  uint32_t getWidth() { return _iWidth; }
  uint32_t getHeight() { return _iHeight; }
  float getSizeRatio() { return _fSizeRatio; }
  // uint32_t getChannel(){ return _iChannel; }//**TODO: REMOVE ** all meshses should use materials and texture slots** 12/8/2017

  void oglSetFilter(TexFilter::e filter); // false = nearest
  TexFilter::e oglGetFilter() { return _eFilter; } // false = nearest
  void setWrapU(TexWrap::e wrap);
  void setWrapV(TexWrap::e wrap);

  bool getTextureDataFromGpu(std::shared_ptr<Img32>  __out_ image);
  static bool getTextureDataFromGpu(std::shared_ptr<Img32>  __out_ image, GLuint iGLTexId);

  void serialize(std::shared_ptr<BinaryFile> fb);
  void deserialize(std::shared_ptr<BinaryFile> fb);

protected:
  std::shared_ptr<GLContext> _pContext = nullptr;

private:
  GLuint _glId;    // - Texture ID assigned by OpenGL
  GLenum _eTextureFormat = 0;
  GLenum _eTextureMipmapFormat = 0;
  uint32_t _iWidth = 0;
  uint32_t _iHeight = 0;
  float _fSizeRatio = 0.0f;
  t_string _strLocation;
  bool _bHasMipmaps = false;
  bool _bRepeatU = false;
  bool _bRepeatV = false;
  bool _bLoadFailed = false;

  void load(t_string loc, bool bRepeatU, bool bRepeatV);
  void load(std::shared_ptr<Img32> s);
  void flipImage(unsigned char* image, int w, int h);
  int32_t generateMipmapLevels();
  bool _bTransparent = false;

  TexFilter::e _eFilter;

  int32_t texFormatToInt(GLenum);
  GLenum intToTexFormat(int32_t);
};


}//ns game



#endif
