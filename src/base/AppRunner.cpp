#include <signal.h>
#include "../base/AppRunner.h"
#include "../base/DebugHelper.h"
#include "../math/MathAll.h"

#include "../base/SoundCache.h"
#include "../base/GLContext.h"
#include "../base/Fingers.h"
#include "../base/Logger.h"
#include "../base/TStr.h"
#include "../base/oglErr.h"
#include "../base/OperatingSystem.h"
#include "../base/Engine.h"
#include "../model/ModelCache.h"

namespace Game { ;

void loadCheckProc() {
    PFNGLUSEPROGRAMPROC proc = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
    if (proc == nullptr) {
        t_string exep;
        exep += "glUseProgram was not found in your graphics driver.  There can be a few reasons for this:\n";
        exep += ("  1. Your primary graphics card is not correct.  You can set your primary graphics card in Windows.\n");
        exep += ("  2. Your graphics card is outdated.  Consider upgrading.\n");
        exep += ("  3. Your Operating System isn't Windows 7 or above.\n");
        BroThrowException(exep);
    }
}
void AppRunner::doShowError(t_string err, Exception* e)
{
    //Just in case - this is needed so we don't regress.
  //  _setupState = EngineSetupState::ErrorExit;

    //    SyncDisable();
    if (e != nullptr)
    {
        OperatingSystem::showErrorDialog(TStr(e->what()), err);
    }
    else
    {
        OperatingSystem::showErrorDialog(TStr("No Error Message"), err);
    }
    // Runtime Error

    //Do not log a second time!
    //BroLogError(err, e->what());

    //Don't destroy engine in error msg.
    //destroyEngineAndExit();
    //  SyncEnable();
}
void AppRunner::initSDL(char* windowTitle)
{
    // int w, h;
    //  SDL_DisplayMode fullscreen_mode;
    SDL_SetMainReady();

    //You have to call this in order to initialize the SDL pointer before anyhing
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
        BroLogError("SDL could not be initialized: ", SDL_GetError());
        exit(-1);
    }

    checkVideoCard();

    // Hard code
    int minGLVersion;
    int minGLSubversion;
    const int c_iMax_Profs = 2;
    GLProfile profs[c_iMax_Profs];
    int iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
    bool bVsync = false;

#ifdef BRO_OS_IPHONE
    minGLVersion = 3;
    minGLSubversion = 0;
    iProfile = SDL_GL_CONTEXT_PROFILE_ES;
#elif BRO_OS_WINDOWS
    minGLVersion = 3;
    minGLSubversion = 3;
    iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
#endif

    profs[0].make(32, minGLVersion, minGLSubversion, iProfile, bVsync);
    profs[1].make(24, minGLVersion, minGLSubversion, iProfile, bVsync);

    for (int iProf = 0; iProf < c_iMax_Profs; ++iProf) {
        //This must be called before creating the window because this sets SDL's PixelFormatDescritpro
        setWindowAndOpenGLFlags(profs[iProf]);
        makeWindow(windowTitle);
        makeGLContext();

        int ver, subver, shad_ver, shad_subver;
        getOpenGLVersion(ver, subver, shad_ver, shad_subver);

        //Make sure we have a good depth value.
        int iDepth = 0;
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &iDepth);
        if (iDepth >= 24 && ver >= minGLVersion && ver >= minGLSubversion) {
            break;
        }
        else {
            BroLogInfo("OpenGL config profile ", iProf, " didn't work. Trying next profile.");
            SDL_DestroyWindow(_pWindow);
            _pWindow = nullptr;
        }
    }

    //Run a secondary check to make sure we didn't **F up 
    checkForOpenGlMinimumVersion(minGLVersion, minGLSubversion);

    loadCheckProc();
    OglErr::checkSDLErr();

    makeAudio();
    OglErr::checkSDLErr();

    loadCheckProc();
    OglErr::checkSDLErr();

}
void AppRunner::checkVideoCard(){
    int i;

    // Declare display mode structure to be filled in.
    SDL_DisplayMode current;

    SDL_Init(SDL_INIT_VIDEO);
    int nDisplays = SDL_GetNumVideoDisplays();
    BroLogInfo(nDisplays, " Displays:");


    // Get current display mode of all displays.
    for (i = 0; i < SDL_GetNumVideoDisplays(); ++i) {

        int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);

        if (should_be_zero != 0){
            // In case of error...
            BroLogInfo("  Could not get display mode for video display #%d: %s", i);
            OglErr::checkSDLErr();
        }
        else{
            // On success, print the current display mode.
            BroLogInfo("  Display ", i, ": ", current.w, "x", current.h, ", ",current.refresh_rate,"hz");
            OglErr::checkSDLErr();

        }
    }

}

void AppRunner::makeWindow(char* windowTitle)
{
  //  SDL_GL_LoadLibrary("C:/Windows/WinSxS/amd64_microsoft-windows-opengl_31bf3856ad364e35_10.0.10586.0_none_9b4a8e6b33bee072/opengl32.dll");

    char* sstitle;

    bool bFullscreen = false;

    int x, y, w, h, flags;
#ifdef BRO_OS_IPHONE
    x = 0, y = 0, w = 320, h = 480, flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
    sstitle = "";
#elif BRO_OS_WINDOWS
    if (bFullscreen) {
        x = 0; y = 0;
        w = 1920; h = 1080;
        flags = SDL_WINDOW_OPENGL;
    }
    else {
        x = 100, y = 100, w = 800, h = 600, flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    }
    sstitle = windowTitle;
#endif

    //No0te: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
    _pWindow = SDL_CreateWindow(sstitle, x, y, w, h, flags);
    OglErr::checkSDLErr();

    //For the WPF app container we need to set the window handle to be the top window
    //https://docs.microsoft.com/en-us/dotnet/api/system.diagnostics.process.mainwindowhandle?view=netframework-4.8
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_pWindow, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    SetActiveWindow(hwnd);
    
    //Fullscreen nonsense
    if (bFullscreen) {
        SDL_SetWindowFullscreen(_pWindow, SDL_WINDOW_FULLSCREEN);
    }
    OglErr::checkSDLErr();

#ifdef BRO_OS_WINDOWS
    BroLogError("We are not making the window icon because there's an error somewhere in SDL here.");
    //**There is an error here
  //  Gu::SDLTrySetWindowIcon(_pWindow, "./data-dc/tex/tx64-icon.png");//_pRoomBase->getIconFullPath());
#endif

}
void AppRunner::setWindowAndOpenGLFlags(GLProfile& prof){
    //Attribs
    SDL_GL_ResetAttributes();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, _pGlState->gl_accum_red_size);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, _pGlState->gl_accum_green_size);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, _pGlState->gl_accum_blue_size);
    //SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, _pGlState->gl_accum_alpha_size);
    //SDL_GL_SetAttribute(SDL_GL_STEREO, _pGlState->gl_stereo);


    //SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);//deprecated

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, prof._iMinVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, prof._iMinSubVersion);
    SDL_GL_SetSwapInterval(prof._bVsync ? 1 : 0);  //Vsync is automatic on IOS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, prof._iProfile);

    //only on GL 3.0 - disables all deprecates tudff - slight performance gain?
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    #ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, prof._iDepthBits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);


   // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, _pGlState->gl_multisamplebuffers);
   // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _pGlState->gl_multisamplesamples);
}
void AppRunner::makeGLContext() {
    _context = SDL_GL_CreateContext(_pWindow);
    if (!_context) {
        BroLogError("SDL_GL_CreateContext() error", SDL_GetError());
        quit(2);
    }



}
void AppRunner::getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver) {
    char* tmp;
    t_string glver;
    t_string glslver;
    ver = subver = shad_ver = shad_subver = -1;

    tmp = (char*)glGetString(GL_VERSION);
    if (tmp != nullptr) {
        glver = tmp;
    }
    else {
        glver = "";
    }
    tmp = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (tmp != nullptr) {
        glslver = tmp;
    }
    else {
        glslver = "";
    }

    //  t_string glver;
    ///  t_string glslver;
    // // glver = glslver = TStr(iMajor,".",iMinor);

    std::vector<t_string> sv;

    if (glver.length() > 0) {
        sv = StringUtil::split(glver, '.');
        if (sv.size() < 2) {
            BroThrowException("Failed to get OpenGL version. Got '", glver, "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
        }
        ver = TypeConv::strToInt(sv[0]);
        subver = TypeConv::strToInt(sv[1]);
    }
    else {
        BroLogError("Failed to get OpenGL version.");
    }
    if (glslver.length() > 0) {
        sv = StringUtil::split(glslver, '.');
        if (sv.size() < 2) {
            BroThrowException("Failed to get OpenGL Shade version. Got '", glslver, "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
        }
        shad_ver = TypeConv::strToInt(sv[0]);
        shad_subver = TypeConv::strToInt(sv[1]);
    }
    else {
        BroLogError("Failed to get OpenGL Shade version.");
    }
}
void AppRunner::checkForOpenGlMinimumVersion(int required_version, int required_subversion)
{
    t_string rver = TStr("", required_version, ".", required_subversion);

    //GLint iMajor, iMinor;
    //glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
    //glGetIntegerv(GL_MINOR_VERSION, &iMinor);
    //After 3.0 we no longer support glGetString
    int ver, subver, shad_ver, shad_subver;
    getOpenGLVersion(ver, subver, shad_ver, shad_subver);

    t_string vendor = t_string((char*)glGetString(GL_VENDOR));
    t_string renderer = t_string((char*)glGetString(GL_RENDERER));

    BroLogInfo("\n"
        , "   OpenGL version ", ver, ".", subver, ".\n"
        , "   GLSL version:          ", shad_ver, ".", shad_subver, "\n"
        , "   Graphics unit:         ", renderer, "\n"
        , "   Graphics unit Vendor:  ", vendor, "\n"
    );

    if (ver < required_version || (ver >= required_subversion && subver < required_subversion)) {
        BroThrowException("\n"
            , "The game could not find the latest version of OpenGL Shading Language.\n\n"
            , "Possible Problems could be:\n\n"
            , "   1) The primary graphics card is incorrect,\n\n"
            , "   2) The graphics driver is out of date,\n\n"
            , "   3) Your Graphics card is old.\n\n"
            , " This application requires OpenGL version ", rver, ".\n"
            , " The system has detected OpenGL version ", ver, ".", subver, ".\n"
            , " Update your graphics driver by going to www.nvidia.com, www.ati.com or www.intel.com.\n\n"

        );
    }
 

}

SDL_bool AppRunner::makeAudio() {
    //AUDIO
    if (SDL_AudioInit(NULL) < 0)
    {
        fprintf(stderr, "Couldn't initialize audio driver: %s\n", SDL_GetError());
        return SDL_FALSE;
    }

    return SDL_TRUE;
}

void AppRunner::printHelpfulDebug() {
    int dw, dh;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    BroLogInfo("Screen BPP    : ", SDL_BITSPERPIXEL(mode.format));
    BroLogInfo("Swap Interval : ", SDL_GL_GetSwapInterval());
    SDL_GetWindowSize(_pWindow, &dw, &dh);
    BroLogInfo("Initial Window Size   : ", dw, "x", dh);
    SDL_GL_GetDrawableSize(_pWindow, &dw, &dh);
    BroLogInfo("Draw Size     : ", dw, "x", dh);
    
    int tmp=0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &tmp);
    BroLogInfo("SDL_GL_DOUBLEBUFFER: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &tmp);
    BroLogInfo("SDL_GL_BUFFER_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);

    BroLogInfo("SDL_GL_DEPTH_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &tmp);
    BroLogInfo("SDL_GL_STENCIL_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &tmp);
    BroLogInfo("SDL_GL_ACCELERATED_VISUAL: ", tmp);

    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &tmp);
    BroLogInfo("SDL_GL_RED_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &tmp);
    BroLogInfo("SDL_GL_GREEN_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &tmp);
    BroLogInfo("SDL_GL_BLUE_SIZE: ", tmp);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &tmp);
    BroLogInfo("SDL_GL_ALPHA_SIZE: ", tmp);

    OglErr::checkSDLErr();
}

void AppRunner::runApp(std::vector<t_string>& args, char* windowTitle, std::shared_ptr<RoomBase> rb) {
    _tvInitStartTime = Gu::getMicroSeconds();

    //**Must come first.
    Gu::initGlobals(rb, args);
    {
        FileSystem::setExecutablePath(args[0]);

        initSDL(windowTitle);
        
        //Must set this to the depth size we support.
        int tmp=24;
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);
        Gu::setSupportedDepthSize(tmp);

        printHelpfulDebug();

        BroLogInfo("Creating GL Context");
        Gu::createGLContext(rb);


        if(runCommands(args) == false) {
            // - Game Loop
            //*Init room
            //*Setup everything
            //Most processing
            runGameLoopTryCatch(rb);
        }
    }
    Gu::deleteGlobals();
}
void SignalHandler(int signal)
{
    BroThrowException("VC Access Violation. signal=", signal, "  This shouldn't work in release build.");
}
void AppRunner::runGameLoopTryCatch(std::shared_ptr<RoomBase> rb){
    typedef void(*SignalHandlerPointer)(int);
    
    //https://stackoverflow.com/questions/457577/catching-access-violation-exceptions
    SignalHandlerPointer previousHandler;
    previousHandler = signal(SIGSEGV, SignalHandler);

    BroLogInfo("Entering Game Loop");
    try
    {
        runGameLoop(rb);
    }
    catch (Exception* e)
    {
        doShowError("Runtime Error", e);
    }
    catch (...)
    {
        doShowError("Runtime Error, Unhandled exception.", nullptr);
    }
}
bool AppRunner::handleEvents(SDL_Event * event)
{
    static SDL_MouseMotionEvent lastEvent;
    int n = 0;
    vec2 delta;
    SDL_Scancode keyCode;
    std::shared_ptr<Fingers> pFingers = Gu::getContext()->getFingers();

    if (event == nullptr)
        return true;

    switch (event->type) {
    case SDL_MOUSEMOTION:
        //Don't use this.  Because of "SDL_warpMouse" which will mess stuff up.
        break;
    case SDL_KEYDOWN:
        keyCode = event->key.keysym.scancode;
        pFingers->setKeyDown(keyCode);
        break;
    case SDL_KEYUP:
        keyCode = event->key.keysym.scancode;
        pFingers->setKeyUp(keyCode);
        break;
    case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
            case SDL_BUTTON_LEFT:
                pFingers->setLmbState(ButtonState::Press);
                break;
            case SDL_BUTTON_MIDDLE:
                break;
            case SDL_BUTTON_RIGHT:
                pFingers->setRmbState(ButtonState::Press);
                break;
            }
        break;
    case SDL_MOUSEBUTTONUP:
        switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            pFingers->setLmbState(ButtonState::Release);
            break;
        case SDL_BUTTON_MIDDLE:
            break;
        case SDL_BUTTON_RIGHT:
            pFingers->setRmbState(ButtonState::Release);
            break;
        }
        break;
    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            return false;
            break;
        }
        break;
    case SDL_MOUSEWHEEL:
        if (event->wheel.y != 0) {

            int n = MathUtils::broMin(10, MathUtils::broMax(-10, event->wheel.y));

            _pEngine->userZoom(n);
            n++;
        }
        if (event->wheel.x != 0) {
            n++;
        }
        break;
    case SDL_QUIT:
        return false;
        break;
    }

    return true;
}

void AppRunner::runGameLoop(std::shared_ptr<RoomBase> rb)
{
    SDL_Event event;
    bool done = false;
    int w, h;



#ifdef __WINDOWS__
    SDL_ShowCursor(SDL_DISABLE);
#endif


    SDL_GL_GetDrawableSize(_pWindow, &w, &h);
    
    _pEngine = new Engine(Gu::getContext(), rb, _pWindow, w, h);
    _pEngine->init();

    SDL_GL_MakeCurrent(_pWindow, _context);

    //Print the setup time.
    t_string str = "";
    str = TStr(str, "\r\n======================================================\r\n");
    str = TStr(str, "  **Total initialization time: ", MathUtils::round((float)((Gu::getMicroSeconds() - _tvInitStartTime) / 1000) / 1000.0f, 2), " seconds\r\n");
    str = TStr(str, "======================================================\r\n");
    BroLogInfo(str);


    std::shared_ptr<Fingers> pFingers = Gu::getContext()->getFingers();

    while (done == false)
    {
        Gu::beginPerf();
        Gu::pushPerf();
        if(false){
            //AV Exception test **doesn't work in anything but debug build.
            *(int *)0 = 0;
        }
        _pEngine->updateDelta();


        while (SDL_PollEvent(&event))
        {
            if (handleEvents(&event) == false)
            {
                done = true;
            }
        }
        pFingers->preUpdate();
        
        doAnnoyingSoundTest();

        _pEngine->updateTouch(pFingers);

        //SDL_RenderClear(rend);
        SDL_GL_GetDrawableSize(_pWindow, &w, &h);
        
        _pEngine->step(w, h);

        SDL_GL_SwapWindow(_pWindow);

        //Update all button states.
        pFingers->postUpdate();
        Gu::popPerf();
        Gu::endPerf();
        DebugHelper::checkMemory();



        //**End of loop error -- Don't Remove** 
        Gu::getContext()->chkErrRt();
        //**End of loop error -- Don't Remove** 
    }

    _pEngine->cleanup();
    DebugHelper::checkMemory();

    //quit(0);
}
void AppRunner::quit(int rc)
{
    if (_context)
    {
        /* SDL_GL_MakeCurrent(0, NULL); *//* doesn't do anything */
        SDL_GL_DeleteContext(_context);
    }

    if (_pRenderer != nullptr)
    {
        SDL_DestroyRenderer(_pRenderer);
        _pRenderer = NULL;
    }
    SDL_DestroyWindow(_pWindow);

    SDL_Quit();

    // delete _pGlState;

    exit(rc);
}

void AppRunner::doAnnoyingSoundTest(){
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // TEST
    //no
  //  static bool bSoundTestSpace = false;
  //  if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
  //      if (bSoundTestSpace == false) {
  //          bSoundTestSpace = true;
  //          std::shared_ptr<SoundSpec> ss = _pEngine->getContext()->getSoundCache()->getOrLoad("./data-dc/snd/ogg-test.ogg");
  //          ss->play();
  //      }
  //  }
  //  else {
  //      bSoundTestSpace = false;
  //  }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
}
bool AppRunner::argMatch(std::vector<t_string>& args, t_string arg1,  int32_t iCount) {
    if(args.size() <= 1){
        return false;
    }
    if (StringUtil::lowercase(arg1) == StringUtil::lowercase(args[1])) {
        
        return true;
    }
    return false;
}
bool AppRunner::runCommands(std::vector<t_string>& args) {
    if (argMatch(args, "/c", 4)) {
        //Convert Mob
        t_string strMob = args[2];
        t_string strFriendlyName = args[3];
        Gu::getContext()->getModelCache()->convertMobToBin(strMob, false, strFriendlyName);
        return true;
    }
    else if (argMatch(args, "/s", 3)) {
        //Compile Shader
        // cw.exe /s "ShaderName"  "dir/ShaderFileDesc.dat"

        return true;
    }
    else{
        return false;
    }

}



}
