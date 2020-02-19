

#include "../base/DebugHelper.h"
#include "../base/AppRunner.h"
#include "../base/Gu.h"

using namespace BR2;
int main(int argc, char** argv) {

  DebugHelper::debugHeapBegin(false);
  {
    std::shared_ptr<AppRunner> ar = std::make_shared<AppRunner>();
    ar->runApp(Gu::argsToVectorOfString(argc, argv));
  }
  DebugHelper::debugHeapEnd();


  return 0;
}
