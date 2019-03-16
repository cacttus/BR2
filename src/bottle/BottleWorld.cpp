

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../base/AppRunner.h"
#include "../base/DebugHelper.h"
#include "../base/Gu.h"
#include "../bottle/BottleRoom.h"

//So esprima doesn't work, simply (i=0;i<5;x++) alert(x); doesn't give an error like it should.
//#include "./../core/src/ext/esprima.h"


int main(int argc, char** argv) {
    
    Game::DebugHelper::debugHeapBegin(false);
    {
        //Game::DebugHelper::setBreakAlloc(221975);
        std::shared_ptr<Game::BottleRoom> cwr = std::make_shared<Game::BottleRoom>();
        std::shared_ptr<Game::AppRunner> ar = std::make_shared<Game::AppRunner>();
        ar->runApp(Game::Gu::argsToVectorOfString(argc, argv), "Bottle World", cwr);
    }
    Game::DebugHelper::debugHeapEnd();


    return 0;
}
