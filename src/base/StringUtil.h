/**
*  @file StringUtil.h
*  @date August 23, 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __STRING_286311063321698159639017_H__
#define __STRING_286311063321698159639017_H__

#include "../base/BaseHeader.h"
#include "../math/MathHeader.h"

namespace Game {

/**
*    @class String
*    @brief String util.
*/
class StringUtil : public VirtualMemory {
public:
  StringUtil();
  VIRTUAL ~StringUtil() DOES_NOT_OVERRIDE;
  static int32_t compare(string_t a, string_t b);
  static bool charIsLetterOrNumber(char c);
  static bool isEmpty(string_t str);
  static bool isNotEmpty(string_t str);
  static char* getBufferCopy(string_t str);
  static string_t createStringFromChars(char* c1, ...);
  static string_t uppercase(const string_t in);
  static string_t lowercase(const  string_t in);
  static string_t lowercase(const char* _in);
  static string_t uppercase(const char* _in);
  static std::vector<string_t> split(string_t in, char del);
  static void split(string_t in, char del, std::vector<string_t>& __out_ ret);
  static std::vector<string_t> split(string_t in, std::vector<char>& dels);
  static void split(string_t __in_ in, const std::vector<char>& __in_ dels, std::vector<string_t>& __out_ ret);
  static string_t::size_type findFirstOf(string_t sin, std::vector<char>& chars);
  static bool isWspaceEx(char c);
  static bool isNewline(char c);
  static string_t stripDoubleQuotes(string_t lc);
  static string_t stripAllChars(string_t lc, char ch);
  static string_t getZeroPaddedNumber(int32_t number, int32_t numberOfZeros);
  static string_t getLeftPaddedNumber(int32_t number, int32_t numberOfChars, char padChar);
  static string_t getPaddedNumber(int32_t number, int32_t maxNumberOfChars, char padChar, bool rightAlignNumber = true, bool expand = true);
  static string_t repeat(string_t strToRepeat, int32_t nRepeats);
  static string_t empty();
  static string_t emptyString();
  static bool equals(string_t a, string_t b);
  static bool doesNotEqual(string_t a, string_t b);
  static bool equalsi(string_t a, string_t b);
  static string_t replaceAll(string_t str, char charToRemove, char charToAdd);
  static string_t replaceAll(string_t str, string_t strToRemove, string_t strToAdd);
  static string_t trim(string_t s);
  static string_t trimBeg(string_t s);
  static string_t trimEnd(string_t s);
  static string_t trimDoubleQuotes(string_t lc);
  static string_t trim(string_t astr, char trimch);
  static string_t trimBeg(string_t astr, char trimch);
  static string_t trimEnd(string_t astr, char trimch);
  static string_t addCommasToNumber(string_t __in_ str);
  static string_t toHex(int value, bool bIncludePrefix);
  static string_t toDec(int value);
  static string_t getEscapedCharLiteral(char c);
  static bool isDelim(char c, string_t delims);
  static bool isChar(char c);
  static bool isWs(char c);
  static bool isWsExceptNewline(char c);
  static string_t removeNewline(string_t str);
  static string_t appendLine(string_t str, string_t toAppend);
  static string_t generate();
  static bool contains(string_t a, string_t b);
  static string_t format(string_t fmt, ...);
  static string_t formatVa(string_t fmt, va_list args);
  static string_t join(string_t delim, std::vector<string_t>& strings);
  static string_t tabify(string_t str_to_tabify, int number_of_tabs, bool use_spaces = true, bool tab_newlines = true, LineBreak lineBreak = LineBreak::DOS);
  static string_t wStrToStr(std::wstring str);
};
#define StrLn(a,...) TStr(a,__VA_ARGS__,"\n");

































}

#endif