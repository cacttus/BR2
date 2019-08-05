#include "../base/Exception.h"
#include "../base/TextDataFile.h"
#include "../base/StringUtil.h"
#include "../base/Gu.h"
#include "../base/FileSystem.h"
#include "../base/Logger.h"

namespace Game {
void TextDataFile::msg(t_string msg, bool error) {
    _vecMessages.push_back( Stz (error ? "ERROR:" : "") + _fileName + ": " + msg);
}
void TextDataFile::loadAndParse()
{
    loadAndParse(_fileLoc);
}
void TextDataFile::loadAndParse(t_string&loc)
{
    _fileData = nullptr;
    _fileLoc = loc;
    _fileName = FileSystem::getFilePartOfFileName(loc);
    _fileSize = 0;
    _vecMessages.clear();

    // - Execute actions before we load
    preLoad();
    
    // - Load and parse
    bool success = false;
	if (FileSystem::SDLFileRead(loc, _fileData, _fileSize, true) != 0) {
		t_string st = FileSystem::getCurrentDirectory();

		//File not found.
		Gu::debugBreak();
	}
	else {
		parse(_fileData, _fileSize);

		FileSystem::SDLFileFree(_fileData);

		// - Execute actions after load
        success = true;
	}

    //Dump messages.
    if (_vecMessages.size() > 0) {
        t_string str = StringUtil::join("\r\n", _vecMessages);
        BroLogInfo("Errors:" + StringUtil::tabify(str, 2));
    }

    postLoad(success);
}
void TextDataFile::save(t_string& loc)
{
    BroThrowNotImplementedException();
}

//+-- CTOR/DTOR --+
TextDataFile::TextDataFile() : _fileData(NULL)
{
}
TextDataFile::TextDataFile(t_string& loc): 
_fileLoc(loc)
{
}
TextDataFile::~TextDataFile()
{

}

}//ns game
