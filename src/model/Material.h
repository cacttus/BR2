/**
*  @file Material.h
*  @date 3 25 2010
*    Updates:
*        12/29/2010
*        20151126 - updated to be generic instance
*/

#pragma once
#ifndef __MATERIAL_9659280356928315689236_H__
#define __MATERIAL_9659280356928315689236_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"
#include "../math/MathAll.h"

namespace BR2 {

/**
*  @class Material
*  Material Lighting Parameters + Texture maps + Binding.
*  @sa TextureManager
*/
class TextureSlot : public VirtualMemory {
public:
  Hash32 _iTexFileHashed = 0;
  string_t _strDebugTextureFileName;
  TextureChannel::e _eChannel = TextureChannel::e::Channel0;
  TextureType::e _eTextureType = TextureType::e::Undefined;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;
  float _fInfluence = 1.0f;
  void deserialize(std::shared_ptr<BinaryFile> fb);
  void serialize(std::shared_ptr<BinaryFile> fb);
};
class Material : public VirtualMemory {
public:
  Material() {}//deser
  Material(string_t name);
  OVERRIDES ~Material() OVERRIDE;
  string_t getName() { return _strName; }

  std::map<TextureChannel::e, std::shared_ptr<TextureSlot>>& getTextureSlots() { return _mapTextureBindings; }
  std::shared_ptr<TextureSlot> getMapByType(TextureType::e texType);

  size_t getNumTextureUnits() { return _mapTextureBindings.size(); }
  bool getIsTransparent() { return _nTransparentTextureCount > 0; }

  void bind(std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound = false, bool bBindParameters = true);
  void addTextureBinding(std::shared_ptr<Texture2DSpec> ptm, TextureChannel::e eChannel, TextureType::e etype, float influence);
  void removeTextureBinding(std::shared_ptr<Texture2DSpec> ptm);

  void setSpec(const vec4&& v) { _v4Spec = v; }
  const vec4& getSpec() { return _v4Spec; }
  void setDiffuse(const vec4&& v) { _v4Diffuse = v; }
  const vec4& getDiffuse() { return _v4Diffuse; }
  const vec4& getMirror() { return _v4Mirror; }
  void setMirror(const vec4&& v) { _v4Mirror = v; }
  void setSpecHardness(const float&& v) { _fHardness = v; }
  float getSpecHardness() { return _fHardness; }
  void setEnableTransparency(bool b) { _bEnableTransparency = b; }
  bool getEnableTransparency() { return _bEnableTransparency; }
  float getTpIOR() { return _fTpIOR; }
  float getTpAlpha() { return _fTpAlpha; }
  float getTpFilter() { return _fTpFilter; }
  void setTpIOR(float f) { _fTpIOR = f; }
  void setTpAlpha(float f) { _fTpAlpha = f; }
  void setTpFilter(float f) { _fTpFilter = f; }

  std::shared_ptr<Material> makeCopy();
  void deserialize(std::shared_ptr<BinaryFile> fb);
  void serialize(std::shared_ptr<BinaryFile> fb);

  virtual void unbind();

protected:
  //Note: Make sure toupdate getCopy()
  string_t _strName;

  //**These defaults are what are in blender**
  //**we use them for for non-material meshes**
  vec4 _v4Spec = vec4(1, 1, 1, 0.5f);
  vec4 _v4Diffuse = vec4(1, 1, 1, 0.8f);
  float _fHardness = (50.0f / 511.0f); //Shininess.
  int32_t _nTransparentTextureCount = 0;
  std::map<TextureChannel::e, std::shared_ptr<TextureSlot>> _mapTextureBindings;
  vec4 _v4Mirror = vec4(0, 0, 0, 0);//mirror power is w
  bool _bEnableTransparency = false;
  float _fTpAlpha = 1.0f;
  float _fTpIOR = 1.0;//index of refraction
  float _fTpFilter = 0.5f; //Filter for diffuse color


};



}
#endif