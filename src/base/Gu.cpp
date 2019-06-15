#include "../base/Gu.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/FileSystem.h"
#include "../base/oglErr.h"
#include "../base/EngineConfigFile.h"
#include "../base/RoomBase.h"
#include "../base/EngineConfig.h"
#include "../base/DiskFile.h"
#include "../base/OperatingSystem.h"
#include "../base/DebugHelper.h"
#include "../base/BufferedFile.h"
#include "../display/Viewport.h"
 
#include "../base/GLContext.h"
#include "../math/Algorithm.h"
#include "../display/CameraNode.h"
#include "../display/ShaderMaker.h"

extern "C" {
    //nothings commented on Apr 12, 2016
    //It's not meant to be #included. Don't #include it, just compile & link it.
    extern int stb_vorbis_decode_filename(const char *filename, int *channels, int *sample_rate, short **output);
}


//don't put this in a header file
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
//#include <objc/objc-runtime.h>
#include <foundation/foundation.h>
//#include <corefoundation/CFBundle.h>
#endif
#endif


namespace Game {
//////////////////////////////////////////////////////////////////////////
std::stack<Stopwatch> Gu::_stopw;
std::stack<GLenum> Gu::_eLastCullFaceStack;
std::stack<GLenum> Gu::_eLastBlendStack;
std::stack<GLenum> Gu::_eLastDepthTestStack;
std::shared_ptr<Logger> Gu::_pLogger = nullptr;
std::shared_ptr<EngineConfig> Gu::_pEngineConfig = nullptr;
std::shared_ptr<GLContext> Gu::_pContext = nullptr;
int Gu::_iSupportedDepthSize = 24;
t_string Gu::_strCachedProf = "";
//////////////////////////////////////////////////////////////////////////

void Gu::checkErrorsDbg() {
    getContext()->chkErrDbg();
}
void Gu::checkErrorsRt() {
    getContext()->chkErrRt();
}
std::shared_ptr<Package> Gu::getPackage() {
    AssertOrThrow2(_pContext != nullptr);
    return _pContext->getPackage();
}
bool Gu::is64Bit() {
    if (sizeof(size_t) == 8)
        return true;
    if (sizeof(size_t) == 4)
        return false;
    //WTF
    BroThrowNotImplementedException();
}
void parsearg(std::string key, std::string value) {
    if (key == "--show-console") {
        Gu::getEngineConfig()->setShowConsole( Game::TypeConv::strToBool(value) );
        BroLogInfo("Overriding show console window: ", value);
    }
    else if (key == "--game-host") {
        Gu::getEngineConfig()->setGameHostAttached(Game::TypeConv::strToBool(value));
        BroLogInfo("Overriding game host: ", value);
    }
    else {
        BroLogWarn("Unrecognized parameter '", key, "' value ='", value, "'");
    }
}
//**TODO Move this crap to AppRunner
void parsearg(std::string arg) {
    bool isvalue = false;
    std::string key = "";
    std::string value = "";
    //**TODO Move this crap to AppRunner
    
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
        //**TODO Move this crap to AppRunner
    }
    parsearg(key, value);
}

void Gu::initGlobals(std::shared_ptr<RoomBase> rb, std::vector<std::string>& args) {
    //Try to create teh cache dir.
    //Make sure to check this on IOS
    FileSystem::createDirectoryRecursive(FileSystem::formatPath(rb->getCacheDir()));

    Gu::_pLogger = std::make_shared<Logger>();
    Gu::_pLogger->init(rb);

    EngineConfigFile ef;
    ef.loadAndParse(rb->getConfigPath());
    Gu::_pEngineConfig = ef.getConfig();

    //Override EngineConfig
    for (std::string arg : args) {
        //TODO: skip arg 0 (app)
        parsearg(arg);
    }

    if (Gu::getEngineConfig()->getEnableLogging() == false) {
        getLogger()->disableLogger();
    }
    if (Gu::getEngineConfig()->getShowConsole() == false) {
        OperatingSystem::hideConsole();
    }
    else {
        OperatingSystem::showConsole();
    }
}
void Gu::createGLContext(std::shared_ptr<RoomBase> rb) {
    Gu::_pContext = std::make_shared<GLContext>();
    if (!_pContext->load(rb)) {
        BroLogError("Failed to load ctx.");
        BroThrowException("Failed to load OpenGL Context. One or more methods were not supported.");
    }

    //Quick GL test.
    _pContext->glUseProgram(0);
}
void Gu::deleteGlobals() {
    // DEL_MEM(_pContext);
      //DEL_MEM(_pLogger);

}
void Gu::SDLTrySetWindowIcon(SDL_Window* w, t_string iconPath) {
    if (iconPath.length()) {
        std::shared_ptr<Img32> img = nullptr;
        SDL_Surface* ss = nullptr;
        Gu::SDLCreateSurfaceFromImage(iconPath, img, ss);
        if (ss != nullptr) {
            SDL_SetWindowIcon(w, ss);
            OglErr::checkSDLErr();
            SDL_FreeSurface(ss);
            OglErr::checkSDLErr();
        }
        if (img != nullptr) {
            //SDL uses the image pointer, so you have to free the image AFTER you free the surface.
            Gu::freeImage(img);
        }
    }
}
void Gu::SDLCreateSurfaceFromImage(const t_string strImage,
    std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface) {
    pImage = nullptr;
    pSurface = nullptr;
    if (FileSystem::fileExists(strImage)) {
        pImage = Gu::loadImage(strImage);
        pSurface = Gu::SDLCreateSurfaceFromImage(pImage);
    }
    else {
        BroLogError("Could not icon image '", strImage, "'");
    }
}
SDL_Surface* Gu::SDLCreateSurfaceFromImage(const std::shared_ptr<Img32> pImage) {
    SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom(
        pImage->getData(), pImage->getWidth(), pImage->getHeight(),
        pImage->getBitsPerPixel(), pImage->getPitch(),
        pImage->getRMask(), pImage->getGMask(), pImage->getBMask(), pImage->getAMask());

    OglErr::checkSDLErr();

    return pSurface;
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

    std::shared_ptr<BufferedFile> fb = std::make_shared<BufferedFile>();
    if (Gu::getPackage()->getFile(imgLoc, fb)) {
        //decode
        err = lodepng_decode32(&image, &width, &height, (unsigned char*)fb->getData().ptr(), fb->getData().count());
        if (err != 0) {
            //FB should free itself.
          //  Gu::SDLFileFree(imgData);
            BroThrowException("Could not load image ", imgLoc, " err code = ", err);
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
            BroLogError("Error'", (int)rc, "' occurred while saving image.");
        }
        allocr.dealloc();
    }
    else {
        BroLogError("LodePng - Error encoding image '", path, "'.");
        bRet = false;
    }
    free(buffer);//lodepng_free

    return bRet;
}
void Gu::freeImage(std::shared_ptr<Img32> b) {
    //b->dea
    //delete b;
}
t_timeval Gu::getMicroSeconds()
{
    int64_t ret;
    std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
    ret = std::chrono::duration_cast<std::chrono::microseconds>(ns).count();
    return ret;
}
t_timeval Gu::getMilliSeconds()
{
    return getMicroSeconds() / 1000;
}
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
static CFStringRef stdStrToCFStr(std::string st)
{
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

void Gu::beginWin32InlineDebug(std::shared_ptr<GLContext> pContext) {
#ifdef _WIN32
#ifdef _DEBUG
    //**INLINE MODE -- REMOVE FOR RELEASE
    Gu::getContext()->getShaderMaker()->shaderBound(nullptr);
    pContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
    pContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    pContext->glBindVertexArray(0);
    pContext->chkErrDbg();

    mat4 mProj = pContext->getCamera()->getProj();
    // mProj.transpose();
    mat4 mView = pContext->getCamera()->getView();
    // mView.transpose();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((GLfloat*)&mProj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((GLfloat*)&mView);
#endif
#endif
}

void Gu::checkMemory() {
#ifdef _WIN32
#ifdef _DEBUG
    _CrtCheckMemory();
#endif
#endif
}





t_string Gu::getOperatingSystemName()
{
    t_string res;
#ifdef BRO_OS_WINDOWS
    OSVERSIONINFOEX vex;
    vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*)&vex);

    // CheckOsErrorsDbg();

    if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 3)
    {
        res.append(" Windows 8.1");
    }
    else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 2)
    {
        res.append(" Windows 8");
    }
    else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 1)
    {
        res.append(" Windows 7");
    }
    else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 0)
    {
        res.append(" Windows Vista");
    }
    else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 2)
    {
        res.append(" Windows XP Pro 64 bit");
    }
    else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 1)
    {
        res.append(" Windows XP");
    }
    else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 0)
    {
        res.append(" Windows 2000");
    }
    else
    {
        res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
    }

    if (vex.wServicePackMajor != 0)
        res += TStr(", Service Pack ", vex.wServicePackMajor, ".", vex.wServicePackMinor);
    else
        res += TStr(", No service pack");

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
#ifdef BRO_OS_WINDOWS
    //TODO: std::this_thread::get_id()
    threadId = (t_uint32)GetCurrentThreadId();
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
std::vector<t_string> Gu::argsToVectorOfString(int argc, char** argv, char delimiter) {
    int squot = 0 , dquot = 0;

    //todo - fix the delimiter thing
    std::vector<t_string> ret;
    for (int i = 0; i < argc; ++i) {

        t_string str(argv[i]);


        ret.push_back(str);
    }
    return ret;
}

int32_t Gu::getSupportedDepthSize() {
    return Gu::_iSupportedDepthSize;

    //None of these work..

    //int tmp = 0;
    //SDL_DisplayMode mode;
    //SDL_GetCurrentDisplayMode(0, &mode);
    //mode.
    ////SDL_GL_GetAttribute(i , &tmp);
    //return tmp;
}
void Gu::pushCullFace() {
    GLint cull;
    glGetIntegerv(GL_CULL_FACE, &cull);

    if (_eLastCullFaceStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastCullFaceStack.push(cull);
    }
}
void Gu::popCullFace() {
    GLint cull = _eLastCullFaceStack.top();

    if (_eLastCullFaceStack.size() > 0) {
        _eLastCullFaceStack.pop();
    }
    if (_eLastCullFaceStack.size() == 0) {
        _eLastCullFaceStack.push(1);
    }

    if (cull) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}
void Gu::pushBlend() {
    GLint cull;
    glGetIntegerv(GL_BLEND, &cull);

    if (_eLastBlendStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastBlendStack.push(cull);
    }
}
void Gu::popBlend() {
    GLint cull = _eLastBlendStack.top();

    if (_eLastBlendStack.size() > 0) {
        _eLastBlendStack.pop();
    }
    if (_eLastBlendStack.size() == 0) {
        _eLastBlendStack.push(1);
    }

    if (cull) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }
}
void Gu::pushDepthTest() {
    GLint cull;
    glGetIntegerv(GL_DEPTH_TEST, &cull);

    if (_eLastDepthTestStack.size() > MaxStackSize) {
        BroLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
    }
    else {
        _eLastDepthTestStack.push(cull);
    }
}
void Gu::popDepthTest() {
    GLint cull = _eLastDepthTestStack.top();

    if (_eLastDepthTestStack.size() > 0) {
        _eLastDepthTestStack.pop();
    }
    if (_eLastDepthTestStack.size() == 0) {
        _eLastDepthTestStack.push(1);
    }

    if (cull) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}
void Gu::guiQuad2d(Box2f& pq, std::shared_ptr<Viewport> vp) {
    //Transforms a quad for the matrix-less gui projection.

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

    //Invert text to show rightside up and divide by perspective
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
void Gu::print(const t_string& msg) {
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

//////////////////////////////////////////////////////////////////////////
static int g_bPerfFrame = false;
void Gu::beginPerf() {
    if (Gu::getContext()->getFingers()->keyPressOrDown(SDL_SCANCODE_F1, KeyMod::e::Shift))
    {
        if (g_bPerfFrame == 0) {
            g_bPerfFrame = 1;
        }
        else if (g_bPerfFrame == 1) {
            g_bPerfFrame = 2;//Key held - disable
        }
    }
    else {
        g_bPerfFrame = 0;
    }
}
void Gu::endPerf() {
    if (g_bPerfFrame == 1) {
        BroLogInfo(_strCachedProf);
        _strCachedProf = "";
    }
}
void Gu::pulsePerf() {
    if (g_bPerfFrame == 1) {
        //Add dots
        int n = (int)_stopw.size();
        t_string str2 = "";
        for (int i = 0; i < n; ++i) {
            str2 += ".";
        }
        _strCachedProf = TStr(_strCachedProf, str2, _stopw.top().pulse(), "\n");
    }
}
void Gu::pushPerf() {
    if (g_bPerfFrame == 1) {
        Stopwatch sw;
        sw.start(DebugHelper::getCallingMethod());
        _stopw.push(sw);
    }
}
void Gu::popPerf() {
    if (g_bPerfFrame == 1) {
        pulsePerf();
        _stopw.pop();
    }
}

uint64_t Gu::getFrameNumber() {
    return Gu::getContext()->getFpsMeter()->getFrameNumber();
}

}//ns Game
