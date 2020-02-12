#include "../base/TextParser.h"
#include "../base/Exception.h"

namespace BR2 {
TextParser::TextParser(char* data) {
  _ptr = data;
}
TextParser::~TextParser() {
}
bool TextParser::charIsWs() {
  return charIsWs(charAt());
}
bool TextParser::charIsWs(char c) {
  return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
}
bool TextParser::charIsEscape() {
  return charIsEscape(charAt());
}
bool TextParser::charIsEscape(char c) {
  return ((c == '\t') || (c == '\r') || (c == '\n'));
}
// - PTR OPERATOR
char TextParser::charAt() {
  return *_ptr;
}
char TextParser::nextChar() {
  if (_ptrState == ptr_eof) {
    throw new std::exception("Tried to get next Character from end of file.(nextChar)");
  }
  return *(_ptr + 1);
}
char TextParser::prevChar() {
  if (_ptrState == ptr_begin) {
    throw new Exception("Tried to get Previous character from beginning of file.", __LINE__, __FILE__);
  }
  return *(_ptr - 1);
}
//seeks one past the character
bool TextParser::seekPastChar(char c) {
  while (charAt() != c) {
    if (_ptrState == ptr_eof) {
      throw new Exception("Tried to get next Character from end of file. (seekPastChar)", __LINE__, __FILE__);
    }
    inc();
  }
  if (_ptrState == ptr_eof) {
    return true;
  }

  inc();
  return false;
}
/**
*    @fn seekToChars()
*    @brief Seeks to any of hte input chars and increments the pointer past it
*/
void TextParser::seekPastChars(std::vector<char> cv) {
  seekToChars(cv);
  inc();
}
/**
*    @fn seekToChars()
*    @brief Seek pointer until one of the specified characters is reached.
*/
void TextParser::seekToChars(std::vector<char> cv) {
  bool exit = false;
  while (1) {
    for (unsigned int i = 0; i < cv.size(); ++i) {
      if (charAt() == cv[i]) {
        exit = true;
        break;
      }
    }

    if (exit) {
      break;
    }

    if (_ptrState == ptr_eof) {
      throw new Exception("Tried to get next Character from end of file. (seekToChar)", __LINE__, __FILE__);
    }
    inc();
  }
}
/**
*    inc()
*    increment pointer
*/
bool TextParser::inc() {
  if (_bEof) {
    return eof(); //throw new Exception("Tried to parse file past EOF.",__LINE__,__FILE__); 
  }
  _ptr++;

  if (charAt() == '\r' && nextChar() == '\n') {
    _linenum++;
  }

  _ptrState = ptr_seek;

  return eof();
}
/**
*    dec()
*    decrement pointer
*/
void TextParser::dec() {
  if (_ptrState == ptr_begin) {
    throw new Exception("Tried to decrement the pointer at the beginning of the file.", __LINE__, __FILE__);
  }

  if (charAt() == '\r' && nextChar() == '\n') {
    _linenum--;
  }
  if (_bEof) {
    _bEof = false;
  }
  _ptr--;


  _ptrState = ptr_seek;
}
/**
*    eof()
*    return true if the poitner is at eof
*/
bool TextParser::eof() {
  //return (*_ptr) == 0;
  if (_bEof) {
    return true;
  }
  if (_bEof = (*(_ptr) == 0)) {
    _ptrState = ptr_eof;
  }
  return _bEof;
}

/**
*    eatws()
*    eats white space
*/
void TextParser::eatws() { while (charIsWs()) inc(); }
/**
*    eatBlockComment()
*    eats a /* comment.
*/
void TextParser::eatBlockComment() {
  if (charAt() == '/' && nextChar() == '*') {
    while (1) {
      if ((charAt() == '*') && (nextChar() == '/')) {
        if (inc()) {
          return;
        }
        break;
      }
      else if (inc()) return;
    }
  }
  else if (nextChar() == 0) {
    throw new Exception(" [Parser] Unmatched Comment Encountered at line ", __LINE__, __FILE__);
  }

}
/**
*    @fn eatLine()
*    @breif Eats a line until \n
*/
void TextParser::eatLine() {
  while (1) {
    if (charAt() == '\n') {
      if (inc()) return;
      break;
    }
    else if (inc()) {
      return;
    }
  }
}

/**
*    @fn eatBody
*    @brief Eat the body of a statement/function
*/
void TextParser::eatBody() {
  if (charAt() == '{') {
    while (1) {
      if (charAt() == '}') {
        inc();
        break;
      }
      else {
        inc();
      }
    }
  }
  else if (nextChar() == 0) {
    throw new Exception("Unmatched Comment Encountered at line ", __LINE__, __FILE__);
  }
}

void TextParser::eatLine(int32_t& lineCountToAddTo) {
  while (1) {
    if (charAt() == '\n') {
      if (inc()) {
        //EOF
        return;
      }
      lineCountToAddTo++;
      break;
    }
    else if (inc()) {
      //EOF
      return;
    }
  }
}





}//Ns Game