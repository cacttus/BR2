#include "../gfx/VulkanApi.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/oglErr.h"
#include "../app/GraphicsWindow.h"
#include "../base/SDLUtils.h"


namespace Game {
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData);

VulkanApi::VulkanApi() {
}
VulkanApi::~VulkanApi() {
}
//void VulkanApi::getDrawableSize(int* w, int* h) {
//}
//void VulkanApi::swapBuffers() {
//}
//void VulkanApi::makeCurrent() {
//}
std::shared_ptr<GraphicsWindow> VulkanApi::createWindow(string_t title, bool isMain) {
  // makeWindow(title, SDL_WINDOW_VULKAN);

   //loadCaps();
  createVulkanInstance(title);
  loadExtensions();
  setupDebug();
  pickPhysicalDevice();
  createLogicalDevice();
  makeSwapChain();
  makeImageViews();

  //TODO: later we fix this
  return nullptr;
}
std::vector<const char*> VulkanApi::getValidationLayers() {
  std::vector<const char*> layerNames{};
  if (_bEnableValidationLayers) {
    layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
  }
  return layerNames;
}
std::vector<const char*> VulkanApi::getExtensionNames() {
  std::vector<const char*> extensionNames{ };
  //Get initial data.
  uint32_t extensionCount;
  SDL_Vulkan_GetInstanceExtensions(Gu::getMainWindow()->getSDLWindow(), &extensionCount, nullptr);
  extensionNames = std::vector<const char*>(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(Gu::getMainWindow()->getSDLWindow(), &extensionCount, extensionNames.data());

  if (_bEnableValidationLayers) {
    extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensionNames;
}
void VulkanApi::createVulkanInstance(string_t title) {
  std::vector<const char*> layerNames = getValidationLayers();
  std::vector<const char*> extensionNames = getExtensionNames();

  //Debug print the extension names.
  std::string exts = "";
  std::string del = "";
  for (const char* st : extensionNames) {
    exts += del + st + "\r\n";
    del = "  ";
  }
  BroLogInfo("Available Vulkan Extensions: \r\n" + exts);

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = title.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createinfo{};
  createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createinfo.pApplicationInfo = &appInfo;
  createinfo.enabledLayerCount = layerNames.size();
  createinfo.ppEnabledLayerNames = layerNames.data();
  createinfo.enabledExtensionCount = extensionNames.size();
  createinfo.ppEnabledExtensionNames = extensionNames.data();
  createinfo.pNext = nullptr;
  createinfo.flags = 0;

  //https://www.reddit.com/r/vulkan/comments/5xg8zx/exception_thrown_at_0x00007ff944943cbb/
  //Possible crash solution
  //Struct packing /Zp
  //8	Packs structures on 8-byte boundaries (default for x86, ARM, and ARM64).
  //16	Packs structures on 16 - byte boundaries(default for x64).
  //https://community.khronos.org/t/how-to-debug-vkcreateinstance-exception-crash/7415/12

  VkResult res = vkCreateInstance(&createinfo, nullptr, &instance);
  if (res != VK_SUCCESS) {
    BroThrowException("Failed to create vulkan instance: " + (int)res);
  }

  if (!SDL_Vulkan_CreateSurface(Gu::getMainWindow()->getSDLWindow(), instance, &surface)) {
    SDLUtils::checkSDLErr();
    BroThrowException("SDL failed to create vulkan window.");
  }
  //You can log every vulkan call to stdout.
}
void VulkanApi::loadExtensions() {
#define VkLoadExt(_v) do{\
_v = (PFN_##_v)vkGetInstanceProcAddr(instance, #_v); \
if (_v == nullptr) { \
    BroLogError("Could not find " + #_v); \
} \
} while(0)

  VkLoadExt(vkCreateDebugUtilsMessengerEXT);
  VkLoadExt(vkDestroyDebugUtilsMessengerEXT);

}

void VulkanApi::cleanup() {
  if (_bEnableValidationLayers) {
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  for (auto imageView : swapChainImageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(device, swapChain, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);

  GraphicsApi::cleanup();
}
//void VulkanApi::createContext(std::shared_ptr<AppBase> app) {
//}
void VulkanApi::setupDebug() {
  if (!_bEnableValidationLayers) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr; // Optional

  if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    BroLogError("vkCreateDebugUtilsMessengerEXT failed.");
  }
}
void VulkanApi::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    BroThrowException("No Vulkan enabled GPUs available.");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  //List all devices for debug, and also pick one.
  std::string devInfo = "\r\n";
  int i = 0;
  for (const auto& device : devices) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    devInfo += Stz "Device " + i + ": " + deviceProperties.deviceName + "\r\n";
    devInfo += Stz "  Driver Version: " + deviceProperties.driverVersion + "\r\n";
    devInfo += Stz "  API Version: " + deviceProperties.apiVersion + "\r\n";

    //**NOTE** deviceFeatures must be modified in the deviceFeatures in logical device createInfo()
    if (physicalDevice == VK_NULL_HANDLE) {
      if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        deviceFeatures.geometryShader &&
        deviceFeatures.fillModeNonSolid) {
        physicalDevice = device;
      }
    }

    i++;
  }
  BroLogInfo(devInfo);

  if (physicalDevice == VK_NULL_HANDLE) {
    BroThrowException("Failed to find a suitable GPU.");
  }

}
void VulkanApi::createLogicalDevice() {
  //**NOTE** deviceFeatures must be modified in the deviceFeatures in isDeviceSuitable
  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.geometryShader = VK_TRUE;
  deviceFeatures.fillModeNonSolid = VK_TRUE;

  //Queues
  findQueueFamilies();

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily.value(), presentFamily.value() };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  //Extensions
  std::vector<const char*> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
  //TODO:
  //for (const auto& extension : availableExtensions) {
  //    deviceExtensions.erase(extension.extensionName);
  //}

  //Logical Device
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  createInfo.enabledLayerCount = 0;

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    BroThrowException("Failed to create logical device.");
  }

  //Create queues
  vkGetDeviceQueue(device, graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, presentFamily.value(), 0, &presentQueue);
}
void VulkanApi::findQueueFamilies() {
  //Retrieve a queue family that we need.

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    //Check for presentation support
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

    if (queueFamily.queueCount > 0 && presentSupport) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        graphicsFamily = i;
      }
      presentFamily = i;
    }
    i++;
  }

  if (graphicsFamily.has_value() == false || presentFamily.has_value() == false) {
    BroThrowException("GPU doesn't contain any suitable queue families.");
  }
}
void VulkanApi::makeSwapChain() {
  VkPresentModeKHR;
  VkSurfaceFormatKHR;
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  if (formatCount != 0) {
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
  }
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  if (presentModeCount != 0) {
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
  }
  VkSurfaceFormatKHR surfaceFormat;
  for (const auto& availableFormat : formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      surfaceFormat = availableFormat;
      break;
    }
  }
  //VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available
  // = VK_PRESENT_MODE_FIFO_KHR = vsync
  VkPresentModeKHR presentMode;
  for (const auto& availablePresentMode : presentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = availablePresentMode;
      break;
    }
  }

  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

  //Image count.
  uint32_t imageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
    imageCount = caps.maxImageCount;
  }

  //Extent - Image size
  if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    swapChainExtent = caps.currentExtent;
  }
  else {
    VkExtent2D actualExtent = { 0, 0 };
    actualExtent.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, actualExtent.height));
    swapChainExtent = actualExtent;
  }

  //Create swapchain
  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = swapChainExtent;
  createInfo.imageArrayLayers = 1; //more than 1 for stereoscopic application
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform = caps.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE; // ** For window resizing.

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
    BroThrowException("Failed to create swap chain.");
  }

  //Retrieve images and set format.
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
  swapChainImageFormat = surfaceFormat.format;
}
void VulkanApi::makeImageViews() {
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
      BroThrowException("Failed to create swapchain image views.");
    }
  }
}
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData) {

  std::string msgtype = "";
  switch (messageSeverity) {

  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:     msgtype = "general";  break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: msgtype = "validation"; break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: msgtype = "performance"; break;
  }

  std::string msg = "";
  if (pCallbackData != nullptr) {
    msg = std::string(pCallbackData->pMessage);
  }

  switch (messageSeverity) {

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    BroLogInfo(msgtype + ":" + msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    BroLogInfo(msgtype + ":" + msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    BroLogWarn(msgtype + ":" + msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    BroLogError(msgtype + ":" + msg);
    break;
  }


  return VK_FALSE;
}



}//ns Game
