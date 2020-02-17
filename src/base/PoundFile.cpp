#include "../base/PoundFile.h"
#include "../base/StringUtil.h"
#include "../base/Logger.h"

namespace BR2 {
PoundFile::PoundFile() {
}
PoundFile::PoundFile(string_t& loc) :
  TextConfigFile(loc) {
}
PoundFile::~PoundFile() {
  _vecTokens.clear();
}
void PoundFile::parse(char* buf, int64_t bufsize) {
  /*
  Note; if you add more delimiters make sure to update isEscapeCharacter()
  */
  char c;
  //char* ptr = buf;
  string_t token;
  int32_t _nStringProfile = 0;
  int32_t _nParenProfile = 0;
  int32_t _nBracketProfile = 0;

  _iCurrentParseLine = 1;

  bool _bEscape = false;

  TextParser tp(buf);

  while (tp.eof() == false) {
    c = tp.charAt();

    if (tp.eof()) {
      //if we're EOF then parse dumb token
      if (token.length()) {
        _vecTokens.push_back(string_t(token));
      }

      token = "";

      if (_vecTokens.size()) {
        pkp(_vecTokens);// - Parse the given token buffer
      }

      break;
    }
    else if (c == '"') {
      _nStringProfile = ~_nStringProfile;

      token += c;
      tp.inc();
    }
    else if (_nStringProfile) {
      //Inside string - do not parse other delimiters
      token += c;
      tp.inc();
    }
    else if (_bEscape) {
      if (!isEscapeCharacter(c)) {
        displayError(Stz "The given character '" + StringUtil::getEscapedCharLiteral(c) + "' is not a valid pound file escape character.");
      }
      token += c;
      _bEscape = false;
      tp.inc();
    }
    else if (c == '\\') {
      _bEscape = true;
      tp.inc();
    }
    else if (c == '#') {
      if (token.length()) {
        _vecTokens.push_back(string_t(token));
      }

      if (_vecTokens.size()) {
        pkp(_vecTokens);// - Parse the given token buffer
      }

      _vecTokens.clear();
      tp.eatLine(_iCurrentParseLine);
    }
    else if (c == '(') {
      _nParenProfile++;

      token += c;
      tp.inc();
    }
    else if (c == ')') {
      _nParenProfile--;

      if (_nParenProfile < 0) {
        displayError("Parentheses profile is invalid. You're missing a parentheses somewhere.");
      }

      token += c;
      tp.inc();
    }
    else if (c == '[') {
      _nBracketProfile++;

      token += c;
      tp.inc();
    }
    else if (c == ']') {
      _nBracketProfile--;

      if (_nBracketProfile < 0) {
        displayError("Bracket profile is invalid. You're missing a Bracket somewhere.");
      }

      token += c;
      tp.inc();
    }
    else if (c == '\n' || c == '\r') {
      if (_nStringProfile || _nParenProfile || _nBracketProfile) {
        displayError("Parser error - you can't have multi-line strings \"\", braces [] or parentheses ()");
      }

      if (token.length()) {
        _vecTokens.push_back(string_t(token));
      }

      token = "";

      if (_vecTokens.size()) {
        pkp(_vecTokens);// - Parse the given token buffer
      }

      _vecTokens.clear();
      tp.eatLine(_iCurrentParseLine);
      //_iCurrentParseLine++;
    }
    // - If we're whitespace but we also are within a string or bracket then do not skip it, add it to
    // the argument.
    else if (StringUtil::isWs(c) && !(_nStringProfile || _nParenProfile || _nBracketProfile)) {
      if (token.length()) {
        _vecTokens.push_back(string_t(token));
      }
      token = "";
      tp.inc();
    }
    else {
      token += c;
      tp.inc();
    }

  }
}
bool PoundFile::isEscapeCharacter(char c) {
  return (c == '"')
    || (c == '\'')
    || (c == '[')
    || (c == ']')
    || (c == '(')
    || (c == ')')
    || (c == '#')
    ;
}
void PoundFile::gatherArgs() {
  for (size_t ixx = 0; ixx < TempArgsSize; ++ixx)
    if (_vecTokens.size() > ixx)
      _tempArgs[ixx] = _vecTokens[ixx];
}
void PoundFile::gatherArgs(string_t* _a, size_t _size) {
  for (size_t ixx = 0; ixx < _size; ++ixx)
    if (_vecTokens.size() > ixx)
      _a[ixx] = _vecTokens[ixx];
}

vec3 PoundFile::rdVec3f(string_t* t, int32_t offset) {
  vec3 v;
  v.x = TypeConv::strToFloat(t[offset + 0]);
  v.y = TypeConv::strToFloat(t[offset + 1]);
  v.z = TypeConv::strToFloat(t[offset + 2]);
  return v;
}
ivec3 PoundFile::rdVec3i(string_t* t, int32_t offset) {
  ivec3 v;
  v.x = TypeConv::strToInt(t[offset + 0]);
  v.y = TypeConv::strToInt(t[offset + 1]);
  v.z = TypeConv::strToInt(t[offset + 2]);
  return v;
}
bool PoundFile::getYesOrNo(string_t& x) //Returns true if yes.
{
  bool yes = StringUtil::equalsi(x, "yes");
  bool no = StringUtil::equalsi(x, "no");
  if (!yes && !no) {
    Br2ThrowException("Failed to parse yes/no value.");
  }
  return yes;
}
string_t PoundFile::tryGetArg(int32_t arg) {
  if ((int32_t)_vecTokens.size() > arg) {
    return _vecTokens[arg];
  }
  return StringUtil::emptyString();
}

void PoundFile::showMessageOnFailure(bool b) {
  if (b == false) {
    displayError("Condition failed.");
  }
}

mat4 PoundFile::parseMat4(string_t tok) {
  mat4 m;
  mat4::parse(tok, m);
  return m;
}
bool PoundFile::parsePrs(string_t tok, vec3& pos, vec4& rot, vec3& scl) {
  // - Parse csv pos3, quat4, scl3.

  bool bValid = true;
  size_t n = 0;
  char c;
  string_t val = "";
  int mat_ind = 0;

  int iind = 0;

  while (n < tok.length()) {
    c = tok[n++];
    if (c == ',' || c == '\n' || n == tok.length()) {
      float fVal = TypeConv::strToFloat(val);
      if (iind == 0) { pos.x = fVal; }
      else if (iind == 1) { pos.y = fVal; }
      else if (iind == 2) { pos.z = fVal; }
      else if (iind == 3) { rot.x = fVal; }
      else if (iind == 4) { rot.y = fVal; }
      else if (iind == 5) { rot.z = fVal; }
      else if (iind == 6) { rot.w = fVal; }
      else if (iind == 7) { scl.x = fVal; }
      else if (iind == 8) { scl.y = fVal; }
      else if (iind == 9) { scl.z = fVal; }
      else {
        parseErr("Input string '" + tok + "'wasn't a valid PRS string..", _iCurrentParseLine);
        bValid = false;
      }
      iind++;
      val = "";
    }
    else if (isalnum(c) || c == '-' || c == '.' || c == '+' || c == 'e') {
      val += c;
    }
  }

  return bValid;
}

}//ns BR2
