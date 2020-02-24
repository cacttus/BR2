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
class Net_Internal;
/**
*  @class Net
*  @brief Manages networking and sending packets. https://www.libsdl.org/projects/SDL_net/
*/
class Net : public VirtualMemory {
public:
  Net();
  virtual ~Net() override;
  void update();
private:
  std::unique_ptr<Net_Internal> _pint = nullptr;
};

}//ns Game



#endif
