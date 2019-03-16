#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/TStr.h"

namespace Game {
///////////////////////////////////////////////////////////////////
Exception::Exception(t_string c, int line, const char* file, bool bLogError ) {
    _line = line;
    _msg = c;
    _file = file;
    if(bLogError) {
        t_string err = TStr(c.c_str(), " ", file, ":", line);
        BroLogError(err);
    }

    Gu::debugBreak();
}









}//ns Game
