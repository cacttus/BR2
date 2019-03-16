/**
*
*    @file EngineConfig.h
*    @date May 6, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __ENGINECONFIG_14940903882644616215_H__
#define __ENGINECONFIG_14940903882644616215_H__

#include "../base/PoundFile.h"

namespace Game {

class EngineConfigFile : public PoundFile {

protected:
    virtual void pkp(std::vector<t_string>& tokens);
    virtual void preLoad();
    virtual void postLoad();

    std::shared_ptr<EngineConfig> _pConfig = nullptr;

public:
    EngineConfigFile();
    virtual ~EngineConfigFile() override;

    std::shared_ptr<EngineConfig> getConfig() { return _pConfig; }

};
}//ns Game



#endif
