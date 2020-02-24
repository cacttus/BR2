/**
*  @file EngineConfig.h
*  @date May 6, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __ENGINECONFIG_14940903882644616215_H__
#define __ENGINECONFIG_14940903882644616215_H__

#include "../base/XmlFile.h"
//#include "../base/PoundFile.h"

namespace Game {

class EngineConfigFile : public XmlFile {
public:
  EngineConfigFile();
  virtual ~EngineConfigFile() override;

  std::shared_ptr<EngineConfig> getConfig() { return _pConfig; }
protected:
  virtual void pkp(std::vector<t_string>& tokens) override;
  virtual void preLoad() override;
  virtual void postLoad(bool success) override;

  std::shared_ptr<EngineConfig> _pConfig = nullptr;
};

}//ns Game



#endif
