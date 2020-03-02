#include "../base/DebugHelper.h"
#include "../base/AppRunner.h"
#include "../base/Gu.h"

//unit test includes
#include "../base/CSharpScript.h"
#include <iostream>


//This is needed, since SDL defines main.
#ifdef main
#  undef main
#endif

using namespace BR2;

int main(int argc, char** argv) {
  DebugHelper::debugHeapBegin(false);
  {
    //BR2::DebugHelper::setBreakAlloc(221975);
    std::shared_ptr<AppRunner> ar = std::make_shared<AppRunner>();
    ar->runApp(Gu::argsToVectorOfString(argc, argv),
          std::vector<std::function<bool()>>({
          []() {
          //const string_t strtest = "using System; namespace MyNS { class MyCL{int[] x = new int[]; int b(float a ){ return b + a*(2*(3+.0006d); }} }";

          //std::vector<CSToken*> toks = CSharpScript::lexTest(strtest);
          //string_t str = CSharpScript::tokensToString(toks);
          //std::cout << str << std::endl;

          //Gu::debugBreak();
          return true;
          }
        })
    );
  }
  DebugHelper::debugHeapEnd();

  return 0;
}
