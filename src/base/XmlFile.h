/**
*  @file XmlFile.h
*  @date August 4, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __XMLFILE_15649469792082348406_H__
#define __XMLFILE_15649469792082348406_H__

#include "../base/TextConfigFile.h"

namespace Game {
/**
*    @class XmlFile
*    @brief Brief interface that handles loading the XML file into a pugixml document.
*/
class XmlFile : public TextConfigFile {
    pugi::xml_document _doc;
    pugi::xml_parse_result _parseResult;


protected:
    virtual void parse(char* buf, int64_t size) override;
    
    //To mimick the behavior of PoundFile.
    virtual void pkp(std::vector<t_string>& tokens) = 0;

    
public:
    XmlFile() {}
    XmlFile(t_string loc);
    virtual ~XmlFile() override;
};

}//ns Game



#endif
