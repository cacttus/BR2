#include "../base/PoundFile.h"
#include "../base/StringUtil.h"
#include "../base/Logger.h"

namespace Game {
;
PoundFile::PoundFile()
{
}
PoundFile::PoundFile(t_string& loc) :
    BaseDataFile(loc)
{
}
PoundFile::~PoundFile()
{
    _vecTokens.clear();
}

t_string PoundFile::getCleanToken(std::vector<t_string>& tokens, int& iind) {
    //**NOTE be care ful with this function when calling in constructors  C++ constructors don't guarantee parameter order, thus
    //you may end up with invalid parameters.
    t_string ret = getCleanTokenInd(tokens, iind);
    iind++;
    return ret;
}
t_string PoundFile::getCleanTokenInd(std::vector<t_string>& tokens, int iind) {
    t_string ret;
    //MSVC default exception handling was fucking ignoring this.  REALLY?
    if((size_t)iind >= tokens.size()) {
        parseErr(TStr("Token index '",iind,"' out of range."), true, true); //throw
    }
    ret = StringUtil::stripDoubleQuotes(tokens[iind]);
    ret = StringUtil::trim(ret);

    return ret;
}

void PoundFile::parse(char* buf)
{
    /*
    Note; if you add more delimiters make sure to update isEscapeCharacter()
    */
    char c;
    ptr = buf;
    t_string token;
    int32_t _nStringProfile = 0;
    int32_t _nParenProfile = 0;
    int32_t _nBracketProfile = 0;

    _iCurrentParseLine = 1;

    bool _bEscape = false;

    while (ptr)
    {
        c = *ptr;

        if (c == 0)
            b_eof = true;

        if (b_eof)
        {
            //if we're EOF then parse dumb token
            if (token.length())
                _vecTokens.push_back(t_string(token));

            token = "";

            if (_vecTokens.size())
                pkp(_vecTokens);// - Parse the given token buffer

            break;
        }
        else if (c == '"')
        {
            _nStringProfile = ~_nStringProfile;

            token += c;
            inc();
        }
        else if (_nStringProfile)
        {
            //Inside string - do not parse other delimiters
            token += c;
            inc();
        }
        else if (_bEscape)
        {
            if (!isEscapeCharacter(c))
                displayError(TStr("The given character '", StringUtil::getEscapedCharLiteral(c), "' is not a valid pound file escape character."));
            token += c;
            _bEscape = false;
            inc();
        }
        else if (c == '\\')
        {
            _bEscape = true;
            inc();
        }
        else if (c == '#')
        {
            if (token.length())
                _vecTokens.push_back(t_string(token));

            if (_vecTokens.size())
                pkp(_vecTokens);// - Parse the given token buffer

            _vecTokens.clear();
            eatLine(_iCurrentParseLine);
        }
        else if (c == '(')
        {
            _nParenProfile++;

            token += c;
            inc();
        }
        else if (c == ')')
        {
            _nParenProfile--;
            if (_nParenProfile < 0)
                displayError(TStr("Parentheses profile is invalid. You're missing a parentheses somewhere."));

            token += c;
            inc();
        }
        else if (c == '[')
        {
            _nBracketProfile++;

            token += c;
            inc();
        }
        else if (c == ']')
        {
            _nBracketProfile--;
            if (_nBracketProfile < 0)
                displayError(TStr("Bracket profile is invalid. You're missing a Bracket somewhere."));

            token += c;
            inc();
        }
        else if (c == '\n' || c == '\r')
        {
            if (_nStringProfile || _nParenProfile || _nBracketProfile)
                displayError(TStr("Parser error - you can't have multi-line strings \"\", braces [] or parentheses ()"));
            if (token.length())
                _vecTokens.push_back(t_string(token));
            token = "";
            if (_vecTokens.size())
                pkp(_vecTokens);// - Parse the given token buffer
            _vecTokens.clear();
            eatLine(_iCurrentParseLine);
            //_iCurrentParseLine++;
        }
        // - If we're whitespace but we also are within a string or bracket then do not skip it, add it to
        // the argument.
        else if (StringUtil::isWs(c) && !(_nStringProfile || _nParenProfile || _nBracketProfile))
        {
            if (token.length())
                _vecTokens.push_back(t_string(token));
            token = "";
            inc();
        }
        else
        {
            token += c;
            inc();
        }

    }
}
bool PoundFile::isEscapeCharacter(char c)
{
    return (c == '"')
        || (c == '\'')
        || (c == '[')
        || (c == ']')
        || (c == '(')
        || (c == ')')
        || (c == '#')
        ;
}
bool PoundFile::validateArguments(t_string& arg, int32_t count)
{
    if (_vecTokens.size() != count) {
        displayError(TStr("While parsing '", _fileLoc, "' invalid number of arguments for '", arg, "'. Got ", _vecTokens.size(), " expected ", count, "."));
    }
    return true;
}
void PoundFile::gatherArgs()
{
    for (size_t ixx = 0; ixx < TempArgsSize; ++ixx)
        if (_vecTokens.size() > ixx)
            _tempArgs[ixx] = _vecTokens[ixx];
}
void PoundFile::gatherArgs(t_string* _a, size_t _size)
{
    for (size_t ixx = 0; ixx < _size; ++ixx)
        if (_vecTokens.size() > ixx)
            _a[ixx] = _vecTokens[ixx];
}

vec3 PoundFile::rdVec3f(t_string* t, int32_t offset)
{
    vec3 v;
    v.x = TypeConv::strToFloat(t[offset + 0]);
    v.y = TypeConv::strToFloat(t[offset + 1]);
    v.z = TypeConv::strToFloat(t[offset + 2]);
    return v;
}
ivec3 PoundFile::rdVec3i(t_string* t, int32_t offset)
{
    ivec3 v;
    v.x = TypeConv::strToInt(t[offset + 0]);
    v.y = TypeConv::strToInt(t[offset + 1]);
    v.z = TypeConv::strToInt(t[offset + 2]);
    return v;
}
bool PoundFile::getYesOrNo(t_string& x) //Returns true if yes.
{
    bool yes = StringUtil::equalsi(x, "yes");
    bool no = StringUtil::equalsi(x, "no");
    if (!yes && !no)
        BroThrowException("Failed to parse yes/no value.");
    return yes;
}
t_string PoundFile::tryGetArg(int32_t arg)
{
    if ((int32_t)_vecTokens.size() > arg)
        return _vecTokens[arg];
    return StringUtil::emptyString();
}
void PoundFile::displayError(t_string& errMsg, bool bThrow)
{
    t_string strErr = TStr("Data File Parse Error:\r\n  ", errMsg, "\r\n   File: ", _fileLoc, "\r\n   Line:", _iCurrentParseLine, "\r\n");
    if (bThrow) {
        BroThrowException(strErr);
    }
}
void PoundFile::showMessageOnFailure(bool b)
{
    if (b == false)
        displayError(TStr("Condition failed."));
}
void PoundFile::parseErr(t_string str, bool bDebugBreak, bool bFatal) {
    t_string strhead = TStr("Error: '" , _fileLoc , "': line ", _iCurrentParseLine, "\r\n  ");
    str = TStr(strhead, str);
    //Throw this if you wnt to have an error in your file.
    if (bFatal) {
        BroThrowException(str);
    }
    else {
        BroLogError(str);
        if (bDebugBreak) {
            Gu::debugBreak();
        }
    }
}
mat4 PoundFile::parseMat4(t_string tok) {
    mat4 m;
    mat4::parse(tok, m);
    return m;
}
bool PoundFile::parsePrs(t_string tok, vec3& pos, vec4& rot, vec3& scl) {
    // - Parse csv pos3, quat4, scl3.

    bool bValid = true;
    size_t n = 0;
    char c;
    t_string val = "";
    int mat_ind = 0;

    int iind = 0;

    while (n < tok.length())
    {
        c = tok[n++];
        if (c == ',' || c == '\n' || n == tok.length())
        {
            float fVal = TypeConv::strToFloat(val);
            if (iind == 0) pos.x = fVal;
            else if (iind == 1) pos.y = fVal;
            else if (iind == 2) pos.z = fVal;
            else if (iind == 3) rot.x = fVal;
            else if (iind == 4) rot.y = fVal;
            else if (iind == 5) rot.z = fVal;
            else if (iind == 6) rot.w = fVal;
            else if (iind == 7) scl.x = fVal;
            else if (iind == 8) scl.y = fVal;
            else if (iind == 9) scl.z = fVal;
            else {
                parseErr(TStr("Input string '", tok, "'wasn't a valid PRS string.."));
                bValid = false;
            }
            iind++;
            val = "";
        }
        else if (isalnum(c) || c == '-' || c == '.' || c == '+' || c == 'e')
        {
            val += c;
        }
    }

    return bValid;
}

}//ns game
