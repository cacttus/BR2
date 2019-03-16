#include "../base/FrameSync.h"
#include "../base/Gu.h"
#include "../base/RoomBase.h"
#include "../base/GLContext.h"
//#include "../base/OperatingSystem.h"
//#include "../math/XMath.h"
//#include "../base/BaseIncludes.h"
//#include "../scene/IScene.h"
//#include "../display/RenderWindow.h"
//#include "../display/Win32ScreenController.h"
//#include "../base/GlobalUtils.h"
//#include "../hardware/GpuDeviceWin32.h"

namespace Game {
;
FrameSync::FrameSync(std::shared_ptr<GLContext> ct) :
    _pContext(ct), _bVsyncDisabled(false)
{

}

FrameSync::~FrameSync() {

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
float FrameSync::getCurrentFrameCap()
{
    if (!_bVsyncDisabled) {
        return _nFramesMax;
    }
    else {
        return _pContext->getFpsMeter()->getFps();
    }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int64_t FrameSync::calculateWaitMicroseconds()
{
    int64_t  d = (int64_t)(_syncEnd - _syncStart);

    float minMillis = 1000000.0f / _nFramesMax;
    float waitms = minMillis - (float)d;
    int64_t waiti = (int)round(waitms);

    return waiti;
}
bool FrameSync::shouldWait(int64_t& waitUs)
{
    waitUs = calculateWaitMicroseconds();

    return (waitUs > 0) && (!_bVsyncDisabled);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FrameSync::syncBegin()
{
    _syncStart = Gu::getMicroSeconds();
}
int FrameSync::syncEnd()
{
    int64_t waitUs;
    int64_t elapsedUs;

    _syncEnd = Gu::getMicroSeconds();

    if (shouldWait(waitUs))
    {
        int64_t waitStart = (int64_t)Gu::getMicroSeconds();

        elapsedUs = 0;
        while (elapsedUs < waitUs)
        {
            // - If we have an idle processor execute some extra computations
            // while we wait for next frame to come around.
            if (_pContext->getRoom() != nullptr) {
                _pContext->getRoom()->idle(waitUs - elapsedUs);//**MUST BE US
            }

            // - update elapsed
            elapsedUs = (int64_t)Gu::getMicroSeconds() - waitStart;
        }
    }
    else {
        //Still call the idle() method so stuff don't get missed
        _pContext->getRoom()->idle(0);
    }
    return 0;
}

void FrameSync::disable()
{
    _bVsyncDisabled = true;
}
void FrameSync::enable()
{
    _bVsyncDisabled = false;
}


}//ns game
