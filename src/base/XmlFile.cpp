#include "../base/XmlFile.h"
#include "../base/Logger.h"

namespace Game {

XmlFile::XmlFile(t_string loc) : TextConfigFile(loc)
{
}
XmlFile::~XmlFile()
{
}

void XmlFile::parse(char* buf, int64_t size) {

    // The block can be allocated by any method; the block is modified during parsing
    _parseResult = _doc.load_buffer_inplace((void*)buf, size);

    auto children = _doc.children();
    
    //PugiXML will skip the <!doctype> node in the document children.

    //https://stackoverflow.com/questions/14329738/pugixml-number-of-child-nodes
    size_t n = std::distance(_doc.children().begin(), _doc.children().end());
    if (n > 2) {
        BroLogWarn("Too many children in configuration file.  File must have a schema tag and 1 root child");
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
