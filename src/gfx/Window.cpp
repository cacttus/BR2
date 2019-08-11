#include "../base/Base.h"
#include "../base/AppBase.h"
#include "../base/Gu.h"
#include "../gfx/Window.h"


namespace Game {
///////////////////////////////////////////////////////////////////
Window::Window(t_string title, int rendersystem)
{

    makeSDLWindow();

    Gu::SDLTrySetWindowIcon(_pSDLWindow, Gu::getApp()->getIconFullPath());

    //Note we may need to adjust for actual start width/height if start width is too large , etc.
    //*Set room width / height
    _iLastWidth = Gu::getApp()->getStartWidth();
    _iLastHeight = Gu::getApp()->getStartHeight();

    _pViewport = std::make_shared<WindowViewport>(_iLastWidth, _iLastHeight);
    Gu::setViewport(_pViewport);


}
void Window::makeSDLWindow(t_string title, int rendersystem) {

    t_string title;
    bool bFullscreen = false;

    int x, y, w, h, flags;
#ifdef BRO_OS_IPHONE
    x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
    title = "";
#elif BRO_OS_WINDOWS

    //int render_system = SDL_WINDOW_OPENGL;
    //if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::e::Vulkan) {
    //    render_system = SDL_WINDOW_VULKAN;
    //}
    if (bFullscreen) {
        x = 0; y = 0;
        w = 1920; h = 1080;
        flags = render_system;
    }
    else {
        x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | render_system | SDL_WINDOW_RESIZABLE;
    }
    title = windowTitle;
#endif

    //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
    _pSDLWindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    OglErr::checkSDLErr();


    //Fullscreen nonsense
    if (bFullscreen) {
        SDL_SetWindowFullscreen(_pWindow, SDL_WINDOW_FULLSCREEN);
    }
    OglErr::checkSDLErr();

#ifdef BRO_OS_WINDOWS
    BroLogError("We are not making the window icon because there's an error somewhere in SDL here.");
    //**There is an error here
  //  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pApp->getIconFullPath());
#endif


}
Window::~Window()
{
    if (_pSDLWindow != nullptr) {
        SDL_DestroyWindow(_pSDLWindow);
        _pSDLWindow = nullptr;
    }
}
void Window::updateWidthHeight(uint32_t w, uint32_t h, bool bForce){
    //update view/cam
    if (_iLastWidth != w || bForce) {
        _pViewport->setWidth(w);
    if (_iLastHeight != h || bForce) {
        _pViewport->setHeight(h);
    }
    if (_iLastHeight != h || _iLastWidth != w || bForce) {
        if(_pRenderPipe != nullptr) {
            _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
        }
        _pApp->screenChanged(w, h, _bFullscreen);
        Gu::getGui()->screenChanged(w, h, _bFullscreen);
    }
    _iLastWidth = w;
    _iLastHeight = h;


}






}//ns Game
