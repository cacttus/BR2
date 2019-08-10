#include "../app/AppMain.h"
#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../model/MeshUtils.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/Viewport.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/CameraNode.h"


namespace Game {
AppMain::AppMain() {
}
AppMain::~AppMain() {
}
void AppMain::drawBackgroundImage() {
    if (_pQuadMeshBackground == nullptr) {
        _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
            Gu::getCamera()->getViewport()->getWidth(), Gu::getCamera()->getViewport()->getHeight());
        _pTex = Gu::getTexCache()->getOrLoad(makeAssetPath("tex", "test_tex3.png"));
    }
    std::shared_ptr<CameraNode> bc = Gu::getCamera();
    Gu::getShaderMaker()->getImageShader_F()->setCameraUf(bc);
    Gu::getShaderMaker()->getImageShader_F()->beginRaster();
    {
        //We want depth test so we can see what's in front.
        //glEnable(GL_DEPTH_TEST);
        _pTex->bind(TextureChannel::e::Channel0, Gu::getShaderMaker()->getImageShader_F());

        Gu::getShaderMaker()->getImageShader_F()->draw(_pQuadMeshBackground);
    }
    Gu::getShaderMaker()->getImageShader_F()->endRaster();

}
void AppMain::debugChangeRenderState() {
#ifdef BRO_OS_WINDOWS
    //#ifdef _DEBUG
    //These must be #ifdef out because glPolygonMOde is not present in gl330 core 
    if (_bDebugShowWireframe == true) {
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }
    if (_bDebugClearWhite == true) {
        Graphics->getClearR() = 1.f;
        Graphics->getClearG() = 1.f;
        Graphics->getClearB() = 1.f;
        Graphics->getClearA() = 1.f;
    }
    else {
        Graphics->getClearR() = 0.01f;
        Graphics->getClearG() = 0.01f;
        Graphics->getClearB() = 0.01f;
        Graphics->getClearA() = 1.0f;
    }
    //#endif
#endif
    if (_bDebugDisableCull == true) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
    }
    if (_bDebugDisableDepthTest == true) {
        glDisable(GL_DEPTH_TEST);
    }
    else {
        glEnable(GL_DEPTH_TEST);
    }
}
void AppMain::drawDeferred(RenderParams& rp) {
    debugChangeRenderState();
}
void AppMain::drawForward(RenderParams& rp) {
    debugChangeRenderState();

    if (_pQuadMeshBackground == nullptr) {
        _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
            Gu::getCamera()->getViewport()->getWidth(), Gu::getCamera()->getViewport()->getHeight());
        _pTex = Gu::getTexCache()->getOrLoad(makeAssetPath("tex", "test_tex3.png"));
    }
}
void AppMain::drawShadow(RenderParams& rp) {
}
void AppMain::drawDebug(RenderParams& rp) {
}
void AppMain::drawNonDepth(RenderParams& rp) {
}
void AppMain::drawTransparent(RenderParams& rp) {
}









}//ns Game
