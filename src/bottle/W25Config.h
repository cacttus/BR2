/**
*  @file W25Config.h
*  @date April 30, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __W25CONFIG_14935947052266226391_H__
#define __W25CONFIG_14935947052266226391_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
/**
*  @class W25Config
*  @brief
*/
class W25Config : public VirtualMemory {
  friend class ObFile;
public:
  W25Config();
  virtual ~W25Config() override;

  uint32_t getMaxGridsGenAsync() { return _iMaxGridsGenAsync; }
  bool getIsDebugMode() { return _bDebugMode; }
  bool getDebugSkipTitle() { return _bDebugMode && _bDebugSkipTitle; }
  bool getDebugNewGrid() { return _bDebugMode && _bDebugNewGrid; }
  DebugGridMode::e getDebugGridMode() { return _eDebugGridMode; }
  int getDebugSeed() { return _bDebugMode && _iDebugSeed; }
  //  bool getRtErrorCheck() { return _bRtErrorCheck; }
  //  bool getGlUseMapBuffer() { return _bGlUseMapBuffer; }
  bool getRenderBlocks() { return _bRenderBlocks; }
  void setRenderBlocks(bool b) { _bRenderBlocks = b; }
  //   bool getStartFullscreen() { return _bStartFullscreen; }

   //  uint32_t getStartWidth() { return _uiStartWidth; }
   //  uint32_t getStartHeight() { return _uiStartHeight; }
   //  bool getForceAspectRatio() { return _bForceAspectRatio; }
  bool getDebugGenerate00Only() { return _bDebugGenerate00Only; }
  uint32_t getGridCountLimit() { return _iGridCountLimit; }
  int32_t getSeed();
  static bool hideTransparentSide(W25Geom opaque, W25Geom transparent, int iSide);

private:
  bool _bDebugMode = false;
  bool _bDebugSkipTitle = false;
  bool _bDebugNewGrid = false;
  DebugGridMode::e _eDebugGridMode = DebugGridMode::e::Flat;
  int _iDebugSeed = 9;
  // bool _bRtErrorCheck = false;
 //  bool _bGlUseMapBuffer = false;
  bool _bRenderBlocks = false;
  // bool _bStartFullscreen = false;
//   bool _bForceAspectRatio = true;
  bool _bDebugGenerate00Only = false;
  //  uint32_t _uiStartWidth = 800;
  //  uint32_t _uiStartHeight = 600;
    //Non-Debu
  int _iGameSeed = 0;
  uint32_t _iMaxGridsGenAsync;
  uint32_t _iGridCountLimit = 2000;

};

}//ns Game



#endif
