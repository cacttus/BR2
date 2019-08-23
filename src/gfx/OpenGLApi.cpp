//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_opengl_glext.h>

#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/GLContext.h"
#include "../app/GraphicsWindow.h"
#include "../app/OpenGLWindow.h"
#include "../gfx/OpenGLApi.h"

namespace Game {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}

std::shared_ptr<GraphicsWindow> OpenGLApi::createWindow(std::string title, bool isMain = false) {
    std::shared_ptr<OpenGLWindow> w = std::make_shared<OpenGLWindow>(title, isMain);

    return w;
}



}//ns Game
