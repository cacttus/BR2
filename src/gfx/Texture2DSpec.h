/**
*    @file Texture2D.h
*    @date August 4, 2011
*    @author MetalMario971
*    20151126 - removed texture map and changed to texure2d - all textures inherit form ti
*        also made texture2d generic instance.
*/
#pragma once
#ifndef __TEXTURE2D_202381107519012904417182_H__
#define __TEXTURE2D_202381107519012904417182_H__

#include "../gfx/GfxHeader.h"
#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class Texture2D
*  @brief Stores information about a texture map on the GPU.
*/
class Texture2DSpec : public VirtualMemoryShared<Texture2DSpec> {
public:
  Texture2DSpec(std::shared_ptr<GLContext> ct);
  Texture2DSpec(string_t loc, std::shared_ptr<GLContext> ctx, bool bRepeatU, bool bRepeatV);
  Texture2DSpec(const std::shared_ptr<Img32>, std::shared_ptr<GLContext> ctx, TexFilter::e eFilter = TexFilter::e::Nearest);
  Texture2DSpec(std::shared_ptr<GLContext> ctx, unsigned  char* texData, int iWidth, int iHeight, bool mipmaps);
  virtual ~Texture2DSpec() override;

  string_t getLocation() { return _strLocation; }
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
  void oglSetFilter(TexFilter::e filter); // false = nearest
  TexFilter::e oglGetFilter() { return _eFilter; } // false = nearest
  void setWrapU(TexWrap::e wrap);
  void setWrapV(TexWrap::e wrap);
  bool getTextureDataFromGpu(std::shared_ptr<Img32>  __out_ image);
  void serialize(std::shared_ptr<BinaryFile> fb);
  void deserialize(std::shared_ptr<BinaryFile> fb);
  static bool getTextureDataFromGpu(std::shared_ptr<Img32>  __out_ image, GLuint iGLTexId);

protected:
  std::shared_ptr<GLContext> _pContext = nullptr;

private:
  GLuint _glId = -1;    // - Texture ID assigned by OpenGL
  GLenum _eTextureFormat = 0;
  GLenum _eTextureMipmapFormat = 0;
  uint32_t _iWidth = 0;
  uint32_t _iHeight = 0;
  float _fSizeRatio = 0.0f;
  string_t _strLocation = "";
  bool _bHasMipmaps = false;
  bool _bRepeatU = false;
  bool _bRepeatV = false;
  bool _bLoadFailed = false;
  TexFilter::e _eFilter = TexFilter::e::Linear;
  bool _bTransparent = false;

  void load(string_t loc, bool bRepeatU, bool bRepeatV);
  void load(std::shared_ptr<Img32> s);
  void flipImage(unsigned char* image, int w, int h);
  int32_t generateMipmapLevels();
  int32_t texFormatToInt(GLenum);
  GLenum intToTexFormat(int32_t);
};


}//ns game



#endif
