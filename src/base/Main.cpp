#include "../base/DebugHelper.h"
#include "../base/AppRunner.h"
#include "../base/Gu.h"
#include "../base/AppMain.h"

//This is needed, since SDL defines main.
#ifdef main
#  undef main
#endif

using namespace BR2;

int main(int argc, char** argv) {
  DebugHelper::debugHeapBegin(false);
  {
    //BR2::DebugHelper::setBreakAlloc(221975);
    std::shared_ptr<AppMain> cwr = std::make_shared<AppMain>();
    Gu::setApp(cwr);

    std::shared_ptr<AppRunner> ar = std::make_shared<AppRunner>();
    ar->runApp(Gu::argsToVectorOfString(argc, argv), "Shake", cwr);
  }
  DebugHelper::debugHeapEnd();

  return 0;
}
