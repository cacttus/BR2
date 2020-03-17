#include "../base/Logger.h"
#include "../base/EngineConfig.h"

#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/HashMap.h"

namespace BR2 {
class EngineConfig_Internal {
public:
  HashMap<BaseProp*> _props;
};
EngineConfig::EngineConfig() {
  _pint = std::make_shared<EngineConfig_Internal>();
}
EngineConfig::~EngineConfig() {

}
void EngineConfig::int32Prop(string_t prop_key, std::vector<string_t>& tokens, int32_t& iind) {
  if (lcmp(tokens[0], prop_key.c_str(), 2)) {
    int32_t prop_value = TypeConv::strToInt(getCleanToken(tokens, iind));
    Int32Prop* p = new Int32Prop(prop_key, prop_value);
    //This may be an incorrect cast (not sure)
    _pint->_props.add(prop_key, (BaseProp*&)p);
  }
}
void EngineConfig::pkp(std::vector<string_t>& tokens) {
  int iind = 1;

  std::shared_ptr<EngineConfig> _pConfig = getThis<EngineConfig>();

  //Note: Don't put any OpenGL stuff in here.  It comes BEFORE we load the context.
  try {

    //TODO:
    //int32Prop(EngineConfig::MaxPointLights, tokens, iind);
    //int32Prop(EngineConfig::MaxDirLights, tokens, iind);


    if (lcmp(tokens[0], "MaxPointLights", 2)) {
      _pConfig->_iMaxPointLights = TypeConv::strToInt(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "MaxDirLights", 2)) {
      _pConfig->_iMaxDirLights = TypeConv::strToInt(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "MaxCubeShadowSamples", 2)) {
      _pConfig->_iMaxCubeShadowSamples = TypeConv::strToInt(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "MaxFrustShadowSamples", 2)) {
      _pConfig->_iMaxFrustShadowSamples = TypeConv::strToInt(getCleanToken(tokens, iind));
    }

    else if (lcmp(tokens[0], "StartFullscreen", 2)) {
      _pConfig->_bStartFullscreen = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "DefaultScreenWidth", 2)) {
      _pConfig->_iDefaultScreenWidth = TypeConv::strToUint(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "DefaultScreenHeight", 2)) {
      _pConfig->_iDefaultScreenHeight = TypeConv::strToUint(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "EnableRuntimeErrorChecking", 2)) {
      _pConfig->_bEnableRuntimeErrorChecking = TypeConv::strToBool(getCleanToken(tokens, iind));
      if (Gu::isDebug()) {
        msg("Runtime error checking is turned on by default in debug build.");
        _pConfig->_bEnableRuntimeErrorChecking = true;
      }
      //*Always default true in debug mode
#ifdef _DEBUG
      _pConfig->_bEnableRuntimeErrorChecking = true;
#endif
    }
    else if (lcmp(tokens[0], "EnableFileLog", 2)) {
      _pConfig->_bEnableLogToFile = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "EnableConsoleLog", 2)) {
      _pConfig->_bEnableLogToFile = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "GameHostTimeoutMs", 2)) {
      _pConfig->_iGameHostTimeoutMs = TypeConv::strToInt(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "GameHostPort", 2)) {
      _pConfig->_iGameHostPort = TypeConv::strToInt(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "ForceAspectRatio", 2)) {
      _pConfig->_bForceAspectRatio = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "GpuDeviceName", 2)) {
      _pConfig->_strGpuDeviceName = getCleanToken(tokens, iind);
    }
    else if (lcmp(tokens[0], "DefaultFieldOfView", 2)) {
      _pConfig->_fDefaultFieldOfView = TypeConv::strToFloat(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "ColorSpace", 2)) {
      string_t csp = getCleanToken(tokens, iind);
      if (StringUtil::equalsi(csp, "SRGB")) {
        _pConfig->_eColorSpace = ColorSpace::e::SRGB;
      }
      else if (StringUtil::equalsi(csp, "Linear")) {
        _pConfig->_eColorSpace = ColorSpace::e::Linear;
      }
      else {
        msg(Stz "Unkown color space " + csp + " defaulting to linear.");
        _pConfig->_eColorSpace = ColorSpace::e::Linear;
      }
    }
    else if (lcmp(tokens[0], "FontBitmapSize", 2)) {
      _pConfig->_iFontBitmapSize = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iFontBitmapSize < 5 || _pConfig->_iFontBitmapSize > 99999) {
        msg(tokens[0] + " invalid.  Defaulting to 1024");
        _pConfig->_iFontBitmapSize = 1024;
      }
    }
    else if (lcmp(tokens[0], "FontBakedCharSize", 2)) {
      _pConfig->_iBakedCharSize = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iBakedCharSize < 5 || _pConfig->_iBakedCharSize > 999) {
        msg(tokens[0] + " was <5 ro > 999.  Defaulting to 64");
        _pConfig->_iBakedCharSize = 64;
      }
    }
    else if (lcmp(tokens[0], "EnableObjectShadows", 2)) {
      _pConfig->_bEnableObjectShadows = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "EnableTerrainShadows", 2)) {
      _pConfig->_bEnableTerrainShadows = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "ShadowMapResolution", 2)) {
      _pConfig->_iShadowMapResolution = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (Alg::isPow2((int32_t)_pConfig->_iShadowMapResolution) == false) {
        msg(tokens[0] + " not a power of 2.  Defaulting to 256");
        _pConfig->_iShadowMapResolution = 256;
      }
    }
    else if (lcmp(tokens[0], "ShadowMapMaxInfluences", 2)) {
      _pConfig->_iShadowMapMaxInfluences = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iShadowMapMaxInfluences > 32) {
        msg(tokens[0] + " >32.  Defaulting to 32");
        _pConfig->_iShadowMapMaxInfluences = 32;
      }
    }
    else if (lcmp(tokens[0], "MaxPointLightShadowDistance", 2)) {
      _pConfig->_fMaxPointLightShadowDistance = TypeConv::strToFloat(getCleanToken(tokens, iind));
      if (_pConfig->_fMaxPointLightShadowDistance > 2000.0f) {
        msg(tokens[0] + " >2000.0f.  Defaulting to 2000.0f");
        _pConfig->_fMaxPointLightShadowDistance = 2000.0f;
      }
    }
    else if (lcmp(tokens[0], "EnableMSAA", 2)) {
      _pConfig->_bEnableMsaa = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "MSAASamples", 2)) {
      _pConfig->_iMsaaSamples = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iMsaaSamples < 0) {
        msg(Stz "MSAA Samples" + _pConfig->_iMsaaSamples + " invalid.");
        _pConfig->_iMsaaSamples = 2;
      }
    }
    else if (lcmp(tokens[0], "NumTextQuads", 2)) {
      _pConfig->_iNumTextQuads = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iNumTextQuads <= 256) {
        msg(tokens[0] + " Too small, set to 256.");
        _pConfig->_iNumTextQuads = 256;
      }
      if (_pConfig->_iNumTextQuads > 16384) {
        msg(tokens[0] + " Too large.  Set to 16384.");
        _pConfig->_iNumTextQuads = 16384;
      }
    }
    else if (lcmp(tokens[0], "BreakOnSDLError", 2)) {
      _pConfig->_bBreakOnSDLError = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "BreakOnOpenGLError", 2)) {
      _pConfig->_bBreakOnOpenGLError = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "ShowConsole", 2)) {
      _pConfig->_bShowConsole = TypeConv::strToBool(getCleanToken(tokens, iind));
    }
    else if (lcmp(tokens[0], "ModelThumbSize", 2)) {
      _pConfig->_iModelThumbSize = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iModelThumbSize < 1) {
        _pConfig->_iModelThumbSize = 1;
      }
      if (_pConfig->_iModelThumbSize > 4192) {
        _pConfig->_iModelThumbSize = 4192;
      }
    }
    else if (lcmp(tokens[0], "MaxHardwareIncomingBufferSizeBytes", 2)) {
      _pConfig->_iMaxHardwareIncomingBufferSizeBytes = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iMaxHardwareIncomingBufferSizeBytes < 1) {
        _pConfig->_iMaxHardwareIncomingBufferSizeBytes = 1;
      }
      if (_pConfig->_iMaxHardwareIncomingBufferSizeBytes > 9999999) {
        _pConfig->_iMaxHardwareIncomingBufferSizeBytes = 9999999;
      }
    }
    else if (lcmp(tokens[0], "MaxHardwareOutgoingBufferSizeBytes", 2)) {
      _pConfig->_iMaxHardwareOutgoingBufferSizeBytes = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iMaxHardwareOutgoingBufferSizeBytes < 1) {
        _pConfig->_iMaxHardwareOutgoingBufferSizeBytes = 1;
      }
      if (_pConfig->_iMaxHardwareOutgoingBufferSizeBytes > 9999999) {
        _pConfig->_iMaxHardwareOutgoingBufferSizeBytes = 9999999;
      }
    }
    else if (lcmp(tokens[0], "MaxSoftwareIncomingBufferSizeBytes", 2)) {
      _pConfig->_iMaxSoftwareIncomingBufferSizeBytes = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iMaxSoftwareIncomingBufferSizeBytes < 1) {
        _pConfig->_iMaxSoftwareIncomingBufferSizeBytes = 1;
      }
      if (_pConfig->_iMaxSoftwareIncomingBufferSizeBytes > 9999999) {
        _pConfig->_iMaxSoftwareIncomingBufferSizeBytes = 9999999;
      }
    }
    else if (lcmp(tokens[0], "MaxSoftwareOutgoingBufferSizeBytes", 2)) {
      _pConfig->_iMaxSoftwareOutgoingBufferSizeBytes = TypeConv::strToInt(getCleanToken(tokens, iind));
      if (_pConfig->_iMaxSoftwareOutgoingBufferSizeBytes < 1) {
        _pConfig->_iMaxSoftwareOutgoingBufferSizeBytes = 1;
      }
      if (_pConfig->_iMaxSoftwareOutgoingBufferSizeBytes > 9999999) { _pConfig->_iMaxSoftwareOutgoingBufferSizeBytes = 9999999; }
    }
    else if (lcmp(tokens[0], "RenderSystem", 2)) {
      string_t tok = getCleanToken(tokens, iind);

      _pConfig->_eRenderSystem = RenderSystem::OpenGL;
      if (StringUtil::equalsi(tok, "Vulkan")) {
        _pConfig->_eRenderSystem = RenderSystem::Vulkan;
      }
      else {
        msg(Stz "Render system '" + tok + "' is invalid.");
      }
    }
    else if (lcmp(tokens[0], "WindowedAspectRatio", 2)) {
      string_t st = getCleanToken(tokens, iind);
      double num, dem;
      std::vector<std::string> vs = StringUtil::split(st, '/');
      if (vs.size() == 2) {
        num = TypeConv::strToDouble(vs[0]);
        dem = TypeConv::strToDouble(vs[1]);
        _pConfig->_windowedAspectRatio = num / dem;
      }
      else {
        msg(Stz "Failed to read '" + tokens[0] + "', must be in the form uint/uint, and width/height, ex: 1920/1080.");
        Gu::debugBreak();
      }
    }
    else if (lcmp(tokens[0], "GamePad", 2)) {
      string_t tok = getCleanToken(tokens, iind);
      if (StringUtil::equalsi(tok, "KeyboardAndMouse")) {
        _pConfig->_eGamePadType = GamePadType::KeyboardAndMouse;
      }
      else {
        msg(Stz "Invalid token for: '" + tokens[0] + "' got: '" + tok + "'.");
        Gu::debugBreak();
      }
    }
    else if (lcmp(tokens[0], "OpenGLProfile", 2)) {
      string_t tok = getCleanToken(tokens, iind);
      if (StringUtil::equalsi(tok, "Compatibility")) {
        _pConfig->_eOpenGLProfile = OpenGLProfile::Compatibility;
      }
      else if (StringUtil::equalsi(tok, "Core")) {
        _pConfig->_eOpenGLProfile = OpenGLProfile::Core;
      }
      else if (StringUtil::equalsi(tok, "ES")) {
        //Technically, this should be automatic if we are sure we're on the phone.
      }
      else {
        msg(Stz "Invalid token for: '" + tokens[0] + "' got: '" + tok + "'.");
        Gu::debugBreak();
      }
    }
    else {
      msg(Stz "Unrecognized engine config token '" + tokens[0] + "'");
      Gu::debugBreak();
    }
  }
  catch (Exception * ex) {
    BRLogErrorEx("Failed to parse '" + (tokens.size() > 0 ? tokens[0] : "<invalid>") + "'in the engine configuration file ", ex);
  }
}
void EngineConfig::preLoad() {
}
void EngineConfig::postLoad(bool success) {
}

}//ns Game
