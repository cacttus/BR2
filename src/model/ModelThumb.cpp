#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../base/ApplicationPackage.h"
#include "../base/BinaryFile.h"
#include "../base/Img32.h"
#include "../base/AppBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderPipeline.h"
#include "../gfx/TexCache.h"
#include "../gfx/LightNode.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/RenderViewport.h"
#include "../model/Model.h"
#include "../model/ModelCache.h"
#include "../model/MeshData.h"
#include "../model/Material.h"
#include "../model/ModelThumb.h"
#include "../world/PhysicsWorld.h"


namespace BR2 {
///////////////////////////////////////////////////////////////////
//We need to generate 8 thumbs.

std::shared_ptr<Img32> ModelThumb::genThumb(std::shared_ptr<ModelSpec> mod, int32_t iThumbSize) {
    ////Create Lights + Camera
    //std::shared_ptr<WindowViewport> pv = std::make_shared<WindowViewport>(iThumbSize, iThumbSize);
    //std::shared_ptr<CameraNode> pThumbCam = CameraNode::create(pv);
    //std::shared_ptr<LightNodePoint> light = Gu::getPhysicsWorld()->makePointLight(vec3(30, 30, 30), 99999, vec4(1, 1, 1, 1), "", false);
    //pThumbCam->setPos(std::move(vec3(20, 20, 20)));
    //pThumbCam->setLookAt(std::move(vec3(0, 0, 0)));
    //std::map<Hash32, std::shared_ptr<Animator>> m;
    //pThumbCam->update(0.0f, m);
    //std::shared_ptr<CameraNode> pOrigCam = Gu::getCamera();
    //Gu::setCamera(pThumbCam);

    ////*Clear color - this isn't working for some reason.
    //vec4 savedClear = Gu::getRenderPipe()->getClear();
    //Gu::getRenderPipe()->setClear(vec4(0, 0, 0, 0));//Zero alpha

    ////Create an instance of model A
    //t_string name = mod->getName();
    //std::shared_ptr<ModelNode> wo = Gu::getPhysicsWorld()->makeObj(mod, vec3(0, 0, 0), vec4(0, 0, 1, 0), vec3(1, 1, 1), "");

    ////Position cam + light outside the model
    //float r = wo->getBoundBoxObject()->outerRadius() * 2.0f;
    //vec3 p(r, r, r);
    //vec3 c = wo->getBoundBoxObject()->center();
    //pThumbCam->setLookAt(std::move(c));
    //pThumbCam->setPos(std::move(p));
    //pThumbCam->update(0.0f, m);
    //light->setPos(std::move(p));

    ////Render scene + get the image
    //std::bitset<8> bits;
    //bits.set(PipeBit::e::Deferred);
    //bits.set(PipeBit::e::Shadow);//I think we need this in order to update lights
    //Gu::getRenderPipe()->renderScene(wo);
    //std::shared_ptr<Img32> thumb = Gu::getRenderPipe()->getResultAsImage();

    ////save to disk (debug)
    //t_string cached = Gu::getApp()->makeAssetPath("cache", Stz "test-thumb-" + name + "-vflip-0.png");
    //thumb->flipV();
    //Gu::saveImage(cached, thumb);
    //thumb->flipV();

    ////Cleanup, Restore state
    //Gu::getPhysicsWorld()->tryRemoveObj(wo);
    //Gu::getPhysicsWorld()->tryRemoveObj(light);
    //Gu::getRenderPipe()->setClear(savedClear);
    //Gu::setCamera(pOrigCam);


    return thumb;
}







}//ns Game
