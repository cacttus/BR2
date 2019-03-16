//
//  AppRunner.h
//  brom
//
//  Created by mac1224 on 11/5/16.
//  Copyright © 2016 Swill. All rights reserved.
//
#ifndef AppRunner_h
#define AppRunner_h

#include "../base/BaseHeader.h"

namespace Game { 


struct GLProfile : public VirtualMemory {
public:
    int _iDepthBits = 0;
    int _iMinVersion = 0;
    int _iMinSubVersion = 0;
    int _iProfile  = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; // Set to true to enable the compatibility profile
    bool _bVsync = true; //Automatic on IOS
    void make(int iDepthBits, int iMinVer, int iMinSubver, int iProfile, bool bVsync) {
        _iDepthBits = iDepthBits;
        _iMinSubVersion = iMinSubver;
        _iMinVersion = iMinVer;
        _iProfile = iProfile;
        _bVsync = bVsync;
    }
};

class AppRunner : public VirtualMemory {
private:
    t_timeval _tvInitStartTime =0;
    SDL_GLContext _context;
    Engine* _pEngine = nullptr;
    SDL_Window* _pWindow = nullptr;
    SDL_AudioSpec _audioSpec;
    SDL_Renderer* _pRenderer = nullptr;

    void makeWindow(char* windowTitle);
    void makeGLContext();
    void setWindowAndOpenGLFlags(GLProfile& prof);
    void checkSDLErrors();
    void doShowError(t_string err, Exception* e);
    SDL_bool makeAudio();
    void initSDL(char* windowTitle);
    void printHelpfulDebug();
    bool handleEvents(SDL_Event * event);
    //static int eventFilter(void* data, SDL_Event* ev);
    void runGameLoop(std::shared_ptr<RoomBase> rb);
    void quit(int rc);
    void doAnnoyingSoundTest();
    void checkForOpenGlMinimumVersion(int required_version, int required_subversion);
    void getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver);
    void runGameLoopTryCatch(std::shared_ptr<RoomBase> rb);
    bool argMatch(std::vector<t_string>& args,t_string arg, int32_t iCount);
    bool runCommands(std::vector<t_string>& args);
    void checkVideoCard();
public:
    void runApp(std::vector<t_string>& args, char* windowTitle, std::shared_ptr<RoomBase> rb);
};


}//NS Game
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

