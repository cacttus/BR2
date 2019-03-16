#include "../base/FileSystem.h"
#include "../base/GLContext.h"
#include "../base/RoomBase.h"
#include "../base/StringUtil.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/TStr.h"
#include "../base/DateTime.h"

namespace Game {
t_string FileSystem::_strExePath = "";
///////////////////////////////////////////////////////////////////
FileSystem::FileSystem()
{

}
FileSystem::~FileSystem()
{

}
///////////////////////////////////////////////////////////////////
t_string FileSystem::appendCachePathToFile(const t_string& strFileName) {
    std::shared_ptr<RoomBase> rb = Gu::getContext()->getRoom();
    t_string ret = rb->makeAssetPath(rb->getCacheDir(), strFileName);
    return ret;
}
t_string FileSystem::getExecutableFullPath() {
    //  t_string ret;
    //  char* p = SDL_GetBasePath();
    //  if (p == nullptr) {
    //      BroLogError("SDL Doesn't implement SDL_GetBasePath. Using program argument '",FileSystem::_strExePath,"'  This will result in errors.");
    t_string ret = FileSystem::_strExePath;
    // }
   //  else {
   //      ret = t_string(p);
  //   }
    return ret;
}
t_string FileSystem::getExecutableName() {
    t_string ret;
    ret = FileSystem::getFileNameFromPath(getExecutableFullPath(), true);
    return ret;
}
t_string FileSystem::getExecutableDirectory() {
    t_string ret;
    ret = FileSystem::getPathFromPath(getExecutableFullPath(), true);
    return ret;
}
t_string FileSystem::combinePath(t_string a, t_string b)
{
    if ((a.length() == 0) || (b.length() == 0))
        return a + b;

    // - Make sure path is a / path not a \\ path
    a = formatPath(a);
    b = formatPath(b);

    // Remove / from before and after strings
    a = StringUtil::trim(a, '/');
    b = StringUtil::trim(b, '/');

    return a + t_string("/") + b;
}
bool FileSystem::createFile(const t_string& filename, bool trunc, bool bLog)
{
    std::fstream pStream;
    bool exists = true;

    //TODO: specify in or out in case of read / write access problems.
    pStream.open(filename.c_str(), std::ios::out | (trunc ? (std::ios::app) : (std::ios::trunc)));

    if (!pStream.good())
    {
        exists = false;
        t_string err = TStr(" [FileSystem] Output file ", filename, " could not be created with trunc=", (int32_t)trunc);
        if (bLog == true) {//For logger initializtion
            BroLogError(err);
        }
        else {
            Gu::print(err);
        }
    }

    pStream.close();
    //CheckOsErrorsDbg();
    return exists;
}

bool FileSystem::createDirectoryRecursive(t_string dirName) {
    t_string dirCpy;
    bool bRet = true;
    dirCpy.assign(dirName);
    dirCpy = formatPath(dirCpy);

    if (directoryExists(dirName) == true) {
        return true;
    }

    std::vector<t_string> dirs = StringUtil::split(dirCpy, '/');
    t_string strDir = dirs[0];
    for (size_t i = 1; i < dirs.size(); ++i)
    {
        strDir = FileSystem::combinePath(strDir, dirs[i]);
        bRet = bRet && FileSystem::createDirectorySingle(strDir);
    }

    return bRet;
}
bool FileSystem::directoryExists(t_string dirName)
{
    bool ret;
    DIR* dir = NULL;

    dir = opendir(dirName.c_str());

    if (dir == NULL) {
        ret = false;
    }
    else {
        ret = true;
    }

    closedir(dir);

    return ret;
}
bool FileSystem::createDirectorySingle(t_string& dirName) {
    int ret = mkdir(dirName.c_str());
    if (ret != 0) {
        if (ret == EEXIST) {
            //BroLogInfo("mkdir directory already exists");
        }
        else if (ret == ENOENT) {
            //DO NOT LOG
            //BroLogError("mkdir failed with code ", ret);
            return false;
        }
        else {
            //DO NOT LOG
            //BroLogWarn("mkdir returned code ", ret);
        }
    }
    return true;
    //#ifdef BRO_OS_WINDOWS
//    //SECURITY_ATTRIBUTES sc;
//    //sc.nLength = sizeof(sc);
//    //sc.bInheritHandle = false;
//    //sc.lpSecurityDescriptor = NULL;
//
//    BOOL b = CreateDirectory(dir.c_str(),NULL);
//#else
    // - Open / Make directory
    //  if (opendir(dir.c_str()) == NULL)
    //  {
    //      if (_mkdir(dir.c_str()) == -1)
    //          throw new Exception("Could not make log directory.",__LINE__,__FILE__);
    //  }
    //use mkdir
    //#endif
}
t_string FileSystem::getFileNameOrDirectoryFromPath(const t_string& name, bool bformatPath)
{
    // - If formatPath is true then we'll convert the path to a / path.

    size_t x;
    t_string fn = "";
    DiskLoc l2;

    // Format for a / path
    if (bformatPath) {
        l2 = formatPath(name);
    }
    else {
        l2 = name;
    }

    if ((x = l2.rfind('/')) != t_string::npos) {
        fn = l2.substr(x + 1, l2.length() - x + 1);
    }
    else {
        fn = name;
    }
    return fn;
}
t_string FileSystem::getFileNameFromPath(const t_string& name, bool bformatPath) {
    return getFileNameOrDirectoryFromPath(name, bformatPath);
}
t_string FileSystem::getDirectoryFromPath(const t_string& name, bool bformatPath){
    return getFileNameOrDirectoryFromPath(name, bformatPath);
}
t_string FileSystem::getPathFromPath(const t_string& name, bool bformatPath)
{
    // - If formatPath is true then we'll convert the path to a / path.
    // returns the path part without the filename.
    // if the filename doesn't have an extension this method considers it a folder.
    //
    size_t x;
    t_string ret = "";
    DiskLoc l2;
    DiskLoc tmpPath = name;
    t_string fn;

    // Format for a / path
    if (bformatPath)
        l2 = formatPath(name);
    else
        l2 = name;

    fn = getFileNameFromPath(l2, false);

    if (fn.rfind('.') == t_string::npos)
        ret = l2;
    else if ((x = l2.rfind('/')) != t_string::npos)
        ret = l2.substr(0, x);
    else
        ret = l2;

    return ret;
}
bool FileSystem::fileExists(t_string filename)
{
    //**NOTE: a portable option would be to use this code from SDL.
    //However the code below is supposed to be "faster"
    /*
    SDL_RWops *rw = SDL_RWFromFile(filename.c_str(), "rb");
    if (rw == nullptr)
    return false;

    SDL_RWclose(rw);
    return true;
    */

    //**Alternate version
    bool exists;
    struct stat buffer;
    exists = (stat(filename.c_str(), &buffer) == 0);

    #ifdef BRO_OS_WINDOWS
    //Fuck this shit
    // we get error 2 and 6 all the time because
    // windows sets it when we call stat()
    // WHY??? i don't need a fucking error when i'm seeing whether a file exists.
    SetLastError(0);
    #else
    #error "Operating System Error"
    #endif

    return exists;
}
void FileSystem::createFileIfFileDoesNotExist(t_string& filename, bool bAlsoCreateDirectoryPath) {
    if (bAlsoCreateDirectoryPath == TRUE) {
        createDirectoryRecursive(getPathFromPath(filename, TRUE));
    }

    if (FileSystem::fileExists(filename) == false) {
        createFile(filename, false);
    }
}
time_t FileSystem::getFileModifyTime(t_string& location) {
    struct stat fileInfo;

    if (FileSystem::fileExists(location) == false) {
        BroThrowException("File '", location, "' does not exist");
    }

    stat(location.c_str(), &fileInfo);
    return fileInfo.st_mtime;
}
t_string FileSystem::getCurrentDirectory() {
    //DO NOT LOG HERE
    char buf[BRO_MAX_PATH];
    _getcwd(buf, BRO_MAX_PATH);
    t_string cwd = t_string(buf);
    return cwd;
}
bool FileSystem::isFile(t_string fileOrDirPath) {
    //https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
    struct stat s;
    if (stat(fileOrDirPath.c_str(), &s) == 0) {
        if (s.st_mode & S_IFREG) {
            return true;
        }
    }
    else {
        //DO NOT LOG HERE
        //BroLogError("stat - failed");
    }
    return false;
}
bool FileSystem::isDir(t_string fileOrDirPath) {
    //https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
    struct stat s;
    if (stat(fileOrDirPath.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }
    else {
        //DO NOT LOG HERE
        //BroLogError("stat - failed");
    }
    return false;
}
bool FileSystem::getAllFilesOrDirs(t_string dir, std::vector<t_string>& __out_ dirs, bool bFiles) {
    dirs.resize(0);

    dirent *ep;
    DIR *dp;
    dp = opendir(dir.c_str());

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            t_string name(ep->d_name);
            if (StringUtil::doesNotEqual(name, "..") && StringUtil::doesNotEqual(name, ".")) {
                t_string nameFull = FileSystem::combinePath(dir, name);
                if (!bFiles && FileSystem::isDir(nameFull)) {
                    dirs.push_back(nameFull);
                }
                else if (bFiles && FileSystem::isFile(nameFull)) {
                    dirs.push_back(nameFull);
                }
            }
        }
        closedir(dp);
    }
    else {
        BroLogError("Couldn't open the directory '", dir, "'");
        return false;
    }
    return true;

}
bool FileSystem::getAllDirs(t_string dir, std::vector<t_string>& __out_ dirs) {
    return getAllFilesOrDirs(dir, dirs, false);
}
bool FileSystem::getAllFiles(t_string dir, std::vector<t_string>& __out_ files) {
    return getAllFilesOrDirs(dir, files, true);
}
bool FileSystem::deleteDirectoryRecursive(t_string dir, std::vector<t_string>& vecFileExts) {
    std::vector<t_string> dirs;
    if (getAllDirs(dir, dirs) == false) {
        return false;
    }
    for (t_string dir : dirs) {
        if (deleteDirectoryRecursive(dir, vecFileExts) == false) {
            return false;
        }
    }

    if (deleteAllFiles(dir, vecFileExts) == false) {
        return false;
    }

    int ret = rmdir(dir.c_str());
    if (ret != 0) {
        BroLogError("Failed to delete directory.");
        return false;
    }
    return true;
}
bool FileSystem::deleteDirectory(t_string dir, std::vector<t_string>& vecFileExts) {

    int ret = rmdir(dir.c_str());
    if (ret != 0) {
        BroLogError("Failed to delete directory.");
        return false;
    }
    return true;
}
bool FileSystem::deleteAllFiles(t_string dir, std::vector<t_string>& vecFileExts) {
    //vecFileExts - list of file extensions with '.' like '.exe', '.dat'

    std::vector<t_string> files;
    if (getAllFiles(dir, files) == false) {
        BroLogError("Failed to get files from '", dir, "'");
        return false;
    }

    for (t_string file : files) {
        if (FileSystem::fileExists(file)) {
            bool bMatch = false;
            for (t_string ext : vecFileExts) {
                if (StringUtil::equalsi(FileSystem::getExtensionPartOfFileName(file), ext)) {
                    bMatch = true;
                    break;
                }
            }
            if (bMatch) {
                if (FileSystem::deleteFile(file) == false) {
                    BroLogError("Failed to delte file '", file, "'");
                    return false;
                }
            }
        }
    }

    return true;
}
bool FileSystem::deleteFile(t_string filename) {
    int32_t ret = std::remove(filename.c_str());
    //Equivalent POSIX function
    //unlink(filename.c_str())
    if (ret != 0) {
        BroLogError("Failed to delete file '", filename, "'");
        return false;
    }
    return true;
}

t_string FileSystem::getFilePartOfFileName(const t_string& fileName) {
    size_t off = fileName.rfind(".");

    if (off >= fileName.size() || off == t_string::npos) {
        BroThrowException("Incorrect file name, no extension given or incorrect extension.");
    }

    return fileName.substr(0, off);
}
t_string FileSystem::getExtensionPartOfFileName(const t_string& name) {
    //2017 12 22 - **Changed this to return the DOT just like .NET ( .exe, .dat etc..)
    size_t x;
    t_string ext = "";
    t_string fileName;

    fileName = FileSystem::getFileNameFromPath(name);

    if ((x = fileName.rfind('.')) != t_string::npos) {
        ext = fileName.substr(x, fileName.length() - x);
    }

    return ext;
}
t_string FileSystem::formatPath(const t_string& p)
{
    return StringUtil::replaceAll(p, '\\', '/');
}
t_string FileSystem::getScreenshotFilename() {
    t_string fname = TStr(DateTime::dateTimeToStr(DateTime::getDateTime()), "_", FileSystem::getExecutableName(), "_frame.png");
    fname = FileSystem::replaceInvalidCharsFromFilename(fname);
    fname = StringUtil::replaceAll(fname, " ", "_");
    fname = FileSystem::appendCachePathToFile(fname);

    return fname;
}
t_string FileSystem::replaceInvalidCharsFromFilename(const t_string& __in_ fnIn, char replaceChar, bool bIgnoreSlashes) {
    //Remove invalid WINDOWS characters from path
    t_string fname = fnIn;
    fname = StringUtil::replaceAll(fname, "<", TStr(replaceChar));
    fname = StringUtil::replaceAll(fname, ">", TStr(replaceChar));
    fname = StringUtil::replaceAll(fname, ":", TStr(replaceChar));
    fname = StringUtil::replaceAll(fname, "\"", TStr(replaceChar));
    if (bIgnoreSlashes == false) {
        fname = StringUtil::replaceAll(fname, "/", TStr(replaceChar));
        fname = StringUtil::replaceAll(fname, "\\", TStr(replaceChar));
    }
    fname = StringUtil::replaceAll(fname, "|", TStr(replaceChar));
    fname = StringUtil::replaceAll(fname, "?", TStr(replaceChar));
    fname = StringUtil::replaceAll(fname, "*", TStr(replaceChar));

    return fname;
}
void FileSystem::SDLFileFree(char*& pOutData) {
    delete[] pOutData;
    pOutData = nullptr;
}
int FileSystem::SDLFileRead(std::string fname, char*& pOutData, uint32_t& _iOutSizeBytes, bool addNull) {

    fname = getFilePath(fname);

    SDL_RWops *rw = SDL_RWFromFile(fname.c_str(), "rb");
    if (rw == NULL)
        return 1;

    _iOutSizeBytes = (uint32_t)SDL_RWsize(rw);
    if (addNull) {
        pOutData = new char[_iOutSizeBytes + 1];
    }
    else {
        pOutData = new char[_iOutSizeBytes];
    }

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = pOutData;
    while (nb_read_total < (uint32_t)_iOutSizeBytes && nb_read != 0) {
        nb_read = (Sint64)SDL_RWread(rw, buf, 1, (std::size_t)((Sint64)_iOutSizeBytes - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);
    if (nb_read_total != _iOutSizeBytes) {
        delete[] pOutData; // free(pOutData);
        return 1;
    }

    if (addNull) {
        pOutData[nb_read_total] = '\0';
    }
    return 0;
}
int FileSystem::SDLFileWrite(std::string fname, char* pData, uint32_t _iDataSizeBytes) {
    fname = getFilePath(fname);

    SDL_RWops *rw = SDL_RWFromFile(fname.c_str(), "wb");
    if (rw == NULL)
        return 1;

    SDL_RWwrite(rw, pData, 1, _iDataSizeBytes);
    SDL_RWclose(rw);

    return 0;
}

std::string FileSystem::getFilePath(std::string name) {
    std::string str;
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
    //Gosh napoleon
    int pos1 = name.rfind("/");
    int pos2 = name.rfind("\\");

    int pos3;
    if (pos1 != std::string::npos && pos2 != std::string::npos) {
        if (pos1 > pos2) {
            pos3 = pos1;
        }
        else {
            pos3 = pos2;
        }
    }
    else if (pos1 != std::string::npos) {
        pos3 = pos1;

    }
    else if (pos2 != std::string::npos) {
        pos3 = pos2;

    }
    else {
        //Error
        throw 0;
        return "";
    }

    std::string fname = name.substr(pos3 + 1, name.length() - (pos3 + 1));
    std::string fdir = name.substr(0, pos3 + 1);

    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFStringRef rName = Gu::stdStrToCFStr(fname);
    CFStringRef rDir = CFSTR("");//stdStrToCFStr(fdir);//CFSTR("");
                                 //It appears that the bundle doesn't use directories we supply  If it does we should change this in the futrue.

                                 //CFStringFileNa
                                 //NULL file extension - indicates extension is in the name
    CFURLRef rUrl = CFBundleCopyResourceURL(mainBundle, rName, NULL, rDir);

    if (rUrl == NULL) {
        throw 0;
        return "";
    }
    //stackoverflow.com/questions/21933548/how-to-read-a-xml-file-using-fstream-in-ios
    CFStringRef filePath = CFURLCopyFileSystemPath(rUrl, kCFURLPOSIXPathStyle);
    CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
    const char* path = CFStringGetCStringPtr(filePath, encodingMethod);
    str = std::string(path);

#endif
#else
    //WIN / ANDROID
    str = name;
#endif
    return str;
}















}//ns Game
