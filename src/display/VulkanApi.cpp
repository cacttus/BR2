#include "../display/VulkanApi.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/oglErr.h"


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
void VulkanApi::createWindow(t_string title) {
    makeWindow(title, SDL_WINDOW_VULKAN);

    //loadCaps();
    createVulkanInstance(title);
    loadExtensions();
    setupDebug();

    pickPhysicalDevice();

    createLogicalDevice();

    //Create queue
    vkGetDeviceQueue(device, getGraphicsQueueFamily().value(), 0, &graphicsQueue);
}
std::vector<const char*> VulkanApi::getValidationLayers() {
    std::vector<const char*> layerNames{};
    if (_bEnableValidationLayers) {
        layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
    }
    return layerNames;
}
std::vector<const char*> VulkanApi::getExtensionNames() {
    std::vector<const char*> extensionNames{};
    //Get initial data.
    uint32_t extensionCount;
    SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionCount, nullptr);
    extensionNames = std::vector<const char*>(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionCount, extensionNames.data());

    if (_bEnableValidationLayers) {
        extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensionNames;
}
void VulkanApi::createVulkanInstance(t_string title) {
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

    if (!SDL_Vulkan_CreateSurface(_pWindow, instance, &surface)) {
        OglErr::checkSDLErr();
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
void VulkanApi::getDrawableSize(int* w, int* h) {
}
void VulkanApi::swapBuffers() {
}
void VulkanApi::makeCurrent() {
}
void VulkanApi::cleanup() {
    if (_bEnableValidationLayers) {
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    GraphicsApi::cleanup();
}
void VulkanApi::createContext(std::shared_ptr<AppBase> app) {
}
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
std::optional<uint32_t> VulkanApi::getGraphicsQueueFamily() {
    //Retrieve a queue family that we need.

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                // if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                return true;
                //}
            }
        }
    }

    return false;
}

void VulkanApi::createLogicalDevice() {
    //The currently available drivers will only allow you to create a small number of queues for each 
    //  queue family and you don't really need more than one. That's because you can create all of the 
    //      command buffers on multiple threads and then submit them all at once on the main thread with 
     //     a single low-overhead call.

    std::optional<uint32_t> queueFamily = getGraphicsQueueFamily();
    if (queueFamily.has_value() == false) {
        BroThrowException("GPU doesn't contain any suitable queue families.");
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    //**NOTE** deviceFeatures must be modified in the deviceFeatures in isDeviceSuitable
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.geometryShader = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    //Previous implementations of Vulkan made a distinction between instance and device specific validation layers,
    //but this is no longer the case. That means that the enabledLayerCount and ppEnabledLayerNames fields of 
    //VkDeviceCreateInfo are ignored by up-to-date implementations.
    createInfo.enabledExtensionCount = 0;
    std::vector<const char*> layerNames;
    createInfo.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
    createInfo.ppEnabledLayerNames = layerNames.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        BroThrowException("Failed to create logical device.");
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
