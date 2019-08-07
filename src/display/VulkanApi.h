/**
*
*    @file VulkanApi.h
*    @date August 6, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __VULKANAPI_15651062481484433440_H__
#define __VULKANAPI_15651062481484433440_H__

#include "../display/GraphicsApi.h"

namespace Game {
/**
*    @class VulkanApi
*    @brief
*
*/
class VulkanApi : public GraphicsApi {
    VkSurfaceKHR surface;
    VkInstance instance;
    void debugPrintExtensions(std::vector<const char*> exts);
public:
    virtual void createWindow(t_string title) override;
    virtual void getDrawableSize(int* w, int* h) override;
    virtual void swapBuffers() override;
    virtual void makeCurrent() override;
    virtual void cleanup() override;
    virtual void createContext(std::shared_ptr<AppBase> app) override;

    VulkanApi();
    virtual ~VulkanApi() override;
};

}//ns Game



#endif
