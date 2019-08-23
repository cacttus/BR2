/**
*
*    @file OpenGLWindow.h
*    @date August 23, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __OPENGLWINDOW_15665854371549001192_H__
#define __OPENGLWINDOW_15665854371549001192_H__

#include "../app/GraphicsWindow.h"

namespace Game {
/**
*    @class OpenGLWindow
*    @brief
*
*/
class OpenGLWindow : public GraphicsWindow {
    struct GLProfile : public VirtualMemory {
    public:
        int _iDepthBits = 0;
        int _iMinVersion = 0;
        int _iMinSubVersion = 0;
        int _iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; // Set to true to enable the compatibility profile
        bool _bVsync = true; //Automatic on IOS
        void make(int iDepthBits, int iMinVer, int iMinSubver, int iProfile, bool bVsync) {
            _iDepthBits = iDepthBits;
            _iMinSubVersion = iMinSubver;
            _iMinVersion = iMinVer;
            _iProfile = iProfile;
            _bVsync = bVsync;
        }
    };

private:

    std::shared_ptr<GLContext> _pContext = nullptr;
    SDL_GLContext _context;
    std::shared_ptr<GLProfile> _pSelectedProfile;
    void setWindowAndOpenGLFlags(GLProfile& prof);
    void initGLContext();
    void checkForOpenGlMinimumVersion(int required_version, int required_subversion);
    void getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver);
    void loadCheckProc();
    void makeCurrent();
    void getDrawableSize(int* w, int* h);
    void cleanup();
    void swapBuffers();
    void createContext(std::shared_ptr<AppBase> app);

public:

    std::shared_ptr<GLContext> getContext() { return _pContext; }

    OpenGLWindow(t_string title, bool is_main = false);
    virtual ~OpenGLWindow() override;

};

}//ns Game



#endif
