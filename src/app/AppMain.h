/**
*
*    @file AppMain.h
*    @date August 9, 2019
*    @author Derek Page
*
*    © 2019
*
*
*/
#pragma once
#ifndef __APPMAIN_1565368851810156743_H__
#define __APPMAIN_1565368851810156743_H__

#include "../base/AppBase.h"


namespace Game {
/**
*    @class AppMain
*    @brief Application entry point.
*
*/
class AppMain : public AppBase {
    bool _bDebugDisableCull = false;
    bool _bDrawDebug = false;
    bool _bShowDebugText = false;
    bool _bDebugShowWireframe = false;
    bool _bDebugClearWhite = false;
    bool _bDebugDisableShadows = false;
    bool _bDebugDisableDepthTest = false;

    void debugChangeRenderState();
    std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
    void drawBackgroundImage();
    std::shared_ptr<Texture2DSpec> _pTex = nullptr;

public:
    AppMain();
    virtual ~AppMain() override;
    virtual void setup(std::shared_ptr<Viewport> pv) override {}
    virtual void step(float dt) override {}
    virtual void teardown() override {}
    virtual void updateTouch(std::shared_ptr<Fingers>, float dt) override {}
    virtual void userZoom(int amount) override {}

    virtual t_string getIconFullPath() { return makeAssetPath("tex", "icon.png"); }
    virtual t_string getConfigPath() override { return makeAssetPath("", "config.xml"); }
    virtual t_string getAssetsDir() override { return "./data/"; }

    virtual t_string getShadersDir() override { return "shr"; }
    virtual t_string getModelsTextDir() override { return "mob"; }
    virtual t_string getModelsBinDir() override { return "mbi"; }
    virtual t_string getCacheDir() override { return "cache"; }

    virtual t_string getEnvTexturePath() override { return "tex/env1_huge.png"; }

    virtual void serverPacketReceived(std::shared_ptr<Packet> temp) override {}
    virtual void drawDeferred(RenderParams& rp) override;
    virtual void drawForward(RenderParams& rp) override;
    virtual void drawShadow(RenderParams& rp) override;
    virtual void drawDebug(RenderParams& rp) override;
    virtual void drawNonDepth(RenderParams& rp) override;
    virtual void drawTransparent(RenderParams& rp) override;

};

}//ns Game



#endif
