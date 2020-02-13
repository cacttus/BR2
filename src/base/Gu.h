/**
*  @file Gu.h
*  @date November 6, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __GU_14784108333124451735_H__
#define __GU_14784108333124451735_H__

#include "../base/BaseHeader.h"
#include "../base/AppHeader.h"
#include "../gfx/GfxHeader.h"
#include "../math/MathHeader.h"
#include "../world/WorldHeader.h"
#include "../base/Stopwatch.h"

namespace BR2 {
/**
*    @class Gu
*    @brief Global Utils For everything.
*/
class Gu : public GameMemory {
public:
  static string_t getEngineName() { return "BR2"; }
  static std::shared_ptr<ApplicationPackage> getAppPackage();
  static void setPackage(std::shared_ptr<ApplicationPackage> x);

  static std::shared_ptr<WindowManager> getWindowManager();

  //Make the graphics context current.
  static void setContext(std::shared_ptr<GLContext> ct);
  static std::shared_ptr<GLContext> getContext();

  // static std::shared_ptr<GLContext> getGraphicsContext();
  static std::shared_ptr<RenderSettings> getRenderSettings();

  static std::shared_ptr<Sequencer> getSequencer();

  //static std::shared_ptr<AppBase> getApp();
  static std::shared_ptr<InputManager> getInputManager();
  static std::shared_ptr<SoundCache> getSoundCache();
  static std::shared_ptr<EngineConfig> getConfig();
  static std::shared_ptr<Logger> getLogger();
  static std::shared_ptr<EngineConfig> getEngineConfig();
  static std::shared_ptr<Net> getNet();

  //These are all context specific
  static std::shared_ptr<GraphicsWindow> getMainWindow();
  static std::shared_ptr<GraphicsWindow> getActiveWindow();

  static void updateGlobals();
  static void createManagers();
  static void checkErrorsDbg();
  static void checkErrorsRt();
  static void initGlobals(const std::vector<std::string>& args);
  static void deleteGlobals();
  static void debugBreak();
  static bool isDebug();

  static t_timeval  getMicroSeconds();
  static t_timeval  getMilliSeconds();

  static bool is64Bit();
  static bool isBigEndian();

  static std::shared_ptr<Img32> loadImage(std::string path);
  static bool saveImage(std::string path, std::shared_ptr<Img32> spec);
  static void freeImage(std::shared_ptr<Img32> b);

  static int loadSound(std::string path, int& iChannels, int& iSampleRate, int16_t*& pData, int& nSamples, int& iDataLenBytes);

  static std::vector<string_t> argsToVectorOfString(int argc, char** argv, char delimiter = '=');

  static void inlineDrawBoxCont(const Box3f* b);
  static float fade(float t);
  static void checkMemory();
  static string_t getOperatingSystemName();
  static uint32_t getCurrentThreadId();

  static std::string getCPPVersion();

  static void print(char msg);
  static void print(const char* msg);
  static void print(const string_t& msg);

  static void guiQuad2d(Box2f& pq, std::shared_ptr<RenderViewport> vp);//Transforms a quad for the matrix-less 

  template < typename Tx >
  static bool addIfDoesNotContain(std::vector<Tx>& vec, Tx& rhs) {
    for (size_t i = 0; i < vec.size(); ++i) {
      if (vec[i] == rhs) {
        return false;
      }
    }
    vec.push_back(rhs);
    return true;
  }
  template <typename Tx>
  static void incrementEnum(Tx& __in_ eValue, Tx iMaxValue) {
    eValue = (Tx)((int)eValue + 1);
    if ((int)eValue >= iMaxValue) {
      eValue = (Tx)0;
    }
  }

private:
  static std::shared_ptr<Sequencer> _pSequencer;
  static std::shared_ptr<InputManager> _pInput;
  static std::shared_ptr<SoundCache> _pSoundCache;
  static std::shared_ptr<ApplicationPackage> _pAppPackage;
  static std::shared_ptr<RenderSettings> _pRenderSettings;
  static std::shared_ptr<EngineConfig> _pEngineConfig;
  static std::shared_ptr<Logger> _pLogger;
  static std::shared_ptr<Net> _pNet;
  static std::shared_ptr<WindowManager> _pWindowManager;
  static std::shared_ptr<GLContext> _pActiveContext;

  static void createCache();
  static void loadConfig(const std::vector<std::string>& args);
};

#define Graphics Gu::getGraphicsContext()
#define Config Gu::getEngineConfig()


}//ns Game



#endif
