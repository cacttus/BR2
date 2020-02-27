/**
*  @file AppBase.h
*  @date January 9, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __ROOMBASE_1483942071882744649_H__
#define __ROOMBASE_1483942071882744649_H__

#include "../base/AppHeader.h"
#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"
namespace BR2 {
/**
*  @class AppBase
*  @brief Base class for application.
*/
class AppBase : public Drawable {
public:
  AppBase();
  virtual ~AppBase() override;
  virtual void init() = 0; //*Note this is not called right after inst

  string_t makeAssetPath(string_t file);
  string_t makeAssetPath(string_t folder, string_t file);

  virtual void drawForwardDebug(RenderParams& rp) override;

  //virtual void screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) { };
  virtual void idle(t_timeval us) {}

  // virtual void step(float dt) = 0;
 //  virtual void teardown() = 0;
  virtual void updateTouch(std::shared_ptr<InputManager>, float dt) = 0;
  virtual void userZoom(int amount) = 0;
  virtual string_t getIconFullPath() { return ""; }
  virtual string_t getConfigPath() = 0;
  virtual string_t getAssetsDir() = 0;
  virtual string_t getShadersDir() = 0; //This is relative to the getAssetsDir.  E.g. "/shr/"
  virtual string_t getCacheDir() = 0; // the place where we store cached shaders, and generated images
  virtual string_t getModelsTextDir() = 0; // MOB dir 
  virtual string_t getModelsBinDir() = 0; // MBI DIr
  virtual string_t getEnvTexturePath() = 0;

  virtual void serverPacketReceived(std::shared_ptr<Packet> temp) = 0;
};

}//ns Game



#endif
