#include "../app/AppBase.h"
#include "../base/FileSystem.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/GraphicsContext.h"


namespace Game {
///////////////////////////////////////////////////////////////////
AppBase::AppBase()
{

}
AppBase::~AppBase()
{

} 
///////////////////////////////////////////////////////////////////
void AppBase::drawDebug() {

    if(Gu::getGraphicsContext() != nullptr && Gu::getCamera() != nullptr) {
        RenderUtils::drawGridShader(1, 1, 1, 20, 5.0f);
        //RenderUtils::drawAxisShader(8.0f, 4.0f);
        //RenderUtils::drawWireSphereShader(4.0f, vec4(1, 1, 1, 1), 8, 8);

        //   Box3f b(vec3(-10, -10, -10), vec3(10, 10, 10));
        //  RenderUtils::drawSolidBoxShaded(&b, vec3(0, 0, 0), vec4(1, 0.82, 0.318, 1));
    }
}

t_string AppBase::makeAssetPath(t_string file) {
    t_string ret = FileSystem::combinePath(getAssetsDir(), file);
    return ret;
}

t_string AppBase::makeAssetPath(t_string folder, t_string file) {
    t_string fold = FileSystem::combinePath(getAssetsDir(), folder);
    t_string ret = FileSystem::combinePath(fold, file);
    return ret;
}


}//ns Game
