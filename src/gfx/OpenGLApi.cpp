//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <SDL_opengl_glext.h>

#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/oglErr.h"
#include "../base/GLContext.h"
#include "../base/Window.h"
#include "../gfx/OpenGLApi.h"

namespace Game {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}
void OpenGLApi::createWindow(t_string windowTitle) {
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
        makeWindow(windowTitle, SDL_WINDOW_OPENGL);
        initGLContext();

        int ver, subver, shad_ver, shad_subver;
        getOpenGLVersion(ver, subver, shad_ver, shad_subver);

        //Make sure we have a good depth value.
        int iDepth = 0;
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &iDepth);
        if (iDepth >= 24 && ver >= minGLVersion && ver >= minGLSubversion) {
            break;
        }
        else {
            BroLogInfo("OpenGL config profile " + iProf + " didn't work. Trying next profile.");
            destroyWindow();
        }
    }

    //Run a secondary check to make sure we didn't **F up 
    checkForOpenGlMinimumVersion(minGLVersion, minGLSubversion);

    //Check that OpenGL initialized successfully
    loadCheckProc();
}
void OpenGLApi::setWindowAndOpenGLFlags(GLProfile& prof) {
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
void OpenGLApi::initGLContext() {
    _context = SDL_GL_CreateContext(_pWindow->getSDLWindow());
    if (!_context) {
        BroThrowException("SDL_GL_CreateContext() error" + SDL_GetError());
    }
}
void OpenGLApi::getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver) {
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

    std::vector<t_string> sv;

    if (glver.length() > 0) {
        sv = StringUtil::split(glver, '.');
        if (sv.size() < 2) {
            BroThrowException("Failed to get OpenGL version. Got '" + glver + "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
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
            BroThrowException("Failed to get OpenGL Shade version. Got '" + glslver + "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
        }
        shad_ver = TypeConv::strToInt(sv[0]);
        shad_subver = TypeConv::strToInt(sv[1]);
    }
    else {
        BroLogError("Failed to get OpenGL Shade version.");
    }
}
void OpenGLApi::checkForOpenGlMinimumVersion(int required_version, int required_subversion) {
    t_string rver = Stz "" + required_version + "." + required_subversion;

    //GLint iMajor, iMinor;
    //glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
    //glGetIntegerv(GL_MINOR_VERSION, &iMinor);
    //After 3.0 we no longer support glGetString
    int ver, subver, shad_ver, shad_subver;
    getOpenGLVersion(ver, subver, shad_ver, shad_subver);

    t_string vendor = t_string((char*)glGetString(GL_VENDOR));
    t_string renderer = t_string((char*)glGetString(GL_RENDERER));

    BroLogInfo("\n"
        + "   OpenGL version " + ver + "." + subver + ".\n"
        + "   GLSL version:          " + shad_ver + "." + shad_subver + "\n"
        + "   GPU:         " + renderer + "\n"
        + "   GPU Vendor:  " + vendor + "\n"
    );

    if (ver < required_version || (ver >= required_subversion && subver < required_subversion)) {
        BroThrowException(Stz "\n"
            + "The game could not find the latest version of OpenGL Shading Language.\n\n"
            + "Possible Problems could be:\n\n"
            + "   1) The primary graphics card is incorrect,\n\n"
            + "   2) The graphics driver is out of date,\n\n"
            + "   3) Your Graphics card is old.\n\n"
            + " This application requires OpenGL version " + rver + ".\n"
            + " The system has detected OpenGL version " + ver + "." + subver + ".\n"
            + " Update your graphics driver by going to www.nvidia.com, www.ati.com or www.intel.com.\n\n"
        );
    }


}
void OpenGLApi::makeCurrent() {
    SDL_GL_MakeCurrent(_pWindow->getSDLWindow(), _context);
}
void OpenGLApi::getDrawableSize(int* w, int* h) {
    SDL_GL_GetDrawableSize(_pWindow->getSDLWindow(), w, h);
}
void OpenGLApi::swapBuffers() {
    SDL_GL_SwapWindow(_pWindow->getSDLWindow());
}
void OpenGLApi::cleanup() {
    if (_context)
    {
        /* SDL_GL_MakeCurrent(0, NULL); *//* doesn't do anything */
        SDL_GL_DeleteContext(_context);
    }
    GraphicsApi::cleanup();
}
void OpenGLApi::createContext(std::shared_ptr<AppBase> app) {

    //Must set this to the depth size we support.
    int tmp = 24;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);
    Gu::setSupportedDepthSize(tmp);

    if (getWindow() != nullptr) {
        getWindow()->printHelpfulDebug();
    }

    std::shared_ptr<GLContext> ct = std::make_shared<GLContext>();
    Gu::setContext(ct);

    if (!ct->load(app)) {
        BroLogError("Failed to load context.");
        BroThrowException("Failed to load OpenGL Context. One or more methods were not supported.");
    }

    //Quick GL test.
    ct->glUseProgram(0);

    BroLogInfo("Creating GL Context");
    Gu::setContext(std::dynamic_pointer_cast<GraphicsContext>(ct));
}

void OpenGLApi::loadCheckProc() {
    //Check that OpenGL initialized successfully by checking a library pointer.
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



}//ns Game
