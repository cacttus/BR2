#include "../gfx/GraphicsApi.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/SDL.h"

namespace Game {
GraphicsApi::GraphicsApi() {
}
GraphicsApi::~GraphicsApi() {
    cleanup();
}
void GraphicsApi::makeWindow(t_string windowTitle, int render_system) {
    _pWindow = std::make_shared<Window>(windowTitle, render_system);

//    t_string title;
//    bool bFullscreen = false;
//
//    int x, y, w, h, flags;
//#ifdef BRO_OS_IPHONE
//    x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
//    title = "";
//#elif BRO_OS_WINDOWS
//
//    //int render_system = SDL_WINDOW_OPENGL;
//    //if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::e::Vulkan) {
//    //    render_system = SDL_WINDOW_VULKAN;
//    //}
//    if (bFullscreen) {
//        x = 0; y = 0;
//        w = 1920; h = 1080;
//        flags = render_system;
//    }
//    else {
//        x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | render_system | SDL_WINDOW_RESIZABLE;
//    }
//    title = windowTitle;
//#endif
//
//    //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
//    _pWindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
//    SDL::checkSDLErr();
//
//
//    //Fullscreen nonsense
//    if (bFullscreen) {
//        SDL_SetWindowFullscreen(_pWindow, SDL_WINDOW_FULLSCREEN);
//    }
//    SDL::checkSDLErr();
//
//#ifdef BRO_OS_WINDOWS
//    BroLogError("We are not making the window icon because there's an error somewhere in SDL here.");
//    //**There is an error here
//  //  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pApp->getIconFullPath());
//#endif
//
}

void GraphicsApi::cleanup() {
    //if (_pWindow != nullptr) {
    //    SDL_DestroyWindow(_pWindow);
    //    _pWindow = nullptr;
    //}

}







}//ns Game
