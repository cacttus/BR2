/**
*  @file XmlFile.h
*  @date August 4, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __XMLFILE_15649469792082348406_H__
#define __XMLFILE_15649469792082348406_H__

#include "../base/TextConfigFile.h"
#include "../base/HashMap.h"

namespace BR2 {
class XmlConfigAttribute : public VirtualMemory {
public:
  string_t _attrib_name;
  string_t _attrib_value;
  XmlConfigAttribute(string_t attr, string_t val) {
    _attrib_value = val; _attrib_name = attr;
  }
};
class XmlConfigEntry: public VirtualMemory {
public:
  string_t _tagname;
  HashMap<XmlConfigAttribute> _attrs;

  XmlConfigEntry(string_t tag) {
    _tagname = tag;
  }
};
/**
*    @class XmlFile
*    @brief Brief interface that handles loading the XML file into a pugixml document.
*/
class XmlFile : public TextConfigFile {
public:
  XmlFile();
  XmlFile(string_t loc);
  virtual ~XmlFile() override;

  static HashMap<XmlConfigEntry> getXMLConfiguration(string_t filepath);

protected:
  virtual void parse(char* buf, int64_t size) override;

  //To mimick the behavior of PoundFile.
  virtual void pkp(std::vector<string_t>& tokens) = 0;
private:
  pugi::xml_document _doc;
  pugi::xml_parse_result _parseResult;
};

}//ns Game



#endif
