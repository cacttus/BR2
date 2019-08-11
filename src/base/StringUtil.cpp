
#include "../base/StringUtil.h"
#include "../base/TypeConv.h"

#include "../base/Logger.h"
#include "../base/Gu.h"

namespace Game {

StringUtil::StringUtil()
{
}
StringUtil::~StringUtil()
{
}
int32_t StringUtil::compare(t_string a, t_string b)
{
    return a.compare(b);
}
bool StringUtil::charIsLetterOrNumber(char c)
{
    int r = isalnum(c);
    return r != 0;
}
bool StringUtil::isEmpty(t_string str)
{
    return str.compare(t_string("")) == 0;
}
bool StringUtil::isNotEmpty(t_string str)
{
    return !StringUtil::isEmpty(str);
}
char* StringUtil::getBufferCopy(t_string str)
{
    char* buf = new char[str.length() + 1];
    memcpy(buf, str.c_str(), str.length());
    buf[str.length()] = 0;
    return buf;
}
t_string StringUtil::createStringFromChars(char* c1, ...)
{
    t_string str;
    char* lu;

    if (c1 != NULL)
        str += *c1;

    va_list args;
    va_start(args, c1);
    do {
        lu = (char*)va_arg(args, char*);

        if (lu != NULL) {
            str += *lu;
        }

    } while (lu != NULL);
    va_end(args);

    return str;
}
// - Returns the Lowercase.
t_string StringUtil::uppercase(t_string in) {
    t_string out = "";
    for (size_t n = 0; n < in.length(); ++n) {
        out += toupper(in[n]);
    }

    return out;
}
// - Returns the Lowercase.
t_string StringUtil::lowercase(t_string in) {
    t_string str2(in);
    std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    return str2;
}
// - Returns the Lowercase.
t_string StringUtil::lowercase(const char* _in) {
    t_string in = t_string(_in);
    return lowercase(in);
}
// - Returns the Lowercase.
t_string StringUtil::uppercase(const char* _in)
{
    t_string in = t_string(_in);
    t_string out = "";
    for (size_t n = 0; n < in.length(); ++n) {
        out += toupper(in[n]);
    }
    in = out;
    return in;
}

bool StringUtil::isWspaceEx(char c)//iswspace with newlines and unprintable chars
{
    return (iswspace(c) || isNewline(c) || (c == '\0'));
}
bool StringUtil::isNewline(char c)
{
    return ((c == '\r') || (c == '\n'));
}
t_string StringUtil::trimBeg(t_string s) {
    t_string e = s;
    while (e.length() && isWspaceEx(e.at(0))) {
        if (e.length() == 1) {
            e = "";
            break;
        }
        else {
            e = e.substr(1, e.length() - 1);
        }
    }
    return e;
}
t_string StringUtil::trimEnd(t_string s) {
    t_string e = s;
    while (e.length() && isWspaceEx(e.at(e.length() - 1))) {
        if (e.length() == 1)
        {
            e = "";
            break;
        }
        else {
            e = e.substr(0, e.length() - 1);
        }
    }
    return e;
}
t_string StringUtil::trim(t_string s)
{
    t_string e;
    e = StringUtil::trimBeg(s);
    e = StringUtil::trimEnd(e);

    return e;
}
t_string StringUtil::stripDoubleQuotes(t_string lc)
{
    return stripAllChars(lc, '\"');
}
t_string StringUtil::trimDoubleQuotes(t_string lc)
{
    return trim(lc, '\"');
}
t_string StringUtil::stripAllChars(t_string lc, char ch)
{
    t_string x = lc;
    t_string::size_type last = 0;
    t_string::size_type off = 0;
    while (1)
    {
        off = x.find(ch, last);
        last = off;
        if (off == std::string::npos)
        {
            break;
        }
        else if (off != 0)
        {
            if (x.at(off - 1) != '\\')
            {
                x.erase(x.begin() + off);
            }
            else
                last = off + 1;
        }
        else
        {
            x = x.substr(off + 1, x.length() - off + 1);
        }
    }

    return x;
}
t_string StringUtil::getZeroPaddedNumber(int32_t number, int32_t numberOfZeros)
{
    return getLeftPaddedNumber(number, numberOfZeros, '0');
}
t_string StringUtil::getLeftPaddedNumber(int32_t number, int32_t numberOfChars, char padChar)
{
    return getPaddedNumber(number, numberOfChars, padChar);
}
/**
*    @fn
*    @brief
*    number - the number to print
*    numberOfChars - the max width of the buffer with given padding
*    padChar -pad char
*    leftPad - if true - number is right aligned, false and it will appear to the left
*    expand - If the number of digits is greater than mmaxNumberOfChars, expand the buffer if true
*            if false, cut off the number to the size of the buffer.
*/
t_string StringUtil::getPaddedNumber(int32_t number, int32_t maxNumberOfChars, char padChar, bool rightAlignNumber, bool expand)
{
    t_string ret;
    char* cbuf;
    int32_t nDigits;

    nDigits = MathUtils::getNumberOfDigits(number);

    if (nDigits > maxNumberOfChars)
        if (expand)
            maxNumberOfChars = nDigits;

    // fancy printf
    cbuf = new char[maxNumberOfChars + 1];
    memset(cbuf, 0, maxNumberOfChars + 1);

    if (rightAlignNumber == (bool)true)
        snprintf(cbuf, maxNumberOfChars + 1, "%0*d", maxNumberOfChars, number);
    else    //right pad
        snprintf(cbuf, maxNumberOfChars + 1, "%-0*d", maxNumberOfChars, number);

    ret = t_string(cbuf);
    delete[] cbuf;

    // replace
    if (nDigits < maxNumberOfChars)
    {
        if (rightAlignNumber == (bool)true)
            std::replace(ret.begin(), ret.begin() + (ret.length() - nDigits), '0', padChar);
        else
            std::replace(ret.begin() + nDigits, ret.end(), '0', padChar);
    }
    return ret;
}
t_string StringUtil::repeat(t_string strToRepeat, int32_t nRepeats)
{
    t_string ret;
    for (int32_t i = 0; i < nRepeats; ++i)
        ret.append(strToRepeat);
    return ret;
}
t_string StringUtil::empty()
{
    return emptyString();
}
t_string StringUtil::emptyString()
{
    return t_string("");
}
bool StringUtil::equals(t_string a, t_string b)
{
    return StringUtil::compare(a, b) == 0;
}
bool StringUtil::doesNotEqual(t_string a, t_string b)
{
    return !StringUtil::equals(a, b);
}
bool StringUtil::equalsi(t_string a, t_string b)
{
    if (a.length() != b.length()) {
        return false;
    }
    for (size_t i = 0; i < a.length(); ++i) {
        if (toupper(a[i]) != toupper(b[i])) {
            return false;
        }
    }
    return true;
}
t_string StringUtil::replaceAll(t_string str, char charToRemove, char charToAdd)
{
    t_string::size_type x = 0;
    t_string ret = str;
    t_string ch;
    ch += charToAdd;
    while ((x = ret.find(charToRemove, x)) != t_string::npos)
    {
        ret.replace(x, 1, ch);
        x += ch.length();
    }
    return ret;
}
t_string StringUtil::replaceAll(t_string str, t_string strToRemove, t_string strToAdd)
{
    t_string::size_type x = 0;
    t_string ret = str;
    while ((x = ret.find(strToRemove, x)) != t_string::npos)
    {
        ret.replace(x, strToRemove.length(), strToAdd);
        x += strToAdd.length();
    }
    return ret;
}
t_string StringUtil::trim(t_string astr, char trimch)
{
    t_string str = astr;
    str = trimBeg(str, trimch);
    str = trimEnd(str, trimch);
    return str;
}
t_string StringUtil::trimBeg(t_string astr, char trimch)
{
    t_string str = astr;
    while (str.length() && str.at(0) == trimch) {
        str = str.substr(1, str.length() - 1);
    }
    return str;
}
t_string StringUtil::trimEnd(t_string astr, char trimch)
{
    t_string str = astr;
    while (str.length() && str.at(str.length() - 1) == trimch) {
        str = str.substr(0, str.length() - 1);
    }
    return str;
}
t_string StringUtil::addCommasToNumber(t_string __in_ str)
{
    t_string ret;

    if (str.length() <= 3)
        return str;

    // we start x at 1 so mod works
    // we use >=1 instead of -1 because we don't want to append commas to
    // the beginning fo the string
    for (int n = (int)str.length() - 1, ix = 0; n >= 1; n--, ix++)
    {

        ret = std::string(str[n], 1) + ret;
        if (ix == 2)
        {
            ret = t_string(",") + ret;
            ix = -1;
        }
    }

    // append final digit
    ret = t_string(str[0], 1) + ret;

    return ret;
}
t_string StringUtil::toHex(int value, bool bIncludePrefix)
{
    std::stringstream ss;
    if (bIncludePrefix)
        ss << "0x" << std::hex << value;
    else
        ss << std::hex << value;
    return ss.str();
}
t_string StringUtil::toDec(int value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}
bool StringUtil::isDelim(char c, t_string delims)
{
    return (delims.find(c) != t_string::npos);
}
bool StringUtil::isChar(char c)
{
    int r = isalnum(c);
    return r != 0;
}
bool StringUtil::isWs(char c)
{
    return (
        (c == ' ') ||
        (c == '\t') ||
        (c == '\r') ||
        (c == '\n')
        );
}
bool StringUtil::isWsExceptNewline(char c)
{
    return (
        (c == ' ') ||
        (c == '\t') ||
        (c == '\r')
        );
}
t_string StringUtil::getEscapedCharLiteral(char c)
{
    if (c <= 8) {
        return TypeConv::intToStr(c);
    }

    if (c == '\a') return "\a";
    else if (c == '\b') return "\b";
    else if (c == '\t') return "\t";
    else if (c == '\n') return "\n";
    else if (c == '\v') return "\v";
    else if (c == '\f') return "\f";
    else if (c == '\r') return "\r";
    else if (c == '\"') return "\"";
    else if (c == '\'') return "\'";
    else if (c == '\?') return "\?";
    else if (c == '\\') return "\\";

    return Stz "Invalid escape char '" + c + "'";
    //if(c=='\x') return "\a";
    //if(c=='\u') return "\a";
    //if(c=='\U') return "\a";
    //if(c=='\x') return "\a";
}
/**
*    @fn
*    @brief Split the string with a delim.
*/
std::vector<t_string> StringUtil::split(t_string in, char del)
{
    std::vector<t_string> ret;
    split(in, del, ret);
    return ret;
}
void StringUtil::split(t_string in, char del, std::vector<t_string>& __out_ ret)
{
    t_string tbuf = "";
    for (size_t n = 0; n < in.length(); ++n)
    {
        if (in[n] == del)
        {
            ret.push_back(tbuf);
            tbuf = "";
        }
        else
            tbuf += in[n];
    }
    if (tbuf.length())
        ret.push_back(tbuf);
}
std::vector<t_string> StringUtil::split(t_string in, std::vector<char>& dels)
{
    std::vector<t_string> v;
    split(in, dels, v);
    return v;
}
void StringUtil::split(t_string __in_ in, const std::vector<char>& __in_ dels, std::vector<t_string>& __out_ ret)
{
    t_string tbuf = "";
    //std::vector<t_string> ret;
    bool bDel;
    for (size_t n = 0; n < in.length(); ++n)
    {
        bDel = (bool)false;
        for (size_t idel = 0; idel < dels.size(); ++idel)
        {
            if ((in[n] == dels[idel]) && (tbuf.length() > 0))
            {
                ret.push_back(tbuf);
                tbuf = "";
                bDel = true;
            }
        }
        if (bDel == (bool)false)
            tbuf += in[n];
    }
    if (tbuf.length()) {
        ret.push_back(tbuf);
    }
}
t_string::size_type StringUtil::findFirstOf(t_string sin, std::vector<char>& chars)
{
    t_string::size_type ret = t_string::npos;
    for (size_t i = 0; i < chars.size(); ++i)
    {
        ret = sin.find(chars[i]);
        if (ret != t_string::npos) {
            return ret;
        }
    }
    return ret;
}
t_string StringUtil::removeNewline(t_string str) {
    t_string out = str;

    if (out.length() == 0) {
        return out;
    }
    if (out[out.length() - 1] == '\0') {
        out = out.substr(0, out.length() - 1);
    }
    if (out.length() == 0) {
        return out;
    }
    if (out[out.length() - 1] == '\n') {
        out = out.substr(0, out.length() - 1);
    }
    if (out.length() == 0) {
        return out;
    }
    if (out[out.length() - 1] == '\r') {
        out = out.substr(0, out.length() - 1);
    }

    return out;
}

t_string StringUtil::appendLine(t_string str, t_string toAppend) {
    str = str+ toAppend+ "\r\n";
    return str;
}
t_string StringUtil::generate() {
    //The string is unique based on std::chrono microseconds.
    t_timeval tv = Gu::getMicroSeconds();
    t_string ret;
    size_t size = sizeof(t_timeval);
    char* c = (char*)&tv;
    for (size_t i = 0; i < size; ++i) {
        ret += c[i];
    }
    return ret;
}
bool StringUtil::contains(t_string a, t_string b) {
    return a.find(b) != std::string::npos;
}
t_string StringUtil::format(t_string aft, ...) {
    t_string strRet;
    va_list args;
    va_start(args, aft);
    strRet = formatVa(aft, args);
    va_end(args);
    return strRet;
}
t_string StringUtil::formatVa(t_string aft, va_list args) {
    t_string strRet = "";
    //he vsnprintf function returns the number of characters written, not counting the terminating null character.
    int nCount = _vsnprintf(nullptr, 0, aft.c_str(), args);

    if (nCount == 0) {
        //Empty, no format
    }
    else {
        std::unique_ptr<char[]> tmp(new char[nCount]);
        _vsnprintf(tmp.get(), nCount, aft.c_str(), args);
        strRet = t_string((char*)tmp.get(), nCount);
    }

    return strRet;
}
t_string StringUtil::join(t_string delim, std::vector<t_string>& strings) {
    t_string ret = "";
    t_string del = "";

    for (t_string st : strings) {
        ret = del + st;
        del = delim;
    }

    return ret;
}
/**
*   @fn tabify
*   @brief Add tabs or spaces to a string.
*   @param use_spaces - Use space instead of tab (\t)
*   @param tab_newlines - Tab the string on newlines.  If False only the beginning of the string is tabified (kind of useless).
*/
t_string StringUtil::tabify(t_string str_to_tabify, int number_of_tabs, bool use_spaces, bool tab_newlines, LineBreak::e lineBreak) {
    t_string ret = str_to_tabify;

    t_string linebreak = "";
    if (lineBreak == LineBreak::e::DOS) {
        linebreak = "\r\n";
    }
    else {
        linebreak = "\n";
    }

    t_string tabchar = "\t";
    if (use_spaces) {
        tabchar = " ";
    }

    t_string tabs = StringUtil::repeat(tabchar, number_of_tabs);

    ret = StringUtil::replaceAll(ret, linebreak, tabs);
    return ret;
}
t_string StringUtil::wStrToStr(std::wstring wstr) {
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    std::string str = converter.to_bytes(wstr);
    return str;

}

}//nS GAME