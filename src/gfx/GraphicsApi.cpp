#include "../gfx/GraphicsApi.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/Window.h"

namespace Game {
GraphicsApi::GraphicsApi() {
}
GraphicsApi::~GraphicsApi() {
    cleanup();
}
void GraphicsApi::makeWindow(t_string windowTitle, int render_system) {
    _pWindow = std::make_shared<Window>(windowTitle, render_system);
}
void GraphicsApi::destroyWindow() {
    _pWindow = nullptr;
}
void GraphicsApi::cleanup() {
}







}//ns Game
