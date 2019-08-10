#include "../base/BaseAll.h"
#include "../model/IboData.h"
//#include "../gfx/GLRenderSystem.h"
//#include "../math/Vec2x.h"
//#include "../math/Vector3.h"
//#include "../math/Vector4.h"

namespace Game {
IboData::IboData(std::shared_ptr<GLContext> ctx, size_t iElementSize) :
GpuBufferData(ctx, GL_ELEMENT_ARRAY_BUFFER, iElementSize)
{
    if(iElementSize != 4 && iElementSize != 2) { 
        BroLogWarn("Index buffer element size invalid:" + iElementSize) ;
        Gu::debugBreak();
    }
    //set GL index format.
  //  _glIndexType = getGlIndexTypeFromIndexFmt(_indexType);
  //  _glIndexMode = getGlIndexModeFromIndexFmt(_indexType);
}
IboData::~IboData()
{ 
}


}//ns game
