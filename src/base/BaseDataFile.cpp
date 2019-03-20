#include "../base/Exception.h"
#include "../base/BaseDataFile.h"
#include "../base/StringUtil.h"
#include "../base/Gu.h"
#include "../base/FileSystem.h"

namespace Game {;
bool BaseDataFile::lcmp(t_string& tok0, const char* ch, int32_t validateArgCount)
{
    bool ret;
    
    ret = StringUtil::equalsi(tok0, t_string(ch));//(StringUtil::lowercase(tok0) == StringUtil::lowercase(t_string(ch)));
    
    if(validateArgCount>=0)
        if(ret==true)
            validateArguments(tok0, validateArgCount);

    return ret;
}
bool BaseDataFile::cmp(t_string& tok0, const char* ch, int32_t validateArgCount)
{
    bool ret = tok0 == t_string(ch);

    if(validateArgCount>=0)
        if(ret==true)
            validateArguments(tok0, validateArgCount);

    return ret;
}

void BaseDataFile::loadAndParse()
{
    loadAndParse(_fileLoc);
}
void BaseDataFile::loadAndParse(t_string&loc)
{
    //FileBuffer* fb;
    
    _fileLoc = loc;

    // - Execute actions before we load
    preLoad();
    
    // - Load and parse
    uint32_t _iFileSize;

	if (FileSystem::SDLFileRead(loc, _fileData, _iFileSize, true) != 0) {
		t_string st = FileSystem::getCurrentDirectory();

		//File not found.
		Gu::debugBreak();

	}
	else {

		//fb = FileSystem::loadFile(loc);

		//if(!fb->ptr())
	   //     throw new Exception("Failed to load file.",__LINE__,__FILE__);
	   // _fileData = fb->ptr();
		ptr = _fileData;
		parse(_fileData);

		FileSystem::SDLFileFree(_fileData);

		// - Execute actions after load
		postLoad();
	}
}
void BaseDataFile::save(t_string& loc)
{
    BroThrowNotImplementedException();
}
bool BaseDataFile::validateArguments(t_string& token, int32_t count)
{
    return true;
}
//+-- CTOR/DTOR --+
BaseDataFile::BaseDataFile() : _fileData(NULL)
{
}
BaseDataFile::BaseDataFile(t_string& loc): 
_fileLoc(loc)
{
}
BaseDataFile::~BaseDataFile()
{

}

}//ns game
