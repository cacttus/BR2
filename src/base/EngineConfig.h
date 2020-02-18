/**
*  @file EngineConfig.h
*  @date May 8, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __ENGINECONFIG_14942867052644616215_H__
#define __ENGINECONFIG_14942867052644616215_H__

#include "../base/BaseHeader.h"
#include "../base/XmlFile.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class EngineConfig
*  @brief Global configuration for the engine. Engine configuration should be per application.
*/
class EngineConfig {
public:
  bool getEnableMsaa() { return _bEnableMsaa; }
  int32_t getMsaaSamples() { return _iMsaaSamples; }
  bool getEnableLogToConsole() { return _bEnableLogToConsole; }
  bool getEnableLogToFile() { return _bEnableLogToFile; }
  bool getStartFullscreen() { return _bStartFullscreen; }
  int32_t getDefaultScreenWidth() { return _iDefaultScreenWidth; }
  int32_t getDefaultScreenHeight() { return _iDefaultScreenHeight; }
  bool getForceAspectRatio() { return _bForceAspectRatio; }
  string_t getGpuDeviceName() { return _strGpuDeviceName; }
  float getDefaultFieldOfView() { return _fDefaultFieldOfView; }
  float getMaxPickDistance() { return _fMaxPickDistance; }
  ColorSpace::e getColorSpace() { return _eColorSpace; }
  int32_t getNumTextQuads() { return _iNumTextQuads; }
  bool& getEnableRuntimeErrorChecking() { return _bEnableRuntimeErrorChecking; }
  bool& getEnableDebugErrorChecking() { return _bEnableDebugErrorChecking; }
  int32_t getBakedCharSize() { return _iBakedCharSize; }
  int32_t getFontBitmapSize() { return _iFontBitmapSize; }
  int32_t getMaxPointLights() { return _iMaxPointLights; }
  int32_t getMaxDirLights() { return _iMaxDirLights; }
  bool getSaveFrameTexture() { return _bSaveFrameTexture; }
  int32_t getSaveFrameTextureMod() { return _iSaveFrameTextureMod; }
  bool getBreakOnSDLError() { return _bBreakOnSDLError; }
  bool getBreakOnOpenGLError() { return _bBreakOnOpenGLError; }
  bool getShowConsole() { return _bShowConsole; }
  void setShowConsole(bool b) { _bShowConsole = b; }
  void setGameHostAttached(bool b) { _bGameHostAttached = b; }
  bool getGameHostAttached() { return _bGameHostAttached; }
  int32_t getGameHostTimeoutMs() { return _iGameHostTimeoutMs; }
  int32_t getGameHostPort() { return _iGameHostPort; }
  bool getEnableObjectShadows() { return _bEnableObjectShadows; }
  bool getEnableTerrainShadows() { return _bEnableTerrainShadows; }
  int32_t getShadowMapResolution() { return _iShadowMapResolution; }
  int32_t getShadowMapMaxInfluences() { return _iShadowMapMaxInfluences; }
  float getMaxPointLightShadowDistance() { return _fMaxPointLightShadowDistance; }
  int32_t getMaxFrustShadowSamples() { return _iMaxFrustShadowSamples; }
  int32_t getMaxCubeShadowSamples() { return _iMaxCubeShadowSamples; }
  int32_t getFullscreenWidth() { return _iFullscreenWidth; }
  int32_t getFullscreenHeight() { return _iFullscreenHeight; }
  int32_t getModelThumbSize() { return _iModelThumbSize; }
  int32_t getMaxHardwareIncomingBufferSizeBytes() { return  _iMaxHardwareIncomingBufferSizeBytes; }
  int32_t getMaxHardwareOutgoingBufferSizeBytes() { return  _iMaxHardwareOutgoingBufferSizeBytes; }
  int32_t getMaxSoftwareIncomingBufferSizeBytes() { return  _iMaxSoftwareIncomingBufferSizeBytes; }
  int32_t getMaxSoftwareOutgoingBufferSizeBytes() { return  _iMaxSoftwareOutgoingBufferSizeBytes; }
  RenderSystem::e getRenderSystem() { return _eRenderSystem; }

private:
  friend class EngineConfigFile;
  //  bool _bEnableGpuDebugLogging = true;
  bool _bEnableLogToFile = true;
  bool _bEnableLogToConsole = true;
  bool _bStartFullscreen = false;
  int32_t _iDefaultScreenWidth = 800;
  int32_t _iDefaultScreenHeight = 600;
  bool _bForceAspectRatio = true;
  string_t _strGpuDeviceName;
  float _fDefaultFieldOfView = 64.734;
  float _fMaxPickDistance = 2000;
  ColorSpace::e _eColorSpace = ColorSpace::e::SRGB;
  //bool _bPrintShaderSourceOnError = true;
  int32_t _iBakedCharSize = 64;
  int32_t _iFontBitmapSize = 2048;
  int32_t _iMaxPointLights = 32;
  int32_t _iMaxDirLights = 32;
  int32_t _iFullscreenWidth = 0;
  int32_t _iFullscreenHeight = 0;
  RenderSystem::e _eRenderSystem = RenderSystem::OpenGL;
  bool _bEnableRuntimeErrorChecking = true;
  bool _bEnableDebugErrorChecking = true;
  bool _bEnableMsaa = false;
  int32_t _iMsaaSamples = 4;
  bool _bSaveFrameTexture = false;
  int32_t _iSaveFrameTextureMod = 10;
  bool _bBreakOnSDLError = false;
  bool _bBreakOnOpenGLError = false;
  int32_t _iNumTextQuads = 1024;
  bool _bShowConsole = false;
  bool _bEnableObjectShadows = true;
  bool _bEnableTerrainShadows = false;
  int32_t _iShadowMapResolution = 256;
  int32_t _iShadowMapMaxInfluences = 3;
  float _fMaxPointLightShadowDistance = 200.0f;
  int32_t _iMaxFrustShadowSamples = 4;
  int32_t _iMaxCubeShadowSamples = 4;
  int32_t _iModelThumbSize = 256;
  int32_t _iMaxHardwareIncomingBufferSizeBytes = 8192;
  int32_t _iMaxHardwareOutgoingBufferSizeBytes = 8192;
  int32_t _iMaxSoftwareIncomingBufferSizeBytes = 8192;
  int32_t _iMaxSoftwareOutgoingBufferSizeBytes = 8192;
  bool _bGameHostAttached = false; // if we are running in an WPF engine wrapper
  int32_t _iGameHostTimeoutMs = 60000;
  int32_t _iGameHostPort = 44244;
};
/**
*  @class EngineConfigFile
*  @brief Configuration data for the engine loaded from engine.xml.
*/
class EngineConfigFile : public XmlFile {
public:
  EngineConfigFile();
  virtual ~EngineConfigFile() override;

  std::shared_ptr<EngineConfig> getConfig() { return _pConfig; }

protected:
  virtual void pkp(std::vector<string_t>& tokens) override;
  virtual void preLoad() override;
  virtual void postLoad(bool success) override;
  std::shared_ptr<EngineConfig> _pConfig = nullptr;
};


}//ns BR2



#endif
