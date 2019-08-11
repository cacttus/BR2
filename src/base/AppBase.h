/**
*
*    @file AppBase.h
*    @date January 9, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __ROOMBASE_1483942071882744649_H__
#define __ROOMBASE_1483942071882744649_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"
namespace Game {
/**
*    @class AppBase
*    @brief Base class for application.
*
*/
class AppBase : public Drawable {
public:
    AppBase();
    virtual ~AppBase() override;
    t_string makeAssetPath(t_string file);
    t_string makeAssetPath(t_string folder, t_string file);

    virtual void drawDebug();
	virtual bool getStartFullscreen();
	virtual uint32_t getStartWidth();
	virtual uint32_t getStartHeight();
	virtual bool getForceAspectRatio();
    virtual void screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) { };
    virtual void idle(t_timeval us) { }

    virtual void step(float dt) = 0;
    virtual void teardown() = 0;
    virtual void updateTouch(std::shared_ptr<Fingers>, float dt) = 0;
    virtual void userZoom(int amount) = 0;
    virtual t_string getIconFullPath() { return ""; }
    virtual t_string getConfigPath() = 0;
    virtual t_string getAssetsDir() = 0;
    virtual t_string getShadersDir() = 0; //This is relative to the getAssetsDir.  E.g. "/shr/"
    virtual t_string getCacheDir() = 0; // the place where we store cached shaders, and generated images
    virtual t_string getModelsTextDir() = 0; // MOB dir 
    virtual t_string getModelsBinDir() = 0; // MBI DIr
    virtual t_string getEnvTexturePath() = 0;

    virtual void serverPacketReceived(std::shared_ptr<Packet> temp) = 0;
};

}//ns Game



#endif
