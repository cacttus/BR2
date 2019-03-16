/**
*
*    @file StringUtil.h
*    @date August 23, 2010
*    @author Derek Page
*
*    © 2011 
*   1/8/18 - modified lowercase()
*
*/
#pragma once
#ifndef __STRING_286311063321698159639017_H__
#define __STRING_286311063321698159639017_H__

#include "../base/BaseHeader.h"
#include "../math/MathHeader.h"

namespace Game {;

/**
*    @class String
*    @brief String util.
*/
class StringUtil : public VirtualMemory {
public:
    StringUtil();
    VIRTUAL ~StringUtil() DOES_NOT_OVERRIDE;
    static int32_t compare(const t_string& a, const t_string& b);
    static bool charIsLetterOrNumber(char c);
    static bool isEmpty(const t_string& str);
    static bool isNotEmpty(const t_string& str);
    static char* getBufferCopy(const t_string& str);
    static t_string createStringFromChars(char* c1, ...);
    static t_string uppercase(const t_string& in );
    static t_string lowercase( const t_string& in ) ;
    static t_string lowercase( const char* _in ) ;
    static t_string uppercase( const char* _in ) ;
    static std::vector<t_string> split( const t_string& in, char del);
    static void split( const t_string& in, char del, std::vector<t_string>& __out_ ret);
    static std::vector<t_string> split( const t_string& in, std::vector<char>& dels);
    static void split( const t_string& __in_ in, const std::vector<char>& __in_ dels, std::vector<t_string>& __out_ ret);
    static t_string::size_type findFirstOf(const t_string& sin, std::vector<char>& chars);
    static bool isWspaceEx(char c);
    static bool isNewline(char c);
    static t_string stripDoubleQuotes(const t_string& lc);
    static t_string stripAllChars(const t_string& lc, char ch);
    static t_string getZeroPaddedNumber(int32_t number, int32_t numberOfZeros);
    static t_string getLeftPaddedNumber(int32_t number, int32_t numberOfChars, char padChar);
    static t_string getPaddedNumber(int32_t number, int32_t maxNumberOfChars, char padChar,  bool rightAlignNumber=true, bool expand=true);
    static t_string repeat(const t_string& strToRepeat, int32_t nRepeats);
    static t_string empty();
    static t_string emptyString();
    static bool equals(const t_string& a, const t_string& b);
    static bool doesNotEqual(const t_string& a, const t_string& b);
    static bool equalsi(const t_string& a, const t_string& b);
    static t_string replaceAll(const t_string& str, char charToRemove, char charToAdd);
    static t_string replaceAll(const t_string& str, const t_string& strToRemove, const t_string& strToAdd);
    static t_string trim(const t_string& s);
    static t_string trimBeg(const t_string& s);
    static t_string trimEnd(const t_string& s);
    static t_string trimDoubleQuotes(const t_string& lc);
    static t_string trim(const t_string& astr, char trimch);
    static t_string trimBeg(const t_string& astr, char trimch);
    static t_string trimEnd(const t_string& astr, char trimch);
    static t_string addCommasToNumber(const t_string& __in_ str);
    static t_string toHex(int value, bool bIncludePrefix);
    static t_string toDec(int value);
    static t_string getEscapedCharLiteral(char c);
    static bool isDelim(char c, const t_string& delims);
    static bool isChar(char c);
    static bool isWs(char c);
    static bool isWsExceptNewline(char c);
    static t_string removeNewline(const t_string& str);
    static t_string appendLine(t_string& str, t_string& toAppend);
    static t_string appendLine(t_string& str, const char* toAppend);
    static t_string generate();
    static bool contains(const t_string& a, const t_string& b);
    static t_string format(t_string fmt, ...);
    static t_string formatVa(t_string fmt, va_list args);

};
#define StrLn(a,...) TStr(a,__VA_ARGS__,"\n");

































} 

#endif