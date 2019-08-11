/**
*
*    @file OpenGLApi.h
*    @date August 5, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __OPENGLGraphicsApi_15650573213471462786_H__
#define __OPENGLGraphicsApi_15650573213471462786_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/GraphicsApi.h"

namespace Game {
/**
*    @class OpenGLApi
*    @brief
*
*/
class OpenGLApi : public GraphicsApi {
public:
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
    SDL_GLContext _context;

    void setWindowAndOpenGLFlags(GLProfile& prof);
    void initGLContext();
    void checkForOpenGlMinimumVersion(int required_version, int required_subversion);
    void getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver);
    void printHelpfulDebug();
    void loadCheckProc();
public:
    virtual void createWindow(t_string title) override;
    virtual void makeCurrent() override;
    virtual void getDrawableSize(int* w, int* h) override;
    virtual void cleanup() override;
    virtual void swapBuffers() override;
    virtual void createContext(std::shared_ptr<AppBase> app) override;


    OpenGLApi();
    virtual ~OpenGLApi() override;
};

}//ns Game



#endif
