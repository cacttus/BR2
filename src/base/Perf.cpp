#include "../base/Perf.h"
#include "../base/Logger.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"

#include <stack>

namespace BR2 {
class PerfInternal : public VirtualMemory {
public:
  static string_t _strCachedProf;
  static std::stack<Stopwatch> _stopw;
  static void pulsePerf();
};
static int g_bPerfFrame = false;
std::stack<Stopwatch> PerfInternal::_stopw;
string_t PerfInternal::_strCachedProf = "";
void PerfInternal::pulsePerf() {
  if (g_bPerfFrame == 1) {
    //Add dots
    int n = (int)_stopw.size();
    string_t str2 = "";
    for (int i = 0; i < n; ++i) {
      str2 += ".";
    }
    _strCachedProf = _strCachedProf + str2 + _stopw.top().pulse() + "\n";
  }
}

//////////////////////////////////////////////////////////////////////////

void Perf::beginPerf() {
  if (Gu::getInputManager()->keyPressOrDown(SDL_SCANCODE_F1, KeyMod::e::Shift)) {
    if (g_bPerfFrame == 0) {
      g_bPerfFrame = 1;
    }
    else if (g_bPerfFrame == 1) {
      g_bPerfFrame = 2;//Key held - disable
    }
  }
  else {
    g_bPerfFrame = 0;
  }
}
void Perf::endPerf() {
  if (g_bPerfFrame == 1) {
    Br2LogInfo(_internal->_strCachedProf);
    _internal->_strCachedProf = "";
  }
}

void Perf::pushPerf() {
  if (g_bPerfFrame == 1) {
    Stopwatch sw;
    sw.start(DebugHelper::getCallingMethod());
    _internal->_stopw.push(sw);
  }
}
void Perf::popPerf() {
  if (g_bPerfFrame == 1) {
    _internal->pulsePerf();
    _internal->_stopw.pop();
  }
}








}//ns BR2
