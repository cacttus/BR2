/**
*  @file TextParser.h
*  @date July 20, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __BUFPARSER_235220395253811918026664_H__
#define __BUFPARSER_235220395253811918026664_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*    @class TextParser
*    @brief Parse Text.  Takes a pointer to the text data to parse and increments its own internal copy of the pointer.
*/
class TextParser : public VirtualMemory {
public:
  TextParser(char* data);
  virtual ~TextParser() override;

  bool charIsWs();
  bool charIsWs(char c);
  bool charIsEscape();
  bool charIsEscape(char c);
  char charAt();
  char nextChar();
  char prevChar();
  bool seekPastChar(char c);
  void seekPastChars(std::vector<char> cv);
  void seekToChars(std::vector<char> cv);
  bool inc();
  void dec();
  bool eof(); // - Return if we are at end of file.
  void eatws();// - EAT
  void eatBlockComment();
  void eatLine();
  void eatBody();// - Eat the body of a statement/function from { to }
  void eatLine(int32_t& lineCountToAddTo);

private:
  typedef enum {
    ptr_begin, // beginning of file
    ptr_seek, // ptr is in the file
    ptr_eof // ptr is end of file
  }t_ptr_state;

  bool _bEof = false;
  int _linenum = 1;
  t_ptr_state _ptrState = t_ptr_state::ptr_begin;
  char* _ptr = nullptr;



};



}//ns game

#endif
