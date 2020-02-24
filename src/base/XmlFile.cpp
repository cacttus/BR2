#include "../base/XmlFile.h"
#include "../base/Logger.h"

#include "../ext/pugixml/pugixml.hpp"
#include "../ext/pugixml/pugiconfig.hpp"

namespace BR2 {
class XmlFile_Internal {
public:
  pugi::xml_document _doc;
  pugi::xml_parse_result _parseResult;
};
XmlFile::XmlFile() {
  _pint = std::make_unique<XmlFile_Internal>();
}
XmlFile::XmlFile(t_string loc) :  TextConfigFile(loc) {
  _pint = std::make_unique<XmlFile_Internal>();
}
XmlFile::~XmlFile() {
  _pint = nullptr;
}
void XmlFile::parse(char* buf, int64_t size) {

  // The block can be allocated by any method; the block is modified during parsing
  _pint->_parseResult = _pint->_doc.load_buffer_inplace((void*)buf, size);

  auto children = _pint->_doc.children();

  //PugiXML will skip the <!doctype> node in the document children.

  //https://stackoverflow.com/questions/14329738/pugixml-number-of-child-nodes
  size_t n = std::distance(_pint->_doc.children().begin(), _pint->_doc.children().end());
  if (n > 2) {
    BRLogWarn("Too many children in configuration file.  File must have a schema tag and 1 root child");
  }

  _vecTokens.clear();

  //For now we're using a strict format of 1 child node.
  for (auto it = children.begin(); it != children.end(); it++) {

    for (auto it_child = it->children().begin(); it_child != it->children().end(); it_child++) {
      _vecTokens.clear();

      t_string key = it_child->name();
      _vecTokens.push_back(key);

      //loop attrs
      //This is donk.  We should use a more formal key/value system but for now just hack it.
      for (auto it_attr = it_child->attributes().begin(); it_attr != it_child->attributes().end(); it_attr++) {
        t_string val = it_attr->value();
        _vecTokens.push_back(val);
      }

      pkp(_vecTokens);
    }
  }

}






}//ns Game
