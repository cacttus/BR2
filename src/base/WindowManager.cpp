#include "../base/Gu.h"
#include "../base/WindowManager.h"
#include "../gfx/GraphicsApi.h"
#include "../base/GraphicsWindow.h"

namespace BR2 {
WindowManager::WindowManager() {
}
WindowManager::~WindowManager() {
}
std::shared_ptr<GraphicsWindow> WindowManager::createWindow(std::string title, RenderSystem::e re) {
  std::shared_ptr<GraphicsWindow> w = std::make_shared<GraphicsWindow>(true, title, re);
  w->init();
  _windows.push_back(w);
  return w;
}
void WindowManager::step() {
  //Async this probably.

  for (std::shared_ptr<GraphicsWindow> w : this->_windows) {
    w->step();
  }
  //std::thread tgr([=]() {
  //  std::lock_guard<std::mutex> guard(button->theme()->loadMutex);

  //  NVGcontext* ctx = nullptr;
  //  int realw, realh;
  //  button->renderBodyTexture(ctx, realw, realh);
  //  self->tex.rrect = { 0, 0, realw, realh };
  //  self->ctx = ctx;
  //  });

  //tgr.detach();
}








}//ns Game
