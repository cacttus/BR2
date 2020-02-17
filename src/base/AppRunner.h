/**
*  @file AppRunner.h
*  @author MetalMario971
*/
#ifndef __57823905782390589020_AppRunner_h_2298357238957238957129348__
#define __57823905782390589020_AppRunner_h_2298357238957238957129348__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {

/**
*  @class AppRunner
*  @brief The entry point of the application from main()
*/
class AppRunner : public VirtualMemory {
public:
  void runApp(const std::vector<string_t>& args);

private:
  SDL_AudioSpec _audioSpec;

  t_timeval _tvInitStartTime = 0;

  // accept a connection coming in on server_tcpsock
  TCPsocket _gameHostSocket;//Send and receive data
  TCPsocket _server_tcpsock;//Accept connections

  void initSDL();
  void doShowError(string_t err, Exception* e = nullptr);
  SDL_bool initAudio();
  void initNet();
  bool handleEvents(SDL_Event* event);
  void runApplication();
  void exitApp(string_t error, int rc);
  void runApplicationTryCatch();
  bool argMatch(const std::vector<string_t>& args, string_t arg, int32_t iCount);
  bool runCommands(const std::vector<string_t>& args);
  void printVideoDiagnostics();
  void updateWindowHandleForGamehost();
  void attachToGameHost();
  bool handleSDLEvents();
  void loadAppPackage();
};


}//ns BR2
#endif /* AppRunner_h */







//        //Init SDL ?? DO we need?
//        if (SDL_Init(0) != 0) {
//            fprintf(stderr, "Couldn't initialize video driver: %s\n",
//                    SDL_GetError());
//            return SDL_FALSE;
//        }
//
//
//        if (SDL_VideoInit(_pGlState->videodriver) < 0) {
//            fprintf(stderr, "Couldn't initialize video driver: %s\n",
//                    SDL_GetError());
//            return SDL_FALSE;
//        }





//Fullscreen
//        SDL_zero(fullscreen_mode);
//        switch (depthBits) {
//            case 16:
//                fullscreen_mode.format = SDL_PIXELFORMAT_RGB565;
//                break;
//            case 24:
//                fullscreen_mode.format = SDL_PIXELFORMAT_RGB24;
//                break;
//        }
//        fullscreen_mode.refresh_rate = _pGlState->refresh_rate;
//
//Create Window.
// char  etitle[1024];
// char* szTitle = NULL;


//SDL_ShowWindow(_pWindow);

//Not necessary - we need to make sure this it
//Create Renderer
// _pGlState->renderer = SDL_CreateRenderer(_pGlState->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
// if (!_pGlState->renderer) {
//     fprintf(stderr, "Couldn't create renderer: %s\n",
//         SDL_GetError());
//     return SDL_FALSE;
// }
// if (_pGlState->logical_w && _pGlState->logical_h) {
//     SDL_RenderSetLogicalSize(_pGlState->renderer, _pGlState->logical_w, _pGlState->logical_h);
// }
// else if (_pGlState->scale) {
//     SDL_RenderSetScale(_pGlState->renderer, _pGlState->scale, _pGlState->scale);
// }

//}

