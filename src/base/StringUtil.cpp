
#include "../base/StringUtil.h"
#include "../base/TypeConv.h"

#include "../base/Logger.h"
#include "../base/Gu.h"

//For wstring conversion
#include <locale>
#include <codecvt>
#include <string>


namespace BR2 {

StringUtil::StringUtil()
{
}
StringUtil::~StringUtil()
{
}
int32_t StringUtil::compare(string_t a, string_t b)
{
    return a.compare(b);
}
bool StringUtil::charIsLetterOrNumber(char c)
{
    int r = isalnum(c);
    return r != 0;
}
bool StringUtil::isEmpty(string_t str)
{
    return str.compare(string_t("")) == 0;
}
bool StringUtil::isNotEmpty(string_t str)
{
    return !StringUtil::isEmpty(str);
}
char* StringUtil::getBufferCopy(string_t str)
{
    char* buf = new char[str.length() + 1];
    memcpy(buf, str.c_str(), str.length());
    buf[str.length()] = 0;
    return buf;
}
string_t StringUtil::createStringFromChars(char* c1, ...)
{
    string_t str;
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
string_t StringUtil::uppercase(const string_t in) {
    string_t out = "";
    for (size_t n = 0; n < in.length(); ++n) {
        out += toupper(in[n]);
    }

    return out;
}
// - Returns the Lowercase.
string_t StringUtil::lowercase(const string_t in) {
    string_t str2(in);
    std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    return str2;
}
// - Returns the Lowercase.
string_t StringUtil::lowercase(const char* _in) {
    string_t in = string_t(_in);
    return lowercase(in);
}
// - Returns the Lowercase.
string_t StringUtil::uppercase(const char* _in)
{
    string_t in = string_t(_in);
    string_t out = "";
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
string_t StringUtil::trimBeg(string_t s) {
    string_t e = s;
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
string_t StringUtil::trimEnd(string_t s) {
    string_t e = s;
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
string_t StringUtil::trim(string_t s)
{
    string_t e;
    e = StringUtil::trimBeg(s);
    e = StringUtil::trimEnd(e);

    return e;
}
string_t StringUtil::stripDoubleQuotes(string_t lc)
{
    return stripAllChars(lc, '\"');
}
string_t StringUtil::trimDoubleQuotes(string_t lc)
{
    return trim(lc, '\"');
}
string_t StringUtil::stripAllChars(string_t lc, char ch)
{
    string_t x = lc;
    string_t::size_type last = 0;
    string_t::size_type off = 0;
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
string_t StringUtil::getZeroPaddedNumber(int32_t number, int32_t numberOfZeros)
{
    return getLeftPaddedNumber(number, numberOfZeros, '0');
}
string_t StringUtil::getLeftPaddedNumber(int32_t number, int32_t numberOfChars, char padChar)
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
string_t StringUtil::getPaddedNumber(int32_t number, int32_t maxNumberOfChars, char padChar, bool rightAlignNumber, bool expand)
{
    string_t ret;
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

    ret = string_t(cbuf);
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
string_t StringUtil::repeat(string_t strToRepeat, int32_t nRepeats)
{
    string_t ret;
    for (int32_t i = 0; i < nRepeats; ++i)
        ret.append(strToRepeat);
    return ret;
}
string_t StringUtil::empty()
{
    return emptyString();
}
string_t StringUtil::emptyString()
{
    return string_t("");
}
bool StringUtil::equals(string_t a, string_t b)
{
    return StringUtil::compare(a, b) == 0;
}
bool StringUtil::doesNotEqual(string_t a, string_t b)
{
    return !StringUtil::equals(a, b);
}
bool StringUtil::equalsi(string_t a, string_t b)
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
string_t StringUtil::replaceAll(string_t str, char charToRemove, char charToAdd)
{
    string_t::size_type x = 0;
    string_t ret = str;
    string_t ch;
    ch += charToAdd;
    while ((x = ret.find(charToRemove, x)) != string_t::npos)
    {
        ret.replace(x, 1, ch);
        x += ch.length();
    }
    return ret;
}
string_t StringUtil::replaceAll(string_t str, string_t strToRemove, string_t strToAdd)
{
    string_t::size_type x = 0;
    string_t ret = str;
    while ((x = ret.find(strToRemove, x)) != string_t::npos)
    {
        ret.replace(x, strToRemove.length(), strToAdd);
        x += strToAdd.length();
    }
    return ret;
}
string_t StringUtil::trim(string_t astr, char trimch)
{
    string_t str = astr;
    str = trimBeg(str, trimch);
    str = trimEnd(str, trimch);
    return str;
}
string_t StringUtil::trimBeg(string_t astr, char trimch)
{
    string_t str = astr;
    while (str.length() && str.at(0) == trimch) {
        str = str.substr(1, str.length() - 1);
    }
    return str;
}
string_t StringUtil::trimEnd(string_t astr, char trimch)
{
    string_t str = astr;
    while (str.length() && str.at(str.length() - 1) == trimch) {
        str = str.substr(0, str.length() - 1);
    }
    return str;
}
string_t StringUtil::addCommasToNumber(string_t __in_ str)
{
    string_t ret;

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
            ret = string_t(",") + ret;
            ix = -1;
        }
    }

    // append final digit
    ret = string_t(str[0], 1) + ret;

    return ret;
}
string_t StringUtil::toHex(int value, bool bIncludePrefix)
{
    std::stringstream ss;
    if (bIncludePrefix)
        ss << "0x" << std::hex << value;
    else
        ss << std::hex << value;
    return ss.str();
}
string_t StringUtil::toDec(int value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}
bool StringUtil::isDelim(char c, string_t delims)
{
    return (delims.find(c) != string_t::npos);
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
string_t StringUtil::getEscapedCharLiteral(char c)
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
std::vector<string_t> StringUtil::split(string_t in, char del)
{
    std::vector<string_t> ret;
    split(in, del, ret);
    return ret;
}
void StringUtil::split(string_t in, char del, std::vector<string_t>& __out_ ret)
{
    string_t tbuf = "";
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
std::vector<string_t> StringUtil::split(string_t in, std::vector<char>& dels)
{
    std::vector<string_t> v;
    split(in, dels, v);
    return v;
}
void StringUtil::split(string_t __in_ in, const std::vector<char>& __in_ dels, std::vector<string_t>& __out_ ret)
{
    string_t tbuf = "";
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
string_t::size_type StringUtil::findFirstOf(string_t sin, std::vector<char>& chars)
{
    string_t::size_type ret = string_t::npos;
    for (size_t i = 0; i < chars.size(); ++i)
    {
        ret = sin.find(chars[i]);
        if (ret != string_t::npos) {
            return ret;
        }
    }
    return ret;
}
string_t StringUtil::removeNewline(string_t str) {
    string_t out = str;

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

string_t StringUtil::appendLine(string_t str, string_t toAppend) {
    str = str+ toAppend+ "\r\n";
    return str;
}
string_t StringUtil::generate() {
    //The string is unique based on std::chrono microseconds.
    t_timeval tv = Gu::getMicroSeconds();
    string_t ret;
    size_t size = sizeof(t_timeval);
    char* c = (char*)&tv;
    for (size_t i = 0; i < size; ++i) {
        ret += c[i];
    }
    return ret;
}
bool StringUtil::contains(string_t a, string_t b) {
    return a.find(b) != std::string::npos;
}
string_t StringUtil::format(string_t aft, ...) {
    string_t strRet;
    va_list args;
    va_start(args, aft);
    strRet = formatVa(aft, args);
    va_end(args);
    return strRet;
}
string_t StringUtil::formatVa(string_t aft, va_list args) {
    string_t strRet = "";
    //he vsnprintf function returns the number of characters written, not counting the terminating null character.
    int nCount = _vsnprintf(nullptr, 0, aft.c_str(), args);

    if (nCount == 0) {
        //Empty, no format
    }
    else {
        std::unique_ptr<char[]> tmp(new char[nCount]);
        _vsnprintf(tmp.get(), nCount, aft.c_str(), args);
        strRet = string_t((char*)tmp.get(), nCount);
    }

    return strRet;
}
string_t StringUtil::join(string_t delim, std::vector<string_t>& strings) {
    string_t ret = "";
    string_t del = "";

    for (string_t st : strings) {
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
string_t StringUtil::tabify(string_t str_to_tabify, int number_of_tabs, bool use_spaces, bool tab_newlines, LineBreak lineBreak) {
    string_t ret = str_to_tabify;

    string_t linebreak = "";
    if (lineBreak == LineBreak::DOS) {
        linebreak = "\r\n";
    }
    else {
        linebreak = "\n";
    }

    string_t tabchar = "\t";
    if (use_spaces) {
        tabchar = " ";
    }

    string_t tabs = StringUtil::repeat(tabchar, number_of_tabs);

    ret = StringUtil::replaceAll(ret, linebreak, tabs);
    return ret;
}
string_t StringUtil::wStrToStr(std::wstring wstr) {
    using convert_type = std::codecvt<wchar_t, char, std::mbstate_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    std::string str = converter.to_bytes(wstr);
    return str;
}
wstring_t StringUtil::strToWStr(std::string str) {
  //oh..stackoverflow
  //https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t/26914562
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(str);
  return wide;
}

}//nS GAME