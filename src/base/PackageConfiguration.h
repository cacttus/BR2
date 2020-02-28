/**
*  @file PackageConfiguration.h
*  @date 20200227
*  @author MetalMario971
*/
#pragma once
#ifndef __PACKAGECONFIGURATION_15828434981618006221_H__
#define __PACKAGECONFIGURATION_15828434981618006221_H__

#include "../base/HashMap.h"

namespace BR2 {
class XmlConfigAttribute : public VirtualMemory {
public:
  string_t _attrib_name;
  string_t _attrib_value;
  XmlConfigAttribute(string_t attr, string_t val) {
    _attrib_value = val; _attrib_name = attr;
  }
  virtual ~XmlConfigAttribute() {}
};
class XmlConfigEntry : public VirtualMemory {
public:
  string_t _tagname;
  HashMap<std::shared_ptr<XmlConfigAttribute>> _attrs;

  XmlConfigEntry(string_t tag) {
    _tagname = tag;
  }
  virtual ~XmlConfigEntry() {}
};
class PackageConfiguration : public VirtualMemory {
public:
  string_t _appName;
  HashMap<XmlConfigEntry> _entries;
};
}//ns Game



#endif
