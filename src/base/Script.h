/**
*  @file Script.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __SCRIPT_15665746341154937548_H__
#define __SCRIPT_15665746341154937548_H__

#include "../base/AppHeader.h"

namespace Game {
/**
*  @class Script
*  @brief
*
*/
class Script : public VirtualMemory {
public:
  Script();
  virtual ~Script() override;

  void compileScripts();

};

}//ns Game



#endif
