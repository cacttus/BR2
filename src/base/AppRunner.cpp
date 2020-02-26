#include "../base/AppRunner.h"
#include "../base/SoundCache.h"
#include "../base/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Logger.h"
#include "../base/SDLIncludes.h"
#include "../base/GraphicsWindow.h"
#include "../base/SDLUtils.h"
#include "../base/Gu.h"
#include "../base/oglErr.h"
#include "../base/OperatingSystem.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/AppBase.h"
#include "../math/MathAll.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/OpenGLApi.h"
#include "../gfx/VulkanApi.h"
#include "../model/ModelCache.h"

#include <signal.h>
#include <chrono>
#include <thread>

namespace BR2 {
//////////////////////////////////////////////////////////////////////////
void SignalHandler(int signal) {
  BRThrowException(Stz "VC Access Violation. signal=" + signal + "  This shouldn't work in release build.");
}
//////////////////////////////////////////////////////////////////////////
class AppRunner_Internal {
public:
  SDL_AudioSpec _audioSpec;
  t_timeval _tvInitStartTime = 0;
  // accept a connection coming in on server_tcpsock
  TCPsocket _gameHostSocket;//Send and receive data
  TCPsocket _server_tcpsock;//Accept connections
  std::shared_ptr<GraphicsApi> _pGraphicsApi = nullptr;

  void initSDLAndCreateGraphicsApi(string_t windowTitle, std::shared_ptr<AppBase> app);
  void doShowError(string_t err, Exception* e);
  void attachToGameHost();
  void printVideoDiagnostics();
  void updateWindowHandleForGamehost();
  SDL_bool initAudio();
  void initNet();
  void runGameLoopTryCatch();
  void exitApp(string_t error, int rc);
  bool argMatch(const std::vector<string_t>& args, string_t arg1, int32_t iCount);
  bool runCommands(const std::vector<string_t>& args);

};

void AppRunner_Internal::initSDLAndCreateGraphicsApi(string_t windowTitle, std::shared_ptr<AppBase> app) {
  //Nix main()
  SDL_SetMainReady();

  //You have to call this in order to initialize the SDL pointer before anyhing
  //Not sur if SDL_init is really needed..
  //if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
  //    exitApp(Stz "SDL could not be initialized: " + SDL_GetError(), -1);
  //}

  printVideoDiagnostics();

  //Create graphics API
  if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::OpenGL) {
    _pGraphicsApi = std::make_shared<OpenGLApi>();
  }
  else if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::Vulkan) {
    _pGraphicsApi = std::make_shared<VulkanApi>();
  }
  else {
    BRThrowException("Invalid render engine.");
  }
  Gu::setGraphicsApi(_pGraphicsApi);

  //TRY MOVING DOWN
  std::shared_ptr<GraphicsWindow> window = _pGraphicsApi->createWindow(windowTitle);
  SDLUtils::checkSDLErr();

  initAudio();
  SDLUtils::checkSDLErr();

  initNet();
  SDLUtils::checkSDLErr();

  if (Gu::getEngineConfig()->getGameHostAttached()) {
    updateWindowHandleForGamehost();
    attachToGameHost();
  }

  //Render System Static Data, which requires a prior context
  BRLogInfo("GraphicsContext - Making Vtx Formats.");
  RenderUtils::makeVertexFormats();

  BRLogInfo("Creating Managers.");
  Gu::createManagers();

  BRLogInfo("Creating Window");
  window->initRenderSystem();
}
void AppRunner_Internal::doShowError(string_t err, Exception* e) {
  if (e != nullptr) {
    OperatingSystem::showErrorDialog(e->what() + err);
  }
  else {
    OperatingSystem::showErrorDialog("No Error Message" + err);
  }
}
void AppRunner_Internal::attachToGameHost() {
  int GameHostPort = Gu::getEngineConfig()->getGameHostPort();

  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, NULL, GameHostPort) == -1) {
    exitApp(std::string("") + "SDLNet_ResolveHost: " + SDLNet_GetError(), -1);
  }

  _server_tcpsock = SDLNet_TCP_Open(&ip);
  if (!_server_tcpsock) {
    exitApp(std::string("") + "SDLNet_TCP_Open:" + SDLNet_GetError(), -1);
  }

  //Wait a few, then exit if we don't get a response.
  t_timeval t0 = Gu::getMilliSeconds();

  while (true) {
    int timeout = Gu::getEngineConfig()->getGameHostTimeoutMs();

    if (Gu::getMilliSeconds() - t0 > timeout) {
      exitApp(Stz "Failed to connect to game host, timeout " + timeout + "ms exceeded.", -1);
      break;//Unreachable, but just in case.
    }

    _gameHostSocket = SDLNet_TCP_Accept(_server_tcpsock);
    if (_gameHostSocket) {
      //  char data[512];

        //while(SDLNet_TCP_Recv(_gameHostSocket, data, 512) <= 0) {
        //    //DebugBreak();
        //    int n = 0;
        //    n++;
        //}
        //DebugBreak();

        // communicate over new_tcpsock
      break;
    }
    else {
      Gu::sleepThread(50);
    }
  }

}
void AppRunner_Internal::printVideoDiagnostics() {
  //Init Video
  //SDL_Init(SDL_INIT_VIDEO);

  //Drivers (useless in sdl2)
  const char* driver = SDL_GetCurrentVideoDriver();
  if (driver) {
    BRLogInfo("Default Video Driver: " + driver);
  }
  BRLogInfo("Installed Video Drivers: ");
  int idrivers = SDL_GetNumVideoDrivers();
  for (int idriver = 0; idriver < idrivers; ++idriver) {
    driver = SDL_GetVideoDriver(idriver);
    BRLogInfo(" " + driver);
  }

  // Get current display mode of all displays.
  int nDisplays = SDL_GetNumVideoDisplays();
  BRLogInfo(nDisplays + " Displays:");
  for (int idisplay = 0; idisplay < nDisplays; ++idisplay) {
    SDL_DisplayMode current;
    int should_be_zero = SDL_GetCurrentDisplayMode(idisplay, &current);

    if (should_be_zero != 0) {
      // In case of error...
      BRLogInfo("  Could not get display mode for video display #%d: %s" + idisplay);
      SDLUtils::checkSDLErr();
    }
    else {
      // On success, print the current display mode.
      BRLogInfo("  Display " + idisplay + ": " + current.w + "x" + current.h + ", " + current.refresh_rate + "hz");
      SDLUtils::checkSDLErr();
    }
  }
}
void AppRunner_Internal::updateWindowHandleForGamehost() {
#ifdef _WINDOWS_
  //For the WPF app container we need to set the window handle to be the top window
  //https://docs.microsoft.com/en-us/dotnet/api/system.diagnostics.process.mainwindowhandle?view=netframework-4.8
  //SDL_SysWMinfo wmInfo;
  //SDL_VERSION(&wmInfo.version);
  //SDL_GetWindowWMInfo(Gu::getWindow(), &wmInfo);
  //HWND hwnd = wmInfo.info.win.window;
  //HWND old = GetActiveWindow();
  //SetActiveWindow(hwnd);
#endif
}
SDL_bool AppRunner_Internal::initAudio() {
  //AUDIO
  if (SDL_AudioInit(NULL) < 0) {
    fprintf(stderr, "Couldn't initialize audio driver: %s\n", SDL_GetError());
    return SDL_FALSE;
  }
  return SDL_TRUE;
}
void AppRunner_Internal::initNet() {
  BRLogInfo("Initializing SDL Net");
  if (SDLNet_Init() == -1) {
    exitApp(Stz "SDL Net could not be initialized: " + SDL_GetError(), -1);
  }
}
void AppRunner_Internal::runGameLoopTryCatch() {
  typedef void(*SignalHandlerPointer)(int);

  //Attempt to catch segfaults that doesn't really work.
  //https://stackoverflow.com/questions/457577/catching-access-violation-exceptions
  SignalHandlerPointer previousHandler;
  previousHandler = signal(SIGSEGV, SignalHandler);

  //test the globals before starting the game loop
  Gu::updateGlobals();

  //Print the setup time.
  BRLogInfo(Stz "**Total initialization time: " + MathUtils::round((float)((Gu::getMicroSeconds() - _tvInitStartTime) / 1000) / 1000.0f, 2) + " seconds\r\n");

  BRLogInfo("Entering Game Loop");
  try {
    _pGraphicsApi->updateLoop();
  }
  catch (Exception * e) {
    doShowError("Runtime Error", e);
  }
  catch (...) {
    doShowError("Runtime Error, Unhandled exception.", nullptr);
  }
}

void AppRunner_Internal::exitApp(string_t error, int rc) {
  OperatingSystem::showErrorDialog(error + SDLNet_GetError());

  Gu::debugBreak();

  Gu::getGraphicsApi()->cleanup();

  SDLNet_Quit();
  SDL_Quit();

  exit(rc);
}
bool AppRunner_Internal::argMatch(const std::vector<string_t>& args, string_t arg1, int32_t iCount) {
  if (args.size() <= 1) {
    return false;
  }
  if (StringUtil::lowercase(arg1) == StringUtil::lowercase(args[1])) {

    return true;
  }
  return false;
}
bool AppRunner_Internal::runCommands(const std::vector<string_t>& args) {
  if (argMatch(args, "/c", 4)) {
    //Convert Mob
    string_t strMob = args[2];
    string_t strFriendlyName = args[3];
    Gu::getModelCache()->convertMobToBin(strMob, false, strFriendlyName);
    return true;
  }
  else if (argMatch(args, "/s", 3)) {
    //Compile Shader
    // cw.exe /s "ShaderName"  "dir/ShaderFileDesc.dat"

    return true;
  }
  else {
    return false;
  }
}

//////////////////////////////////////////////////////////////////////////
AppRunner::AppRunner() {
  _pint = std::make_unique<AppRunner_Internal>();
}
AppRunner::~AppRunner() {
  _pint = nullptr;
}
void AppRunner::runApp(const std::vector<string_t>& args, string_t windowTitle, std::shared_ptr<AppBase> app) {
  _pint->_tvInitStartTime = Gu::getMicroSeconds();

  //Root the engine FIRST so we can find the EngineConfig.dat
  FileSystem::setExecutablePath(args[0]);
  string_t a = FileSystem::getCurrentDirectory();
  FileSystem::setCurrentDirectory(FileSystem::getExecutableDirectory());
  string_t b = FileSystem::getCurrentDirectory();

  //**Must come first before other logic
  Gu::initGlobals(app, args);
  {
    _pint->initSDLAndCreateGraphicsApi(windowTitle, app);

    if (_pint->runCommands(args) == false) {
      _pint->runGameLoopTryCatch();
    }
  }
  Gu::deleteGlobals();
}


}//End of ns game
