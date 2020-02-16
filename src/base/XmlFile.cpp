#include "../base/XmlFile.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/HashMap.h"

namespace BR2 {
XmlFile::XmlFile() {
}
XmlFile::XmlFile(string_t loc) : TextConfigFile(loc) {
}
XmlFile::~XmlFile() {
}
std::shared_ptr<PackageConfiguration> XmlFile::getXMLConfiguration(string_t filepath) {
  pugi::xml_document doc;

  char* fdata;
  int64_t fsize;
  FileSystem::SDLFileRead(filepath, fdata, fsize, true);
  pugi::xml_parse_result parseResult = doc.load_buffer_inplace((void*)fdata, fsize);
  FileSystem::SDLFileFree(fdata);

  //HashMap<XmlConfigEntry> items;
  std::shared_ptr<PackageConfiguration> ret;

  //https://stackoverflow.com/questions/14329738/pugixml-number-of-child-nodes
  size_t n = std::distance(doc.children().begin(), doc.children().end());
  if (n > 2) {
    Br2LogWarn("Too many root level children in configuration file.  File must have a schema tag and 1 root child");
  }

  //ApplicationPackage tag
  pugi::xml_node root = doc.find_child([](pugi::xml_node node) {
    if (StringUtil::equalsi(node.name(), "ApplicationPackage")) {
      return true;
    }
    return false;
    });
  if (root.empty()) {
    Br2LogWarn("Could not find <ApplicationPackage> root node.  The node should be at the document root.");
    Gu::debugBreak();
  }
  else {
    pugi::xml_attribute att = root.find_attribute([](pugi::xml_attribute attr) {
      if (StringUtil::equalsi(attr.name(), "name")) {
        return true;
      }
      return false;
      });
    if (att.empty()) {
      Br2LogWarn("<ApplicationPackage> did not contain a 'name' attribute.");
      Gu::debugBreak();
    }
    else {
      ret->_appName = string_t(att.value());
    }
  }

  //Parse sub-items.
  //The root level element is by itself
  for (auto it_root_child = doc.children().begin(); it_root_child != doc.children().end(); it_root_child++) {
    for (auto it_child = doc.children().begin(); it_child != doc.children().end(); it_child++) {
      XmlConfigEntry ent(it_child->name());
      for (auto it_attr = it_child->attributes().begin(); it_attr != it_child->attributes().end(); it_attr++) {
        std::string st(it_attr->name());
        ent._attrs.add(st, std::make_shared<XmlConfigAttribute>(st, it_attr->value()));
      }
      ret->_entries.add(ent._tagname, ent);
    }
  }

  return ret;
}

void XmlFile::parse(char* buf, int64_t size) {
  // The block can be allocated by any method; the block is modified during parsing
  _parseResult = _doc.load_buffer_inplace((void*)buf, size);

  auto children = _doc.children();

  //PugiXML will skip the <!doctype> node in the document children.

  //https://stackoverflow.com/questions/14329738/pugixml-number-of-child-nodes
  size_t n = std::distance(_doc.children().begin(), _doc.children().end());
  if (n > 2) {
    Br2LogWarn("Too many children in configuration file.  File must have a schema tag and 1 root child");
  }

  _vecTokens.clear();

  //For now we're using a strict format of 1 child node.
  for (auto it = children.begin(); it != children.end(); it++) {

    for (auto it_child = it->children().begin(); it_child != it->children().end(); it_child++) {
      _vecTokens.clear();

      string_t key = it_child->name();
      _vecTokens.push_back(key);

      //loop attrs
      //This is donk.  We should use a more formal key/value system but for now just hack it.
      for (auto it_attr = it_child->attributes().begin(); it_attr != it_child->attributes().end(); it_attr++) {
        string_t val = it_attr->value();
        _vecTokens.push_back(val);
      }

      pkp(_vecTokens);
    }
  }

}






}//ns Game
