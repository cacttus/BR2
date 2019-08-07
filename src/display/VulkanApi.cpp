#include "../display/VulkanApi.h"
#include "../base/Logger.h"
#include "../base/Exception.h"


namespace Game {
VulkanApi::VulkanApi(){
}
VulkanApi::~VulkanApi(){
}
void VulkanApi::createWindow(t_string title) {
    makeWindow(title, SDL_WINDOW_VULKAN);

    VkApplicationInfo appInfo{};
   // appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   // appInfo.pApplicationName = title.c_str();
   // appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
   // appInfo.pEngineName = "No Engine";
   // appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
   // appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char *> layerNames{};
     layerNames.push_back("VK_LAYER_LUNARG_standard_validation");

    uint32_t extensionCount;
    SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionCount, nullptr);
    std::vector<const char*> extensionNames(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionCount, extensionNames.data());

    debugPrintExtensions(extensionNames);

    VkInstanceCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createinfo.pApplicationInfo = &appInfo;
    createinfo.enabledLayerCount = 0;// layerNames.size();
    createinfo.ppEnabledLayerNames = layerNames.data();
    createinfo.enabledExtensionCount = 0;// extensionNames.size();
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
        BroThrowException("SDL failed to create vulkan window.");
    }
    //You can log every vulkan call to stdout.

}
void VulkanApi::getDrawableSize(int* w, int* h) {
}
void VulkanApi::swapBuffers() {
}
void VulkanApi::makeCurrent() {
}
void VulkanApi::cleanup() {
    vkDestroyInstance(instance, nullptr);
    GraphicsApi::cleanup();
}
void VulkanApi::createContext(std::shared_ptr<AppBase> app) {
}
void VulkanApi::debugPrintExtensions(std::vector<const char*> exts) {
    for (const char* st : exts) {
        BroLogInfo(st);
    }
}









}//ns Game
