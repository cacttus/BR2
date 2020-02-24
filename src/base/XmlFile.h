/**
*  @file XmlFile.h
*  @date August 4, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __XMLFILE_15649469792082348406_H__
#define __XMLFILE_15649469792082348406_H__

#include "../base/TextConfigFile.h"

namespace BR2 {
class XmlFile_Internal;
/**
*  @class XmlFile
*  @brief Brief interface that handles loading the XML file into a pugixml document.
*/
class XmlFile : public TextConfigFile {
public:
  XmlFile();
  XmlFile(string_t loc);
  virtual ~XmlFile() override;

protected:
  virtual void parse(char* buf, int64_t size) override;
  //To mimick the behavior of PoundFile.
  virtual void pkp(std::vector<string_t>& tokens) = 0;

private:
  std::unique_ptr<XmlFile_Internal> _pint = nullptr;
};

}//ns Game



#endif
