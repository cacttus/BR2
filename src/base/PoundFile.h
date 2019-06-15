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

//#include "../base/ExLib.h"
//#include "../library/LibraryHeader.h"
//#include "../base/BufParser.h"
#include "../base/BaseDataFile.h"
#include "../math/MathAll.h"
namespace Game {
/**
*    @class PoundFile
*    @brief
*
*/
class PoundFile : public BaseDataFile {
private:
    enum {TempArgsSize=64};
    t_string _tempArgs[TempArgsSize];

    virtual void parse(char* buf);
    virtual void preLoad(){}
    virtual void postLoad(){}

    int32_t _iCurrentParseLine;    //crurent parse line.

    bool isEscapeCharacter(char c);
    
protected:
    t_string& getTempArg(size_t off) { return _tempArgs[off]; }
    t_string* getTempArgs() { return _tempArgs; }

    std::vector<t_string> _vecTokens;
    virtual bool validateArguments(t_string& arg, int32_t count=-1);    // throw exception
    
    void gatherArgs();
    void gatherArgs(t_string* _a, size_t _size);
    vec3 rdVec3f(t_string* t, int32_t offset);
    ivec3 rdVec3i(t_string* t, int32_t offset);
    bool getYesOrNo(t_string& x);
 //   void writeCmd(DiskFile* df, t_string* cmdName, ...);
    t_string tryGetArg(int32_t arg);
    void displayError(t_string& errMsg, bool bThrow = true);
    void showMessageOnFailure(bool b);
   // t_string getCleanToken(std::vector<t_string>& tokens, int iind); //Gets, doesn't increment iind
    ////////////////////////////////////////////////////////////////
    // Must be implemented
    virtual void pkp(std::vector<t_string>& tokens)=0;
    
public:
    t_string getCleanTokenInd(std::vector<t_string>& tokens, int iind); //Increments iind
    t_string getCleanToken(std::vector<t_string>& tokens, int& iind); //Increments iind
    void parseErr(t_string str, bool bDebugBreak = true, bool bFatal = false);
    mat4 parseMat4(t_string tok);
    bool parsePrs(t_string tok, vec3& pos, vec4& quat, vec3& scl);
    int32_t getCurrentParseLine() { return _iCurrentParseLine; }
    //+-- CTOR/DTOR --+
    PoundFile();
    PoundFile(t_string& loc);
    virtual ~PoundFile() override ;
};

}//ns game



#endif
