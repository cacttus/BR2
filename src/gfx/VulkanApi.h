/**
*
*    @file VulkanApi.h
*    @date August 6, 2019
*    @author Derek Page
*
*    � 2019 
*
*
*/
#pragma once
#ifndef __VULKANAPI_15651062481484433440_H__
#define __VULKANAPI_15651062481484433440_H__

#include "../gfx/GraphicsApi.h"

namespace Game {
#define VkExtFn(_vkFn) PFN_##_vkFn _vkFn = nullptr;
/**
*    @class VulkanApi
*    @brief
*
*/
class VulkanApi : public GraphicsApi {
   // std::vector<VkLayerProperties> _availableLayers;

    bool _bEnableValidationLayers = true;

    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    std::vector<const char*> getValidationLayers();
    std::vector<const char*> getExtensionNames();

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkExtent2D swapChainExtent;
    VkFormat swapChainImageFormat;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;


    VkQueue graphicsQueue; //Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup.
    VkQueue presentQueue; 

   // void loadCaps();
    bool extensionSupported(std::string ext);
    void setupDebug();
    void loadExtensions();
    void createVulkanInstance(t_string title);
    void pickPhysicalDevice();
    void findQueueFamilies();
    void createLogicalDevice();
    void makeSwapChain();
    void makeImageViews();

    VkExtFn(vkCreateDebugUtilsMessengerEXT);  //PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    VkExtFn(vkDestroyDebugUtilsMessengerEXT);  
    
    
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