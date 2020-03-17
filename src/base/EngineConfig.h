/**
*  @file EngineConfig.h
*  @date May 8, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __ENGINECONFIG_14942867052644616215_H__
#define __ENGINECONFIG_14942867052644616215_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../base/XmlFile.h"

namespace BR2 {
class BaseProp : public VirtualMemoryShared<BaseProp> {
public:
  string_t key() { return _key; }
protected:
  string_t _key = "";
};
class BoolProp : BaseProp {
public:
  BoolProp(string_t key, bool value) { _key = key;  _value = value; }
  bool value() { return _value; }
private:
  bool _value;
};
class Int32Prop : BaseProp {
public:
  Int32Prop(string_t key, int32_t value) { _key = key;  _value = value; }
  int32_t value() { return _value; }
private:
  int32_t _value;
};
class StringProp : BaseProp {
public:
  StringProp(string_t key, string_t value) { _key = key; _value = value; }
  const string_t& value() { return _value; }
private:
  string_t _value;
};

/**
*  @class EngineConfig
*  @brief Stores configuration options for the graphics engine.
* TODO: this class is getting too big.  Let's create a generic property system for it.

#define XMLCONFIG_PROP(x) const string_t ##x = #x

class EngineConfig  ..  {

  HashMap<Prop> _props;

  //Define all props here.
  XMLCONFIG_PROP(OpenGLProfile);

  std::shared_ptr<Prop> getProp(string_t prop)
  {
    std::shared_ptr<Prop> prop = _props.find(prop);
    if(prop==nullptr){
      BRLogWarn("Could not find property '"+ prop +"' in '" + getFileName + "'");
    }
    return prop;
  }
};
*/

#define XMLCONFIG_PROP(x) const string_t ##(x) = #x
class EngineConfig_Internal;
class EngineConfig : public XmlFile {
public:
  EngineConfig();
  virtual ~EngineConfig();
  XMLCONFIG_PROP(EnableMSAA);
  XMLCONFIG_PROP(MSAASamples);
  XMLCONFIG_PROP(EnableLogToConsole);
  XMLCONFIG_PROP(EnableLogToFile);
  XMLCONFIG_PROP(StartFullscreen);
  XMLCONFIG_PROP(DefaultScreenWidth);
  XMLCONFIG_PROP(DefaultScreenHeight);
  XMLCONFIG_PROP(ForceAspectRatio);   // <<<< REMOVE THIS
  XMLCONFIG_PROP(GpuDeviceName);
  XMLCONFIG_PROP(DefaultFieldOfView);
  XMLCONFIG_PROP(MaxPickDistance);
  XMLCONFIG_PROP(ColorSpace);
  XMLCONFIG_PROP(NumTextQuads);
  XMLCONFIG_PROP(EnableRuntimeErrorChecking);
  XMLCONFIG_PROP(EnableDebugErrorChecking);
  XMLCONFIG_PROP(BakedCharSize);
  XMLCONFIG_PROP(FontBitmapSize);
  XMLCONFIG_PROP(MaxPointLights);
  XMLCONFIG_PROP(MaxDirLights);
  XMLCONFIG_PROP(SaveFrameTexture);
  XMLCONFIG_PROP(SaveFrameTextureMod);
  XMLCONFIG_PROP(BreakOnSDLError);
  XMLCONFIG_PROP(BreakOnOpenGLError);
  XMLCONFIG_PROP(ShowConsole);
  XMLCONFIG_PROP(GameHostAttached); // << Remove this
  XMLCONFIG_PROP(GameHostTimeoutMs);
  XMLCONFIG_PROP(GameHostPort);
  XMLCONFIG_PROP(EnableObjectShadows);
  XMLCONFIG_PROP(EnableTerrainShadows);
  XMLCONFIG_PROP(ShadowMapResolution);
  XMLCONFIG_PROP(ShadowMapMaxInfluences);
  XMLCONFIG_PROP(MaxPointLightShadowDistance);
  XMLCONFIG_PROP(MaxFrustShadowSamples);
  XMLCONFIG_PROP(MaxCubeShadowSamples);
  XMLCONFIG_PROP(FullscreenWidth);
  XMLCONFIG_PROP(FullscreenHeight);
  XMLCONFIG_PROP(ModelThumbSize);
  XMLCONFIG_PROP(MaxHardwareIncomingBufferSizeBytes);
  XMLCONFIG_PROP(MaxHardwareOutgoingBufferSizeBytes);
  XMLCONFIG_PROP(MaxSoftwareIncomingBufferSizeBytes);
  XMLCONFIG_PROP(MaxSoftwareOutgoingBufferSizeBytes);
  //XMLCONFIG_PROP(RenderSystem);
  //XMLCONFIG_PROP(WindowedAspectRatio);
  //XMLCONFIG_PROP(GamePadType);
  //XMLCONFIG_PROP(OpenGLProfile);

  void int32Prop(string_t a, std::vector<string_t>& toks, int32_t& iind);
  void boolProp(string_t a, std::vector<string_t>& toks, int32_t& iind);
  void floatProp(string_t a, std::vector<string_t>& toks, int32_t& iind);


  bool getEnableMsaa() { return _bEnableMsaa; }
  void setEnableMsaa(bool b) { _bEnableMsaa = b; }
  int32_t getMsaaSamples() { return _iMsaaSamples; }
  bool getEnableLogToConsole() { return _bEnableLogToConsole; }
  bool getEnableLogToFile() { return _bEnableLogToFile; }
  bool getStartFullscreen() { return _bStartFullscreen; }
  int getDefaultScreenWidth() { return _iDefaultScreenWidth; }
  int getDefaultScreenHeight() { return _iDefaultScreenHeight; }
  bool getForceAspectRatio() { return _bForceAspectRatio; }
  string_t getGpuDeviceName() { return _strGpuDeviceName; }
  float getDefaultFieldOfView() { return _fDefaultFieldOfView; }
  float getMaxPickDistance() { return _fMaxPickDistance; }
  ColorSpace::e getColorSpace() { return _eColorSpace; }
  int32_t getNumTextQuads() { return _iNumTextQuads; }
  bool& getEnableRuntimeErrorChecking() { return _bEnableRuntimeErrorChecking; }
  bool& getEnableDebugErrorChecking() { return _bEnableDebugErrorChecking; }
  int getBakedCharSize() { return _iBakedCharSize; }
  int getFontBitmapSize() { return _iFontBitmapSize; }
  int getMaxPointLights() { return _iMaxPointLights; }
  int getMaxDirLights() { return _iMaxDirLights; }
  bool getSaveFrameTexture() { return _bSaveFrameTexture; }
  int getSaveFrameTextureMod() { return _iSaveFrameTextureMod; }
  bool getBreakOnSDLError() { return _bBreakOnSDLError; }
  bool getBreakOnOpenGLError() { return _bBreakOnOpenGLError; }
  bool getShowConsole() { return _bShowConsole; }
  void setShowConsole(bool b) { _bShowConsole = b; }
  void setGameHostAttached(bool b) { _bGameHostAttached = b; }
  bool getGameHostAttached() { return _bGameHostAttached; }
  int getGameHostTimeoutMs() { return _iGameHostTimeoutMs; }
  int getGameHostPort() { return _iGameHostPort; }
  bool getEnableObjectShadows() { return _bEnableObjectShadows; }
  bool getEnableTerrainShadows() { return _bEnableTerrainShadows; }
  int32_t getShadowMapResolution() { return _iShadowMapResolution; }
  int32_t getShadowMapMaxInfluences() { return _iShadowMapMaxInfluences; }
  float getMaxPointLightShadowDistance() { return _fMaxPointLightShadowDistance; }
  int getMaxFrustShadowSamples() { return _iMaxFrustShadowSamples; }
  int getMaxCubeShadowSamples() { return _iMaxCubeShadowSamples; }
  int getFullscreenWidth() { return _iFullscreenWidth; }
  int getFullscreenHeight() { return _iFullscreenHeight; }
  int getModelThumbSize() { return _iModelThumbSize; }
  int getMaxHardwareIncomingBufferSizeBytes() { return  _iMaxHardwareIncomingBufferSizeBytes; }
  int getMaxHardwareOutgoingBufferSizeBytes() { return  _iMaxHardwareOutgoingBufferSizeBytes; }
  int getMaxSoftwareIncomingBufferSizeBytes() { return  _iMaxSoftwareIncomingBufferSizeBytes; }
  int getMaxSoftwareOutgoingBufferSizeBytes() { return  _iMaxSoftwareOutgoingBufferSizeBytes; }
  RenderSystem::e getRenderSystem() { return _eRenderSystem; }
  double windowedAspectRatio() { return _windowedAspectRatio; }
  GamePadType gamePadType() { return _eGamePadType;  }
  OpenGLProfile openGLProfile() { return _eOpenGLProfile; }

protected:
  virtual void pkp(std::vector<string_t>& tokens) override;
  virtual void preLoad() override;
  virtual void postLoad(bool success) override;


private:
  std::shared_ptr< EngineConfig_Internal> _pint = nullptr;

  OpenGLProfile _eOpenGLProfile = OpenGLProfile::Compatibility;
  GamePadType _eGamePadType = GamePadType::KeyboardAndMouse;
  double _windowedAspectRatio = 1920.0 / 1080.0;
  bool _bEnableLogToFile = true;
  bool _bEnableLogToConsole = true;
  bool _bStartFullscreen = false;
  int _iDefaultScreenWidth = 800;
  int _iDefaultScreenHeight = 600;
  bool _bForceAspectRatio = true;
  string_t _strGpuDeviceName;
  float _fDefaultFieldOfView = 64.734;
  float _fMaxPickDistance = 2000;
  ColorSpace::e _eColorSpace = ColorSpace::e::SRGB;
  int _iBakedCharSize = 64;
  int _iFontBitmapSize = 2048;
  int _iMaxPointLights = 32;
  int _iMaxDirLights = 32;
  int _iFullscreenWidth = 0;
  int _iFullscreenHeight = 0;
  RenderSystem::e _eRenderSystem = RenderSystem::OpenGL;

  bool _bEnableRuntimeErrorChecking = true;
#ifdef _DEBUG
  bool _bEnableDebugErrorChecking = true;
#else
  bool _bEnableDebugErrorChecking = false;
#endif
  bool _bEnableMsaa = false;
  int32_t _iMsaaSamples = 4;
  bool _bSaveFrameTexture = false;
  int _iSaveFrameTextureMod = 10;
  bool _bBreakOnSDLError = false;
  bool _bBreakOnOpenGLError = false;
  int32_t _iNumTextQuads = 1024;
  bool _bShowConsole = false;
  bool _bEnableObjectShadows = true;
  bool _bEnableTerrainShadows = false;
  int32_t _iShadowMapResolution = 256;
  int32_t _iShadowMapMaxInfluences = 3;
  float _fMaxPointLightShadowDistance = 200.0f;
  int _iMaxFrustShadowSamples = 4;
  int _iMaxCubeShadowSamples = 4;
  int _iModelThumbSize = 256;
  int _iMaxHardwareIncomingBufferSizeBytes = 8192;
  int _iMaxHardwareOutgoingBufferSizeBytes = 8192;
  int _iMaxSoftwareIncomingBufferSizeBytes = 8192;
  int _iMaxSoftwareOutgoingBufferSizeBytes = 8192;
  bool _bGameHostAttached = false; // if we are running in an WPF engine wrapper
  int _iGameHostTimeoutMs = 60000;
  int _iGameHostPort = 44244;
};
}//ns Game

#endif
