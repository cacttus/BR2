/**
*  @file PoundFile.h
*  @date October 9, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __POUNDFILE_15666252306464270808067_H__
#define __POUNDFILE_15666252306464270808067_H__

#include "../base/TextConfigFile.h"
#include "../math/MathAll.h"

namespace BR2 {
/**
*  @class PoundFile
*  @brief A file that uses # as comments.
*       Moving this to XML. (8/4/19)
*/
class PoundFile : public TextConfigFile {
public:
  PoundFile();
  PoundFile(string_t& loc);
  virtual ~PoundFile() override;

  mat4 parseMat4(string_t tok);
  bool parsePrs(string_t tok, vec3& pos, vec4& quat, vec3& scl);
  int32_t getCurrentParseLine() { return _iCurrentParseLine; }

protected:
  string_t& getTempArg(size_t off) { return _tempArgs[off]; }
  string_t* getTempArgs() { return _tempArgs; }


  void gatherArgs();
  void gatherArgs(string_t* _a, size_t _size);
  vec3 rdVec3f(string_t* t, int32_t offset);
  ivec3 rdVec3i(string_t* t, int32_t offset);
  bool getYesOrNo(string_t& x);
  string_t tryGetArg(int32_t arg);

  void showMessageOnFailure(bool b);
private:
  enum { TempArgsSize = 64 };
  string_t _tempArgs[TempArgsSize];

  virtual void parse(char* buf, int64_t filesize) override;
  virtual void preLoad() override {}  // Override for custom pre/post
  virtual void postLoad(bool success) override {} // Override for custom pre/post

  bool isEscapeCharacter(char c);
};

}//ns BR2



#endif
