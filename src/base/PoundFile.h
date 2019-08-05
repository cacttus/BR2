/**
*
*    @file PoundFile.h
*    @date October 9, 2014
*    @author Derek Page
*
*    © 2014
*
*
*/
#pragma once
#ifndef __POUNDFILE_15666252306464270808067_H__
#define __POUNDFILE_15666252306464270808067_H__

#include "../base/TextConfigFile.h"
#include "../math/MathAll.h"

namespace Game {
/**
*    @class PoundFile
*    @brief A file that uses # as comments.
*       Moving this to XML. (8/4/19)
*/
class PoundFile : public TextConfigFile {
private:
    enum { TempArgsSize = 64 };
    t_string _tempArgs[TempArgsSize];

    virtual void parse(char* buf, int64_t filesize) override;
    virtual void preLoad() override { }  // Override for custom pre/post
    virtual void postLoad(bool success) override { } // Override for custom pre/post

    bool isEscapeCharacter(char c);

protected:
    t_string& getTempArg(size_t off) { return _tempArgs[off]; }
    t_string* getTempArgs() { return _tempArgs; }


    void gatherArgs();
    void gatherArgs(t_string* _a, size_t _size);
    vec3 rdVec3f(t_string* t, int32_t offset);
    ivec3 rdVec3i(t_string* t, int32_t offset);
    bool getYesOrNo(t_string& x);
    t_string tryGetArg(int32_t arg);

    void showMessageOnFailure(bool b);

public:

    mat4 parseMat4(t_string tok);
    bool parsePrs(t_string tok, vec3& pos, vec4& quat, vec3& scl);
    int32_t getCurrentParseLine() { return _iCurrentParseLine; }
    //+-- CTOR/DTOR --+
    PoundFile();
    PoundFile(t_string& loc);
    virtual ~PoundFile() override;
};

}//ns game



#endif
