#include "../gfx/GraphicsApi.h"
#include "../base/oglErr.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"

namespace Game {
GraphicsApi::GraphicsApi() {
}
GraphicsApi::~GraphicsApi() {
    cleanup();
}

void GraphicsApi::addWindow(std::shared_ptr<GraphicsWindow> w) {
    //std::shared_ptr<GraphicsWindow> w = nullptr;
   // w = std::make_shared<GraphicsWindow>(windowTitle, render_system);
    if (_pvecWindows.size() == 0) {
        _pMainWindow = w;
    }
    _pvecWindows.push_back(w);
}
void GraphicsApi::destroyMainWindow() {
    _pMainWindow = nullptr;
    _pvecWindows.clear();
}
void GraphicsApi::destroyWindow(std::shared_ptr<GraphicsWindow> w) {
    w = nullptr;
    std::vector<std::shared_ptr<GraphicsWindow>>::iterator it = std::find(_pvecWindows.begin(), _pvecWindows.end(), w);
    if (it != _pvecWindows.end()) {
        _pvecWindows.erase(it);
    }
}
void GraphicsApi::cleanup() {

}







}//ns Game
