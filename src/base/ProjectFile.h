/**
*  @file ProjectFile.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __PROJECTFILE_15665753691467122366_H__
#define __PROJECTFILE_15665753691467122366_H__

#include "../base/AppHeader.h"
#include "../base/XmlFile.h"

namespace BR2 {
/**
*  @class ProjectFile
*  @brief
*
*/
class ProjectFile : public XmlFile {
public:
  ProjectFile();
  virtual ~ProjectFile() override;

  string_t _name;
  string_t _path;

};

}//ns BR2



#endif
