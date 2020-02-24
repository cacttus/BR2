/**
*  @file TextDataFile.h
*  @date November 22, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __BASEDATAFILE_210628895103872956110227_H__
#define __BASEDATAFILE_210628895103872956110227_H__

#include "../base/TextParser.h"

namespace BR2 {
/**
*  @class TextDataFile
*  @brief Base class for in-memory text files.
*/
class TextDataFile : public VirtualMemory {
public:
  TextDataFile();
  TextDataFile(string_t& loc);
  virtual ~TextDataFile() override;

  string_t& getFileLoc() { return _fileLoc; } //This is used don't change
  void loadAndParse();// - If empty is specified then we use _fileLoc
  void loadAndParse(string_t& loc);// - If empty is specified then we use _fileLoc
  virtual void save(string_t& loc);

protected:
  string_t _fileName;
  string_t _fileLoc;
  char* _fileData = nullptr;
  int64_t _fileSize = 0;
  std::vector<string_t> _vecMessages;
  int32_t _curIdx;// gets IDX of current token array value
  // Override these methods.
  virtual void preLoad() = 0;
  virtual void parse(char* buf, int64_t filesize) = 0;
  virtual void postLoad(bool success) = 0;
  void msg(string_t msg, bool error = false);
};

}//ns game



#endif
