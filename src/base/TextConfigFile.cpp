#include "../base/TextConfigFile.h"
#include "../base/Logger.h"
#include "../base/Exception.h"

namespace Game {
TextConfigFile::TextConfigFile(t_string loc) : TextDataFile(loc) {

}
TextConfigFile::~TextConfigFile() {

}
t_string TextConfigFile::getCleanToken(std::vector<t_string>& tokens, int& iind) {
  //**NOTE be care ful with this function when calling in constructors  C++ constructors don't guarantee parameter order, thus
  //you may end up with invalid parameters.
  t_string ret = getCleanTokenInd(tokens, iind);
  iind++;
  return ret;
}
t_string TextConfigFile::getCleanTokenInd(std::vector<t_string>& tokens, int iind) {
  t_string ret;
  //MSVC default exception handling was fucking ignoring this.  REALLY?
  if ((size_t)iind >= tokens.size()) {
    parseErr(Stz "Token index '" + iind + "' out of range.", true, true); //throw
  }
  ret = StringUtil::stripDoubleQuotes(tokens[iind]);
  ret = StringUtil::trim(ret);

  return ret;
}
void TextConfigFile::parseErr(const t_string& str, bool bDebugBreak, bool bFatal) {
  t_string strhead = Stz "Error: '" + _fileLoc + "': line " + _iCurrentParseLine + "\r\n  ";
  t_string str2 = strhead + str;
  //Throw this if you wnt to have an error in your file.
  if (bFatal) {
    BRThrowException(str2);
  }
  else {
    BRLogError(str2);
    if (bDebugBreak) {
      Gu::debugBreak();
    }
  }
}
bool TextConfigFile::validateArguments(t_string& arg, int32_t count) {
  if (_vecTokens.size() != count) {
    displayError("While parsing '" + _fileLoc + "' invalid number of arguments for '" + arg + "'. Got " + _vecTokens.size() + " expected " + count + ".");
  }
  return true;
}

void TextConfigFile::displayError(const t_string& errMsg, bool bThrow) {
  t_string strErr = Stz "Data File Parse Error:\r\n  " + errMsg + "\r\n   File: " + _fileLoc + "\r\n   Line:" + _iCurrentParseLine + "\r\n";
  if (bThrow) {
    BRThrowException(strErr);
  }
}
bool TextConfigFile::lcmp(t_string& tok0, const char* ch, int32_t validateArgCount) {
  bool ret;

  ret = StringUtil::equalsi(tok0, t_string(ch));//(StringUtil::lowercase(tok0) == StringUtil::lowercase(t_string(ch)));

  if (validateArgCount >= 0) {
    if (ret == true) {
      validateArguments(tok0, validateArgCount);
    }
  }

  return ret;
}
bool TextConfigFile::cmp(t_string& tok0, const char* ch, int32_t validateArgCount) {
  bool ret = tok0 == t_string(ch);

  if (validateArgCount >= 0) {
    if (ret == true) {
      validateArguments(tok0, validateArgCount);
    }
  }

  return ret;
}



}//ns Game
