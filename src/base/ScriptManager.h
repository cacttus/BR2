/**
*  @file ScriptManager.h
*  @date 20200213
*  @author MetalMario971
*/
#pragma once
#ifndef __SCRIPTMANAGER_15816538872085599039_H__
#define __SCRIPTMANAGER_15816538872085599039_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class ScriptManager
*  @brief
*/
class ScriptManager : public VirtualMemoryShared<ScriptManager> {
public:
    ScriptManager();
    virtual ~ScriptManager() override;
    void update(float delta);

    std::shared_ptr<CSharpScript> loadScript(string_t filepath);
private:
  std::shared_ptr<CSharpCompiler> _compiler = nullptr;
};

}//ns Game



#endif
