/**
*  @file VulkanWindow.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __VULKANWINDOW_15665854373552580904_H__
#define __VULKANWINDOW_15665854373552580904_H__

#include "../base/GraphicsWindow.h"

namespace BR2 {
/**
*  @class VulkanWindow
*  @brief
*/
class VulkanWindow : public GraphicsWindow {
public:
  VulkanWindow(bool ismain);
  virtual ~VulkanWindow() override;
  virtual void create(string_t) override {}
};

}//ns Game



#endif
