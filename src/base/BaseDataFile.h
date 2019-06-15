/**
*
*    @file BaseDataFile.h
*    @date November 22, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __BASEDATAFILE_210628895103872956110227_H__
#define __BASEDATAFILE_210628895103872956110227_H__


//#include "../base/ExLib.h"
//#include "../library/LibraryHeader.h"
#include "../base/BufParser.h"

namespace Game {
/**
*    @class BaseDataFile
*    @brief Base class for data files.
*
*/
class BaseDataFile : public BufParser {
protected:
    t_string _fileLoc;
    char* _fileData;
protected:
    // Override these methods.
    virtual void preLoad()=0;
    virtual void parse(char* buf)=0;
    virtual void postLoad()=0;

    //Parse Helpers
    int32_t _curIdx;// gets IDX of current token array value
    //void initp(){_curIdx=0;}
    //t_string& np(t_string* arr){ return *arr[_curIdx++]; }


    virtual bool validateArguments(t_string& token, int32_t count=-1);    // throw exception
public:
    bool lcmp(t_string& tok0, const char* ch, int32_t validateArgCount = -1);    // insensitive string cmp
    bool cmp(t_string& tok0, const char* ch, int32_t validateArgCount = -1);    // sensitive string cmp

    t_string& getFileLoc(){ return _fileLoc; } //This is used don't change
    void loadAndParse();// - If empty is specified then we use _fileLoc
    void loadAndParse(t_string& loc);// - If empty is specified then we use _fileLoc
    virtual void save(t_string& loc);

    //+-- CTOR/DTOR --+
    BaseDataFile();
    BaseDataFile(t_string& loc);
    virtual ~BaseDataFile() override ;
};

}//ns game



#endif
