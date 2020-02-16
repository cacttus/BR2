#include "../base/Perf.h"
#include "../base/Logger.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"


namespace BR2 {
static int g_bPerfFrame = false;

std::stack<Stopwatch> Perf::_stopw;
string_t Perf::_strCachedProf = "";

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
    Br2LogInfo(_strCachedProf);
    _strCachedProf = "";
  }
}
void Perf::pulsePerf() {
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
void Perf::pushPerf() {
  if (g_bPerfFrame == 1) {
    Stopwatch sw;
    sw.start(DebugHelper::getCallingMethod());
    _stopw.push(sw);
  }
}
void Perf::popPerf() {
  if (g_bPerfFrame == 1) {
    pulsePerf();
    _stopw.pop();
  }
}








}//ns Game
