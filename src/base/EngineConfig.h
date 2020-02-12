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

namespace Game {
/**
*  @class EngineConfig
*  @brief Global configuration for the engine.
*  Engine configuration should be per application.
*/
class EngineConfig {
public:
  bool getEnableMsaa() { return _bEnableMsaa; }
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
  //void setEnableRuntimeErrorChecking(bool b) { _bEnableRuntimeErrorChecking = b; }//For convenience to speed up debugging
  int getMaxFrustShadowSamples() { return _iMaxFrustShadowSamples; }
  int     getMaxCubeShadowSamples() { return _iMaxCubeShadowSamples; }
  int getFullscreenWidth() { return _iFullscreenWidth; }
  int getFullscreenHeight() { return _iFullscreenHeight; }
  int getModelThumbSize() { return _iModelThumbSize; }
  int getMaxHardwareIncomingBufferSizeBytes() { return  _iMaxHardwareIncomingBufferSizeBytes; }
  int getMaxHardwareOutgoingBufferSizeBytes() { return  _iMaxHardwareOutgoingBufferSizeBytes; }
  int getMaxSoftwareIncomingBufferSizeBytes() { return  _iMaxSoftwareIncomingBufferSizeBytes; }
  int getMaxSoftwareOutgoingBufferSizeBytes() { return  _iMaxSoftwareOutgoingBufferSizeBytes; }
  RenderSystem::e getRenderSystem() { return _eRenderSystem; }

private:
  friend class EngineConfigFile;
  //  bool _bEnableGpuDebugLogging = true;
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
  //bool _bPrintShaderSourceOnError = true;
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
