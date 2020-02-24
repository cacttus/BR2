#include "../base/Exception.h"
#include "../base/Logger.h"


namespace BR2 {
Exception::Exception(t_string c, int line, const char* file, bool bLogError) {
  _line = line;
  _msg = c;
  _file = file;
  if (bLogError) {
    t_string err = c + " " + file + ":" + line;
    BRLogError(err);
  }

  Gu::debugBreak();
}
t_string Exception::what() {
  return _msg + " file:" + _file + " line:" + _line;
}








}//ns Game
