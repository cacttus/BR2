/**
*
*    @file TextConfigFile.h
*    @date August 4, 2019
*    @author Derek Page
*
*    © 2019 
*
*
*/
#pragma once
#ifndef __PARSABLEFILE_156494899751183385_H__
#define __PARSABLEFILE_156494899751183385_H__

#include "../base/TextDataFile.h"

namespace Game {
/**
*    @class TextConfigFile
*    @brief Any file that can be processed as a list of string name-value tokens.
*/
class TextConfigFile : public TextDataFile {
private:
protected:
    int32_t _iCurrentParseLine = 1;    //crurent parse line.
    std::vector<t_string> _vecTokens;

    //Send in a list of parameters [key] [val] [val] [val]
    virtual void pkp(std::vector<t_string>& tokens) = 0;// Must be implemented

    //Utils.
    virtual bool validateArguments(t_string& token, int32_t count = -1);    // throw exception
    void displayError(const t_string& errMsg, bool bThrow = true);
    bool cmp(t_string& tok0, const char* ch, int32_t validateArgCount) ;
public:
    bool lcmp(t_string& tok0, const char* ch, int32_t validateArgCount = -1);
    void parseErr(const t_string& str, bool bDebugBreak = false, bool bFatal = false);
    t_string getCleanToken(std::vector<t_string>& tokens, int& iind);
    t_string getCleanTokenInd(std::vector<t_string>& tokens, int iind);

    TextConfigFile() {}
    TextConfigFile(t_string loc);
    virtual ~TextConfigFile() override;
};

}//ns Game



#endif
