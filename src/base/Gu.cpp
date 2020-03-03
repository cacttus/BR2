#include "../base/Logger.h"
#include "../base/SDLIncludes.h"
#include "../base/SDLGLIncludes.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/FileSystem.h"
#include "../base/oglErr.h"
#include "../base/EngineConfigFile.h"
#include "../base/EngineConfig.h"
#include "../base/DiskFile.h"
#include "../base/OperatingSystem.h"
#include "../base/DebugHelper.h"
#include "../base/BinaryFile.h"
#include "../base/GraphicsWindow.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/InputManager.h"
#include "../base/ApplicationPackage.h"
#include "../base/Logger.h"
#include "../base/Sequencer.h"
#include "../base/SoundCache.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../base/Net.h"
#include "../base/ProjectFile.h"
#include "../math/Algorithm.h"
#include "../gfx/TexCache.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/UiControls.h"   
#include "../gfx/ParticleManager.h"   
#include "../gfx/ShaderMaker.h"
#include "../gfx/OpenGLApi.h"
#include "../base/GLContext.h"
#include "../model/ModelCache.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"
#include "../ext/lodepng.h" 
#include <chrono>
#include <thread>
#include <iostream>


extern "C" {
  //nothings commented on Apr 12, 2016
  //It's not meant to be #included. Don't #include it, just compile & link it.
  extern int stb_vorbis_decode_filename(const char* filename, int* channels, int* sample_rate, short** output);
}

//don't put this in a header file
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
//#include <objc/objc-runtime.h>
#include <foundation/foundation.h>
//#include <corefoundation/CFBundle.h>
#endif
#endif

namespace BR2 {
std::shared_ptr<TexCache> Gu::_pTexCache = nullptr;
std::shared_ptr<Sequencer> Gu::_pSequencer = nullptr;
std::shared_ptr<InputManager> Gu::_pInputManager = nullptr;
std::shared_ptr<SoundCache> Gu::_pSoundCache = nullptr;
std::shared_ptr<ShaderMaker> Gu::_pShaderMaker = nullptr;
std::shared_ptr<ModelCache> Gu::_pModelCache = nullptr;
std::shared_ptr<ApplicationPackage> Gu::_pPackage = nullptr;
std::shared_ptr<RenderSettings> Gu::_pRenderSettings = nullptr;
std::shared_ptr<GraphicsApi> Gu::_pGraphicsApi = nullptr;
std::shared_ptr<Logger> Gu::_pLogger = nullptr;
std::shared_ptr<EngineConfig> Gu::_pEngineConfig = nullptr;
std::shared_ptr<Net> Gu::_pNet = nullptr;

std::shared_ptr<RenderSettings> Gu::getRenderSettings() { return _pRenderSettings; }
std::shared_ptr<ApplicationPackage> Gu::getPackage() { return _pPackage; }
std::shared_ptr<ModelCache> Gu::getModelCache() { return _pModelCache; }
std::shared_ptr<Sequencer> Gu::getSequencer() { return _pSequencer; }
std::shared_ptr<InputManager> Gu::getInputManager() { return _pInputManager; }
std::shared_ptr<SoundCache> Gu::getSoundCache() { return _pSoundCache; }
std::shared_ptr<TexCache> Gu::getTexCache() { return _pTexCache; }
std::shared_ptr<ShaderMaker> Gu::getShaderMaker() { return _pShaderMaker; }
std::shared_ptr<EngineConfig> Gu::getEngineConfig() { return _pEngineConfig; }
std::shared_ptr<Logger> Gu::getLogger() { return _pLogger; }
std::shared_ptr<GraphicsApi> Gu::getGraphicsApi() { return _pGraphicsApi; }
std::shared_ptr<EngineConfig> Gu::getConfig() { return _pEngineConfig; }
std::shared_ptr<Net> Gu::getNet() { return _pNet; }
std::shared_ptr<GLContext> Gu::getCoreContext() {
  std::shared_ptr<GraphicsApi> api = Gu::getGraphicsApi();
  std::shared_ptr<OpenGLApi> oglapi = std::dynamic_pointer_cast<OpenGLApi>(Gu::getGraphicsApi());
  if (!oglapi || oglapi->getCoreContext() == nullptr) {
    BRLogErrorOnce("Tried to retrieve a null Core Context. Application may fail.");
    return nullptr;
  }
  return oglapi->getCoreContext()->getThis<GLContext>();
}
void Gu::setGraphicsApi(std::shared_ptr<GraphicsApi> api) { AssertOrThrow2(api != nullptr); _pGraphicsApi = api; }
void Gu::checkErrorsDbg() { Gu::getCoreContext()->chkErrDbg(); }
void Gu::checkErrorsRt() { Gu::getCoreContext()->chkErrRt(); }
bool Gu::is64Bit() {
  if (sizeof(size_t) == 8) {
    return true;
  }
  else if (sizeof(size_t) == 4) {
    return false;
  }
  else {
    BRThrowNotImplementedException();
  }
}

//**TODO Move to AppRunner
void parsearg(std::string arg, string_t& __out_ out_key, string_t& __out_ out_value) {
  bool isvalue = false;
  std::string key = "";
  std::string value = "";
  //**TODO Move this to AppRunner

  for (int i = 0; i < arg.length(); ++i) {
    if (arg[i] == '=') {
      isvalue = true;
    }
    else if (isvalue) {
      value += arg[i];
    }
    else {
      key += arg[i];
    }
    //**TODO Move to AppRunner
  }

  out_key = StringUtil::trim(key);
  out_value = StringUtil::trim(value);
}
void processArg(std::string key, std::string value) {
  if (key == "--show-console") {
    Gu::getEngineConfig()->setShowConsole(BR2::TypeConv::strToBool(value));
    BRLogInfo("Overriding show console window: " + value);
  }
  else if (key == "--game-host") {
    Gu::getEngineConfig()->setGameHostAttached(BR2::TypeConv::strToBool(value));
    BRLogInfo("Overriding game host: " + value);
  }
  else {
    BRLogWarn("Unrecognized parameter '" + key + "' value ='" + value + "'");
  }
}
bool Gu::checkArg(const std::vector<string_t>& args, string_t inkey, string_t invalue) {
  string_t key, value;
  for (std::string arg : args) {
    parsearg(arg, key, value);
    if (StringUtil::equalsi(key, inkey)) {
      if (StringUtil::equalsi(value, invalue)) {
        return true;
      }
      else {
        return false;
      }
      break;
    }
  }
  return false;
}
void Gu::createLogger(string_t logfile_dir, const std::vector<string_t>& args) {
  //These are essentially the system defaults
  bool log_async = true;
  bool disabled = false;
  if (Gu::checkArg(args, "log_async", "true")) {
    log_async = true;
  }
  else if (Gu::checkArg(args, "log_async", "false")) {
    log_async = false;
  }
  if (Gu::checkArg(args, "log_disable", "true")) {
    disabled = true;
  }
  else if (Gu::checkArg(args, "log_disable", "false")) {
    disabled = false;
  }
  Gu::_pLogger = std::make_shared<Logger>(log_async, disabled);
  Gu::_pLogger->init(logfile_dir);
}
void Gu::initGlobals(const std::vector<std::string>& args) {
  //Config
  loadConfig(args);

  //Override EngineConfig
  for (std::string arg : args) {
    //TODO: skip arg 0 (app)
    string_t key, value;
    parsearg(arg, key, value);
    processArg(key, value);
  }

  //Setup Global Configuration
  getLogger()->enableLogToFile(Gu::getEngineConfig()->getEnableLogToFile());
  getLogger()->enableLogToConsole(Gu::getEngineConfig()->getEnableLogToConsole());

  //Print some environment Diagnostics
  BRLogInfo(Stz  "Operating System: " + Gu::getOperatingSystemName());
  BRLogInfo(Stz  "C++ Version: " + Gu::getCPPVersion());

  if (Gu::getEngineConfig()->getShowConsole() == false) {
    OperatingSystem::hideConsole();
  }
  else {
    OperatingSystem::showConsole();
  }
}
void Gu::loadConfig(const std::vector<std::string>& args) {
  string_t configPath = ApplicationPackage::getEngineConfigFilePath();
  BRLogInfo("Loading config from '" + configPath + "'");
  if (!FileSystem::fileExists(configPath)) {
    BRThrowException("Engine configuration file '" + configPath + "' does not exist.");
  }
  else {
    EngineConfigFile ef;
    ef.loadAndParse(configPath);
    Gu::_pEngineConfig = ef.getConfig();
  }

  //Override the EngineConfig
  for (std::string arg : args) {
    string_t key, value;
    parsearg(arg, key, value);
    processArg(key, value);
  }

}
//void Gu::setContext(std::shared_ptr<GraphicsContext> rb) {
//    Gu::_pContext = rb;
//}
void Gu::deleteManagers() {
  //This must be called in order to delete these in order.
  //Teh GL context needs to come at the end.
  _pTexCache = nullptr;
  _pSequencer = nullptr;

  _pSoundCache = nullptr;
  _pShaderMaker = nullptr;
  _pModelCache = nullptr;
  _pRenderSettings = nullptr;
  _pEngineConfig = nullptr;

  //System Level
  _pInputManager = nullptr;
  _pPackage = nullptr;
  _pLogger = nullptr;
  _pNet = nullptr;

  _pGraphicsApi = nullptr;
}

bool Gu::isBigEndian() {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  return true;
#else
  return false;
#endif
}
std::shared_ptr<Img32> Gu::loadImage(std::string imgLoc) {
  unsigned char* image = nullptr; //the raw pixels
  unsigned int width, height;
  int err;
  //char* imgData = 0;
 // uint32_t imgSize;
  std::shared_ptr<Img32> ret = nullptr;

  std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>();
  if (Gu::getPackage()->getFile(imgLoc, fb)) {
    //decode
    err = lodepng_decode32(&image, &width, &height, (unsigned char*)fb->getData().ptr(), fb->getData().count());
    if (err != 0) {
      //FB should free itself.
    //  Gu::SDLFileFree(imgData);
      BRThrowException(Stz "Could not load image " + imgLoc + " err code = " + err);
    }
    else {
      Img32::flipImage20161206(image, width, height);
      // create(image, width, height, false);

      ret = std::make_shared<Img32>(width, height, (uint8_t*)image);

      //delete image;
    }
    //FileBuffer will free itself
    //Gu::SDLFileFree(imgData);
  }

  return ret;
}
bool Gu::saveImage(std::string path, std::shared_ptr<Img32> spec) {
  //unsigned char* image = nullptr; //the raw pixels
  //unsigned int width, height;
  //int err;
  //char* imgData = 0;
  //uint32_t imgSize;
  //err = Gu::SDLFileRead(imgLoc.c_str(), imgData, imgSize);
  bool bRet = true;
  unsigned char* imageData = (unsigned char*)spec->getData()->ptr();
  unsigned int w = spec->getWidth();
  unsigned int h = spec->getHeight();
  unsigned char* buffer = 0;
  size_t buffersize = 0;
  //For some reason we have to call this twice, i'm not sure.  Maybe LODEPNG wants buffer to be allocated?
 // lodepng_encode_memory(&buffer, &buffersize, imageData, w, h, LCT_RGBA, 8);
  unsigned error = lodepng_encode_memory(&buffer, &buffersize, imageData, w, h, LCT_RGBA, 8);
  if (error == 0) {
    Allocator<char> allocr(buffersize, (char*)buffer);

    RetCode rc = DiskFile::writeAllBytes(path, allocr);
    if (rc != GR_OK) {
      BRLogError("Error'" + (int)rc + "' occurred while saving image.");
    }
    allocr.dealloc();
  }
  else {
    BRLogError("LodePng - Error encoding image '" + path + "'.");
    bRet = false;
  }
  free(buffer);//lodepng_free

  Gu::checkErrorsDbg();

  return bRet;
}
void Gu::freeImage(std::shared_ptr<Img32> b) {
  //b->dea
  //delete b;
}
t_timeval Gu::getMicroSeconds() {
  int64_t ret;
  std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
  ret = std::chrono::duration_cast<std::chrono::microseconds>(ns).count();
  return ret;
}
t_timeval Gu::getMilliSeconds() {
  return getMicroSeconds() / 1000;
}
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
static CFStringRef stdStrToCFStr(std::string st) {
  CFStringRef str;
  //kCFStringEncodingUTF8
  str = CFStringCreateWithCString(NULL, st.c_str(), CFStringGetSystemEncoding());
  return str;
}
#endif
#endif

void Gu::debugBreak() {
#ifdef _WIN32
#ifdef _DEBUG
  DebugBreak();
#endif
#endif
}

int Gu::loadSound(std::string path, int& __out_ iChannels, int& __out_ iSampleRate,
  int16_t*& __out_ pData, int& __out_ nSamples, int& __out_ iDataLenBytes) {
  //  it returns the number of
  // samples decoded, and you told it the number of channels and the sample rate.Multiply 
  // those three together, and you get the total number of shorts in the buffer;
  // multiply by sizeof(short), and voilá; size of that buffer in bytes.
  nSamples = stb_vorbis_decode_filename(path.c_str(), &iChannels, &iSampleRate, &pData);

  if (nSamples <= 0) {
    return nSamples;
  }

  iDataLenBytes = nSamples * iChannels * sizeof(int16_t);

  return 1;
}

//
//float Gu::frand() {
//    return mint(
//    return ((float)(rand() % 1000)*0.001f);
//}
//float Gu::frand1() {
//    return ((float)(-1000 + (rand() % 2000))*0.001f);
//}
//vec3 Gu::rv3() {
//    return vec3(Gu::frand1(), Gu::frand1(), Gu::frand1());
//}

void Gu::inlineDrawBoxCont(const Box3f* b) {
#ifdef _WIN32
  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_min.z);

  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_min.z);

  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_min.x, b->_max.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_max.x, b->_max.y, b->_min.z);


  glVertex3f(b->_max.x, b->_max.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_min.z);
  glVertex3f(b->_max.x, b->_max.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_min.z);

  glVertex3f(b->_min.x, b->_max.y, b->_max.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);


  glVertex3f(b->_min.x, b->_max.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_max.z);


  glVertex3f(b->_max.x, b->_min.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);
#endif
}


float Gu::fade(float t) {
  //T = [0,1]
  // Fade function as defined by Ken Perlin.  This eases coordinate values
  // so that they will ease towards integral values.  This ends up smoothing
  // the final output.
  return 6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3);
  //  return 6*(t*t*t*t*t) - 15*(t*t*t*t) + 10*(t*t*t);         // 6t^5 - 15t^4 + 10t^3

}

void Gu::checkMemory() {
#ifdef _WIN32
#ifdef _DEBUG
  _CrtCheckMemory();
#endif
#endif
}

string_t Gu::getOperatingSystemName() {
  string_t res;
#ifdef BR2_OS_WINDOWS
  OSVERSIONINFOEX vex;
  vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO*)&vex);

  // CheckOsErrorsDbg();

  if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 3) {
    res.append(" Windows 8.1");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 2) {
    res.append(" Windows 8");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 1) {
    res.append(" Windows 7");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 0) {
    res.append(" Windows Vista");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 2) {
    res.append(" Windows XP Pro 64 bit");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 1) {
    res.append(" Windows XP");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 0) {
    res.append(" Windows 2000");
  }
  else {
    res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
  }

  if (vex.wServicePackMajor != 0)
    res += Stz ", Service Pack " + vex.wServicePackMajor + "." + vex.wServicePackMinor;
  else
    res += Stz ", No service pack";

  if (vex.wProductType == VER_NT_DOMAIN_CONTROLLER)
    res.append(", Domain Controller, note operating system may be incorrect as this is not supported");
  else if (vex.wProductType == VER_NT_SERVER)
    res.append(", Server, note operating system may be incorrect as this is not supported");
  else if (vex.wProductType == VER_NT_WORKSTATION)
    res.append(", Workstation");

#else
  //#error "Operating System Error"

#endif
    //CheckOsErrorsDbg();
  return res;
}

uint32_t Gu::getCurrentThreadId() {
  uint32_t threadId = 0;
#ifdef BR2_OS_WINDOWS
  //TODO: std::this_thread::get_id()
  threadId = (uint32_t)GetCurrentThreadId();
#else
  //#error "Operating System Error"
  return std::hash<std::thread::id>()(std::this_thread::get_id());
#endif
  return threadId;

}
bool Gu::isDebug() {
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}
std::vector<string_t> Gu::argsToVectorOfString(int argc, char** argv, char delimiter) {
  int squot = 0, dquot = 0;

  //todo - fix the delimiter thing
  std::vector<string_t> ret;
  for (int i = 0; i < argc; ++i) {

    string_t str(argv[i]);


    ret.push_back(str);
  }
  return ret;
}

void Gu::guiQuad2d(Box2f& pq, std::shared_ptr<RenderViewport> vp) {
  //Transforms a quad for the matrix-less Gui projection.

  //The resulting coordinates for the GPU are -0.5 +0.5 in both axes with the center being in the center of the screen
  //Translate a 2D screen quad to be rendered in a shader.
  //So* our quad is from TOP Left - OpenGL is Bottom Left - this fixes this.
  float w = (float)vp->getWidth();
  float w2 = w * 0.5f;
  float h = (float)vp->getHeight();
  float h2 = h * 0.5f;

  //Subtract from viewport center
  pq._p0.x -= w2;
  pq._p1.x -= w2;

  //Invert text to show rightsize up and divide by perspective
  pq._p0.x = pq._p0.x / w2;
  pq._p0.y = (h2 - pq._p0.y - 1) / h2;
  pq._p1.x = pq._p1.x / w2;
  pq._p1.y = (h2 - pq._p1.y - 1) / h2;
}
void Gu::print(char msg) {
  char c[2];
  c[0] = msg;
  c[1] = 0;
  print((char*)c);
}
void Gu::print(const string_t& msg) {
  print(msg.c_str());
}
void Gu::print(const char* msg) {
  if (Gu::getEngineConfig() == nullptr) {
    std::cout << msg;
  }
  else if (Gu::getEngineConfig()->getShowConsole()) {
    std::cout << msg;
  }
}

std::string Gu::getCPPVersion() {
  //https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler

  if (__cplusplus == 201703L) {
    return Stz "C++17";
  }
  else if (__cplusplus == 201403L) {
    return Stz "C++14";
  }
  else if (__cplusplus == 201103L) {
    return Stz "C++11";
  }
  else if (__cplusplus == 199711L) {
    return Stz "C++98";
  }
  return Stz "pre-standard C++";
}

void Gu::createManagers() {
  std::shared_ptr<GLContext> ct = std::dynamic_pointer_cast<GLContext>(Gu::getCoreContext());
  _pRenderSettings = RenderSettings::create();

  BRLogInfo("GLContext - Building ApplicationPackage");
  _pPackage = std::make_shared<ApplicationPackage>();
  _pPackage->build(FileSystem::getExecutableFullPath());

  BRLogInfo("GLContext - Creating TexCache");
  _pTexCache = std::make_shared<TexCache>(Gu::getCoreContext());
  BRLogInfo("GLContext - Creating Sequencer");
  _pSequencer = std::make_shared<Sequencer>();
  BRLogInfo("GLContext - Creating Fingers");
  _pInputManager = std::make_shared<InputManager>();
  _pInputManager->init();
  BRLogInfo("GLContext - Creating SoundCache");
  _pSoundCache = std::make_shared<SoundCache>();
  BRLogInfo("GLContext - Creating ShaderMaker & base shaders");
  _pShaderMaker = std::make_shared<ShaderMaker>();
  _pShaderMaker->initialize();
  BRLogInfo("GLContext - Model Cache");
  _pModelCache = std::make_shared<ModelCache>(Gu::getCoreContext());
  BRLogInfo("Network");
  _pNet = std::make_shared<Net>();
}
void Gu::updateManagers() {

  if (_pSequencer != nullptr) {
    _pSequencer->update();
  }

  if (_pSoundCache != nullptr) {
    _pSoundCache->update();
  }
  if (_pNet != nullptr) {
    _pNet->update();
  }

}
void Gu::sleepThread(uint64_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

}//ns Game
