/**
*  @file Net.h
*  @date May 9, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __NET_15574636493489018174_H__
#define __NET_15574636493489018174_H__


#include "../base/BaseHeader.h"

namespace BR2 {
class NetInternal;
/**
*  @class Net
*  @brief Networking manager class using SDL_net.
*  TODO: change to NetworkManager
*  https://www.libsdl.org/projects/SDL_net/
*/
class Net : public VirtualMemory {
public:
  Net();
  virtual ~Net() override;

  void update();

private:
  bool _bError = false;

  std::unique_ptr<NetInternal> _pInternal = nullptr;

  std::shared_ptr<SyncTimer> _pTimer;

  void init();
};

}//ns BR2



#endif
