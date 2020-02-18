//#include "../gfx/.h"
#include "../gfx/RenderSettings.h"


namespace BR2 {

void RenderSettings::init() {
  _pRenderDebug = std::make_shared<RenderDebug>();
}


}//ns BR2
