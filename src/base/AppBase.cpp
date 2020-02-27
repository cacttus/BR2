#include "../base/AppBase.h"
#include "../base/FileSystem.h"
#include "../base/Gu.h"
#include "../base/EngineConfig.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/GraphicsContext.h"


namespace BR2 {
AppBase::AppBase() {

}
AppBase::~AppBase() {

}
void AppBase::drawForwardDebug(RenderParams& rp) {

  if (Gu::getCoreContext() != nullptr /*&& Gu::getCamera() != nullptr*/) {
    RenderUtils::drawGridShader(rp.getCamera(), 1, 1, 1, 20, 5.0f);
    //RenderUtils::drawAxisShader(8.0f, 4.0f);
    //RenderUtils::drawWireSphereShader(4.0f, vec4(1, 1, 1, 1), 8, 8);

    //   Box3f b(vec3(-10, -10, -10), vec3(10, 10, 10));
    //  RenderUtils::drawSolidBoxShaded(&b, vec3(0, 0, 0), vec4(1, 0.82, 0.318, 1));
  }
}

string_t AppBase::makeAssetPath(string_t file) {
  string_t ret = FileSystem::combinePath(getAssetsDir(), file);
  return ret;
}

string_t AppBase::makeAssetPath(string_t folder, string_t file) {
  string_t fold = FileSystem::combinePath(getAssetsDir(), folder);
  string_t ret = FileSystem::combinePath(fold, file);
  return ret;
}


}//ns Game
